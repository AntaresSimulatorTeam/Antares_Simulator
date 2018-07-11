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

   FONCTION: Apres la resolution d'un probleme relaxe, ce sous-programme 
   initialise le numero de la prochaine variable a instancier dans les 
   2 noeuds fils.
         
   AUTEUR: R. GONZALEZ

************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "pne_fonctions.h"

/*---------------------------------------------------------------------------------------------------------*/

int BB_ChoixDesVariablesAInstancier( BB * Bb,
				      NOEUD *  NoeudCourant,
                                      int *   ValeurDInstanciationAGauche,
                                      int *   NombreDeVariablesAInstancierAGauche,
                                      int **  NumerosDesVariablesAInstancierAGauche,
				      int *   ValeurDInstanciationADroite,
                                      int *   NombreDeVariablesAInstancierADroite,
                                      int **  NumerosDesVariablesAInstancierADroite
                                    )
{
int CodeRetour; PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Bb->ProblemePneDuBb;

CodeRetour = PAS_DE_VARIABLES_A_INSTANCIER;

if( NoeudCourant->LaSolutionRelaxeeEstEntiere == OUI ) {
  CodeRetour = PAS_DE_VARIABLES_A_INSTANCIER;
  return( CodeRetour );
}
 
if( NoeudCourant->NoeudTerminal == OUI ) {
  CodeRetour = PAS_DE_VARIABLES_A_INSTANCIER;
  return( CodeRetour );
}
   
/* Si le noeud est A_REJETER, il n'y a rien a instancier */
if ( NoeudCourant->StatutDuNoeud == A_REJETER ) {
  CodeRetour = PAS_DE_VARIABLES_A_INSTANCIER;
  return( CodeRetour );
}

/* Appel de la partie PNE: on recupere soit une variable, soit une Gub */
PNE_ChoixDesVariablesAInstancier( Pne,
                                  ValeurDInstanciationAGauche,
                                  NombreDeVariablesAInstancierAGauche,
                                  NumerosDesVariablesAInstancierAGauche,
				                          ValeurDInstanciationADroite,
                                  NombreDeVariablesAInstancierADroite,
                                  NumerosDesVariablesAInstancierADroite
				);
						
CodeRetour = VARIABLES_A_INSTANCIER;
return( CodeRetour );

}

