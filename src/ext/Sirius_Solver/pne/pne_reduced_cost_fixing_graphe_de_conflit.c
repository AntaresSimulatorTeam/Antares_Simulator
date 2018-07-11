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

   FONCTION: On applique le graphe de conflit quand on simule l'instanciation
	           d'une variable dans le reduced cost fixing.
                
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

void PNE_ReducedCostFixingConflictGraph( PROBLEME_PNE * Pne, int VariableFixee,
                                         double ValeurVariableFixee,
																				 double * DeltaCout, double Delta, double * CoutsReduits,  
																				 double * Xmin, double * Xmax,
																				 int * PositionDeLaVariable, int * Nb, int * T, int * Num,
																				 int * Arret )
{
int Pivot; int Noeud; int Edge; int Complement; int Nv; int * Adjacent; int * Next;
int * First; CONFLICT_GRAPH * ConflictGraph;

if ( T[VariableFixee] == 1 ) return;

if ( ValeurVariableFixee == 1 ) {
  if( PositionDeLaVariable[VariableFixee] == HORS_BASE_SUR_BORNE_INF ) {
    *DeltaCout = *DeltaCout + fabs( CoutsReduits[VariableFixee] * ( Xmax[VariableFixee] - Xmin[VariableFixee] ) );
	}
}
else {
  if( PositionDeLaVariable[VariableFixee] == HORS_BASE_SUR_BORNE_SUP ) {
    *DeltaCout = *DeltaCout + fabs( CoutsReduits[VariableFixee] * ( Xmax[VariableFixee] - Xmin[VariableFixee] ) );
	}
}

if ( *DeltaCout > Delta ) {
  *DeltaCout = *DeltaCout + 1.e-8; 
	*Arret = OUI_PNE;
  return;
}

Num[*Nb] = VariableFixee;
*Nb = *Nb + 1;
T[VariableFixee] = 1;

ConflictGraph = Pne->ConflictGraph;
if ( ConflictGraph == NULL ) return;
Adjacent = ConflictGraph->Adjacent;
Next = ConflictGraph->Next;
First = ConflictGraph->First;
Pivot = ConflictGraph->Pivot;

Noeud = VariableFixee;
Complement = VariableFixee + Pivot;
if ( ValeurVariableFixee == 0.0 ) {
  Noeud = VariableFixee + Pivot;
  Complement = VariableFixee;
}

if ( First[Noeud] < 0 ) return;

Edge = First[Noeud];
while ( Edge >= 0 ) {
  Nv = Adjacent[Edge];
	if ( Nv == Complement ) goto NextEdge;
	if ( Nv < Pivot ) {
	  /* La valeur 1 de Nv est interdite on doit la passer a 0 et on regarde son impact avec le cout reduit */
	  VariableFixee = Nv;
    ValeurVariableFixee = Xmin[VariableFixee];		
    PNE_ReducedCostFixingConflictGraph( Pne, VariableFixee, ValeurVariableFixee, DeltaCout, Delta, CoutsReduits, Xmin, Xmax,
		                                    PositionDeLaVariable, Nb, T, Num, Arret );
	  if ( *Arret == OUI_PNE ) break;
	}
	else {
	  /* La valeur 0 de Nv est interdite on doit la passer a 1 et on regarde son impact avec le cout reduit */
    VariableFixee = Nv - Pivot;	
    ValeurVariableFixee = Xmax[VariableFixee];		
    PNE_ReducedCostFixingConflictGraph( Pne, VariableFixee, ValeurVariableFixee, DeltaCout, Delta, CoutsReduits, Xmin, Xmax,
		                                    PositionDeLaVariable, Nb, T, Num, Arret );
	  if ( *Arret == OUI_PNE ) break;
	}	
  NextEdge:
  Edge = Next[Edge];
}

return;
}
