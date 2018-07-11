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

   FONCTION: Determination des variables a instancier.
             SP appele par la partie Branch and Bound.
                            
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"
							     
# include "pne_fonctions.h"
# include "pne_define.h"

/*----------------------------------------------------------------------------*/

void PNE_ChoixDesVariablesAInstancier( PROBLEME_PNE * Pne,
                                       int *  ValeurDInstanciationAGauche,
                                       int *  NombreDeVariablesAInstancierAGauche,
                                       int ** NumerosDesVariablesAInstancierAGauche,
				                               int *  ValeurDInstanciationADroite,
                                       int *  NombreDeVariablesAInstancierADroite,
                                       int ** NumerosDesVariablesAInstancierADroite
				     )
{

/* S'il y a des Gub on les prend en priorite */
if ( Pne->NbVarGauche <= 0 || Pne->NbVarDroite <= 0 ) {
   Pne->ValeurAGauche = 0;
   Pne->NbVarGauche   = 1;
   Pne->PaquetDeGauche[0] = Pne->VariableLaPlusFractionnaire;
   Pne->ValeurADroite = 1;
   Pne->NbVarDroite   = 1;
   Pne->PaquetDeDroite[0] = Pne->VariableLaPlusFractionnaire; 
}

*ValeurDInstanciationAGauche           = Pne->ValeurAGauche;
*NombreDeVariablesAInstancierAGauche   = Pne->NbVarGauche;
*NumerosDesVariablesAInstancierAGauche = Pne->PaquetDeGauche;

*ValeurDInstanciationADroite           = Pne->ValeurADroite;
*NombreDeVariablesAInstancierADroite   = Pne->NbVarDroite;
*NumerosDesVariablesAInstancierADroite = Pne->PaquetDeDroite;

return;
}
