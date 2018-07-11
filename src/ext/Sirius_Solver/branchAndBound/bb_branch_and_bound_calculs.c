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

   FONCTION: Pilotage du branch and bound  
 
   Appele comme un sous-programme du programme fortran maitre. Ce 
   sous-programme orchestre le déroulement de la recherche arborescente. 
   Les etapes sont les suivantes:
   1- Creation du noeud racine.
   2- Recherche en profondeur a partie du noeud racine pour trouver une  
      solution entiere admissible.
   3- Recherche en largeur a partir du noeud racine pour trouver la  
      solution optimale. 
       
   AUTEUR: R. GONZALEZ

************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "spx_define.h"

#include "pne_fonctions.h"

char TestLocalBranching = NON/*OUI*/;

/*---------------------------------------------------------------------------------------------------------*/

int BB_BranchAndBoundCalculs( BB * Bb,
			                        int TempsDexecutionMaximum, int NombreMaxDeSolutionsEntieres,
                              double ToleranceDOptimalite, int NbVar, int NbContr,
			                        int NbVarEntieres, char AffichageDesTraces,
			                        int * NumVarEntieres )
{
NOEUD * NoeudAntecedent; NOEUD * Noeud; NOEUD * NoeudDeDepart; int ProfondeurDuNoeud; int ProfondeurDuNoeudDeDepart;
int i; int YaUneSolution; int ValeurDeLaNouvelleVariableIntanciee; int IndiceDeLaNouvelleVariableIntanciee;
int FilsACreer; PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Bb->ProblemePneDuBb;
  
Bb->ArreterLesCalculs      = NON;
Bb->TempsDexecutionMaximum = TempsDexecutionMaximum;
Bb->ForcerAffichage        = NON;
Bb->AffichageDesTraces     = AffichageDesTraces;
Bb->NombreMaxDeSolutionsEntieres = NombreMaxDeSolutionsEntieres;
Bb->ToleranceDOptimalite         = ToleranceDOptimalite;

Bb->CoutDeLaMeilleureSolutionEntiere  = PLUS_LINFINI;
Bb->NoeudDeLaMeilleureSolutionEntiere = 0;
Bb->ProfondeurMoyenneDesSolutionsEntieres = -1;

Bb->ValeurDuMeilleurMinorant = -PLUS_LINFINI;

Bb->NombreDeVariablesEntieresDuProbleme = NbVarEntieres;
	
Bb->ProfondeurMaxiSiPlongeePendantUneRechercheEnLargeur = -1;

Bb->NombreDEvaluationDuMeilleurMinorant = NOMBRE_DEVALUATIONS_DU_MEILLEUR_MINORANT;  

Bb->EnleverToutesLesCoupesDuPool = NON;

Bb->NbMaxDeCoupesCalculeesAtteint    = NON;
Bb->NombreMaxDeCoupes                = NOMBRE_MAX_DE_COUPES;
Bb->NombreMoyenMinimumDeCoupesUtiles = NOMBRE_MOYEN_MINIMUM_DE_COUPES_UTILES;

#if VERBOSE_BB
  printf("\n Nombre de variable entieres du probleme: %d\n", Bb->NombreDeVariablesEntieresDuProbleme);
#endif 

Bb->NombreDeVariablesDuProbleme = NbVar ;

#if VERBOSE_BB
  printf(" Nombre de variable du probleme: %d\n", Bb->NombreDeVariablesDuProbleme);
#endif 

Bb->NombreDeContraintesDuProbleme = NbContr;

#if VERBOSE_BB
  printf(" Nombre de contraintes du probleme: %d\n", Bb->NombreDeContraintesDuProbleme);
#endif

Bb->NombreDeProblemesRelaxesResolus              = 0;
Bb->NombreDeProblemesResolus                     = 0;  
Bb->NombreDeProblemesResolusDepuisLaRAZDesCoupes = 0; 
Bb->NombreDeSolutionsEntieresTrouvees            = 0;
Bb->NbProbPourLaPremiereSolutionEntiere          = 0;

Bb->NombreTotalDeCoupesDuPoolUtilisees = 0;
Bb->NombreTotalDeGDuPoolUtilisees = 0;
Bb->NombreTotalDeIDuPoolUtilisees = 0;
Bb->NombreTotalDeKDuPoolUtilisees = 0;

Bb->NombreDeNoeudsElagues = 0;
Bb->AveragePruningDepth = 0;
Bb->SommeDesProfondeursDesSolutionsAmeliorantes = 0;
Bb->NombreDeSolutionsAmeliorantes = 0;

Bb->SommeDesProfondeursDElaguage = 0;

Bb->NombreDeSimplexes = 0;
Bb->SommeDuNombreDIterations = 0;

Bb->EvaluerLesFilsDuMeilleurMinorant = NON;
Bb->EcartBorneInf = 1.e+70;

Bb->NbNoeudsOuverts = 0;

Bb->DernierNoeudResolu = 0;

BB_BranchAndBoundAllouerProbleme( Bb );

for ( i = 0 ; i < Bb->NombreDeVariablesEntieresDuProbleme ; i++ ) { 
  Bb->NumerosDesVariablesEntieresDuProbleme[i] = NumVarEntieres[i];
}

Bb->NbNoeuds1_PNE_BalayageEnProfondeur                    = 0;
Bb->NbNoeuds1_PNE_NettoyerLArbreDeLaRechercheEnProfondeur = 0;
Bb->NbNoeuds2_PNE_NettoyerLArbreDeLaRechercheEnProfondeur = 0;
Bb->NbNoeuds1_PNE_BalayageEnLargeur                       = 0;
Bb->NbNoeuds2_PNE_BalayageEnLargeur                       = 0;
Bb->NbNoeuds1_PNE_EliminerLesNoeudsSousOptimaux           = 0;
Bb->NbNoeuds2_PNE_EliminerLesNoeudsSousOptimaux           = 0;
Bb->NbNoeuds1_PNE_SupprimerTousLesDescendantsDUnNoeud     = 0;
Bb->NbNoeuds2_PNE_SupprimerTousLesDescendantsDUnNoeud     = 0;

Bb->NombreMaxDeCoupesParProblemeRelaxe = (int) ceil ( MAX_COUPES_PAR_PROBLEME_RELAXE * Bb->NombreDeContraintesDuProbleme );
if ( Bb->NombreMaxDeCoupesParProblemeRelaxe < 100 ) Bb->NombreMaxDeCoupesParProblemeRelaxe = 100;

/* Creation du noeud racine */

NoeudAntecedent   = 0; 
ProfondeurDuNoeud = 0;
FilsACreer        = FILS_DROIT; /* Mais cela n'a pas d'importance car c'est le noeud racine */
ValeurDeLaNouvelleVariableIntanciee = 1;
IndiceDeLaNouvelleVariableIntanciee = -1;

Noeud = BB_AllouerUnNoeud( Bb, NoeudAntecedent,
			                     ProfondeurDuNoeud,
			                     FilsACreer,
                           ValeurDeLaNouvelleVariableIntanciee,
			                     0,
			                     &IndiceDeLaNouvelleVariableIntanciee, 
                           -PLUS_LINFINI );
Bb->NoeudRacine = Noeud;
Bb->BaseDisponibleAuNoeudRacine = NON;

/* Recherche d'une solution entiere admissible par un balayage en profondeur a partir du noeud racine */
Bb->UtiliserCoutDeLaMeilleureSolutionEntiere = NON_SPX;
Bb->TypeDExplorationEnCours = PROFONDEUR_TOUT_SEUL; 

ProfondeurDuNoeudDeDepart = 0; 
NoeudDeDepart             = Bb->NoeudRacine;

YaUneSolution = BB_BalayageEnProfondeur( Bb, NoeudDeDepart , ProfondeurDuNoeudDeDepart );

if ( Pne->YaUneSolutionEntiere == NON_PNE ) YaUneSolution = NON_PNE;

if( YaUneSolution != OUI ) {
  /* On a neanmoins pu trouver une solution entiere */
  if ( Pne->YaUneSolutionEntiere == NON_PNE ) {
    #if VERBOSE_BB
      printf("PAS DE SOLUTION ENTIERE \n"); 
    #endif
    YaUneSolution = NON;
    goto FinDuBranchAndBound;
	}
}

/**********************************************************************/
if ( TestLocalBranching == OUI ) {
  TestLocalBranching = NON;
  goto RestitutionPremiereSolution;
}
/**********************************************************************/

if ( Bb->NombreDeVariablesEntieresDuProbleme > 0 ) {

  /* Recherche de la solution optimale par un balayage en largeur a partir du noeud racine */
  Bb->NbProbPourLaPremiereSolutionEntiere = Bb->NombreDeProblemesResolus;

  Bb->UtiliserCoutDeLaMeilleureSolutionEntiere = OUI_SPX;

  ProfondeurDuNoeudDeDepart = 0; 
  NoeudDeDepart             = Bb->NoeudRacine;
  BB_BalayageEnLargeur( Bb, NoeudDeDepart, ProfondeurDuNoeudDeDepart );

  #if VERBOSE_BB
    printf("\n \n  FIN DE LA RECHERCHE EN LARGEUR  \n \n");
  #endif

}  

if ( Pne->YaUneSolutionEntiere == NON_PNE ) YaUneSolution = NON_PNE;

if( YaUneSolution != OUI ) {
  /* On a neanmoins pu trouver une solution entiere */
  if ( Pne->YaUneSolutionEntiere == NON_PNE ) {
    #if VERBOSE_BB
      printf("PAS DE SOLUTION ENTIERE \n"); 
    #endif
    YaUneSolution = NON;
    goto FinDuBranchAndBound;
	}
}

/* Resultat */

RestitutionPremiereSolution:

YaUneSolution = OUI;
if ( Bb->ArreterLesCalculs == OUI ) {
  if ( Bb->EcartBorneInf > Bb->ToleranceDOptimalite ) YaUneSolution = ARRET_CAR_TEMPS_MAXIMUM_ATTEINT;	
}

PNE_RestituerLaSolutionArchivee( Pne );

FinDuBranchAndBound:

if ( Bb->NombreDeVariablesEntieresDuProbleme > 0 && YaUneSolution != NON && Bb->AffichageDesTraces == OUI ) {
  printf("\n");	 
  printf("** Solution summary **\n");
  if ( YaUneSolution == ARRET_CAR_TEMPS_MAXIMUM_ATTEINT ) { 
    printf("      Stopping calculation because maximum allowed time has been reached.\n");
  }
  else if ( YaUneSolution == OUI ) {
    printf("      Optimal solution found\n");
  }	 
  printf("      Solved nodes before finding the first feasible solution: %d\n",Bb->NbProbPourLaPremiereSolutionEntiere);
  if ( YaUneSolution == ARRET_CAR_TEMPS_MAXIMUM_ATTEINT ) { 
    printf("      Best feasible solution found after: %d nodes\n",Bb->NumeroDeProblemeDeLaSolutionAmeliorante);
  }
  else if ( YaUneSolution == OUI ) {
    printf("      Solved nodes to find an optimal solution: %d\n",Bb->NumeroDeProblemeDeLaSolutionAmeliorante);
  }
  printf("      Total solved nodes: %d\n",Bb->NombreDeProblemesResolus);    
  printf("      Feasible solutions found: %d\n",Bb->NombreDeSolutionsEntieresTrouvees);

  { PROBLEME_PNE * Pne; double TempsEcoule;
    Pne = (PROBLEME_PNE *) Bb->ProblemePneDuBb;
    time( &(Pne->HeureDeCalendrierCourant) );
    TempsEcoule = difftime( Pne->HeureDeCalendrierCourant , Pne->HeureDeCalendrierDebut );
    if ( TempsEcoule <= 0.0 ) TempsEcoule = 0.0;  
    printf("      Elapsed time: %d seconds\n",(int) TempsEcoule);
  }
  
  printf("      Solved relaxed problems: %d\n",Bb->NombreDeProblemesRelaxesResolus);
  printf("\n");	 
}
  
/* On libere le probleme */
BB_BranchAndBoundDesallouerProbleme( Bb );

return( YaUneSolution ); 

}

