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

void SPX_CalculerLesCoutsReduitsAvecBaseComplete( PROBLEME_SPX * Spx )
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
