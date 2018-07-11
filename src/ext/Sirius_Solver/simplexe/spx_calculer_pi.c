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

   FONCTION:   Calcul de Pi = c_B * B^{-1} c'est a dire              
               resolution de u B = c

   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"

/*----------------------------------------------------------------------------*/

void SPX_CalculerPi( PROBLEME_SPX * Spx )
{

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
  SPX_CalculerPiAvecBaseReduite( Spx );
}
else {
  SPX_CalculerPiAvecBaseComplete( Spx );
}

return;  
}

/*----------------------------------------------------------------------------*/
/*                        Mise a jour de Pi                                   */
							       
void SPX_MettreAJourPi( /* PROBLEME_SPX * Spx */ )
{
/* A faire dans le futur car cela n'a pas grand interet */

return;
}
