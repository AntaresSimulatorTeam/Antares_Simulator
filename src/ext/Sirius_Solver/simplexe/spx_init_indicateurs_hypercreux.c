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

   FONCTION: Initialisation des inticateurs hyper creux
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"
  
/*----------------------------------------------------------------------------*/

void SPX_InitialiserLesIndicateursHyperCreux( PROBLEME_SPX * Spx )
{

Spx->CalculErBMoinsUnEnHyperCreux       = OUI_SPX;
Spx->CalculErBMoinsEnHyperCreuxPossible = OUI_SPX;
Spx->CountEchecsErBMoins                = 0;                
Spx->AvertissementsEchecsErBMoins       = 0;      
Spx->NbEchecsErBMoins                   = 0;

Spx->CalculABarreSEnHyperCreux         = OUI_SPX;
Spx->CalculABarreSEnHyperCreuxPossible = OUI_SPX;
Spx->CountEchecsABarreS                = 0;                
Spx->AvertissementsEchecsABarreS       = 0;    
Spx->NbEchecsABarreS                   = 0;

return;
}
