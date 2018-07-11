/*
** Copyright 2007-2018 RTE
** Author: Robert Gonzalez
**
** This file is part of Sirius_Solver.
** This program and the accompanying materials are made available under the
** terms of the Eclipse Public License 2.0 which is available at
** http://www.eclipse.org/legal/epl-2.0.
**
** This Source Code may also be made available under the following Secondary
** Licenses when the conditions for such availability set forth in the Eclipse
** Public License, v. 2.0 are satisfied: GNU General Public License, version 3
** or later, which is available at <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: EPL-2.0 OR GPL-3.0
*/
/***********************************************************************

   FONCTION: Conflict graph et cliques.
	           Determination des cliques a partir du conflict graph en
						 utilisant l'algorithme de Bron-Kerbosch .
                 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"  
  
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
  
# include "bb_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define VERBOSE_TOUTES_LES_CLIQUES_TROUVEE NON_PNE

# define DUREE_DE_RECHERCHE_DE_CLIQUES 1800 /*1*/ /* En secondes */

# define INCREMENT_ALLOC_CLIQUES 10000 /*1000*/
# define INCREMENT_ALLOC_TAILLE_CLIQUES  (5*INCREMENT_ALLOC_CLIQUES)
# define NOMBRE_MAX_DE_CLIQUES 100000 /*50000*/
# define DIVISEUR_POUR_NOMBRE_MAX_DE_CLIQUES 4.
# define MAX_LOCAL_EDGES 20000

void PNE_AllocCliques( PROBLEME_PNE * );
void PNE_ReallocNbCliques( PROBLEME_PNE * );
void PNE_ReallocTailleCliques( PROBLEME_PNE * );
void PNE_MaxClique( PROBLEME_PNE * , int * , int * , int * , int , int * , int , int * , int , int * , char * );
										
/*----------------------------------------------------------------------------*/

void PNE_AllocCliques( PROBLEME_PNE * Pne )
{
CLIQUES * Cliques; int i;
Pne->Cliques = (CLIQUES *) malloc( sizeof( CLIQUES ) );
if ( Pne->Cliques == NULL ) return;
Cliques = Pne->Cliques;
Cliques->NbCliquesAllouees = INCREMENT_ALLOC_CLIQUES;
Cliques->First = (int *) malloc( Cliques->NbCliquesAllouees * sizeof( int ) );
if ( Cliques->First == NULL ) {
  free( Cliques->First ); Pne->Cliques = NULL;
	return;
}
Cliques->NbElements = (int *) malloc( Cliques->NbCliquesAllouees * sizeof( int ) );
if ( Cliques->NbElements == NULL ) {
  free( Cliques->First ); Cliques->First = NULL;
  free( Pne->Cliques ); Pne->Cliques = NULL;
	return;
}
Cliques->TailleCLiquesAllouee = INCREMENT_ALLOC_TAILLE_CLIQUES;
Cliques->Noeud = (int *) malloc( Cliques->TailleCLiquesAllouee * sizeof( int ) );
if ( Cliques->Noeud == NULL ) {
  free( Cliques->First ); Cliques->First = NULL;
  free( Cliques->NbElements ); Cliques->NbElements = NULL;
  free( Pne->Cliques ); Pne->Cliques = NULL;
	return;
}

Cliques->LaCliqueEstDansLePool = (char *) malloc( Cliques->NbCliquesAllouees * sizeof( char ) );
if ( Cliques->LaCliqueEstDansLePool == NULL ) {
  free( Cliques->First ); Cliques->First = NULL;
  free( Cliques->NbElements ); Cliques->NbElements = NULL;
  free( Cliques->Noeud ); Cliques->Noeud = NULL;	
  free( Pne->Cliques ); Pne->Cliques = NULL;
	return;
}

Cliques->CliqueDeTypeEgalite = (char *) malloc( Cliques->NbCliquesAllouees * sizeof( char ) );
if ( Cliques->CliqueDeTypeEgalite == NULL ) {
  free( Cliques->First ); Cliques->First = NULL;
  free( Cliques->NbElements ); Cliques->NbElements = NULL;
  free( Cliques->Noeud ); Cliques->Noeud = NULL;
  free( Cliques->LaCliqueEstDansLePool ); Cliques->LaCliqueEstDansLePool = NULL;	
  free( Pne->Cliques ); Pne->Cliques = NULL;
	return;
}

Cliques->NumeroDeCliqueDuNoeud = (int *) malloc( 2 * Pne->NombreDeVariablesTrav * sizeof( int ) );
if ( Cliques->NumeroDeCliqueDuNoeud == NULL ) {
  free( Cliques->First ); Cliques->First = NULL;
  free( Cliques->NbElements ); Cliques->NbElements = NULL;
  free( Cliques->Noeud ); Cliques->Noeud = NULL;	
  free( Cliques->LaCliqueEstDansLePool ); Cliques->LaCliqueEstDansLePool = NULL;
  free( Cliques->CliqueDeTypeEgalite ); Cliques->CliqueDeTypeEgalite = NULL;	
  free( Pne->Cliques ); Pne->Cliques = NULL;
	return;
}

Cliques->NombreDeCliques = 0;
Cliques->Full = NON_PNE;

for ( i = 0 ; i < 2 * Pne->NombreDeVariablesTrav ; i++ ) Cliques->NumeroDeCliqueDuNoeud[i] = -1;

return;
}

/*----------------------------------------------------------------------------*/
void PNE_ReallocNbCliques( PROBLEME_PNE * Pne )
{
int i; int * pt; char * ptc; CLIQUES * Cliques;
Cliques = Pne->Cliques;
i = Cliques->NbCliquesAllouees + INCREMENT_ALLOC_CLIQUES;
pt = (int *) realloc( Cliques->First , i * sizeof( int ) );
if ( pt == NULL ) { Cliques->Full = OUI_PNE; return; }
Cliques->First = pt;
pt = (int *) realloc( Cliques->NbElements , i * sizeof( int ) );
if ( pt == NULL ) { Cliques->Full = OUI_PNE; return; }
Cliques->NbElements = pt;
ptc = (char *) realloc( Cliques->LaCliqueEstDansLePool , i * sizeof( char ) );
if ( ptc == NULL ) { Cliques->Full = OUI_PNE; return; }
Cliques->LaCliqueEstDansLePool = ptc;
ptc = (char *) realloc( Cliques->CliqueDeTypeEgalite , i * sizeof( char ) );
if ( ptc == NULL ) { Cliques->Full = OUI_PNE; return; }
Cliques->CliqueDeTypeEgalite = ptc;
Cliques->NbCliquesAllouees = i;
return;
}
/*----------------------------------------------------------------------------*/
void PNE_ReallocTailleCliques( PROBLEME_PNE * Pne )
{
int i; int * pt; CLIQUES * Cliques;
Cliques = Pne->Cliques;
i = Cliques->TailleCLiquesAllouee + INCREMENT_ALLOC_TAILLE_CLIQUES;
pt = (int *) realloc( Cliques->Noeud , i * sizeof( int ) );
if ( pt == NULL ) { Cliques->Full = OUI_PNE; return; }
Cliques->Noeud = pt;
Cliques->TailleCLiquesAllouee = i;
return;
}

/*----------------------------------------------------------------------------*/

void PNE_ArchiverMaxClique( PROBLEME_PNE * Pne, int NbElemDansK, int * K )
{
int i; CLIQUES * Cliques; int c; int Index; int * Noeud; int j; int Pivot;
int * NumeroDeCliqueDuNoeud;

Pivot = Pne->ConflictGraph->Pivot;

/* Finalement on ne retient pas les cliques a 2 elements car elles sont dans les implications */
if ( NbElemDansK <= 2 ) return;

/* Controle: si la clique ne contient que 2 termes on regarde si c'est une variable et
   sont complement. Si c'est le cas on ne la regarde pas */
if ( NbElemDansK == 2 ) {
  i = K[0];
	j = K[1];
	if ( j == i + Pivot ) return;
	if ( i == j + Pivot ) return;
}
if ( Pne->Cliques == NULL ) {
  PNE_AllocCliques( Pne );
}
if ( Pne->Cliques == NULL ) return;   

Cliques = Pne->Cliques;
Noeud = Cliques->Noeud;
NumeroDeCliqueDuNoeud = Cliques->NumeroDeCliqueDuNoeud;

c = Cliques->NombreDeCliques - 1;
if ( c >= 0 ) Index = Cliques->First[c] + Cliques->NbElements[c];
else Index = 0;
c++;

if ( c == Cliques->NbCliquesAllouees ) {
  PNE_ReallocNbCliques( Pne );
	if ( Cliques->Full == OUI_PNE ) return;
}

Cliques->First[c] = Index;
for ( i = 0 ; i < NbElemDansK ; i++ ) {
  if ( Index == Cliques->TailleCLiquesAllouee ) {
	  /*printf("ReallocTailleCliques\n");*/
    PNE_ReallocTailleCliques( Pne );
	  if ( Cliques->Full == OUI_PNE ) return;
    Noeud = Cliques->Noeud;	
  }		
  Noeud[Index] = K[i];
  /* Affectation d'un numero de clique a la variable */
	if ( NumeroDeCliqueDuNoeud[K[i]] < 0 ) NumeroDeCliqueDuNoeud[K[i]] = c;
	else {
    if ( Cliques->NbElements[NumeroDeCliqueDuNoeud[K[i]]] < NbElemDansK ) {
      NumeroDeCliqueDuNoeud[K[i]] = c;
		}
	}
	Index++;		
}

Cliques->NbElements[c] = NbElemDansK;
Cliques->LaCliqueEstDansLePool[c] = NON_PNE;
Cliques->CliqueDeTypeEgalite[c] = NON_PNE;
Cliques->NombreDeCliques++;

if ( Pne->Cliques->NombreDeCliques > NOMBRE_MAX_DE_CLIQUES ) {
  Cliques->Full = OUI_PNE;
	/*printf("Attention Cliques->Full = OUI_PNE \n");*/
}

# if VERBOSE_TOUTES_LES_CLIQUES_TROUVEE == OUI_PNE
  printf("Clique maximale \n");
  for ( i = 0 ; i < NbElemDansK ; i++ ) printf(" %d ",K[i]);
  printf("\n");
# endif

return;
}   

/*----------------------------------------------------------------------------*/

void PNE_MaxClique( PROBLEME_PNE * Pne, int * First, int * Adjacent, int * NbEdges,
                    int NbElemDansK, int * K, int NbElemDansC, int * C, 
										int NbElemDansA, int * A, char * LeNoeudEstDansC )
{
int i; int j; int n; int k; int Edge; int NbDansC; int * Cbuff; int * Abuff;
int u; int Maximal; int NbDansA; int EdgeMx;

if (Pne->Cliques != NULL ) {
  if ( Pne->Cliques->Full == OUI_PNE )  {		
	  return;
	}
}

/* Si C est vide on a une clique maximale */
if ( NbElemDansC == 0 ) {
  /* On test si la clique est violee */
  PNE_ArchiverMaxClique( Pne, NbElemDansK, K );	
	return;
}

/* Recherche de u dans C tel que C inter V(u) est maximal */
/* Attention, cette sequence peut etre tres couteuse en temps de calcul */

/*for ( n = 0 ; n < NbElemDansC ; n++ ) LeNoeudEstDansC[C[n]] = OUI_PNE;*/

Maximal = -1;
u = 1;
for ( n = 0 ; n < NbElemDansC ; n++ ) {
  i = C[n];
	NbDansC = 0;	
  Edge = First[i];
	EdgeMx = Edge + NbEdges[i];	
	while ( Edge < EdgeMx ) {
		if ( LeNoeudEstDansC[Adjacent[Edge]] == OUI_PNE ) { NbDansC++; break; }
	  Edge++;
  }
	if ( NbDansC > Maximal ) {
	  Maximal = NbDansC;  
		u = i;
	}
}

for ( n = 0 ; n < NbElemDansC ; n++ ) LeNoeudEstDansC[C[n]] = NON_PNE;

if ( Maximal < 0 ) {
  return;
}
/* Fin de la sequence couteuse en temps de calcul */

for ( n = 0 ; n < NbElemDansA ; n++ ) {

	/* i doit etre dans A */
  i = A[n];
	/* i doit etre dans A - V(u) i.e pas dans V(u) */
	k = 1;
  Edge = First[u];
	EdgeMx = Edge + NbEdges[u];
	while ( Edge < EdgeMx ) {	
    if ( Adjacent[Edge] == i ) { k = 0; break; }		
	  Edge++;
  }
	if ( k == 0 ) continue;

	/* Alloc de Cbuff et de Abuff */
  Cbuff = (int *) malloc( ( NbElemDansC + NbElemDansA ) * sizeof( int ) );	
  if ( Cbuff == NULL ) continue;
  Abuff = (int *) &Cbuff[NbElemDansC];
	
	/* Construction de C inter Voisins(i) et de A inter Voisins(i) */
  NbDansC = 0;
  NbDansA = 0;
  Edge = First[i];
	EdgeMx = Edge + NbEdges[i];
	while ( Edge < EdgeMx ) {	
    j = Adjacent[Edge];		
		for ( k = 0 ; k < NbElemDansC ; k++ ) {
	    /* Ajouter j au nouveau C */
		  if ( C[k] == j ) { Cbuff[NbDansC] = j; LeNoeudEstDansC[j] = OUI_PNE; NbDansC++; break; }
		}
		for ( k = 0 ; k < NbElemDansA ; k++ ) {
	    /* Ajouter j au nouveau A */
		  if ( A[k] == j ) { Abuff[NbDansA] = j; NbDansA++; break; }
		}				
	  Edge++;
  }	
	
	K[NbElemDansK] = i;
	NbElemDansK += 1;

  PNE_MaxClique( Pne, First, Adjacent, NbEdges, NbElemDansK, K, NbDansC, Cbuff, NbDansA, Abuff, LeNoeudEstDansC );

	NbElemDansK -= 1;
	
	/* On enleve i de l'ensemble A */	 	
	A[n] = A[NbElemDansA-1];
	NbElemDansA--;
	n--;		
	
	free( Cbuff );
	
	if ( Pne->Cliques != NULL ) {
    if ( Pne->Cliques->Full == OUI_PNE ) break;
  }

  if ( Pne->ArreterCliquesEtProbing == NON_PNE ) {	
    time( &(Pne->HeureDeCalendrierCourantCliquesEtProbing) );
    Pne->TempsEcoule = difftime( Pne->HeureDeCalendrierCourantCliquesEtProbing, Pne->HeureDeCalendrierDebutCliquesEtProbing );
    if ( Pne->Controls == NULL || 1 ) {		
	    if ( Pne->TempsEcoule > DUREE_DE_RECHERCHE_DE_CLIQUES ) {
			  Pne->ArreterCliquesEtProbing = OUI_PNE;
			  if ( Pne->AffichageDesTraces == OUI_PNE ) {
			   printf("Cliques computation was stopped. Timeout is %d elapsed is %e\n",DUREE_DE_RECHERCHE_DE_CLIQUES,Pne->TempsEcoule);
			  }				
			}
		}
		else {
      /* On est dans une heuristique */
	    if ( Pne->TempsEcoule > 1 ) Pne->ArreterCliquesEtProbing = OUI_PNE;			
		}						
  }
	
  if ( Pne->ArreterCliquesEtProbing == OUI_PNE ) break;
		
}

return;   
}

/*----------------------------------------------------------------------------*/

void PNE_CliquesConflictGraph( PROBLEME_PNE * Pne )
{
int i; int NombreDeVariablesTrav; int * Adjacent; int * Next; int * First;
CONFLICT_GRAPH * ConflictGraph; int * K; int * C; int * A; int NbElemDansK;
int NbElemDansC; int NbElemDansA; PROBING_OU_NODE_PRESOLVE * Prb;
int NbNoeudsDuGraphe; char * LeNoeudEstDansC; int * PremiereArete; int * NombreAretes;
int * NoeudAdjacent; char * NoeudDejaExamine; int * NbEdgesParNoeud; int N; int NbN;
int Node; int Cpt; int * NoeudLocal; int j; int * Pile; int NbAretes; int Nb; int Edge;

if ( Pne->Controls != NULL ) {
  if ( Pne->Controls->RechercherLesCliques == NON_PNE ) {
	  return;
	}
}

Prb = Pne->ProbingOuNodePresolve;
ConflictGraph = Pne->ConflictGraph;
if ( ConflictGraph == NULL ) return;

if ( ConflictGraph->NbEdges <= ConflictGraph->NbEdgesLast ) return;
ConflictGraph->NbEdgesLast = ConflictGraph->NbEdges;

NombreDeVariablesTrav = Pne->NombreDeVariablesTrav;
NbNoeudsDuGraphe = ConflictGraph->NbNoeudsDuGraphe;
Adjacent = ConflictGraph->Adjacent;
Next = ConflictGraph->Next;
First = ConflictGraph->First;

/* On reinitialise les cliques car sinon on peut empiler plusieurs cliques identiques */
if ( Pne->Cliques != NULL ) {
	/* S'il y avait deja trop de cliques, on arrete */
	if ( Pne->Cliques->Full == OUI_PNE ) return;
	
  Pne->Cliques->NombreDeCliques = 0;
  Pne->Cliques->Full = NON_PNE;
  for ( i = 0 ; i < 2 * Pne->NombreDeVariablesTrav ; i++ ) Pne->Cliques->NumeroDeCliqueDuNoeud[i] = -1;	
}

K = (int *) malloc( NbNoeudsDuGraphe * sizeof( int ) );
C = (int *) malloc( NbNoeudsDuGraphe * sizeof( int ) );
A = (int *) malloc( NbNoeudsDuGraphe * sizeof( int ) );
LeNoeudEstDansC = (char *) malloc( NbNoeudsDuGraphe * sizeof( char ) );

Pile = (int *) malloc( ConflictGraph->NbEdges * sizeof( int ) );
NbEdgesParNoeud = (int *) malloc( NbNoeudsDuGraphe * sizeof( int ) );
NoeudLocal = (int *) malloc( NbNoeudsDuGraphe * sizeof( int ) );
PremiereArete = (int *) malloc( NbNoeudsDuGraphe * sizeof( int ) );
NombreAretes = (int *) malloc( NbNoeudsDuGraphe * sizeof( int ) );
NoeudAdjacent = (int *) malloc( ConflictGraph->NbEdges * sizeof( int ) );
NoeudDejaExamine = (char *) malloc( NbNoeudsDuGraphe * sizeof( char ) );

if ( K == NULL || C == NULL || A == NULL || LeNoeudEstDansC == NULL ||
     Pile == NULL || NbEdgesParNoeud == NULL  || NoeudLocal == NULL ||
		 PremiereArete == NULL || NombreAretes == NULL || NoeudAdjacent == NULL ||
		 NoeudDejaExamine == NULL ) {
  free( K ); free( C ); free( A ); free( LeNoeudEstDansC );
	free( Pile );	free( NbEdgesParNoeud ); free( NoeudLocal );	free( PremiereArete );
	free( NombreAretes );	free( NoeudAdjacent ); free( NoeudDejaExamine );
	return;	
}

for ( N = 0 ; N < NbNoeudsDuGraphe ; N++ ) {
  NoeudDejaExamine[N] = NON_PNE;
  NbAretes = 0;
  Edge = First[N];
  while ( Edge >= 0 ) {	
	  NbAretes++;
	  Edge = Next[Edge];
  }
	NbEdgesParNoeud[N] = NbAretes;
}

for ( N = 0 ; N < NbNoeudsDuGraphe ; N++ ) {
  if ( NoeudDejaExamine[N] == OUI_PNE ) continue;
	if ( First[N] < 0 ) continue;
	NbN = 0;
	Cpt = 0;
	Node = N;
  NbAretes = 0;
  DepartNoeud:
  NoeudLocal[NbN] = Node;
	
	NbAretes += NbEdgesParNoeud[Node];
	if ( NbAretes > MAX_LOCAL_EDGES ) goto GrapheUtile;
	
  NbN++;
	NoeudDejaExamine[Node] = OUI_PNE;
 /* Construction de la liste des noeuds de la composante connexe */
  Edge = First[Node];
	DepartEdge:
  while ( Edge >= 0 ) {
    j = Adjacent[Edge];
		if ( NoeudDejaExamine[j] == NON_PNE ) {
		  /* On met j dans la pile et on examine ses voisins */
		  Pile[Cpt] = Edge;		
		  Cpt++;
			Node = j;
		  goto DepartNoeud;
		}
	  Edge = Next[Edge];
  }
	Cpt--;
	if ( Cpt >= 0 ) {
	  Edge = Pile[Cpt];
		goto DepartEdge;
	}
	GrapheUtile:
	
	if ( NbN <= 4 ) continue;
  /* Transfert du graphe utile */
	NbAretes = 0;
	for ( j = 0 ; j < NbN ; j++ ) {
    Node = NoeudLocal[j];
		Nb = 0;
    Edge = First[Node];
		PremiereArete[Node] = NbAretes;
    while ( Edge >= 0 ) {
      NoeudAdjacent[NbAretes] = Adjacent[Edge];
			NbAretes++;
			Nb++;
	    Edge = Next[Edge];
    }
		NombreAretes[Node] = Nb;
	}
	
	if ( NbAretes <= 3 ) continue;
	
  NbElemDansK = 0;
  NbElemDansC = 0;
  NbElemDansA = 0;

  /* C et A : tous noeuds */  
  for ( j = 0 ; j < NbN ; j++ ) { 
	  Node = NoeudLocal[j];  
    LeNoeudEstDansC[Node] = NON_PNE;
    if ( NombreAretes[Node] < 0 ) continue;
    C[NbElemDansC] = Node;
    LeNoeudEstDansC[Node] = OUI_PNE;
 	  NbElemDansC++;
	  A[NbElemDansA] = Node;
	  NbElemDansA++;	
  }

  Pne->ArreterCliquesEtProbing = NON_PNE;
  time( &(Pne->HeureDeCalendrierDebutCliquesEtProbing) );
	
  PNE_MaxClique( Pne, PremiereArete, NoeudAdjacent, NombreAretes, NbElemDansK, K, NbElemDansC, C, NbElemDansA, A, LeNoeudEstDansC );

  if ( Pne->Cliques != NULL ) Pne->Cliques->Full = NON_PNE;
  Pne->ArreterCliquesEtProbing = NON_PNE;			
 
}

free( K );
free( C );
free( A );
free( LeNoeudEstDansC );

free( Pile );
free( NbEdgesParNoeud );
free( NoeudLocal );
free( PremiereArete );
free( NombreAretes );
free( NoeudAdjacent );
free( NoeudDejaExamine );
  
if ( Pne->AffichageDesTraces == OUI_PNE ) {
  if ( Pne->Cliques != NULL ) {
    if ( Pne->Cliques->NombreDeCliques > 0 ) {
      printf("Found %d clique(s) using the conflict graph\n",Pne->Cliques->NombreDeCliques);
		}
	}
}

if ( Pne->Cliques != NULL && 0 ) {
  if ( Pne->Cliques->NombreDeCliques > 0 ) {
	  /* Apres essais je n'ai jamais trouve de cas ou on pouvait transformer en egalites.
		   Donc inutile d'y passer du temps. */
    /*PNE_TransformerCliquesEnEgalites( Pne );*/
		/*
		printf("Exit dans cliques determination\n");
		exit(0);
		*/
	}
}



return;
}













