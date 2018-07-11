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

   FONCTION: On fixe des variables du conflict graph.
	           Si un noeud et son complement on un meme noeud voisin,
						 ce noeud voisin est fixe a la valeur de son complement.
                 
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

/*----------------------------------------------------------------------------*/
/* Pour chaque arc d'egalite, si un noeud est voisin de chaque extremite de l'arc,
   on peut fixer sa valeur et supprimer tous les arcs qui partent du noeud */
	 
void PNE_ConflictGraphFixerVariables( PROBLEME_PNE * Pne )
{
int Pivot; int NbNoeudsAExaminer; int Noeud; int Edge; int Complement; int V_Noeud;
int * Adjacent;int * Next;int * First; int * T; CONFLICT_GRAPH * ConflictGraph;
double * UminTrav; double * UmaxTrav; int Var; double ValeurDeVar;

if ( Pne->ConflictGraph == NULL ) return;
ConflictGraph = Pne->ConflictGraph;
if ( ConflictGraph->Full == OUI_PNE ) return;
UminTrav = Pne->UminTrav;
UmaxTrav = Pne->UmaxTrav;
Adjacent = ConflictGraph->Adjacent;
Next = ConflictGraph->Next;
First = ConflictGraph->First;
Pivot = ConflictGraph->Pivot;  
NbNoeudsAExaminer = Pivot;

T = (int *) malloc( (NbNoeudsAExaminer*2) * sizeof( int ) );
if ( T == NULL ) return;
memset( (char *) T, 0, (NbNoeudsAExaminer*2) * sizeof( int ) );
	 
/* Il suffit de balayer les noeuds a 1 */
for ( Noeud = 0 ; Noeud < NbNoeudsAExaminer ; Noeud++ ) {
  Complement = Pivot + Noeud;
  Edge = First[Noeud];
	/* On verifie la presence du complement */
  while ( Edge >= 0 ) {
	  if ( Adjacent[Edge] == Complement ) goto TestFixationDeVariable;
		Edge = Next[Edge];
  }
	continue;
	TestFixationDeVariable:
	/* Il y a une egalite entre Noeud et Complement */
	/* On marque la liste des voisins de Complement */
  Edge = First[Complement];
  while ( Edge >= 0 ) {
		T[Adjacent[Edge]] = 1;
    Edge = Next[Edge];
	}
	T[Noeud] = 0; /* Pour ne pas prendre en compte l'arc d'egalite */
	/* On parcours la liste des voisins de Noeud. Si T = 1 alors le noeud doit etre fixe */
  Edge = First[Noeud];
  while ( Edge >= 0 ) {
		V_Noeud = Adjacent[Edge];
		if ( T[V_Noeud] == 1 ) {
		  T[V_Noeud] = 0; /* Comme il peut y avoir changement du chainage, on ne veut pas etudier 2 fois le meme noeud */
		  /*printf("Fixation du noeud %d\n",V_Noeud);*/
      /* PNE_ConflictGraphFixerLesNoeudsVoisinsDunNoeud va egalement supprimer du graphe le noeud
				 et son complement */
	    if ( V_Noeud < Pivot ) {
				/* C'est la valeur 1 qui est interdite, on fixe la variable a 0 et on en tire
					 les consequences pour ses voisins dans le graphe */
        Var = V_Noeud;
		    /*if ( UminTrav[Var] == UmaxTrav[Var] ) continue;*/ /* Pour ne pas faire Edge = Next[Edge] */								
				ValeurDeVar = 0;
        /* Mise a jour des indicateurs de variables a instancier a nouveau */
        PNE_ProbingMajFlagVariablesAInstancier( Pne, Var, ValeurDeVar );				
				/* Cette fonction va aussi supprimer du graphe les noeuds "Pivot + V_Noeud" et "V_Noeud" */
        PNE_ConflictGraphFixerLesNoeudsVoisinsDunNoeud( Pne, Pivot + V_Noeud );				
				/* On redemarre au debut de Noeud car le chainage a pu changer */
				Edge = First[Noeud];
				continue; /* Pour ne pas faire Edge = Next[Edge] */
      }
			else {
				/* C'est la valeur 0 qui est interdite, on fixe la variable a 1 et on en tire
					 les consequences pour ses voisins dans le graphe */
        Var = V_Noeud - Pivot;
		    /*if ( UminTrav[Var] == UmaxTrav[Var] ) continue;*/ /* Pour ne pas faire Edge = Next[Edge] */							
				ValeurDeVar = 1;
        /* Mise a jour des indicateurs de variables a instancier a nouveau */
        PNE_ProbingMajFlagVariablesAInstancier( Pne, Var, ValeurDeVar );								 
				/* Cette fonction va aussi supprimer du graphe les noeuds "V_Noeud - Pivot" et "V_Noeud" */					 
        PNE_ConflictGraphFixerLesNoeudsVoisinsDunNoeud( Pne, V_Noeud - Pivot );				
				/* On redemarre au debut de Noeud car le chainage a pu changer */
				Edge = First[Noeud];
				continue; /* Pour ne pas faire Edge = Next[Edge] */
			}												
		}
		/*NextEdge:*/
    Edge = Next[Edge];
	}
  Edge = First[Complement];
  while ( Edge >= 0 ) {
		T[Adjacent[Edge]] = 0;
    Edge = Next[Edge];
	}	
}

free( T );
return;
}










