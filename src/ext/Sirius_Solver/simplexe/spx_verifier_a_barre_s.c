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

   FONCTION: Verification de A_BARRE_S = B-1 * AS
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h" 

# define CYCLE_DE_VERIFICATION_DE_ABARRES              10  
# define CYCLE_DE_VERIFICATION_DE_ABARRES_HYPER_CREUX  20

/*----------------------------------------------------------------------------*/
/*            Verification du calcul de ABarreS pour savoir s'il faut
              refactoriser la base: on verifie sur k contraintes tirees
              au hasard  		         			      */

void SPX_VerifierABarreS( PROBLEME_SPX * Spx )
{

if ( Spx->TypeDeStockageDeABarreS == COMPACT_SPX ) {
  if ( Spx->Iteration % CYCLE_DE_VERIFICATION_DE_ABARRES_HYPER_CREUX != 0 ) return;
}
else {
  if ( Spx->Iteration % CYCLE_DE_VERIFICATION_DE_ABARRES != 0 ) return;
}

/*		   
printf("Verification de ABarreS dans SPX_VerifierABarreS iteration %d\n",Spx->Iteration); 
*/

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
  SPX_VerifierABarreSAvecBaseReduite( Spx );
}
else {
  SPX_VerifierABarreSAvecBaseComplete( Spx );
}

return;
}

