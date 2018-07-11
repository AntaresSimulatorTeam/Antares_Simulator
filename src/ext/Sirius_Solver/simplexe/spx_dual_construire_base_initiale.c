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
   FONCTION: Construction de la base dans le cas ou la base de depart 
             est fournie
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

/*----------------------------------------------------------------------------*/

void SPX_DualConstruireLaBaseInitiale( PROBLEME_SPX * Spx,
                                       int   NbVar_E,
                                       int * PositionDeLaVariable_E,
                                       int   NbVarDeBaseComplementaires_E,
                                       int * ComplementDeLaBase_E,
                                       char * TypeDeContrainte_E ) 
{
int Cnt; int Cnt_E ; int j ; int Var_E; int VarSimplexe; int il; int VarEcart;

if ( Spx->LaBaseDeDepartEstFournie == NON_SPX ) {
  printf(" Bug dans SPX_DualConstruireLaBaseInitiale, la base initiale doit etre fournie\n");
  exit(0);
}
  
/* Cas ou la base est fournie */
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ )  Spx->VariableEnBaseDeLaContrainte[Cnt] = -1;

/* Complement de base */
for ( Var_E = 0 ; Var_E < NbVarDeBaseComplementaires_E ; Var_E++ ) {
  Cnt_E = ComplementDeLaBase_E[Var_E];
  Cnt   = Spx->CorrespondanceCntEntreeCntSimplexe[Cnt_E]; 
  if ( TypeDeContrainte_E[Cnt_E] == '=' ) {
    /* Si le type de la contrainte est = on cree la variable */
    if ( Cnt >= 0 ) SPX_DualCreerVariableDeBase( Spx , Cnt );
  }
  else {
    /* Si la contrainte est de type inegalite, on peut utiliser la variable d'ecart */
    if ( Cnt >= 0 ) { 
      /* A ce stade là, la contrainte a déja été mise sous forme standard: la variable 
         d'ecart a été créee */
      il = Spx->Mdeb[Cnt] + Spx->NbTerm[Cnt] - 1;
      VarEcart = Spx->Indcol[il];
      Spx->PositionDeLaVariable        [VarEcart] = EN_BASE_LIBRE;
      Spx->ContrainteDeLaVariableEnBase[VarEcart] = Cnt; 
      Spx->VariableEnBaseDeLaContrainte[Cnt]      = VarEcart;
    }
  }
}

/* Pour les contraintes restantes */
for ( j = 0 , Var_E = 0 ; Var_E < NbVar_E ; Var_E++ ) {
  if ( PositionDeLaVariable_E[Var_E] != EN_BASE ) continue;
  VarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe[Var_E];
	if ( VarSimplexe < 0 ) continue;
  Spx->PositionDeLaVariable[VarSimplexe] = EN_BASE_LIBRE;
  while ( Spx->VariableEnBaseDeLaContrainte[j] != - 1 ) j++;
  Spx->ContrainteDeLaVariableEnBase[VarSimplexe] = j; 
  Spx->VariableEnBaseDeLaContrainte[j]           = VarSimplexe;
  j++;
}

return;
}



































