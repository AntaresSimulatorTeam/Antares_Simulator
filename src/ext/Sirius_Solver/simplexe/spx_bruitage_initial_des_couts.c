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

   FONCTION: Bruitage intial des couts
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "pne_fonctions.h"

/*----------------------------------------------------------------------------*/

void SPX_BruitageInitialDesCouts( PROBLEME_SPX * Spx )
{
int Var; double Random; double RandomMax; double RandomMin; double * C; char * TypeDeVariable;
double * SeuilDAmissibiliteDuale; double CoutMoyen; int NbCoutsNonNuls; double RdSup;
double EcartMoyen; double * Xmin; double * Xmax; 

C = Spx->C;
TypeDeVariable = Spx->TypeDeVariable;
Xmin = Spx->Xmin;
Xmax = Spx->Xmax;
CoutMoyen = 0;
EcartMoyen = 0;
NbCoutsNonNuls = 0;
Spx->CoutMoyen = 0;
Spx->EcartDeBornesMoyen = 0;
Spx->PerturbationMax = 10;
/* Calcul du cout moyen sur les couts non nuls */
for ( Var = 0 ; Var < Spx->NombreDeVariablesNatives ; Var++ ) {
  if ( TypeDeVariable[Var] == NON_BORNEE ) continue;
  if ( C[Var] != 0 ) {
    CoutMoyen += fabs( C[Var] );  
		NbCoutsNonNuls++;
		if ( TypeDeVariable[Var] == BORNEE ) EcartMoyen += Xmax[Var] - Xmin[Var];
		else EcartMoyen += 1000;
	}
}
if ( NbCoutsNonNuls != 0 ) {
  CoutMoyen /= NbCoutsNonNuls;
  Spx->CoutMoyen = CoutMoyen;
  EcartMoyen /= NbCoutsNonNuls;
  Spx->EcartDeBornesMoyen = EcartMoyen;
	Spx->PerturbationMax = 0.0001 * Spx->CoutMoyen * Spx->EcartDeBornesMoyen;
}

# if FAIRE_UN_BRUITAGE_INITIAL_DES_COUTS != OUI_SPX
  return;
# endif

SeuilDAmissibiliteDuale = Spx->SeuilDAmissibiliteDuale1;

Spx->LesCoutsOntEteModifies = OUI_SPX;

RdSup = SEUIL_ADMISSIBILITE_DUALE_2;

for ( Var = 0 ; Var < Spx->NombreDeVariablesNatives ; Var++ ) {
	if ( C[Var] == 0 ) continue;
  if ( TypeDeVariable[Var] == NON_BORNEE ) continue;
  Spx->A1 = PNE_Rand( Spx->A1 ); /* Nombre aleatoire entre 0 et 1 */
	
  RandomMax = SeuilDAmissibiliteDuale[Var];	
	if ( RandomMax > RdSup ) RandomMax = RdSup;
	
  RandomMin = -RandomMax;
	
  Random = RandomMin;
  Random += Spx->A1 * ( RandomMax - RandomMin );
	
	C[Var] += Random;
}

return;
}
