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

   FONCTION:  Solveur de PLNE appele par le solveur pour resoudre un
	            sous probleme reduit.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

void PNE_SolveurProblemeReduit( PROBLEME_A_RESOUDRE * Probleme,
                                CONTROLS * Controls )
{
PROBLEME_PNE * Pne; void * Tas;

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE
  Probleme->ExistenceDUneSolution = ARRET_CAR_ERREUR_INTERNE;
  return; /* On n'utilise pas le malloc proprietaire */
  Tas = MEM_Init(); 
  Pne = (PROBLEME_PNE *) MEM_Malloc( Tas, sizeof( PROBLEME_PNE ) );
  if ( Pne == NULL ) {
    printf("Saturation memoire, impossible d'allouer l'objet PROBLEME_PNE\n");
    Probleme->ExistenceDUneSolution = ARRET_CAR_ERREUR_INTERNE;
    return;
  }				
	Pne->Tas = Tas;
# else 		
  Pne = (PROBLEME_PNE *) malloc( sizeof( PROBLEME_PNE ) );
  if ( Pne == NULL ) {
    printf("Saturation memoire, impossible d'allouer l'objet PROBLEME_PNE\n");
    Probleme->ExistenceDUneSolution = ARRET_CAR_ERREUR_INTERNE;
    return;
  }
	Tas = NULL;
	Pne->Tas = Tas;
  Pne->Controls = (CONTROLS *) malloc( sizeof( CONTROLS ) );
  if ( Pne->Controls == NULL ) {
    printf("Saturation memoire, impossible d'allouer l'objet PROBLEME_PNE\n");
    Probleme->ExistenceDUneSolution = ARRET_CAR_ERREUR_INTERNE;
    return;
  }	
# endif

Pne->AnomalieDetectee = NON_PNE ;
setjmp( Pne->Env );  
if ( Pne->AnomalieDetectee == OUI_PNE ) {
  /* Liberation du probleme */ 
  PNE_LibereProbleme( Pne );
  Probleme->ExistenceDUneSolution = ARRET_CAR_ERREUR_INTERNE;
  fflush(stdout);
  return;
} 
else {
 
  if ( Probleme->SortirLesDonneesDuProbleme == OUI_PNE ) {
    PNE_EcrireJeuDeDonneesMPS( Pne, Probleme );   
  }

  /* Optimisation */
  
  Pne->ProblemeSpxDuSolveur = NULL; 
  Pne->ProblemeBbDuSolveur  = NULL; 
  Pne->ProblemePrsDuSolveur = NULL;

  /* Initialisation du temps */
  time( &(Pne->HeureDeCalendrierDebut) );

  memcpy( (char *) Pne->Controls, (char *) Controls, sizeof( CONTROLS ) );
	
  PNE_SolveurCalculs( Probleme , Pne ); 
}

/* Pour vider le buffer des traces */
fflush(stdout);

return;
}
