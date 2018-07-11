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

void SPX_CalculerPiAvecBaseComplete( PROBLEME_SPX * Spx )
{
int Cnt; double * Pi; double * C; int * VariableEnBaseDeLaContrainte;
char TypeDEntree; char TypeDeSortie; char CalculEnHyperCreux; 

Pi = Spx->Pi;
C  = Spx->C;
VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;

/* Boucle sur les variables en base */
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  Pi[Cnt] = C[VariableEnBaseDeLaContrainte[Cnt]];
}

TypeDEntree  = VECTEUR_LU;
TypeDeSortie = VECTEUR_LU;
CalculEnHyperCreux = NON_SPX;
SPX_ResoudreUBEgalC( Spx, TypeDEntree, Pi, NULL, NULL, NULL, CalculEnHyperCreux  );

return;      
}

/*----------------------------------------------------------------------------*/

