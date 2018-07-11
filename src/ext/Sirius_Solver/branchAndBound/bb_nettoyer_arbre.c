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

   FONCTION: 

  Appele pour nettoyer l'arbre apres chaque resolution du probleme relaxe.
  Si la solution du probleme relaxe n'est pas entiere, on compare son cout au cout de la meilleure solution 
  entiere trouvee: 
    1- S'il est superieur ou egal, alors le noeud est marque A_REJETER
    2- S'il est inferieur, alors on ne peut pas trancher et le statut du noeud reste a EVALUE 
  Si la solution du probleme relaxe est entiere, on compare son cout au cout de la meilleure solution 
  entiere trouvee: 
    1- S'il est superieur, alors le noeud est marque A_REJETER
    2- S'il est inferieur alors:
         a- Son cout remplace celui de la meilleure solution entiere trouvee
         b- L'arbre est reparcouru et tous les noeuds dont le cout est superieur au nouveau cout sont
            supprimes

       
   AUTEUR: R. GONZALEZ

************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "pne_define.h"
#include "pne_fonctions.h"

# define MARGE 1.e-6 /* 1.e-9 */

# define COEFF_PROFONDEUR_LIMITE 1.25

/*---------------------------------------------------------------------------------------------------------*/

void BB_NettoyerLArbre( BB * Bb, int * YaUneSolution, NOEUD * NoeudCourant )
{
PROBLEME_PNE * Pne; NOEUD * NoeudPere; NOEUD * NoeudFils; double Marge;

Pne = (PROBLEME_PNE *) Bb->ProblemePneDuBb; 

NoeudCourant->StatutDuNoeud = EVALUE;
NoeudCourant->NbValeursFractionnairesApresResolution = Pne->NombreDeVariablesAValeurFractionnaire;
NoeudCourant->NormeDeFractionnalite                  = Pne->NormeDeFractionnalite;

/* Mise a jour du nombre de noeuds ouverts en regardant le pere */
NoeudPere = NoeudCourant->NoeudAntecedent;
if ( NoeudPere != NULL ) {
  NoeudFils = NoeudPere->NoeudSuivantGauche;
  if ( NoeudFils != NULL ) {
    if ( NoeudFils->StatutDuNoeud != A_EVALUER || NoeudFils == NoeudCourant ) {
      NoeudFils = NoeudPere->NoeudSuivantDroit;
      if ( NoeudFils != NULL ) {
        if ( NoeudFils->StatutDuNoeud != A_EVALUER || NoeudFils == NoeudCourant ) {
          Bb->NbNoeudsOuverts--;
        }
      }
    }
  }
}

if ( *YaUneSolution != OUI ) {			 	
  NoeudCourant->StatutDuNoeud = A_REJETER;  
  NoeudCourant->NoeudTerminal = OUI;
  NoeudCourant->NoeudSuivantGauche = 0;
  NoeudCourant->NoeudSuivantDroit  = 0;
  NoeudCourant->NbValeursFractionnairesApresResolution = Bb->NombreDeVariablesEntieresDuProbleme;
  NoeudCourant->NormeDeFractionnalite = PLUS_LINFINI;
	
  /* On peut recuperer de la place */ 
  BB_DesallocationPartielleDUnNoeud( NoeudCourant );

  Bb->NbNoeudsOuverts--;
  
  return;
}

if ( NoeudCourant->LaSolutionRelaxeeEstEntiere == NON ) { /* Cas ou la solution du probleme relaxe n'est pas entiere */

  /* On elimine le noeud si la solution entiere disponible est optimale par rapport au sous arbre */
	Marge = Bb->ToleranceDOptimalite * 0.01 * fabs( NoeudCourant->MinorantDuCritereAuNoeud );
	if ( Marge < MARGE ) Marge = MARGE;
	
  if ( NoeudCourant->MinorantDuCritereAuNoeud >= Bb->CoutDeLaMeilleureSolutionEntiere - Marge /*MARGE*/ ) { 
		/* Le cout de la solution du probleme relaxe est superieur a celui de la meilleure solution entiere => noeud a rejeter */   		
    NoeudCourant->StatutDuNoeud = A_REJETER;
    NoeudCourant->NoeudTerminal = OUI;
    NoeudCourant->NoeudSuivantGauche = 0;
    NoeudCourant->NoeudSuivantDroit  = 0;
    /* On peut recuperer de la place */        
    BB_DesallocationPartielleDUnNoeud( NoeudCourant );
    
    Bb->NbNoeudsOuverts--;
    
  }	
  return; /* rq: dans le cas ou on ne peut pas trancher, le statut du noeud reste a EVALUE */
}

/* La solution est entiere, on demande l'evaluation des fils du noeud du meilleur minorant */
Bb->NbNoeudsOuverts--; /* Car on ne regardera pas sous le noeud vu que la solution est entiere */
Bb->EvaluerLesFilsDuMeilleurMinorant = OUI;
 
/* Le solution est entiere, le noeud est marque terminal et on essaie de nettoyer l'arbre */
NoeudCourant->NoeudTerminal = OUI;
NoeudCourant->NoeudSuivantGauche = 0;
NoeudCourant->NoeudSuivantDroit  = 0;

if ( NoeudCourant->MinorantDuCritereAuNoeud >= Bb->CoutDeLaMeilleureSolutionEntiere ) {
    /* Le cout de la solution est superieur a celui de la meilleure solution entiere => noeud a rejeter */
    #if VERBOSE_BB
      printf(" Solution entiere trouvee mais elle est plus chere que la solution entiere courante: le noeud est terminal a rejeter \n");
    #endif			
		NoeudCourant->StatutDuNoeud = A_REJETER;    
    return;
}

/* Si c'est la premiere solution entiere on relance le calcul des coupes */
if ( Bb->NoeudDeLaMeilleureSolutionEntiere == 0 ) {
  Bb->NbMaxDeCoupesCalculeesAtteint    = NON;
  Bb->NombreMaxDeCoupes                = Bb->NoeudRacine->NombreDeCoupesGenereesAuNoeud + NOMBRE_MAX_DE_COUPES;
  Bb->NombreMoyenMinimumDeCoupesUtiles = NOMBRE_MOYEN_MINIMUM_DE_COUPES_UTILES;
}

/* La solution est entiere et il s'agit du meilleur cout rencontre */
Bb->SommeDesProfondeursDesSolutionsAmeliorantes += NoeudCourant->ProfondeurDuNoeud;
Bb->NombreDeSolutionsAmeliorantes++;

Bb->SolutionAmelioranteTrouvee              = OUI;
Bb->NumeroDeProblemeDeLaSolutionAmeliorante = Bb->NombreDeProblemesResolus;
Bb->NoeudDeLaMeilleureSolutionEntiere       = NoeudCourant;
Bb->CoutDeLaMeilleureSolutionEntiere        = NoeudCourant->MinorantDuCritereAuNoeud;
if ( (int) (COEFF_PROFONDEUR_LIMITE * Bb->NoeudDeLaMeilleureSolutionEntiere->ProfondeurDuNoeud) > 
      Bb->ProfondeurMaxiSiPlongeePendantUneRechercheEnLargeur ) {
  Bb->ProfondeurMaxiSiPlongeePendantUneRechercheEnLargeur = (int) (COEFF_PROFONDEUR_LIMITE * Bb->NoeudDeLaMeilleureSolutionEntiere->ProfondeurDuNoeud);
}

/*
if ( Bb->NombreDeVariablesEntieresDuProbleme > 0 ) {
  Bb->ForcerAffichage = OUI;
}
*/

if ( Bb->NombreDeVariablesEntieresDuProbleme > 0 ) {
  Bb->ForcerAffichage = OUI;
  BB_AfficherLesTraces( Bb, NoeudCourant );
}
Bb->ForcerAffichage = NON;

/*
for ( i= 0 ; i < Bb->NombreDeVariablesEntieresDuProbleme ; i++ ) {
  Bb->ValeursOptimalesDesVariablesEntieres[i] = Bb->ValeursCalculeesDesVariablesEntieresPourLeProblemeRelaxeCourant[i];
}
*/
/*
for ( i= 0 ; i < Bb->NombreDeVariablesDuProbleme ; i++ ) {
  Bb->ValeursOptimalesDesVariables[i] = Bb->ValeursCalculeesDesVariablesPourLeProblemeRelaxeCourant[i];
}
*/

/* On demande à la PNE de stocker la solution */

PNE_ArchiverLaSolutionCourante( Pne );
	 
/* Nettoyage de l'arbre: tout l'arbre est explore a partir de la racine */
BB_EliminerLesNoeudsSousOptimaux( Bb );

Bb->NombreDEvaluationDuMeilleurMinorant = NOMBRE_DEVALUATIONS_DU_MEILLEUR_MINORANT;
              
return;
}  

