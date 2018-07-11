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

   FONCTION: Creation des noeuds fis apres evaluation du pere
       
   AUTEUR: R. GONZALEZ

************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "pne_define.h"
#include "pne_fonctions.h"

/*-----------------------------------------------------------------------------------------------*/

void BB_CreerLesNoeudsFils( BB * Bb, NOEUD * NoeudCourant )
{  
NOEUD * Noeud; int CodeRetour    ; int FilsACreer; 
int ValeurDInstanciationAGauche; int NombreDeVariablesAInstancierAGauche;
int * NumerosDesVariablesAInstancierAGauche;
int ValeurDInstanciationADroite; int NombreDeVariablesAInstancierADroite;
int * NumerosDesVariablesAInstancierADroite;
#if VERBOSE_BB
  int i;
#endif

CodeRetour = BB_ChoixDesVariablesAInstancier( Bb,
					                                    NoeudCourant,
                                              &ValeurDInstanciationAGauche,
                                              &NombreDeVariablesAInstancierAGauche,
                                              &NumerosDesVariablesAInstancierAGauche,
				                                      &ValeurDInstanciationADroite,
                                              &NombreDeVariablesAInstancierADroite,
                                              &NumerosDesVariablesAInstancierADroite );    
					      
if ( CodeRetour == PAS_DE_VARIABLES_A_INSTANCIER ) {
  BB_DesallocationPartielleDUnNoeud( NoeudCourant );
  return;
}

#if VERBOSE_BB
  printf("Valeur d'instanciation a gauche -> %d\n",ValeurDInstanciationAGauche);
  printf("Instanciation a gauche de la variable ->");
  for ( i = 0 ; i < NombreDeVariablesAInstancierAGauche ; i++ ) {
    printf(" %d ",NumerosDesVariablesAInstancierAGauche[i]);
  }
  printf("\n");
  printf("Valeur d'instanciation a droite -> %d\n",ValeurDInstanciationADroite);
  printf("Instanciation a droite de la variable ->",NumerosDesVariablesAInstancierADroite[i]);
  for ( i = 0 ; i < NombreDeVariablesAInstancierADroite ; i++ ) {
    printf(" %d ",NumerosDesVariablesAInstancierADroite[i]);
  }
  printf("\n");  
#endif
  
/* Si le noeud courant a deja son fils NoeudSuivantGauche, on le cree pas */
Noeud = NoeudCourant->NoeudSuivantGauche; 
if ( Noeud == 0 ) {
                            /* Noeud gauche */
  FilsACreer = FILS_GAUCHE;
  Noeud = BB_AllouerUnNoeud( Bb,
			                       NoeudCourant,
                             NoeudCourant->ProfondeurDuNoeud + 1,
			                       FilsACreer,
                             ValeurDInstanciationAGauche,
			                       NombreDeVariablesAInstancierAGauche,
			                       NumerosDesVariablesAInstancierAGauche, 
                             Bb->MinorantEspereAEntierInf );
  NoeudCourant->NoeudSuivantGauche = Noeud;
} 

/* Si le noeud courant a deja son fils NoeudSuivantDroit, on le cree pas */
Noeud = NoeudCourant->NoeudSuivantDroit; 
if ( Noeud == 0 ) {
                            /* Noeud droit */
  FilsACreer = FILS_DROIT;
  Noeud = BB_AllouerUnNoeud( Bb,
			                       NoeudCourant,                         
                             NoeudCourant->ProfondeurDuNoeud + 1,
			                       FilsACreer,
                             ValeurDInstanciationADroite,
			                       NombreDeVariablesAInstancierADroite,
			                       NumerosDesVariablesAInstancierADroite, 
                             Bb->MinorantEspereAEntierSup );
  NoeudCourant->NoeudSuivantDroit = Noeud;
}

return;
}                        



