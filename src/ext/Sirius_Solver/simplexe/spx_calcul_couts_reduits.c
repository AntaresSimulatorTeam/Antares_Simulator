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

   FONCTION: Calcul des couts reduits des variables hors base 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_define.h"
# include "spx_fonctions.h"

/*----------------------------------------------------------------------------*/

void SPX_CalculerLesCoutsReduits( PROBLEME_SPX * Spx )
{
int Var; int il; int ilMax; double S; int i; int * NumerosDesVariablesHorsBase;
int * Cdeb; int * CNbTerm; int * NumeroDeContrainte; double * Pi; double * ACol;
double * C; double * CBarre; 

NumerosDesVariablesHorsBase = Spx->NumerosDesVariablesHorsBase;
Cdeb    = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;
Pi      = Spx->Pi;
ACol    = Spx->ACol;
CBarre  = Spx->CBarre;
C       = Spx->C;
NumeroDeContrainte = Spx->NumeroDeContrainte;

/* Boucle sur les variables hors base */

for ( i = 0 ; i < Spx->NombreDeVariablesHorsBase ; i++ ) {
  Var = NumerosDesVariablesHorsBase[i];
  /* Utilisation du chainage de la transposee de la matrice 
     des contraintes */
  il    = Cdeb[Var];
  ilMax = il + CNbTerm[Var];
  S     = 0.;
  while ( il < ilMax ) {
    S += Pi[NumeroDeContrainte[il]] * ACol[il];
    il++;
  }
  CBarre[Var] = C[Var] - S;	
}

Spx->CBarreAEteCalculeParMiseAJour = NON_SPX;

return;
}

/*----------------------------------------------------------------------------*/
/*                    Cas de la mise a jour de CBarre                         */

void SPX_MettreAJourLesCoutsReduits( PROBLEME_SPX * Spx )
{
double X; int i; double * CBarre; double * NBarreR; int * NumVarNBarreRNonNul;
int * NumerosDesVariablesHorsBase; int VariableEntrante; int VariableSortante;
int Var;

CBarre = Spx->CBarre;
NumerosDesVariablesHorsBase = Spx->NumerosDesVariablesHorsBase;
NBarreR                     = Spx->NBarreR;  

VariableEntrante = Spx->VariableEntrante;
VariableSortante = Spx->VariableSortante;

/* Pour etre coherent il vaut mieux prendre NBarreR car c'est ce qui a servi au test du ratio */
X = -CBarre[VariableEntrante] / NBarreR[VariableEntrante];

if ( X == 0.0 ) goto FinMajCoutsReduits;

/* Boucle sur les variables hors base */
if ( Spx->TypeDeStockageDeNBarreR == ADRESSAGE_INDIRECT_SPX ) {
	NumVarNBarreRNonNul = Spx->NumVarNBarreRNonNul;
  for ( i = 0 ; i < Spx->NombreDeValeursNonNullesDeNBarreR ; i++ ) {
	  Var = NumVarNBarreRNonNul[i];				
    CBarre[Var]+= X * NBarreR[Var];				
	}
}
else {
  for ( i = 0 ; i < Spx->NombreDeVariablesHorsBase ; i++ ) {
	  Var = NumerosDesVariablesHorsBase[i];
    if ( NBarreR[Var] != 0.0 ) {		
      CBarre[Var]+= X * NBarreR[Var];  						
	  }		
  }
}

FinMajCoutsReduits:

CBarre[VariableSortante] = X;
CBarre[VariableEntrante] = 0.;

Spx->CBarreAEteCalculeParMiseAJour = OUI_SPX;

return;
}
