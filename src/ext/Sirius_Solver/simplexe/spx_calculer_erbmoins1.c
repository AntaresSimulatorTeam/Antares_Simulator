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

   FONCTION: Resolution du systeme transpose pour obtenir une ligne
             de l'inverse de la base    

   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"   

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"

/*----------------------------------------------------------------------------*/
/* Calcul de la ligne de B^{-1} de la variable sortante */
void SPX_CalculerErBMoins1( PROBLEME_SPX * Spx, char CalculEnHyperCreux ) 
{

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
  SPX_CalculerErBMoins1AvecBaseReduite( Spx, CalculEnHyperCreux );
}
else {
  SPX_CalculerErBMoins1AvecBaseComplete( Spx, CalculEnHyperCreux );
}
										 
return;
}
