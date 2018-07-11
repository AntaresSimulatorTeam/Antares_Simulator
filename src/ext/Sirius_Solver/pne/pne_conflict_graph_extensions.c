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

   FONCTION: Extensions du graphe de conflit: on ajoute des arcs et on
	           fixe des variables.
                 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"  
  
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
  
# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif
 
/*----------------------------------------------------------------------------*/
/* Extension du graphe du conflict graphe: pour chaque arc d'egalite, on cree
   un arc entre les voisins des extremites de chaque arc d'egalite */
	 
void PNE_ExtendConflictGraph( PROBLEME_PNE * Pne )
{
int Pivot; int NbNoeudsAExaminer; int Noeud; int Edge; int Complement;
int V_Complement; int ArcNoeud; int V_Noeud; int ArcV_Noeud; int NbEdges; 
int * Adjacent;int * Next;int * First; char ConflictGraphExtended;
CONFLICT_GRAPH * ConflictGraph;
  
if ( Pne->ConflictGraph == NULL ) return;
ConflictGraph = Pne->ConflictGraph;
if ( ConflictGraph->Full == OUI_PNE ) return;
Adjacent = ConflictGraph->Adjacent;
Next = ConflictGraph->Next;
First = ConflictGraph->First;
Pivot = ConflictGraph->Pivot;
NbNoeudsAExaminer = Pivot;

/* Pour chaque arc d'egalite, on ajoute un arc entre les paires de noeuds dont
   1 est lie a 1 noeud de l'egalite et l'autre est lie a l'autre noeud de l'egalite */
ExtendConflictGraph:
ConflictGraphExtended = NON_PNE;
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
	/* On balaye la liste des voisins de Complement */
  Edge = First[Complement];
  while ( Edge >= 0 ) {			  
		V_Complement = Adjacent[Edge];
		if ( V_Complement == Noeud ) goto NextEdge;
    /* On cree un arc entre V_Complement et tous les adjacents de Noeud */
		ArcNoeud = First[Noeud];
    while ( ArcNoeud >= 0 ) {
      V_Noeud = Adjacent[ArcNoeud];
      if ( V_Noeud == Complement || V_Noeud == V_Complement ) goto NextArcNoeud;			
		  /* Creation d'un arc entre V_Complement et V_Noeud */		
      ArcV_Noeud = First[V_Noeud];
      while ( ArcV_Noeud >= 0 ) {
        if ( Adjacent[ArcV_Noeud] == V_Complement ) goto NextArcNoeud; /* L'arc existe deja */						
	      ArcV_Noeud = Next[ArcV_Noeud];
      }
			/* V_Noeud et V_Complement ne sont pas voisins: on cree un arc */
			NbEdges = ConflictGraph->NbEdges;
	    if ( NbEdges + 2 >= ConflictGraph->TailleAllouee ) {
        PNE_ReallocConflictGraph( Pne );
        if ( ConflictGraph->Full == OUI_PNE ) { ConflictGraph->NbEdges = NbEdges; return; }
        Adjacent = ConflictGraph->Adjacent;
        Next = ConflictGraph->Next;
	    }
      /* On ajoute l'arc vers V_Complement a la liste des arc de V_Noeud */
	    Adjacent[NbEdges] = V_Complement;
      Next[NbEdges] = First[V_Noeud];
      First[V_Noeud] = NbEdges;
      NbEdges++;
      /* On ajoute l'arc vers V_Noeud a la liste des arc de V_Complement */
	    Adjacent[NbEdges] = V_Noeud;
      Next[NbEdges] = First[V_Complement];
      First[V_Complement] = NbEdges;
      NbEdges++;	          
      ConflictGraph->NbEdges = NbEdges;
			/* MAJ de l'indicateur d'extension */	
			ConflictGraphExtended = OUI_PNE;			
			NextArcNoeud:
	    ArcNoeud = Next[ArcNoeud];
		}
		NextEdge:
		Edge = Next[Edge];
  }
}

if ( ConflictGraphExtended == OUI_PNE ) {
  /*printf("Le graphe de conflit a ete etendu \n");*/
  goto ExtendConflictGraph;
}

return;
}









