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

   FONCTION:  Solveur de PLNE 

   IMPORTANT: 
            * Inclure le header "pne_constantes_externes.h" dans le module 
              appelant. Il contient les valeurs des constantes symboliques 
              telles que OUI_PNE NON_PNE ENTIER REEL etc .. 
            * Ne jamais utiliser directement les valeurs numeriques des 
              constantes symboliques mais seulement leurs noms.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/
/*
  Point d'entree du solveur de PNE:  
  -> Inclure le header "pne_constantes_externes.h" dans le module appelant. Il contient les valeurs 
     des constantes symboliques telles que OUI_PNE NON_PNE ENTIER REEL etc .. 
  -> Les variables sont numerotée à partir de 0. Exemple: s'il y a 3 variables, 
     leurs numeros sont 0 1 et 2.
  -> En consequence s'il y a N inconnues, les colonnes de la matrice des contraintes 
     vont de 0 a N-1 inclus.
  -> Les contraintes sont numerotée à partir de 0. Exemple: s'il y a 2 contraintes, 
     leurs numeros sont 0 et 1.
  -> En consequence s'il y a M contraintes, les lignes de la matrice des contraintes 
     vont de 0 a M-1 inclus.
*/
/*----------------------------------------------------------------------------*/

void PNE_Solveur( PROBLEME_A_RESOUDRE * Probleme )
{
PROBLEME_PNE * Pne; void * Tas;

/* Controle anti piratage tres simplifie: on controle l'adresse MAC de la machine */
/*PNE_ControleMacAdresse( );*/

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
	Pne->Controls = NULL;
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
  
  PNE_SolveurCalculs( Probleme , Pne ); 
}

/* Pour vider le buffer des traces */
fflush(stdout);

return;
}
