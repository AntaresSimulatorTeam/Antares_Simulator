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

   FONCTION: On essaie de reinitialiser les couts natifs sur les variables
	           hors base si cela ne cree pas d'infaisabilite duale.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "pne_fonctions.h"

# define ITERATIONS_MIN 500
# define ITERATIONS_MAX 1500

# define TRACES 1

/*----------------------------------------------------------------------------*/

void SPX_ReinitialiserLesCoutsNatifSiCestPossible( PROBLEME_SPX * Spx )
{
int Var; int i; char * TypeDeVariable; double * CBarre; char * PositionDeLaVariable;
double * C; double * Csv; char * CorrectionDuale; double XCBarre; int NombreDeCoutsBruite;
int NombreDeCoutesRestaures; int * NumerosDesVariablesHorsBase; double X;
double * SeuilDAmissibiliteDuale;
# if TRACES == 1
  double VolumeDeBruitage;
# endif

return;

if ( Spx->Iteration < Spx->ProchaineIterationDeReinitDesCouts ) return;

Spx->A1 = PNE_Rand( Spx->A1 ); /* Nombre aleatoire entre 0 et 1 */
X = ITERATIONS_MIN;
X += Spx->A1 * ( ITERATIONS_MAX - ITERATIONS_MIN );
Spx->ProchaineIterationDeReinitDesCouts = (int) ceil( X ) + Spx->Iteration;

if ( Spx->CBarreAEteCalculeParMiseAJour == OUI_SPX ) {
  SPX_CalculerPi( Spx );              /* Calcul de Pi = c_B * B^{-1} */
  SPX_CalculerLesCoutsReduits( Spx ); /* Calcul de CBarre = c_N - < Pi , N > */ 
  Spx->CalculerCBarre = NON_SPX; 		
}

NumerosDesVariablesHorsBase = Spx->NumerosDesVariablesHorsBase;
TypeDeVariable              = Spx->TypeDeVariable;
CBarre                      = Spx->CBarre;
PositionDeLaVariable        = Spx->PositionDeLaVariable;
C                           = Spx->C;
Csv                         = Spx->Csv;
CorrectionDuale             = Spx->CorrectionDuale;

SeuilDAmissibiliteDuale = Spx->SeuilDAmissibiliteDuale1;

NombreDeCoutsBruite = 0;
NombreDeCoutesRestaures = 0;
# if TRACES == 1
  VolumeDeBruitage = 0.0;
# endif

/* Examen des couts reduits pour chaque type de variable */
for ( i = 0 ; i < Spx->NombreDeVariablesHorsBase ; i++ ) {
  Var = NumerosDesVariablesHorsBase[i];
	if ( C[Var] == Csv[Var] ) continue;
  NombreDeCoutsBruite++;
  # if TRACES == 1
	  VolumeDeBruitage += fabs( Csv[Var] - C[Var] );
  # endif
  XCBarre = CBarre[Var] - C[Var] + Csv[Var];	
  if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF ) {
		if ( XCBarre >= -SeuilDAmissibiliteDuale[Var] ) {
      # if TRACES == 1
			  VolumeDeBruitage -= fabs( Csv[Var] - C[Var] );
      # endif
      CBarre[Var] = XCBarre;
		  C[Var] = Csv[Var];
			CorrectionDuale[Var] = NOMBRE_MAX_DE_PERTURBATIONS_DE_COUT;
			NombreDeCoutesRestaures++;
		}
	}
  else if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) {
		if ( XCBarre <= SeuilDAmissibiliteDuale[Var] ) {
      # if TRACES == 1
			  VolumeDeBruitage -= fabs( Csv[Var] - C[Var] );
      # endif
      CBarre[Var] = XCBarre;
		  C[Var] = Csv[Var];
			CorrectionDuale[Var] = NOMBRE_MAX_DE_PERTURBATIONS_DE_COUT;
			NombreDeCoutesRestaures++;
		}
	}
	else if ( PositionDeLaVariable[Var] == HORS_BASE_A_ZERO ) {	
		if ( fabs( XCBarre ) <=  SEUIL_ADMISSIBILITE_DUALE_2 ) {
      # if TRACES == 1
			  VolumeDeBruitage -= fabs( Csv[Var] - C[Var] );
      # endif
      CBarre[Var] = XCBarre;
		  C[Var] = Csv[Var];
			CorrectionDuale[Var] = NOMBRE_MAX_DE_PERTURBATIONS_DE_COUT;
			NombreDeCoutesRestaures++;
		}
	}
}

#if TRACES == 1
  if ( NombreDeCoutesRestaures > 0 ) {
	  printf("Iteration %d , nombre de couts restaures: %d sur %d , restent %d couts bruites , volume de bruitage %e\n",Spx->Iteration,NombreDeCoutesRestaures,NombreDeCoutsBruite,
		        NombreDeCoutsBruite-NombreDeCoutesRestaures,VolumeDeBruitage);		
	}
#endif
		
return;
}


