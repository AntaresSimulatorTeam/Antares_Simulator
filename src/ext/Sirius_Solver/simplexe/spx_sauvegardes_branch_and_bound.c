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

   FONCTION: Translater les bornes en sortie pour tout remettre     
             en ordre
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

/*----------------------------------------------------------------------------*/

void SPX_SauvegardesBranchAndBoundAndCut( PROBLEME_SPX * Spx )
{
	int Cnt; int Var; /*int i;*/ 

/* Sauvegardes pour le strong branching */

for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
  Spx->XSV                           [Var] = Spx->X[Var];
  Spx->PositionDeLaVariableSV        [Var] = Spx->PositionDeLaVariable[Var];
  Spx->CBarreSV                      [Var] = Spx->CBarre[Var];    
  Spx->InDualFrameworkSV             [Var] = Spx->InDualFramework[Var];  
  Spx->ContrainteDeLaVariableEnBaseSV[Var] = Spx->ContrainteDeLaVariableEnBase[Var];
}

for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {   
  Spx->BBarreSV                      [Cnt] = Spx->BBarre[Cnt]; 
  Spx->DualPoidsSV                   [Cnt] = Spx->DualPoids[Cnt]; 
  Spx->VariableEnBaseDeLaContrainteSV[Cnt] = Spx->VariableEnBaseDeLaContrainte[Cnt];
  Spx->CdebBaseSV                    [Cnt] = Spx->CdebBase[Cnt];
  Spx->NbTermesDesColonnesDeLaBaseSV [Cnt] = Spx->NbTermesDesColonnesDeLaBase[Cnt]; 
}

return;
}



















