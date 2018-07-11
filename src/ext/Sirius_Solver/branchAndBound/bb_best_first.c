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

   FONCTION: Exploration du meilleur minorant. Cette routine ne peut
             etre appelee que si on dispose deja d'une solution
	     entiere.

   AUTEUR: R. GONZALEZ

**************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "pne_define.h"

/*---------------------------------------------------------------------------------------------------------*/

void BB_BestFirst( BB * Bb )
{
double Gap; double Seuil; double MinorantDuCritereAuNoeud; int Nb; int NbMx; double X;
char Break; NOEUD * Noeud;

/* Precaution */
if ( Bb->NombreDeSolutionsEntieresTrouvees <= 0 ) return; 
/* Precaution */
Bb->EvaluerLesFilsDuMeilleurMinorant = OUI;

NbMx  = Bb->NombreDEvaluationDuMeilleurMinorant;

Nb    = 0;
BB_RechercherLeMeilleurMinorant( Bb, RECHERCHER_LE_PLUS_PETIT );
if ( Bb->NoeudDuMeilleurMinorant == 0 ) return;

Bb->ValeurDuMeilleurPremierMinorant = Bb->NoeudDuMeilleurMinorant->MinorantDuCritereAuNoeud;

Bb->ValeurDuMeilleurDernierMinorant = Bb->ValeurDuMeilleurPremierMinorant;

Gap = Bb->CoutDeLaMeilleureSolutionEntiere - Bb->ValeurDuMeilleurMinorant;
Seuil = 0.99 * Gap;
   
if ( Bb->NoeudDuMeilleurMinorant != 0 ) {
  MinorantDuCritereAuNoeud = Bb->NoeudDuMeilleurMinorant->MinorantDuCritereAuNoeud;
  while ( Nb < NbMx && (Bb->NoeudDuMeilleurMinorant->MinorantDuCritereAuNoeud - MinorantDuCritereAuNoeud) < Seuil &&
	  Bb->ArreterLesCalculs == NON ) {    
    Nb++;
    X = Bb->NoeudDuMeilleurMinorant->MinorantDuCritereAuNoeud;
    BB_EvaluerLesDeuxFilsDuMeilleurMinorant( Bb, Bb->NoeudDuMeilleurMinorant );
    Break = NON;
    if ( Bb->NoeudDuMeilleurMinorant != 0 && 0 ) {
      Noeud = Bb->NoeudDuMeilleurMinorant->NoeudSuivantGauche;
      if ( Noeud != 0 ) {
        if ( Noeud->MinorantDuCritereAuNoeud - X < 1.e-8 ) Break = OUI;
      }
      Noeud = Bb->NoeudDuMeilleurMinorant->NoeudSuivantDroit; 
      if ( Noeud != 0 ) {
        if ( Noeud->MinorantDuCritereAuNoeud - X < 1.e-8 ) Break = OUI;
      }
    }
    BB_RechercherLeMeilleurMinorant( Bb, RECHERCHER_LE_PLUS_PETIT );
    if ( Bb->NoeudDuMeilleurMinorant == 0 ) break;
    Bb->ValeurDuMeilleurDernierMinorant = Bb->NoeudDuMeilleurMinorant->MinorantDuCritereAuNoeud;
    if ( Break == OUI ) break;
  }
}
 
/* Precaution */
Bb->EvaluerLesFilsDuMeilleurMinorant = NON;

if ( Bb->ValeurDuMeilleurDernierMinorant - Bb->ValeurDuMeilleurPremierMinorant < 1.e-7 ) {
  /* On n'arrive pas a faire monter les minorants.
     Il vaut mieux passer du temps a chercher des solutions entieres */
  /*Bb->NombreDEvaluationDuMeilleurMinorant = 2;*/
  /* Apres test: mieux vaut conserver la meme valeur */
  Bb->NombreDEvaluationDuMeilleurMinorant = NOMBRE_DEVALUATIONS_DU_MEILLEUR_MINORANT;
}
else {
  Bb->NombreDEvaluationDuMeilleurMinorant = NOMBRE_DEVALUATIONS_DU_MEILLEUR_MINORANT;
}

return;
}





