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

   FONCTION: Variable probing
                
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

# define VERBOSE_CREATION_GRAPHE NON_PNE
# define AJOUTER_COMPLEMENT 1 /* Vaut imperativement 1 */

/*----------------------------------------------------------------------------*/
/* Mise a jour du conflict graph */
void PNE_MajConflictGraph( PROBLEME_PNE * Pne, int VariableInstanciee, double ValeurDeVariableInstanciee )
{
int i; int VariableFixee; int * NumeroDesVariablesFixees; int Edge; int * Adjacent;
int * Next; int * First; int NbEdges; char * BorneInfConnue; char Exist;
CONFLICT_GRAPH * ConflictGraph; PROBING_OU_NODE_PRESOLVE * Prb; int Complement;
int Noeud_1; int Noeud_2; int Pivot;

/* Attention on ne doit creer le complement que s'il n'existe pas. S'il existe il faut verifier que la liaison
 egalite existe */
 
Prb = Pne->ProbingOuNodePresolve;
if ( Pne->ConflictGraph == NULL ) PNE_AllocConflictGraph( Pne );
ConflictGraph = Pne->ConflictGraph;
if ( ConflictGraph == NULL ) return;
if ( ConflictGraph->Full == OUI_PNE ) return;

NumeroDesVariablesFixees = Prb->NumeroDesVariablesFixees;
BorneInfConnue = Prb->BorneInfConnue;
Adjacent = ConflictGraph->Adjacent;
Next = ConflictGraph->Next;
First = ConflictGraph->First;
NbEdges = ConflictGraph->NbEdges;
Pivot = ConflictGraph->Pivot;

# if VERBOSE_CREATION_GRAPHE == OUI_PNE
  printf("Instanciation de la variable %d a %e\n",VariableInstanciee,ValeurDeVariableInstanciee);
# endif

if ( ValeurDeVariableInstanciee == 0.0 ) {
  Noeud_1 = Pivot + VariableInstanciee;
  Complement = VariableInstanciee;
}
else {
  Noeud_1 = VariableInstanciee;
  Complement = Pivot + VariableInstanciee;
}

/* Si on examine VariableInstanciee pour la premiere fois on cree la liaison avec son complement */

if ( First[Noeud_1] == -1 && AJOUTER_COMPLEMENT == 1 ) {
	if ( NbEdges >= ConflictGraph->TailleAllouee ) {
    PNE_ReallocConflictGraph( Pne );
    if ( ConflictGraph->Full == OUI_PNE ) { ConflictGraph->NbEdges = NbEdges; return; }
    Adjacent = ConflictGraph->Adjacent;
    Next = ConflictGraph->Next;
	}			
	Adjacent[NbEdges] = Complement;
  Next[NbEdges] = First[Noeud_1];
  First[Noeud_1] = NbEdges;
  NbEdges++;
}
if ( First[Complement] == -1 && AJOUTER_COMPLEMENT == 1 ) {
	if ( NbEdges >= ConflictGraph->TailleAllouee ) {
    PNE_ReallocConflictGraph( Pne );
    if ( ConflictGraph->Full == OUI_PNE ) { ConflictGraph->NbEdges = NbEdges; return; }
    Adjacent = ConflictGraph->Adjacent;
    Next = ConflictGraph->Next;
	}			
	Adjacent[NbEdges] = Noeud_1;
  Next[NbEdges] = First[Complement];
  First[Complement] = NbEdges;
  NbEdges++;
}

for ( i = 0 ; i < Prb->NombreDeVariablesFixees ; i++ ) {
  # if VERBOSE_CREATION_GRAPHE == OUI_PNE
    printf("Variables fixee % d a %e\n",NumeroDesVariablesFixees[i],Prb->ValeurDeBorneInf[NumeroDesVariablesFixees[i]]);
  # endif
	/* les variables a 1 du graphe vont de 0 a Pivot-1, les variables a 0 vont de Pivot
	   a 2*Pivot -1 . Remarque on pourra avantageusement reduire la taille en indicant
		 sur les variables eniteres uniquement */
		 
	VariableFixee = NumeroDesVariablesFixees[i];
	if ( VariableFixee == VariableInstanciee ) continue;
	
	if ( BorneInfConnue[VariableFixee] == FIXATION_SUR_BORNE_SUP ) {
	  /* Le lien de conflit ira vers la valeur 0 */
    Noeud_2 = Pivot + VariableFixee;
		Complement = VariableFixee;
	}
	else if ( BorneInfConnue[VariableFixee] == FIXATION_SUR_BORNE_INF ) {
	  /* Le lien de conflit ira vers la valeur 1 */
    Noeud_2 = VariableFixee;
		Complement = Pivot + VariableFixee;
	}
	else {
    printf("Bug dans PNE_MajConflictGraph: indicateur BorneInfConnue faux\n");
		continue;
	}
		
  /* Si on examine VariableFixee pour la premiere fois on cree la liaison avec son complement */
	if ( First[Noeud_2] == -1 && AJOUTER_COMPLEMENT == 1 ) {			
	  if ( NbEdges >= ConflictGraph->TailleAllouee ) {
      PNE_ReallocConflictGraph( Pne );
      if ( ConflictGraph->Full == OUI_PNE ) { ConflictGraph->NbEdges = NbEdges; return; }
      Adjacent = ConflictGraph->Adjacent;
      Next = ConflictGraph->Next;
	  }		
	  Adjacent[NbEdges] = Complement;
    Next[NbEdges] = First[Noeud_2];
    First[Noeud_2] = NbEdges;
    NbEdges++;
	}
  if ( First[Complement] == -1 && AJOUTER_COMPLEMENT == 1 ) {
	  if ( NbEdges >= ConflictGraph->TailleAllouee ) {
      PNE_ReallocConflictGraph( Pne );
      if ( ConflictGraph->Full == OUI_PNE ) { ConflictGraph->NbEdges = NbEdges; return; }
      Adjacent = ConflictGraph->Adjacent;
      Next = ConflictGraph->Next;
	  }			
	  Adjacent[NbEdges] = Noeud_2;
    Next[NbEdges] = First[Complement];
    First[Complement] = NbEdges;
    NbEdges++;
  }
				
  /* On veut ajouter l'arc a la liste des arc de VariableInstanciee */
  /* On verifie que l'arc n'existe pas deja */
	Exist = 0;
  Edge = First[Noeud_1];
  while ( Edge >= 0 ) {
		/* L'arc existe deja ? */
    if ( Adjacent[Edge] == Noeud_2 ) {
		  /* L'arc existe deja */
			Exist = 1;
		  break;
		}
	  Edge = Next[Edge];
  }
	if ( Exist == 0 ) {
	  /* Il faut verifier s'il y a de la place */
	  if ( NbEdges >= ConflictGraph->TailleAllouee ) {
      PNE_ReallocConflictGraph( Pne );
      if ( ConflictGraph->Full == OUI_PNE ) { ConflictGraph->NbEdges = NbEdges; return; }
      Adjacent = ConflictGraph->Adjacent;
      Next = ConflictGraph->Next;
	  }			
    /* On ajoute l'arc a la liste des arc de VariableInstanciee */		
	  Adjacent[NbEdges] = Noeud_2;
    Next[NbEdges] = First[Noeud_1];
    First[Noeud_1] = NbEdges;
    NbEdges++;
	}

  /* On veut ajouter l'arc a la liste des arc de VariableFixee */
	Exist = 0;
  Edge = First[Noeud_2];
  while ( Edge >= 0 ) {
		/* L'arc existe deja ? */
    if ( Adjacent[Edge] == Noeud_1 ) {
		  /* L'arc existe deja */
			Exist = 1;
		  break;
		}
	  Edge = Next[Edge];
  }
	if ( Exist == 0 ) {
	  /* Il faut verifier s'il y a de la place */
	  if ( NbEdges >= ConflictGraph->TailleAllouee ) {
      PNE_ReallocConflictGraph( Pne );
      if ( ConflictGraph->Full == OUI_PNE ) { ConflictGraph->NbEdges = NbEdges; return; }
      Adjacent = ConflictGraph->Adjacent;
      Next = ConflictGraph->Next;
	  }			
    /* On ajoute l'arc a la liste des arc de VariableFixee */		
	  Adjacent[NbEdges] = Noeud_1;
    Next[NbEdges] = First[Noeud_2];
    First[Noeud_2] = NbEdges;
    NbEdges++;
	}
}
ConflictGraph->NbEdges = NbEdges;

return;
}
















