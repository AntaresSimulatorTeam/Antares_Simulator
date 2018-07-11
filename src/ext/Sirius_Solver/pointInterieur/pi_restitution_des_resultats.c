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
/*        Restitution des resultats dans la numerotation d'entree         */ 

void PI_RestitutionDesResultats( PROBLEME_PI * Pi,
                                 int     NombreDeVariables_E   , 
                                 double * U_E                   , 
                                 double * S1_E                  , 
                                 double * S2_E                  , 
				 int     NombreDeContraintes_E ,
                                 double * VariablesDualesDeContraintes )
{
int i; int Var_E; int Cnt_E;  

for ( i = 0 ; i < NombreDeVariables_E ; i++ ) {
  Var_E = Pi->CorrespondanceVarEntreeVarPi[i];
  if ( Var_E >= 0 ) {
    U_E[i]  = Pi->U[Var_E];
    /*
    S1_E[i] = Pi->S1[Var_E];
    S2_E[i] = Pi->S2[Var_E];
    */
  }
}


for ( i = 0 ; i <  NombreDeContraintes_E ; i++ ) {
  VariablesDualesDeContraintes[i] = 0;
  Cnt_E = Pi->CorrespondanceCntEntreeCntPi[i];
  if ( Cnt_E >= 0 ) {
     VariablesDualesDeContraintes[i] = Cnt_E;
  }				  
}

return;
}	       

