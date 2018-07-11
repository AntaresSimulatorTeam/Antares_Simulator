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
/***********************************************************************************

   FONCTION:    

                     
   AUTEUR: R. GONZALEZ

************************************************************************************/

# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

/*------------------------------------------------------------------------*/
/*                     Initialisation des variables                       */

void PI_InitXS( PROBLEME_PI * Pi )     
{
int Var; double Div;

for ( Var = 0 ; Var < Pi->NombreDeVariables ; Var++ ) {

  Pi->Qpar2[Var] = 2. * Pi->Q[Var];

  Pi->UkMoinsUmin[Var] = Pi->U[Var]    - Pi->Umin[Var];
  Pi->UmaxMoinsUk[Var] = Pi->Umax[Var] - Pi->U[Var]; 

  Div = Pi->UkMoinsUmin[Var];
  if ( Div < ZERO ) Div = ZERO;
  Pi->UnSurUkMoinsUmin[Var] = 1. / Div;
  Div = Pi->UmaxMoinsUk[Var];
  if ( Div < ZERO ) Div = ZERO;
  Pi->UnSurUmaxMoinsUk[Var] = 1. / Div;

  if ( Pi->TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {
    Pi->UnSurUmaxMoinsUk[Var] = 0.;
    Pi->UmaxMoinsUk     [Var] = 0.;     
  }
  else if ( Pi->TypeDeVariable[Var] == BORNEE_SUPERIEUREMENT ) {
    Pi->UnSurUkMoinsUmin[Var] = 0.;
    Pi->UkMoinsUmin     [Var] = 0.; 
  }
  else if ( Pi->TypeDeVariable[Var] == NON_BORNEE ) {
    Pi->UnSurUkMoinsUmin[Var] = 0.;
    Pi->UnSurUmaxMoinsUk[Var] = 0.;
    Pi->UkMoinsUmin     [Var] = 0.; 
    Pi->UmaxMoinsUk     [Var] = 0.; 
  }
  
}

return;
}

