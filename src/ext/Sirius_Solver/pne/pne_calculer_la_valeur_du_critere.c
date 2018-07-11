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

   FONCTION: Calcul du critere.
                            
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"
							     
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"

/*----------------------------------------------------------------------------*/

void PNE_CalculerLaValeurDuCritere( PROBLEME_PNE * Pne )
{
int i ; 

/* Calcul du critere */
Pne->Critere = Pne->Z0;

for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) Pne->Critere+= Pne->LTrav[i] * Pne->UTrav[i];
 
#if VERBOSE_PNE
  printf(" Valeur du critere apres optimisation du probleme relaxe: %lf \n",Pne->Critere);
  printf(" ********************************************************     \n");
#endif

if ( Pne->NombreDeCoupesCalculees == 0 ) Pne->ValeurOptimaleDuProblemeCourantAvantNouvellesCoupes = Pne->Critere; 

return;
}

