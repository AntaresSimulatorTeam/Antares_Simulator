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

   FONCTION: Verification de vecteurs de travail. Utilise en mode debug
	           pour verifier qu'on a bien remis la bonne valeur dans tous les
						 vecteurs de travail. 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"  

# include "spx_fonctions.h"
# include "spx_define.h"

/*----------------------------------------------------------------------------*/

void SPX_VerifierLesVecteursDeTravail( PROBLEME_SPX * Spx )
{
int i;

for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) {
  if ( Spx->T[i] != -1 ) {
	  printf("SPX_VerifierLesVecteursDeTravail: T incorrect\n");
		exit(0);
	}
}
for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) {
  if ( Spx->Marqueur[i] != -1 ) {
	  printf("SPX_VerifierLesVecteursDeTravail: Marqueur incorrect Marqueur[%d] = %d\n",i,Spx->Marqueur[i]);
		exit(0);
	}	
  if ( Spx->AReduit[i] != 0 ) {
	  printf("SPX_VerifierLesVecteursDeTravail: AReduit incorrect AReduit[%d] = %e\n",i,Spx->AReduit[i]);
		exit(0);
	}	
  if ( Spx->Tau[i] != 0 ) {
    printf("SPX_VerifierLesVecteursDeTravail: Tau incorrect Tau[%d] = %e\n",i,Spx->Tau[i]);
    exit(0);
  }	
}

return;
}
