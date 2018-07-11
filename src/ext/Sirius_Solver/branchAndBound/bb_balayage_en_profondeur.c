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

   FONCTION: Exploration d'une branche en profondeur a partir d'un 
   noeud (y compris le noeud lui-meme). 

   Utilite: trouver une solution admissible au plus tot.

   Attention: le noeud de depart doit deja exister.

   Apres chaque noeud explore on cree les 2 noeuds fils.
       
   AUTEUR: R. GONZALEZ

************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "pne_define.h" 
#include "pne_fonctions.h"

# define CYCLE_DE_PROFONDEUR_POUR_LE_CALCUL_DES_GOMORY  1 /*1*/
# define MARGE_SUR_LES_MINORANTS                        1.e-4
# define NEUTRALISATION_CALCUL_COUPES                   0 /*0*/

# define TAILLE_MOYENNE_MOBILE 10
# define MARGE_POUR_SOMME_MOYENNE_MOBILE  0.25 /*0.0*/
# define CYCLE_TEST_MOYENNE_MOBILE 1

/*---------------------------------------------------------------------------------------------------------*/

int BB_BalayageEnProfondeur( BB * Bb , NOEUD * NoeudDeDepart , int ProfondeurDuNoeudDeDepart )
{
NOEUD * NoeudsAExplorer[2]; NOEUD * NoeudCourant  ; NOEUD * NoeudAntecedent ;   
NOEUD * NoeudPereSuivant  ; ldiv_t QuotientEtReste; int    ProfondeurLimite; char ProfondeurRapide;
int ProfondeurMoyenneDesSolutionsEntieres; int ProfondeurMoyenneDesSolutionsAmeliorantes;
double MinorantPredit0; char MinorantPredit0Initialise; 
double MinorantPredit1; char MinorantPredit1Initialise; 
int IndiceChoisi      ; char   UnIndiceAEteChoisi         ; char Les2ChoixSontPossibles;
char EvaluerLAutreNoeud; double MinorantPreditDeLAutreNoeud; int NbProblemesResolus    ;
int NombreDeNoeudsAExplorer; /* Utile uniquement pour l'appel a BB_NettoyerLArbre */
int NombreDeNoeudsAExplorerAuProchainEtage; /* Utile uniquement pour l'appel a BB_NettoyerLArbre */                    
int YaUneSolution; int ProfondeurDuNoeud; int SolutionEntiereTrouvee;
int NombreMaxDeProblemesProfondeurDansLargeur;  int i; PROBLEME_PNE * Pne;
double VecteurPourMoyenneMobile[TAILLE_MOYENNE_MOBILE]; char TesterLaDecroissance; int NbTestsMoyenneMobile;
int IndexMoyenneMobile; double SommePourMoyenneMobilePrecedente; double SommePourMoyenneMobile;
char SommePourMoyenneMobilePrecedenteEstInitialisee;

Pne = (PROBLEME_PNE *) Bb->ProblemePneDuBb;

ProfondeurRapide = OUI;

MinorantPredit0 = PLUS_LINFINI; /* Pour eviter les warning de compilation */
MinorantPredit1 = PLUS_LINFINI; /* Pour eviter les warning de compilation */

SolutionEntiereTrouvee   = NON;
NoeudAntecedent          = 0; 
NoeudsAExplorer[0]       = NoeudDeDepart;
NoeudsAExplorer[1]       = 0;
ProfondeurDuNoeud        = ProfondeurDuNoeudDeDepart;
NombreDeNoeudsAExplorer  = 2;
NombreDeNoeudsAExplorerAuProchainEtage = 0;

IndexMoyenneMobile = 0;
SommePourMoyenneMobilePrecedente = 0;
SommePourMoyenneMobile = 0;
SommePourMoyenneMobilePrecedenteEstInitialisee = NON;
TesterLaDecroissance = NON;

Bb->NbNoeuds1_PNE_BalayageEnProfondeur = NombreDeNoeudsAExplorer; /* Pour le nettoyage eventuel */
Bb->Liste1_PNE_BalayageEnProfondeur    = NoeudsAExplorer;         /* Pour le nettoyage eventuel */

/* Calcul de la profondeur limite dans le cas d'une recherche en profondeur pendant une recherche en largeur */
ProfondeurLimite = (int) ( COEFFICIENT_POUR_LE_CALCUL_DE_LA_PROFONDEUR_LIMITE *
                            ( Bb->NombreDeVariablesEntieresDuProbleme - NoeudDeDepart->NombreDeVariablesEntieresInstanciees ) );
if ( ProfondeurLimite < PROFONDEUR_MIN_PENDANT_LA_RECHERCHE_EN_LARGEUR ) ProfondeurLimite = PROFONDEUR_MIN_PENDANT_LA_RECHERCHE_EN_LARGEUR;
if ( ProfondeurLimite > PROFONDEUR_MAX_PENDANT_LA_RECHERCHE_EN_LARGEUR ) ProfondeurLimite = PROFONDEUR_MAX_PENDANT_LA_RECHERCHE_EN_LARGEUR;

ProfondeurLimite += ProfondeurDuNoeudDeDepart;

/* On s'autorise a descendre au moins jusqu'a la profondeur ou on a trouve la meilleure solution entiere */ 
if ( Bb->TypeDExplorationEnCours == PROFONDEUR_DANS_LARGEUR ) {
  if ( Bb->ProfondeurMaxiSiPlongeePendantUneRechercheEnLargeur > ProfondeurLimite ) {
    ProfondeurLimite = Bb->ProfondeurMaxiSiPlongeePendantUneRechercheEnLargeur;
  }	
}

ProfondeurMoyenneDesSolutionsEntieres = Bb->ProfondeurMoyenneDesSolutionsEntieres;

ProfondeurMoyenneDesSolutionsAmeliorantes = Bb->NombreDeVariablesEntieresDuProbleme;
if ( Bb->NombreDeSolutionsAmeliorantes != 0 ) {
  ProfondeurMoyenneDesSolutionsAmeliorantes = (int) ceil( (float) Bb->SommeDesProfondeursDesSolutionsAmeliorantes /
	                                                        (float) Bb->NombreDeSolutionsAmeliorantes );
  ProfondeurMoyenneDesSolutionsAmeliorantes	= (int) ( 0.8 * ProfondeurMoyenneDesSolutionsAmeliorantes );
  if ( ProfondeurLimite < ProfondeurMoyenneDesSolutionsAmeliorantes ) ProfondeurLimite = ProfondeurMoyenneDesSolutionsAmeliorantes;	
}

/* Etant donne que la recherche de la premiere solution entiere se fait en profondeur d'abord, il n'est pas rare qu'on la trouve tres
   bas dans l'arbre. Il faut donc eviter un coefficient trop grand sinon on risque de passer trop de temps dans la recherche
	 en profondeur dans les etapes suivantes */
	 
NombreMaxDeProblemesProfondeurDansLargeur = (int) ceil( 1.0 /*1.5*/ * Bb->NbProbPourLaPremiereSolutionEntiere );

/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/* Le 30/01/2015: Test d'une strategie supplementaire: on tient compte du nombre d'etages qu'on s'autorise a parcourir */  
if ( Bb->TypeDExplorationEnCours == PROFONDEUR_DANS_LARGEUR ) {		 
  if ( NombreMaxDeProblemesProfondeurDansLargeur < 2 * ( ProfondeurLimite - ProfondeurDuNoeudDeDepart ) ) {
	  NombreMaxDeProblemesProfondeurDansLargeur = 2 * ( ProfondeurLimite - ProfondeurDuNoeudDeDepart );
	}	
}
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/
/***********************************************************************************************************************/

if ( NombreMaxDeProblemesProfondeurDansLargeur < NOMBRE_MIN_DE_PROBLEMES_PROFONDEUR_DANS_LARGEUR ) {
  NombreMaxDeProblemesProfondeurDansLargeur = NOMBRE_MIN_DE_PROBLEMES_PROFONDEUR_DANS_LARGEUR;	
}

/* On arrete  la recherche en profondeur sur un critere de variation de la moyenne mobile de la fractionnalitea
   a partir du moment ou on a depasse NombreMaxDeProblemesProfondeurDansLargeur */
	 
if ( NombreMaxDeProblemesProfondeurDansLargeur < TAILLE_MOYENNE_MOBILE * 3/*2*/ ) {
  NombreMaxDeProblemesProfondeurDansLargeur = TAILLE_MOYENNE_MOBILE * 3/*2*/;
}

/*
if ( Bb->TypeDExplorationEnCours == PROFONDEUR_DANS_LARGEUR ) {		 
	printf("********************** Profondeur dans largeur NombreMaxDeProblemesProfondeurDansLargeur %d ProfondeurLimite %d\n",
	        NombreMaxDeProblemesProfondeurDansLargeur,ProfondeurLimite);
}
*/

NbTestsMoyenneMobile = NombreMaxDeProblemesProfondeurDansLargeur;

NbProblemesResolus = 0; 
while ( 1 ) {

  if ( Bb->ArreterLesCalculs == OUI ) {
    Bb->NbNoeuds1_PNE_BalayageEnProfondeur = 0; 
    Bb->NoeudEnExamen = 0;  
    return( ARRET_CAR_TEMPS_MAXIMUM_ATTEINT );
  }

  /* Test exploration rapide en profondeur */
  if ( Bb->TypeDExplorationEnCours == PROFONDEUR_DANS_LARGEUR && NbProblemesResolus >= 10 && ProfondeurRapide == OUI && 0 ) { /* Car pas encore au point */
    BB_ExplorationRapideEnProfondeur( Bb );
    NbProblemesResolus = 0;
    ProfondeurRapide = OUI/*NON*/;
    if ( Bb->NombreDeSolutionsEntieresTrouvees != 0 ) {
      /* On a trouve une solution entiere */
      Bb->NbNoeuds1_PNE_BalayageEnProfondeur = 0;
      Bb->NoeudEnExamen = 0;
      return( OUI );
    }
  }
  /* Fin test exploration rapide en profondeur */   

  if ( Bb->TypeDExplorationEnCours == PROFONDEUR_DANS_LARGEUR ) {	
    if ( ProfondeurDuNoeud  > ProfondeurLimite || NbProblemesResolus > NombreMaxDeProblemesProfondeurDansLargeur ) {
		  TesterLaDecroissance = OUI;
		  if ( IndexMoyenneMobile == TAILLE_MOYENNE_MOBILE ) {
		    /* Calcul de la moyenne mobile */
			  SommePourMoyenneMobile = 0;;			
		    for ( i = 0 ; i < TAILLE_MOYENNE_MOBILE ; i++ ) SommePourMoyenneMobile += VecteurPourMoyenneMobile[i];
		    if ( SommePourMoyenneMobilePrecedenteEstInitialisee == NON ) {
			    SommePourMoyenneMobilePrecedente = SommePourMoyenneMobile;
					SommePourMoyenneMobilePrecedenteEstInitialisee = OUI;
			    TesterLaDecroissance = NON; /* Car la premiere fois on calcule SommePourMoyenneMobilePrecedente */
        }
			}
			else TesterLaDecroissance = NON; /* Pas assez de valeurs pour calculer la moyenne mobile */
			if ( TesterLaDecroissance == OUI && NbProblemesResolus % CYCLE_TEST_MOYENNE_MOBILE == 0 ) {
	      if ( NbTestsMoyenneMobile >= 0 && SommePourMoyenneMobile <= SommePourMoyenneMobilePrecedente + (MARGE_POUR_SOMME_MOYENNE_MOBILE*SommePourMoyenneMobilePrecedente) ) {
			    /* Si la moyenne mobile du nombre de variables fractionnaires decroit ou reste stationnaire on n'arrete pas */
					/*
					printf("La moyenne mobile decroit alors on continue: SommePourMoyenneMobilePrecedente %e SommePourMoyenneMobile %e\n",
					        SommePourMoyenneMobilePrecedente,SommePourMoyenneMobile);
				  */ 
					SommePourMoyenneMobilePrecedente = SommePourMoyenneMobile;
					NbTestsMoyenneMobile--;
        }
			  else {
          /* Traces */
					/*
					printf("La moyenne mobile ne decroit pas on arrete: SommePourMoyenneMobilePrecedente %d SommePourMoyenneMobile %d\n",
					        SommePourMoyenneMobilePrecedente,SommePourMoyenneMobile);
			    */
					/*
					printf("NbProblemesResolus %d ProfondeurDuNoeud %d\n",NbProblemesResolus,ProfondeurDuNoeud);
					if ( ProfondeurDuNoeud  > ProfondeurLimite ) printf("->>>>>>>>>> Fin par atteinte de ProfondeurLimite %d\n",ProfondeurLimite); 
          else if ( NbProblemesResolus > NombreMaxDeProblemesProfondeurDansLargeur ) {
				    printf("->>>>>>>>>> Fin par atteinte de NombreMaxDeProblemesProfondeurDansLargeur %d Profondeur %d\n",
					                      NombreMaxDeProblemesProfondeurDansLargeur,ProfondeurDuNoeud);		
          }
				  */
					/* Fin traces */
          break;
			  }
			}			
		}  				
  }
	
  /* Dans un balayage en profondeur, on ne calcule des Gomory qu'apres avoir descendu un certain nombre d'etages */
  Bb->CalculerDesCoupes = NON_PNE;
  if ( Bb->TypeDExplorationEnCours == PROFONDEUR_DANS_LARGEUR ) { 
    Bb->CalculerDesCoupes         = OUI_PNE;  
    Bb->CalculerDesCoupesDeGomory = OUI_PNE;  
		/*printf(" Demande de calcul de coupes dans PROFONDEUR_DANS_LARGEUR \n");*/		
  } 
  else {
    if ( NbProblemesResolus >= NEUTRALISATION_CALCUL_COUPES ) {
      QuotientEtReste = ldiv( (int)(ProfondeurDuNoeud - ProfondeurDuNoeudDeDepart) , (int)CYCLE_DE_PROFONDEUR_POUR_LE_CALCUL_DES_GOMORY ); 
      if ( QuotientEtReste.rem == 0 ) {
        Bb->CalculerDesCoupes         = OUI_PNE;
        Bb->CalculerDesCoupesDeGomory = OUI_PNE;
      }
    }
  }
   
  /* Evaluation du noeud de plus petit minorant attendu */
  NoeudCourant = NoeudsAExplorer[0]; 
  MinorantPredit0Initialise = NON; 
  if ( NoeudCourant != 0 ) {
    if( NoeudCourant->NoeudTerminal != OUI && NoeudCourant->StatutDuNoeud != A_REJETER && NoeudCourant->StatutDuNoeud == A_EVALUER ) {
      MinorantPredit0 = NoeudCourant->MinorantPredit;
      MinorantPredit0Initialise = OUI;
		
    }
  }   
  NoeudCourant = NoeudsAExplorer[1]; 
  MinorantPredit1Initialise = NON; 
  if ( NoeudCourant != 0 ) {
    if( NoeudCourant->NoeudTerminal != OUI && NoeudCourant->StatutDuNoeud != A_REJETER && NoeudCourant->StatutDuNoeud == A_EVALUER ) {
      MinorantPredit1 = NoeudCourant->MinorantPredit;	
      MinorantPredit1Initialise = OUI;						
    }
  }

  UnIndiceAEteChoisi     = NON;
  Les2ChoixSontPossibles = NON;
  if ( MinorantPredit0Initialise == OUI ) {
    IndiceChoisi       = 0;
    UnIndiceAEteChoisi = OUI;
    if ( MinorantPredit1Initialise == OUI ) {
      Les2ChoixSontPossibles = OUI;      
      if ( MinorantPredit1 < MinorantPredit0 ) { 
        IndiceChoisi           = 1;
        UnIndiceAEteChoisi     = OUI;	
      }
    } 
  } 
  else {
    if ( MinorantPredit1Initialise == OUI ) { 
      IndiceChoisi       = 1;
      UnIndiceAEteChoisi = OUI;
    }
  } 
												     
  /* Evaluation du noeud choisi */
  if ( UnIndiceAEteChoisi == OUI ) {

    NoeudCourant = NoeudsAExplorer[IndiceChoisi];
    if( NoeudCourant->StatutDuNoeud == EVALUE && NoeudCourant->LaSolutionRelaxeeEstEntiere == OUI ) { 
      Bb->NbNoeuds1_PNE_BalayageEnProfondeur = 0; 
      Bb->NoeudEnExamen = 0;
      return( OUI );
    }
    #if VERBOSE_BB
      if ( Bb->TypeDExplorationEnCours == PROFONDEUR_TOUT_SEUL ) { 
        printf(" ************************************************ \n"); 
        printf(" Noeud (%d) %x / Recherche en profondeur seule, profondeur du noeud %d\n",IndiceChoisi,
				         NoeudCourant,ProfondeurDuNoeud);
      }
      else { 
        printf(" ************************************************ \n"); 
        printf(" Noeud (%d) %x / Recherche en profondeur dans un balayage en largeur, profondeur de depart %d , profondeur du noeud %d profondeur limite %d\n",
                 IndiceChoisi, NoeudCourant,ProfondeurDuNoeudDeDepart,ProfondeurDuNoeud,
								 ProfondeurDuNoeudDeDepart+ProfondeurLimite); 
      }
      fflush(stdout);
    #endif
    
    if ( NoeudCourant == Bb->NoeudRacine ) {
      /* Calcul de coupes au noeud racine */
      Bb->CalculerDesCoupes         = OUI_PNE;
      Bb->CalculerDesCoupesDeGomory = OUI_PNE;
    }            
    
    NbProblemesResolus++;
    YaUneSolution = BB_ExaminerUnNoeudEnProfondeur( Bb, NoeudCourant, &SolutionEntiereTrouvee 
                                                    /*, NombreDeNoeudsAExplorer , NoeudsAExplorer 
                                                    , NombreDeNoeudsAExplorerAuProchainEtage , NoeudsAExplorerAuProchainEtage*/ );
																									
    if ( SolutionEntiereTrouvee == OUI ) {			
      /* On a trouve une solution entiere */			
      /*printf("!!!!!!!!!!!!!!!!!!!!!! Fin par atteinte de SolutionEntiereTrouvee \n");*/
      Bb->NbNoeuds1_PNE_BalayageEnProfondeur = 0;
      Bb->NoeudEnExamen = 0;	
      return( OUI ); 
    }
    else {

      if ( Bb->TypeDExplorationEnCours == PROFONDEUR_DANS_LARGEUR && YaUneSolution == OUI ) {	
		    if ( IndexMoyenneMobile < TAILLE_MOYENNE_MOBILE ) {
	        if ( Pne->NombreDeVariablesAValeurFractionnaire > 0 ) {
					  VecteurPourMoyenneMobile[IndexMoyenneMobile] = Pne->NormeDeFractionnalite / Pne->NombreDeVariablesAValeurFractionnaire;
					}
					else VecteurPourMoyenneMobile[IndexMoyenneMobile] = 0;
	        IndexMoyenneMobile++;
        }
		    else {
		      /* On fait de la place a la fin */
		      for ( i = 0 ; i < TAILLE_MOYENNE_MOBILE - 1 ; i++ ) {
			      VecteurPourMoyenneMobile[i] = VecteurPourMoyenneMobile[i+1];
			    }
			    /* On range la valeur a la fin */
	        if ( Pne->NombreDeVariablesAValeurFractionnaire > 0 ) {
			      VecteurPourMoyenneMobile[TAILLE_MOYENNE_MOBILE - 1] = Pne->NormeDeFractionnalite / Pne->NombreDeVariablesAValeurFractionnaire;
					}
					else VecteurPourMoyenneMobile[TAILLE_MOYENNE_MOBILE - 1] = 0;					
		    }
			}
			
      /* Choix de la variable a instancier si le noeud est choisi au prochain etage */      
      BB_CreerLesNoeudsFils( Bb, NoeudCourant );
      if ( Bb->SolutionEntiereTrouveeParHeuristique == OUI ) {			
        /* printf("!!!!!!!!!!!!!!!!! Fin par atteinte de SolutionEntiereTrouveeParHeuristique \n");	 */			
        Bb->NbNoeuds1_PNE_BalayageEnProfondeur = 0;
        Bb->NoeudEnExamen = 0;
        return( OUI );				
		  }
			
    }  
      
    /* Si le critere du probleme (sans nouvelles coupes) est plus eleve que le minorant predit de l'autre noeud, 
       on evalue aussi l'autre noeud */
    EvaluerLAutreNoeud = NON; 
    if ( Les2ChoixSontPossibles == OUI ) {
      if ( IndiceChoisi == 1 ) IndiceChoisi = 0; 
      else IndiceChoisi = 1; 
      MinorantPreditDeLAutreNoeud = NoeudsAExplorer[IndiceChoisi]->MinorantPredit;
      /* Si on est en phase de recherche en largeur, on evalue les 2 noeuds */
      if ( Bb->TypeDExplorationEnCours == PROFONDEUR_DANS_LARGEUR ) {
        /* Pour ne pas trop se perdre dans les calculs on se contente de diminuer le minorant predit */
        MinorantPreditDeLAutreNoeud *= 0.5;								
      }
            
      if ( PNE_ValeurOptimaleDuProblemeCourantAvantNouvellesCoupes( Pne ) >= MinorantPreditDeLAutreNoeud - MARGE_SUR_LES_MINORANTS ) { 
        #if VERBOSE_BB
          printf(" Minorant calcule au noeud %lf minorant de l'autre noeud %e\n",
                   PNE_ValeurOptimaleDuProblemeCourantAvantNouvellesCoupes( Pne ),MinorantPreditDeLAutreNoeud);
        #endif
        EvaluerLAutreNoeud = OUI; 
      }
						
      if ( YaUneSolution == NON ) EvaluerLAutreNoeud = OUI;
      /* Si le noeud au dessus est le noeud racine, on evalue les 2 noeuds */ 
      if ( NoeudCourant->ProfondeurDuNoeud <= 2 ) EvaluerLAutreNoeud = OUI;
      
    }
		    
    if ( EvaluerLAutreNoeud == OUI ) {
      NoeudCourant = NoeudsAExplorer[IndiceChoisi];
      if( NoeudCourant->StatutDuNoeud == EVALUE && NoeudCourant->LaSolutionRelaxeeEstEntiere == OUI ) { 
        Bb->NbNoeuds1_PNE_BalayageEnProfondeur = 0;
        Bb->NoeudEnExamen = 0;       
        return( OUI );
      }      
      #if VERBOSE_BB
        if ( Bb->TypeDExplorationEnCours == PROFONDEUR_TOUT_SEUL ) { 
          printf(" ************************************************ \n"); 
          printf(" Noeud (%d) %x (erreur de minorant predit) / Recherche en profondeur seule, profondeur du noeud %d\n",
                   IndiceChoisi,NoeudCourant,ProfondeurDuNoeud);
        }
        else { 
          printf(" ************************************************ \n"); 
          printf(" Noeud (%d) %x (erreur de minorant predit) / Recherche en profondeur dans un balayage en largeur, profondeur de depart %d , profondeur du noeud %d profondeur limite %d\n",
                 IndiceChoisi,NoeudCourant,ProfondeurDuNoeudDeDepart,ProfondeurDuNoeud,ProfondeurDuNoeudDeDepart+ProfondeurLimite); 
        }
        fflush(stdout);
      #endif
 
      if ( NoeudCourant == Bb->NoeudRacine ) {
        /* Calcul de coupes au noeud racine */
        Bb->CalculerDesCoupes         = OUI_PNE;
        Bb->CalculerDesCoupesDeGomory = OUI_PNE;
      }     
            
      NbProblemesResolus++;
      YaUneSolution = BB_ExaminerUnNoeudEnProfondeur( Bb, NoeudCourant, &SolutionEntiereTrouvee 
                                                      /*, NombreDeNoeudsAExplorer , NoeudsAExplorer 
                                                      , NombreDeNoeudsAExplorerAuProchainEtage , NoeudsAExplorerAuProchainEtage*/ );
      if ( SolutionEntiereTrouvee == OUI ) {
        /*printf("!!!!!!!!!!!!!!!!!!!!!! Fin par atteinte de SolutionEntiereTrouvee \n");*/
        /* On a trouve une solution entiere */    
        Bb->NbNoeuds1_PNE_BalayageEnProfondeur = 0;
        Bb->NoeudEnExamen = 0;
	      return( OUI );	                   	         
      }
      else {
			
        if ( Bb->TypeDExplorationEnCours == PROFONDEUR_DANS_LARGEUR && YaUneSolution == OUI ) {	
		      if ( IndexMoyenneMobile < TAILLE_MOYENNE_MOBILE ) {
	          if ( Pne->NombreDeVariablesAValeurFractionnaire > 0 ) {
	            VecteurPourMoyenneMobile[IndexMoyenneMobile] = Pne->NormeDeFractionnalite / Pne->NombreDeVariablesAValeurFractionnaire;
						}
						else VecteurPourMoyenneMobile[IndexMoyenneMobile] = 0;
	          IndexMoyenneMobile++;
          }
		      else {
		        /* On fait de la place a la fin */
		        for ( i = 0 ; i < TAILLE_MOYENNE_MOBILE - 1 ; i++ ) {
			        VecteurPourMoyenneMobile[i] = VecteurPourMoyenneMobile[i+1];
			      }
			      /* On range la valeur a la fin */
	          if ( Pne->NombreDeVariablesAValeurFractionnaire > 0 ) {
			        VecteurPourMoyenneMobile[TAILLE_MOYENNE_MOBILE - 1] = Pne->NormeDeFractionnalite / Pne->NombreDeVariablesAValeurFractionnaire;
						}
						else VecteurPourMoyenneMobile[TAILLE_MOYENNE_MOBILE - 1] = 0;
		      }
			  }
				
        /* Choix de la variable a instancier si le noeud est choisi au prochain etage */
        BB_CreerLesNoeudsFils( Bb, NoeudCourant );
        if ( Bb->SolutionEntiereTrouveeParHeuristique == OUI ) {
          /* printf("!!!!!!!!!!!!!!!!!!!!!! Fin par atteinte de SolutionEntiereTrouveeParHeuristique \n");	*/		
          Bb->NbNoeuds1_PNE_BalayageEnProfondeur = 0;
          Bb->NoeudEnExamen = 0;					
          return( OUI );
				}
      }        
    }
  }
  
  /* Preparation de l'etage suivant */
  NoeudPereSuivant = BB_NoeudPereSuivantDansRechercheEnProfondeur( NoeudsAExplorer ); 
                       
  if ( NoeudPereSuivant == 0 ) {	
    /*printf("Pas de NoeudPereSuivant NbProblemesResolus = %d\n",NbProblemesResolus);*/		
    NoeudPereSuivant = BB_RemonterDansRechercheEnProfondeur( NoeudAntecedent, ProfondeurDuNoeudDeDepart );
    if ( NoeudPereSuivant != 0 ) { 
      /* Le NoeudPereSuivant est le pere de celui la */
      NoeudPereSuivant = NoeudPereSuivant->NoeudAntecedent;  
    }
    else {
      /*printf("Fin par NoeudPereSuivant = 0 \n");*/			
      /*printf("Confirmation pas de NoeudPereSuivant NbProblemesResolus = %d\n",NbProblemesResolus);*/
      break; /* Pas de solution */
    }

    /* Normalement les 2 fils doivent deja exister sinon c'est un bug */
    if ( NoeudPereSuivant->NoeudSuivantGauche == 0 || NoeudPereSuivant->NoeudSuivantDroit == 0 ) {
      printf(" BUG dans la recherche en profondeur: le noeud pere n'a pas ses 2 fils \n");
      Bb->AnomalieDetectee = OUI;
      longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
    }
  }                   
 
  NoeudAntecedent   = NoeudPereSuivant;
  ProfondeurDuNoeud = NoeudAntecedent->ProfondeurDuNoeud; /* Profondeur du prochain etage */  
  ProfondeurDuNoeud++; /* Profondeur du prochain etage */  

/* printf("NoeudPereSuivant %08X \n",NoeudPereSuivant); fflush(stdout); */

                 /* Renseigner la table des noeuds du prochain etage */
  NoeudsAExplorer[0] = NoeudPereSuivant->NoeudSuivantGauche; 
  NoeudsAExplorer[1] = NoeudPereSuivant->NoeudSuivantDroit; 

/* Fin while */  				     
}

Bb->NbNoeuds1_PNE_BalayageEnProfondeur = 0; /* Pour le nettoyage eventuel */

#if VERBOSE_BB
  printf("\n Le probleme n'a pas de solution entiere dans la recherche en profondeur \n"); fflush(stdout);
#endif
   
Bb->NoeudEnExamen = 0;
SolutionEntiereTrouvee = NON;
/*printf(" !!!!!!!!!!!!! Pas de solution trouvee dans la recherche en profondeur \n");*/
return( NON ); /* return avec l'indicateur ya pas de solution */
}

/*-----------------------------------------------------------------------------------------------*/

int BB_ExaminerUnNoeudEnProfondeur( BB * Bb, NOEUD * NoeudCourant,
                                    int * SolutionEntiereTrouvee 
                                    /*int NombreDeNoeudsAExplorer,
																		  NOEUD ** NoeudsAExplorer,
                                      int NombreDeNoeudsAExplorerAuProchainEtage,
																		  NOEUD ** NoeudsAExplorerAuProchainEtage*/ )
{
int YaUneSolution;

Bb->NoeudEnExamen = NoeudCourant;
			     /* Exploration du noeud */
YaUneSolution = BB_ResoudreLeProblemeRelaxe( Bb, NoeudCourant, SolutionEntiereTrouvee ); 

BB_NettoyerLArbre( Bb, &YaUneSolution , NoeudCourant ); /* Fait aussi la mise a jour du statut */

return( YaUneSolution );
}

/*-----------------------------------------------------------------------------------------------*/
/*
       Recherche d'un noeud pere suivant en descendant dans l'arbre. Le noeud pere sera 
       eventuellement divise.
                                                                                                 */

NOEUD * BB_NoeudPereSuivantDansRechercheEnProfondeur( NOEUD ** NoeudsAExplorer ) 
{
NOEUD * NoeudGauche; NOEUD * NoeudDroit; NOEUD * NoeudPereSuivant; NOEUD * NoeudFils;
double PlusPetitMinorantPredit;

/* 
Noeud pere suivant:
- Ne doit pas etre TERMINAL
- Ne doit pas etre A_REJETER
- Est celui des 2 qui a le plus petit minorant
*/

NoeudPereSuivant = 0;

/* printf("\n Recherche d un NoeudPereSuivant sur NoeudsAExplorer[0] et NoeudsAExplorer[1] \n"); fflush(stdout); */
				       
NoeudGauche = NoeudsAExplorer[0];
NoeudDroit  = NoeudsAExplorer[1];

PlusPetitMinorantPredit = PLUS_LINFINI;

if ( NoeudGauche != 0 && NoeudDroit != 0 ) {
  /* Les 2 noeuds existent: on prend celui dont les fils ont le plus petit minorant predit */
  if ( NoeudGauche->NoeudTerminal == OUI || NoeudGauche->StatutDuNoeud == A_REJETER ) {
    /* Le noeud gauche est inutilisable, il reste donc le noeud droit */
    if ( NoeudDroit->NoeudTerminal != OUI && NoeudDroit->StatutDuNoeud != A_REJETER ) { 
      NoeudPereSuivant = NoeudDroit; 
    }
    return( NoeudPereSuivant );    
  }
  if ( NoeudDroit->NoeudTerminal == OUI || NoeudDroit->StatutDuNoeud == A_REJETER ) {
    /* Le noeud droit est inutilisable, il reste donc le noeud gauche */
    if ( NoeudGauche->NoeudTerminal != OUI && NoeudGauche->StatutDuNoeud != A_REJETER ) { 
      NoeudPereSuivant = NoeudGauche;
    }
    return( NoeudPereSuivant );    
  }
  /* On peut utiliser les 2 noeud => on prend celui qui a le plus petit minorant predit */
  /*
  NoeudPereSuivant = NoeudGauche;
  if ( NoeudDroit->MinorantDuCritereAuNoeud < NoeudGauche->MinorantDuCritereAuNoeud ) { 
    NoeudPereSuivant = NoeudDroit;
  }
  */
  /* Alternative: On peut utiliser les 2 noeud => on prend celui dont les fils ont les plus petits minorant predit */
  NoeudFils = NoeudGauche->NoeudSuivantGauche;
  if ( NoeudFils != 0 ) {
    if ( NoeudFils->MinorantPredit < PlusPetitMinorantPredit ) {
      PlusPetitMinorantPredit = NoeudFils->MinorantPredit;
      NoeudPereSuivant        = NoeudGauche;
    }
  }
  NoeudFils = NoeudGauche->NoeudSuivantDroit;
  if ( NoeudFils != 0 ) {
    if ( NoeudFils->MinorantPredit < PlusPetitMinorantPredit ) {
      PlusPetitMinorantPredit = NoeudFils->MinorantPredit;
      NoeudPereSuivant        = NoeudGauche;
    }
  }
  NoeudFils = NoeudDroit->NoeudSuivantGauche;
  if ( NoeudFils != 0 ) {
    if ( NoeudFils->MinorantPredit < PlusPetitMinorantPredit ) {
      PlusPetitMinorantPredit = NoeudFils->MinorantPredit;
      NoeudPereSuivant        = NoeudDroit;
    }
  }
  NoeudFils = NoeudDroit->NoeudSuivantDroit;
  if ( NoeudFils != 0 ) {
    if ( NoeudFils->MinorantPredit < PlusPetitMinorantPredit ) {
      PlusPetitMinorantPredit = NoeudFils->MinorantPredit;
      NoeudPereSuivant        = NoeudDroit;
    }
  }

  return( NoeudPereSuivant );
}

/* On a soit NoeudGauche = 0 soit NoeudDroit = 0 soit les deux */
if ( NoeudGauche != 0 ) {
  /* Alors NoeudDroit = 0 n'est pas utilisable */
  if ( NoeudGauche->NoeudTerminal != OUI && NoeudGauche->StatutDuNoeud != A_REJETER ) { 
    NoeudPereSuivant = NoeudGauche;
  }
  return( NoeudPereSuivant );
}

if ( NoeudDroit != 0 ) {
  /* Alors NoeudGauche = 0 n'est pas utilisable */
  if ( NoeudDroit->NoeudTerminal != OUI && NoeudDroit->StatutDuNoeud != A_REJETER ) { 
    NoeudPereSuivant = NoeudDroit;
  }
  return( NoeudPereSuivant );
}

return( NoeudPereSuivant );    

}

/*-----------------------------------------------------------------------------------------------*/
/* 
      Cas ou on n'a pas trouve de noeud pere en descendant. On remonte donc l'arbre jusqu'a  
      trouver un noeud pere qui sera eventuellement divise.                   			 
                                                                                                 */

NOEUD * BB_RemonterDansRechercheEnProfondeur( NOEUD * NoeudAntecedent, int ProfondeurDuNoeudDeDepart )                        
{
NOEUD * Noeud; NOEUD * NoeudPereSuivant; NOEUD * NoeudCourant; 
/* 
       Aucun noeud a diviser => 2 possibilites: 
       - Les noeuds de l'etage examine ont deja ete divises et ne sont pas terminaux. Dans ce cas on descend sur les fils.
       - Les 2 noeuds sont terminaux. Comme on n'a pas encore trouve de solution, on remonte dans l'arbre pour prendre un 
         autre chemin non encore explore. 
*/

NoeudPereSuivant = 0;
NoeudCourant     = NoeudAntecedent; 
   
while ( NoeudCourant != 0 && NoeudCourant->ProfondeurDuNoeud >= ProfondeurDuNoeudDeDepart ) { 
  /*
  printf("\n Recherche en profondeur, on remonte d'un cran NoeudCourant %d ProfondeurDuNoeud %d ProfondeurDuNoeudDeDepart %d\n",
             NoeudCourant,NoeudCourant->ProfondeurDuNoeud,ProfondeurDuNoeudDeDepart);
  */
  /* 
     On s'arrete se remonter lorsqu'on a trouve un NoeudCourant:
     - qui n'est ni A_REJETER ni Terminal
     et dont un des fils:
     - n'est ni A_REJETER ni Terminal
     Dans ce cas le NoeudPereSuivant est le fils  
  */

  if ( NoeudCourant->StatutDuNoeud != A_REJETER && NoeudCourant->NoeudTerminal != OUI ) {
    /* Le noeud n'est ni a rejeter ni terminal => on peut regarder */
    /* Si un de ses fils n'est ni a rejeter ni terminal, on le prend */
    Noeud = NoeudCourant->NoeudSuivantGauche; /* Fils 0 */
    if ( Noeud != 0 ) { 
      if ( Noeud->StatutDuNoeud != A_REJETER && Noeud->NoeudTerminal != OUI ) {
        NoeudPereSuivant = Noeud;  
        break;
      }
    }
    Noeud = NoeudCourant->NoeudSuivantDroit; /* Fils 1 */
    if ( Noeud != 0 ) { 
      if ( Noeud->StatutDuNoeud != A_REJETER && Noeud->NoeudTerminal != OUI ) {
        NoeudPereSuivant = Noeud;   
        break;
      }
    }
    /* Comme les 2 noeuds sont terminaux ou a rejeter, le pere est terminal et a rejeter aussi */
    NoeudCourant->StatutDuNoeud = A_REJETER; 
    NoeudCourant->NoeudTerminal = OUI;       
  }
  /* On remonte encore d'un cran */
  NoeudCourant = NoeudCourant->NoeudAntecedent;   
}

return( NoeudPereSuivant );    

}





