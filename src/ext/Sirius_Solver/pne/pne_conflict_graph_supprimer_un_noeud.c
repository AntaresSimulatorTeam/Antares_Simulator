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

   FONCTION: Suppression d'un noeud dans le conflict graph.
	           On ne recupere pas la place car trop couteux en temps
						 de calcul.
                 
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
/* Suppression de l'arc partant de Nv vers Noeud */
void PNE_ConflictGraphSupprimerUnArc( int Nv, int Noeud, int * First, int * Adjacent, int * Next )
{
int PreviousEdge; int Edge; char Found;
Found = NON_PNE;
PreviousEdge = -1;
Edge = First[Nv];
while ( Edge >= 0 ) {
	if ( Adjacent[Edge] == Noeud ) {
	  if ( PreviousEdge >= 0 ) Next[PreviousEdge] = Next[Edge];
		else First[Nv] = Next[Edge];  
		Found = OUI_PNE;
    break;
	}
	PreviousEdge = Edge;
	Edge = Next[Edge];
}
if ( Found == NON_PNE ) {
  printf("BUG arc partant du noeud %d vers le noeud %d pas trouve\n",Nv,Noeud);
	exit(0);
}
return;
}

/*----------------------------------------------------------------------------*/
/* Pour chaque arc d'egalite, si un noeud est voisin de chaque extremite de l'arc,
   on peut fixer sa valeur et supprimer tous les arcs qui partent du noeud */
	 
void PNE_ConflictGraphSupprimerUnNoeud( int Noeud, int * First, int * Adjacent, int * Next )
{
int Edge; int Nv;
/*printf("Suppression noeud %d\n",Noeud);*/
Edge = First[Noeud];
while ( Edge >= 0 ) {
  Nv = Adjacent[Edge];
  PNE_ConflictGraphSupprimerUnArc( Nv, Noeud, First, Adjacent, Next );
	Edge = Next[Edge];
}
First[Noeud] = -1;
return;
}










