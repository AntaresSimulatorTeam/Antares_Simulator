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

   FONCTION: Controle des cliques soit avant le strong branching
	           soit avant la resolution d'un probleme relaxe sur la
						 base des variables instancees.
                 
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

void PNE_ControleCliquesAvantResolutionProblemeRelaxe( PROBLEME_PNE * Pne, int * Faisabilite )
{
double S; CLIQUES * Cliques; int c; int * First; int * NbElements; int il; int ilMax;
double * Coeff; int * Indice; int Pivot; char * LaCliqueEstDansLePool; int * NoeudDeClique;
double * Xmin; double * Xmax;

/* Remarque: je constate qu'il est tres rare que des cliques soient violees avant la resolution du noeud,
donc il n'est pas prouve que ca vaille le coup */

return;

if ( Pne->Cliques == NULL ) return;

Xmin = Pne->UminTrav;
Xmax = Pne->UmaxTrav;
Cliques = Pne->Cliques;
First = Cliques->First;  
NbElements = Cliques->NbElements;
NoeudDeClique = Cliques->Noeud;
LaCliqueEstDansLePool = Cliques->LaCliqueEstDansLePool;
Pivot = Pne->ConflictGraph->Pivot;

Coeff = Pne->Coefficient_CG;
Indice = Pne->IndiceDeLaVariable_CG;

for ( c = 0 ; c < Cliques->NombreDeCliques ; c++ ) {
  if ( LaCliqueEstDansLePool[c] == OUI_PNE ) continue;
  il = First[c];
	if ( il < 0 ) continue; /* On ne sait jamais ... */	
  ilMax = il + NbElements[c];
	S = 0;
  while ( il < ilMax ) {
	  if ( NoeudDeClique[il] < Pivot ) {
		  S += Xmin[NoeudDeClique[il]];
		}
		else {
		  S += 1 - Xmax[NoeudDeClique[il]-Pivot];
		}
	  il++;
	}	
  if ( S > 1.0001 ) {
	  /* La clique est violee des le depart */
		printf("!!!! clique violee avant la resolution du pb relaxe  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    *Faisabilite = NON_PNE;
		return;
	}
}	

printf("pas de clique violee avant la resolution du pb relaxe\n");

return;
}   















