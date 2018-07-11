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
/*************************************************************************

   FONCTION: Evaluation des 2 fils du noeud comportant le meilleur
             minorant

   AUTEUR: R. GONZALEZ

**************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "pne_define.h"

/*---------------------------------------------------------------------------------------------------------*/

void BB_EvaluerLesDeuxFilsDuMeilleurMinorant( BB * Bb, NOEUD * NoeudDuMeilleurMinorant )
{
int YaUneSolution; NOEUD * NoeudCourant; int SolutionEntiereTrouvee;  

#if VERBOSE_BB
  printf("Minorant du meilleur noeud %12.8e\n",NoeudDuMeilleurMinorant->MinorantDuCritereAuNoeud);
#endif
  
NoeudCourant = NoeudDuMeilleurMinorant->NoeudSuivantGauche;
if ( NoeudCourant != 0 ) {
  if ( NoeudCourant->StatutDuNoeud == A_EVALUER && 
       NoeudCourant->NoeudTerminal != OUI       && 
       NoeudCourant->StatutDuNoeud != A_REJETER ) {

    #if VERBOSE_BB
      printf("Evaluation fils gauche du meilleur minorant\n");
    #endif

    Bb->NoeudEnExamen = NoeudCourant;
    YaUneSolution = BB_ResoudreLeProblemeRelaxe( Bb, NoeudCourant , &SolutionEntiereTrouvee ); 

    BB_NettoyerLArbre( Bb, &YaUneSolution , NoeudCourant );  /* Fait aussi la mise a jour du statut */

    
    BB_CreerLesNoeudsFils( Bb, NoeudCourant );    

  }
}

NoeudCourant = NoeudDuMeilleurMinorant->NoeudSuivantDroit;
if ( NoeudCourant != 0 ) {
  if ( NoeudCourant->StatutDuNoeud == A_EVALUER && 
       NoeudCourant->NoeudTerminal != OUI       && 
       NoeudCourant->StatutDuNoeud != A_REJETER ) {

    #if VERBOSE_BB
      printf("Evaluation fils droit du meilleur minorant\n");
    #endif

    Bb->NoeudEnExamen = NoeudCourant;
    YaUneSolution = BB_ResoudreLeProblemeRelaxe( Bb, NoeudCourant , &SolutionEntiereTrouvee ); 
  
    BB_NettoyerLArbre( Bb, &YaUneSolution , NoeudCourant );  /* Fait aussi la mise a jour du statut */

    BB_CreerLesNoeudsFils( Bb, NoeudCourant );    

  }
}

return;
}

