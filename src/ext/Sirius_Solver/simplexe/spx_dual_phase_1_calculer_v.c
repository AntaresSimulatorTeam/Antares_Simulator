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

   FONCTION: Phase 1 de l'algorithme dual. 
             Calcul de V = B^{-1} * a^tilde 
               
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"

/*----------------------------------------------------------------------------*/

void SPX_DualPhase1CalculerV( PROBLEME_SPX * Spx )
{
int Var; int il; int ilMax; char Save; char SecondMembreCreux; int i;
int * NumerosDesVariablesHorsBase; char * FaisabiliteDeLaVariable; double * V;
int * Cdeb; int * CNbTerm; double * ACol; int * NumeroDeContrainte;
char TypeDEntree; char TypeDeSortie; char CalculEnHyperCreux;

NumerosDesVariablesHorsBase = Spx->NumerosDesVariablesHorsBase;
FaisabiliteDeLaVariable     = Spx->FaisabiliteDeLaVariable;
V = Spx->V;
Cdeb    = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;
ACol    = Spx->ACol;
NumeroDeContrainte = Spx->NumeroDeContrainte;

/* Calcul du vecteur ATilde */
memset( (char *) V , 0 , Spx->NombreDeContraintes * sizeof( double ) );

/* Boucle sur les variables hors base */

for ( i = 0 ; i < Spx->NombreDeVariablesHorsBase ; i++ ) {
  Var = NumerosDesVariablesHorsBase[i];
  if ( FaisabiliteDeLaVariable[Var] == DUALE_INFAISABLE_PAR_COUT_REDUIT_NEGATIF ) {
    il    = Cdeb[Var];
    ilMax = il + CNbTerm[Var];
    while ( il < ilMax ) {
      V[NumeroDeContrainte[il]]+= ACol[il];
      il++;
    }
    continue;
  }

  if ( FaisabiliteDeLaVariable[Var] == DUALE_INFAISABLE_PAR_COUT_REDUIT_POSITIF ) {
    il    = Cdeb[Var];
    ilMax = il + CNbTerm[Var];
    while ( il < ilMax ) {
      V[NumeroDeContrainte[il]]-= ACol[il];
      il++;
    }
    continue;
  }

}

TypeDEntree  = VECTEUR_LU;
TypeDeSortie = VECTEUR_LU;
CalculEnHyperCreux = NON_SPX;
Save = NON_LU;
SecondMembreCreux = OUI_LU;

SPX_ResoudreBYegalA( Spx, TypeDEntree, Spx->V, NULL, NULL, &TypeDeSortie, 
										 CalculEnHyperCreux, Save, SecondMembreCreux );

/* Traces */
/*
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  printf(" Variable en base %d V %lf ",Spx->VariableEnBaseDeLaContrainte[Cnt],Spx->V[Cnt]);
  if ( Spx->TypeDeVariable[Spx->VariableEnBaseDeLaContrainte[Cnt]] == BORNEE ) printf(" variable BORNEE\n");
  if ( Spx->TypeDeVariable[Spx->VariableEnBaseDeLaContrainte[Cnt]] == BORNEE_INFERIEUREMENT ) printf(" variable BORNEE_INFERIEUREMENT\n");
  if ( Spx->TypeDeVariable[Spx->VariableEnBaseDeLaContrainte[Cnt]] == NON_BORNEE ) printf(" variable NON_BORNEE\n");
}      
*/
/* */

return;
}



