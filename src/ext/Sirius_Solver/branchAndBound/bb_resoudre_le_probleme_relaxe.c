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

   FONCTION: Resolution d'un probleme relaxe  
        
   AUTEUR: R. GONZALEZ

************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"  
  
#include "spx_define.h"
#include "spx_fonctions.h"

#include "pne_define.h"  
#include "pne_fonctions.h"

# ifdef BB_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "bb_memoire.h"
# endif  

#define MISE_EN_SERVICE_DES_COUPES               OUI /*OUI*/
/* En dessous de cette profondeur du noeud on ne calcule plus de coupes */
/* En pourcentage de la profondeur limite */
#define PROFONDEUR_LIMITE_POUR_CALCUL_DE_COUPES  0.1  /*0.05*/ /* negatif si profondeur infinie */
#define SEUIL_PROFONDEUR_LIMITE_1   30 /*30*/ /* Profondeur limite de calcul de coupes tant qu'on n'a pas de solution entiere */
#define SEUIL_PROFONDEUR_LIMITE_2   10 /*10*/ /* Profondeur limite de calcul de coupes quand on a une solution entiere */
#define REOPTIMISATION_DU_NOEUD_APRES_COUPES     OUI  

#define DERNIER_ROUND_DE_COUPES_AU_NOEUD_RACINE   20 /*20*/
#define DERNIER_ROUND_DE_COUPES_AUX_AUTRES_NOEUDS 2

# define NB_MAX_NOEUDS_ARRET_COUPES 1000
 
# define CYCLE_CALCUL_COUPES 100
# define CYCLE_CALCUL_COUPES_RAPIDE_1 5
# define CYCLE_CALCUL_COUPES_RAPIDE_2 2
# define PROFONDEUR_AUGMENTATION_FREQUENCE_COUPES_1 20
# define PROFONDEUR_AUGMENTATION_FREQUENCE_COUPES_2 50

# define PROFONDEUR_LIMITE_PRUNING_COUPES 100

/* Pilotage additionnel du nombre de round de coupes */
#define POURCENTAGE_VARIATION_CRITERE         0.0001 /*0.0001*/ /* Seuil de variation en pourcentage pour autoriser le rebouclage */
#define MAX_SI_POURCENTAGE_VARIATION_CRITERE  1      /*10*/ /* Valeur max qui s'applique au pourcentage ci-dessus */
#define VALEUR_ABSOLUE_VARIATION_CRITERE      0.001  /*0.001*/ /* Seuil de variation en valeur absolue pour autoriser le rebouclage */

# define MULTIPLICATEUR_TEMPS 3.0 /*2.0*/
# define LIMITE_TEMPS 60.0

# define FIN_COUPES_NOEUD_RACINE_SUR_TEMPS_MAX  NON_PNE /* Si NON_PNE on sort en nombre max d'iterations de simplexe pendant le calcul des coupes */

# define MULTIPLICATEUR_NB_ITERATIONS_COUPES_NOEUD_RACINE 1

# define MIN_ITERATION_COUPES_NOEUD_RACINE 1000

/*---------------------------------------------------------------------------------------------------------*/
/* Ca retourne OUI_PNE ou NON_Pne-> Si OUI_PNE on archive toutes les coupes y compris celles qui ne sont
   pas saturees. Si NON_PNE on n'archive pas les coupes qui ne sont pas saturees */
   
char BB_ArchiverToutesLesCoupesLorsDuTri( BB * Bb )
{
char ArchiverToutesLesCoupes;

if ( MISE_EN_SERVICE_DES_COUPES == NON ) {
  ArchiverToutesLesCoupes = NON_PNE;
  return( ArchiverToutesLesCoupes );
}
/* Donc on calcule des coupes */
if ( REOPTIMISATION_DU_NOEUD_APRES_COUPES == NON ) {
  /* Comme on ne reoptimise pas apres le calcul des coupes, on les conserve toutes */
  ArchiverToutesLesCoupes = OUI_PNE;	
  return( ArchiverToutesLesCoupes );
}
/* Comme on reoptimise le noeud apres le calcul des coupes, on n'archive que les coupes saturees */
/* Sauf eventuelle au noeud racine (c'est pas encore tranche) */

if ( Bb->NoeudEnExamen == Bb->NoeudRacine ) {
  ArchiverToutesLesCoupes = OUI_PNE;	
  return( ArchiverToutesLesCoupes );
}
ArchiverToutesLesCoupes = NON_PNE;
if ( Bb->NoeudEnExamen->ProfondeurDuNoeud <= 2 ) ArchiverToutesLesCoupes = OUI_PNE;
return( ArchiverToutesLesCoupes );
}

/*---------------------------------------------------------------------------------------------------------*/

void BB_LeverLeFlagPourEnleverToutesLesCoupes( BB * Bb )
{
  Bb->EnleverToutesLesCoupesDuPool = OUI;
  return;
}

/*---------------------------------------------------------------------------------------------------------*/

void BB_LeverLeFlagDeSortieDuSimplexeParDepassementDuCoutMax( BB * Bb )
{
  Bb->SortieParDepassementDuCoutMax = OUI;
  Bb->NombreDeNoeudsElagues++;
  Bb->SommeDesProfondeursDElaguage += Bb->NoeudEnExamen->ProfondeurDuNoeud;				
  return;
}

/*---------------------------------------------------------------------------------------------------------*/

int BB_ResoudreLeProblemeRelaxe( BB * Bb,
				                         NOEUD * NoeudCourant, 
                                 int *  SolutionEntiereTrouvee 
                                )
{
int i; int YaUneSolution; double ValeurOptimale; double Marge; char CalculerDesCoupes; char NumeroDePasse;
int NbCoupesAlloc; char ReFaireUnCalculSansCalculerDesCoupes; int NbVarDeBaseComplementairesSv; int * ComplementDeLaBaseSv;
int * PositionDeLaVariableSv; int YaUneSolutionSv; int MargeAlloc; char RebouclageApresNettoyageDesCoupesFait;
char ChoisirLaVariableAInstancier; char MiseEnServiceDesCoupes; int ProfondeurLimite; char DemandeExpresse;
int DernierRoundDeCoupes; char PremiereResolutionAuNoeudRacine; char ReoptimisationDuNoeudApresCoupes; double AncienneValeurDuCritere; 
char YaDesCoupesAjoutees; PROBLEME_PNE * Pne; double TempsEcoule; double X; int CycleCalculCoupes; double LimiteTemps;
double Temps; char PasDeSolutionEntiereDansSousArbre; time_t HeureDeCalendrierDebut; time_t HeureDeCalendrierCourant;
int NbSat; int j; double m; PROBLEME_SPX * Spx; int NbIterationSimplexeCoupes; int MxIterationsSimplexeCoupesNoeudRacine;

Pne = (PROBLEME_PNE *) Bb->ProblemePneDuBb;

ReoptimisationDuNoeudApresCoupes = REOPTIMISATION_DU_NOEUD_APRES_COUPES;

MiseEnServiceDesCoupes = MISE_EN_SERVICE_DES_COUPES;

CycleCalculCoupes = CYCLE_CALCUL_COUPES;
if ( Bb->NombreDeNoeudsElagues != 0 ) {
  if ( (int) ceil( Bb->SommeDesProfondeursDElaguage / Bb->NombreDeNoeudsElagues ) > PROFONDEUR_AUGMENTATION_FREQUENCE_COUPES_2 ) {
    CycleCalculCoupes = CYCLE_CALCUL_COUPES_RAPIDE_2;
	}
	else if ( (int) ceil( Bb->SommeDesProfondeursDElaguage / Bb->NombreDeNoeudsElagues ) > PROFONDEUR_AUGMENTATION_FREQUENCE_COUPES_1 ) {
    CycleCalculCoupes = CYCLE_CALCUL_COUPES_RAPIDE_1;
	}
}

/* On force le calcul des coupes au noeud racine */
if ( NoeudCourant == Bb->NoeudRacine ) MiseEnServiceDesCoupes = OUI;

if ( PROFONDEUR_LIMITE_POUR_CALCUL_DE_COUPES >= 0.0 ) {
  ProfondeurLimite = (int) ceil( PROFONDEUR_LIMITE_POUR_CALCUL_DE_COUPES * Bb->NombreDeVariablesEntieresDuProbleme );
  /*
  printf("Bb->NombreDeVariablesEntieresDuProbleme %d  ProfondeurLimite %d\n",Bb->NombreDeVariablesEntieresDuProbleme,ProfondeurLimite);
  */
  if ( Bb->NoeudDeLaMeilleureSolutionEntiere == 0 ) {
    /* On n'a pas encore trouve de solution entiere */
    if ( ProfondeurLimite > SEUIL_PROFONDEUR_LIMITE_1 ) ProfondeurLimite = SEUIL_PROFONDEUR_LIMITE_1;
  }
  else {
    if ( ProfondeurLimite > SEUIL_PROFONDEUR_LIMITE_2 ) ProfondeurLimite = SEUIL_PROFONDEUR_LIMITE_2;
  }
  if ( NoeudCourant->ProfondeurDuNoeud > ProfondeurLimite ) {
    MiseEnServiceDesCoupes = NON;
  }  
}

/* Si on a depasse la profondeur limite on calcule des coupes lorsque le nombre de noeuds evalues est multiple
   d'un certain nombre */
if ( Bb->NombreDeSolutionsEntieresTrouvees > 0 || 1 ) {
  if ( (Bb->NombreDeProblemesResolus % CycleCalculCoupes) == 0 ) {
    MiseEnServiceDesCoupes = OUI;
    Bb->CalculerDesCoupes = OUI_PNE;
    Bb->CalculerDesCoupesDeGomory = OUI_PNE;
  }
}

/* Test: on calcule des coupes si on est au dessus de l'average pruning depth */
/* Il faut que le nombre de noeuds elagues soit significatif pour calculer une moyenne */
if ( Bb->NombreDeNoeudsElagues > 10 && NoeudCourant->ProfondeurDuNoeud < PROFONDEUR_LIMITE_PRUNING_COUPES && 0 ) {
  if ( NoeudCourant->ProfondeurDuNoeud < (int) ceil( Bb->SommeDesProfondeursDElaguage / Bb->NombreDeNoeudsElagues ) ) {
    MiseEnServiceDesCoupes = OUI;
    Bb->CalculerDesCoupes = OUI_PNE;
    Bb->CalculerDesCoupesDeGomory = OUI_PNE;		
	}
}
/* Fin test */

DernierRoundDeCoupes = DERNIER_ROUND_DE_COUPES_AU_NOEUD_RACINE;
if ( NoeudCourant != Bb->NoeudRacine ) {
  DernierRoundDeCoupes = 1;
  if ( REOPTIMISATION_DU_NOEUD_APRES_COUPES == OUI && MiseEnServiceDesCoupes == OUI ) {
    DernierRoundDeCoupes = DERNIER_ROUND_DE_COUPES_AUX_AUTRES_NOEUDS;
    if ( NoeudCourant->ProfondeurDuNoeud <= 2 ) DernierRoundDeCoupes+= 2; 
  }  
}

/* Test */
/* Si le noeud a un minorant a moins de 1% de la meilleure solution, on active le calcul des coupes */
if ( Bb->NoeudDeLaMeilleureSolutionEntiere != 0 && NoeudCourant != Bb->NoeudRacine && MiseEnServiceDesCoupes == NON ) {
  if( fabs( NoeudCourant->MinorantPredit - Bb->CoutDeLaMeilleureSolutionEntiere ) < 0.01 * fabs( Bb->CoutDeLaMeilleureSolutionEntiere ) ) {
    MiseEnServiceDesCoupes = OUI;
    DernierRoundDeCoupes   = 3;
    Bb->CalculerDesCoupes = OUI_PNE;
    Bb->CalculerDesCoupesDeGomory = OUI_PNE;		
  }
}
/* Fin test */

/* Pas de coupes si pas de variables entieres */ 
if ( Bb->NombreDeVariablesEntieresDuProbleme <= 0 ) {
  MiseEnServiceDesCoupes           = NON;
  ReoptimisationDuNoeudApresCoupes = NON;
}

BB_AfficherLesTraces( Bb, NoeudCourant );

if ( Bb->NoeudRacine->NombreDeCoupesGenereesAuNoeud >= Bb->NombreMaxDeCoupes ) {
  /* Si le nombre moyen de coupes utilisees est trop petit par rapport au nombre de coupes et que le
     nombre de coupes est trop grand, on nettoie */
  if ( Bb->AffichageDesTraces == OUI ) {
    /*printf("-> Demande de nettoyage des coupes car trop de coupes\n");*/  
  }	
  DemandeExpresse = NON; /* Il vaut mieux mettre NON car ca destabilise la recherche. Le
                            OUI se justifie dans le cas d'instabilites numeriques ou les coupes
			                      foutent le bordel dans le simplexe */

  BB_NettoyerLesCoupes( Bb, DemandeExpresse );
}

/* Si on utilise trop peu de coupes du pool on nettoie vraiment toutes les coupes a condition qu'il y ait quand-meme
   quelques coupes dedans */
/* Lorsque le pool est plein, il n'est pas judicieux de le nettoyer car cela peut faire baiser des minorants
   et bien que l'on puisse calculer de nouvelles coupes, elles sont moins bonnes car ce sont les premieres
	 qui sont les meilleurs etant donne qu'elles ont ete calculees au sommet de l'arbre */
if ( Bb->NoeudRacine->NombreDeCoupesGenereesAuNoeud > 10 ) {
  if ( Bb->NombreDeProblemesResolusDepuisLaRAZDesCoupes > 100 ) {		
    i = (int)(Bb->NombreTotalDeCoupesDuPoolUtilisees/Bb->NombreDeProblemesResolusDepuisLaRAZDesCoupes);
    /* Si moins de x% des coupes est utilise on nettoie */
    X = 0.01;
    if ( ( i * 100. ) / Bb->NoeudRacine->NombreDeCoupesGenereesAuNoeud < X /*&& i < 10*/ ) {
      if ( Bb->AffichageDesTraces == OUI ) {
				printf("Cleaning the pool cuts because too much cuts were flagged as useless\n");
				/* Pour forcer le reaffichage de la legende */
				Bb->NombreDAffichages = CYCLE_DAFFICHAGE_LEGENDE; 
      }		 
      DemandeExpresse = OUI;
      BB_NettoyerLesCoupes( Bb, DemandeExpresse );     
    }
  }

  /* Si on a arrete le calcul des coupes depuis trop longtemps */  
  if ( Bb->NbMaxDeCoupesCalculeesAtteint == OUI ) {
    if ( Bb->NombreDeNoeudsEvaluesSansCalculdeCoupes > NB_MAX_NOEUDS_ARRET_COUPES ) {		
      if ( fabs( Bb->EcartBorneInfALArretDesCoupes - Bb->EcartBorneInf ) < 0.01 * Bb->EcartBorneInfALArretDesCoupes ) {		
      /*if ( fabs( Bb->EcartBorneInfALArretDesCoupes - Bb->EcartBorneInf ) < 0.5 * Bb->EcartBorneInfALArretDesCoupes ) {*/			
        if ( Bb->AffichageDesTraces == OUI ) {
				  printf("Cleaning the pool cuts to make room for new cuts\n");
				  /* Pour forcer le reaffichage de la legende */
				  Bb->NombreDAffichages = CYCLE_DAFFICHAGE_LEGENDE;
				}
		    DemandeExpresse = OUI;
        BB_NettoyerLesCoupes( Bb, DemandeExpresse );
			  Bb->NombreDeNoeudsEvaluesSansCalculdeCoupes = 0;
			}
		}
  }		
}

if ( Bb->NbMaxDeCoupesCalculeesAtteint == OUI ) {
  if ( Bb->NombreDeNoeudsEvaluesSansCalculdeCoupes == 0 && Bb->NoeudDeLaMeilleureSolutionEntiere != NULL ) {
    Bb->EcartBorneInfALArretDesCoupes = Bb->EcartBorneInf;
	}
  Bb->NombreDeNoeudsEvaluesSansCalculdeCoupes++;
}

NbVarDeBaseComplementairesSv = 0;
AncienneValeurDuCritere = 0.0;

RebouclageApresNettoyageDesCoupesFait = NON;
Bb->SolutionEntiereTrouveeParHeuristique = NON;

/* Si noeud racine on enclenche un chronometre pour eviter de perde trop de temps dans le
   rebouclage */

LimiteTemps = 3.0; /* 2.0 */
if ( Bb->NombreDeVariablesEntieresDuProbleme > 0 ) {
  if ( NoeudCourant == Bb->NoeudRacine ) time( &HeureDeCalendrierDebut );	 
}

NbIterationSimplexeCoupes = 0;
MxIterationsSimplexeCoupesNoeudRacine= MIN_ITERATION_COUPES_NOEUD_RACINE;

DebutGeneral:

ReFaireUnCalculSansCalculerDesCoupes = NON;
ComplementDeLaBaseSv                 = NULL;
PositionDeLaVariableSv               = NULL;

Debut:

if ( Bb->NbMaxDeCoupesCalculeesAtteint == OUI ) {	
  MiseEnServiceDesCoupes = NON;	
}

Bb->VariableProposeePourLInstanciation = -1;

/* Ajout des coupes */
Bb->NombreDeCoupesAjoutees = 0;
BB_InsererLesCoupesDansLeProblemeCourant( Bb, NoeudCourant ); 

free( Bb->CoupeSaturee );
free( Bb->CoupeSatureeAEntierInf );
free( Bb->CoupeSatureeAEntierSup );

Bb->CoupeSaturee           = NULL;
Bb->CoupeSatureeAEntierInf = NULL;
Bb->CoupeSatureeAEntierSup = NULL;

Bb->MajorantDuNombreDeCoupesAjouteesApresResolutionDuProblemeRelaxe = 0;

/* Appel du solveur pour la resolution du probleme relaxe */

Marge = 0.;
if ( Bb->UtiliserCoutDeLaMeilleureSolutionEntiere == OUI_SPX ) { 
  Marge = 1.e-6 * fabs ( Bb->CoutDeLaMeilleureSolutionEntiere );
  if ( Marge < 1.e-6 ) Marge = 1.e-6;
  if ( Marge > 1.e+1 ) Marge = 1.e+1;
}

if ( 
     Bb->UtiliserCoutDeLaMeilleureSolutionEntiere == NON_SPX || 
     ( 
       Bb->UtiliserCoutDeLaMeilleureSolutionEntiere == OUI_SPX && 
       ( NoeudCourant->MinorantPredit < Bb->CoutDeLaMeilleureSolutionEntiere + Marge )   
     )
     
   ) {

  #if VERBOSE_BB
    printf(" Nombre de problemes resolus: %d\n",Bb->NombreDeProblemesResolus); 
  #endif

  CalculerDesCoupes = Bb->CalculerDesCoupes;
  
  if ( ReFaireUnCalculSansCalculerDesCoupes == OUI ) { 
    CalculerDesCoupes = NON_PNE;
  }

  /* Inhibition eventuelle */
  if ( MiseEnServiceDesCoupes == NON ) CalculerDesCoupes = NON_PNE;  

  Bb->ComplementDeBaseModifie = NON;
  NumeroDePasse              = 1;

  Bb->NombreDeProblemesDepuisLeDernierAffichage++;

  Bb->ControlerLesCoupesNonInclusesPourUnNouvelleResolution = NON;
  if ( MiseEnServiceDesCoupes == NON ) Bb->ControlerLesCoupesNonInclusesPourUnNouvelleResolution = OUI;

  /* Finalement, d'apres les tests, c'etait pas judicuex de mettre OUI */  
  Bb->ControlerLesCoupesNonInclusesPourUnNouvelleResolution = NON; 

  ResolutionDuNoeud:

  Bb->DernierNoeudResolu = NoeudCourant;

  /* Sauvegarde la base complementaire pour le cas ou on ajouterai des coupes et que ca 
     convergerait pas ou mal. On ne peut malheureusement pas recuperer le resultat de la 
     premiere resolution car il ne correspond pas a l'etat de saturation des coupes */
  if ( ReFaireUnCalculSansCalculerDesCoupes == NON && 
       NumeroDePasse                        == 1 &&
       Bb->NombreDeVariablesEntieresDuProbleme > 0 ) { 

    ComplementDeLaBaseSv   = (int *) malloc( NoeudCourant->NbVarDeBaseComplementaires * sizeof(int) );
    PositionDeLaVariableSv = (int *) malloc( (Bb->NombreDeVariablesDuProbleme /*+ Bb->NombreDeContraintesDuProbleme*/) * sizeof(int) );    
    if ( ComplementDeLaBaseSv == NULL || PositionDeLaVariableSv == NULL ) { 
      printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_ResoudreLeProblemeRelaxe\n");
      Bb->AnomalieDetectee = OUI;
      longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
    }
    for ( i = 0 ; i < Bb->NombreDeVariablesDuProbleme /*+ Bb->NombreDeContraintesDuProbleme*/ ; i++ ) {
      PositionDeLaVariableSv[i] = NoeudCourant->PositionDeLaVariable[i]; 
    }
    for ( i = 0 ; i < NoeudCourant->NbVarDeBaseComplementaires ; i++ ) {
      ComplementDeLaBaseSv[i] = NoeudCourant->ComplementDeLaBase[i];  
    }
    NbVarDeBaseComplementairesSv = NoeudCourant->NbVarDeBaseComplementaires;

  }

  Bb->SortieParDepassementDuCoutMax = NON; 		      
  Bb->NombreDeProblemesRelaxesResolus++;

  ChoisirLaVariableAInstancier = OUI_PNE;
  if ( MiseEnServiceDesCoupes == OUI && ReFaireUnCalculSansCalculerDesCoupes == NON ) {
    if ( ReoptimisationDuNoeudApresCoupes == OUI ) {
      if ( NumeroDePasse != DernierRoundDeCoupes ) ChoisirLaVariableAInstancier = NON_PNE;
    }
  }

  /* Car ChoisirLaVariableAInstancier a pu etre mis a non juste au dessus */
	if ( CalculerDesCoupes == NON_PNE && Bb->NombreDeVariablesEntieresDuProbleme > 0 ) ChoisirLaVariableAInstancier = OUI_PNE;
  
  PremiereResolutionAuNoeudRacine = NON_PNE;
  if ( Bb->NoeudEnExamen == Bb->NoeudRacine ) {  
    if ( NumeroDePasse == 1 ) PremiereResolutionAuNoeudRacine = OUI_PNE;
  }

	if ( Bb->AffichageDesTraces == OUI_PNE ) {
	  if ( Bb->NoeudEnExamen == Bb->NoeudRacine ) {
      if ( NumeroDePasse > 1 && NumeroDePasse != DernierRoundDeCoupes ) printf("Root node: new round of cuts\n");
    }
	}
	
  PNE_BranchAndBoundSolvePbRlx( Pne,	
                       PremiereResolutionAuNoeudRacine,
											 NumeroDePasse,
                      &CalculerDesCoupes,
		                   ChoisirLaVariableAInstancier,
		                   &NoeudCourant->NombreDeVariablesEntieresInstanciees,
		                   NoeudCourant->IndicesDesVariablesEntieresInstanciees,
		                   NoeudCourant->ValeursDesVariablesEntieresInstanciees,
                       /* Specifique simplexe */
                      &Bb->CoutDeLaMeilleureSolutionEntiere,  
                      &Bb->UtiliserCoutDeLaMeilleureSolutionEntiere,
                      &NoeudCourant->BaseFournie, 
		                   NoeudCourant->PositionDeLaVariable, 
		                  &NoeudCourant->NbVarDeBaseComplementaires, 
                       NoeudCourant->ComplementDeLaBase, 
                       /* */  
		                  &ValeurOptimale,
		                  &YaUneSolution,
		                   SolutionEntiereTrouvee,
		                  &Bb->VariableProposeePourLInstanciation,
                      &Bb->MinorantEspereAEntierInf,
                      &Bb->MinorantEspereAEntierSup,
		                   Bb->ValeursCalculeesDesVariablesEntieresPourLeProblemeRelaxeCourant,
 		                   Bb->ValeursCalculeesDesVariablesPourLeProblemeRelaxeCourant,
                       /* Bases de depart associees à la variable proposée pour l'instanciation */  
                       Bb->PositionDeLaVariableAEntierInf      , 
                      &Bb->NbVarDeBaseComplementairesAEntierInf,    
                       Bb->ComplementDeLaBaseAEntierInf        ,    
                       Bb->PositionDeLaVariableAEntierSup      , 
                      &Bb->NbVarDeBaseComplementairesAEntierSup,    
                       Bb->ComplementDeLaBaseAEntierSup        ,
                       /* Indicateur de disponibilite des bases pour les fils */       		       
                      &Bb->BasesFilsDisponibles                 
		              );
  if ( Bb->NombreDeVariablesEntieresDuProbleme <= 0 ) goto FinResolutionDuProblemeRelaxe;
	
  /* S'il s'agit du noeud racine et que c'est la premiere optimisation ( sans coupes ) on stocke la base */
  if ( Bb->NoeudEnExamen == Bb->NoeudRacine && NumeroDePasse == 1 && YaUneSolution == OUI ) {

    Spx = (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur;		
    if ( MULTIPLICATEUR_NB_ITERATIONS_COUPES_NOEUD_RACINE * Spx->Iteration > MxIterationsSimplexeCoupesNoeudRacine ) {
		  MxIterationsSimplexeCoupesNoeudRacine = MULTIPLICATEUR_NB_ITERATIONS_COUPES_NOEUD_RACINE * Spx->Iteration;	
    }
		
    time( &HeureDeCalendrierCourant );
    Temps = difftime( HeureDeCalendrierCourant, HeureDeCalendrierDebut );
    if ( Temps <= 1.0 ) Temps = 1.0;
    LimiteTemps = Temps * MULTIPLICATEUR_TEMPS;
		if ( LimiteTemps > Temps + LIMITE_TEMPS ) LimiteTemps = Temps + LIMITE_TEMPS;
		
    /* Autre methode car celle-ci ne donne pas satisfaction */
		if ( Temps > LIMITE_TEMPS ) LimiteTemps = Temps;
		else LimiteTemps = LIMITE_TEMPS;
    /* On reinitialise le compteur */
		time( &HeureDeCalendrierDebut );	 
			
    Bb->BaseDisponibleAuNoeudRacine = OUI;
    memcpy( (char * ) Bb->NoeudRacine->PositionDeLaVariableSansCoupes , (char * ) Bb->NoeudRacine->PositionDeLaVariable ,
                      Bb->NombreDeVariablesDuProbleme * sizeof( int ) );
    Bb->NoeudRacine->NbVarDeBaseComplementairesSansCoupes = Bb->NoeudRacine->NbVarDeBaseComplementaires;      
    memcpy( (char * ) Bb->NoeudRacine->ComplementDeLaBaseSansCoupes , (char * ) Bb->NoeudRacine->ComplementDeLaBase ,
		      Bb->NoeudRacine->NbVarDeBaseComplementaires * sizeof( int ) );       
  }

  if ( Bb->NoeudEnExamen == Bb->NoeudRacine && NumeroDePasse > 1 && YaUneSolution == OUI ) {

		NbSat = 0;
    for ( j = 0 ; j < Pne->Coupes.NombreDeContraintes ; j++ ) {
      if ( Pne->Coupes.PositionDeLaVariableDEcart[j] != EN_BASE ) NbSat++;
    }

    Spx = (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur;		
    NbIterationSimplexeCoupes += Spx->Iteration;
		
    # if FIN_COUPES_NOEUD_RACINE_SUR_TEMPS_MAX != OUI_PNE		
	    if ( Bb->AffichageDesTraces == OUI_PNE && CalculerDesCoupes == OUI_PNE ) {     
		    printf("..Root cutting: simplexe iterations count %d max. is %d\n",NbIterationSimplexeCoupes,MxIterationsSimplexeCoupesNoeudRacine);				
		  }
		# endif
		
	  if ( Bb->AffichageDesTraces == OUI_PNE && CalculerDesCoupes == OUI_PNE ) {			
		  if ( fabs( AncienneValeurDuCritere ) > 1.e-9 ) {
		    printf("..Root cutting increases lower bound by: %e (relative is %e %%)\n",fabs( Pne->Critere - AncienneValeurDuCritere ),
				        fabs( (Pne->Critere - AncienneValeurDuCritere) / (0.01 * AncienneValeurDuCritere) ) );			
      }
			else {
		    printf("..Root cutting increases lower bound by: %e\n",fabs( Pne->Critere - AncienneValeurDuCritere ) );			
			}			     
			printf("..Found %d cuts of which %d are tight\n",Pne->Coupes.NombreDeContraintes,NbSat);			
		}	  	
  }	

  /* Si on n'a pas calcule de coupes, on controle la violation des coupes du pool. Si des coupes
     sont violees, on les ajoute dans le probleme et on reoptimise */
  if ( Bb->ControlerLesCoupesNonInclusesPourUnNouvelleResolution == OUI && YaUneSolution == OUI ) {      
    BB_ControlerLesCoupesNonInsereesInitialement( Bb, &YaDesCoupesAjoutees );
    if ( YaDesCoupesAjoutees == OUI ) {
      printf("Rebouclage vers une nouvelle resolution du noeud car des coupes du pool ont ete ajoutees\n");
      NumeroDePasse++;
      goto ResolutionDuNoeud; 
    }
  }
			      
  /* Si on a calcule des coupes, on reoptimise le meme noeud */
  if ( ReoptimisationDuNoeudApresCoupes == OUI ) {
    if ( CalculerDesCoupes == OUI_PNE && YaUneSolution == OUI ) {      
      NumeroDePasse++;     
      if ( NumeroDePasse == DernierRoundDeCoupes ) CalculerDesCoupes = NON_PNE;      
      Bb->ComplementDeBaseModifie = NON;	
      NoeudCourant->BaseFournie  = OUI_SPX;
      /* NumeroDePasse vaut 2 apres la premiere resolution du noeud */
      if ( NumeroDePasse > 2 ) {
        /* Si le critere n'a pas beaucoup bouge, on arrete le calcul des coupes, on evalue le noeud une derniere fois
           pour choisir la variable a instancier */
       /* 1- Calcul en pourcentage */
        if ( fabs( Pne->Critere - AncienneValeurDuCritere ) < fabs( POURCENTAGE_VARIATION_CRITERE * Pne->Critere ) && Bb->NoeudEnExamen != Bb->NoeudRacine && 0 ) {
          /* Le critere n'a pas assez varie en relatif => on arrete le calcul des coupes */
          /*
          printf("Le critere n'a pas assez varie en relatif => on arrete le calcul des coupes Pne->Critere - AncienneValeurDuCritere %e\n",
                  Pne->Critere - AncienneValeurDuCritere);
          */					
          if ( fabs( Pne->Critere - AncienneValeurDuCritere ) < MAX_SI_POURCENTAGE_VARIATION_CRITERE ) {										
            CalculerDesCoupes = NON_PNE;
            /* Et pour pouvoir faire le strong branching */
             NumeroDePasse = DernierRoundDeCoupes;						
					}
        }
        /* 2- Calcul en valeur absolue pour le cas des petites valeurs de criteres */
        if ( CalculerDesCoupes == OUI_PNE ) {											
          if ( fabs( Pne->Critere - AncienneValeurDuCritere ) < VALEUR_ABSOLUE_VARIATION_CRITERE ) {
            /* Le critere n'a pas assez varie en valeur absolue => on arrete le calcul des coupes */
            /*
            printf("Le critere n'a pas assez varie en valeur absolue => on arrete le calcul des coupes Pne->Critere - AncienneValeurDuCritere %e\n",
                    Pne->Critere - AncienneValeurDuCritere);
            */						
            CalculerDesCoupes = NON_PNE;
            /* Et pour pouvoir faire le strong branching */
             NumeroDePasse = DernierRoundDeCoupes;						
          }
        }

        /* Sortie si on a deja passe trop de temps ou Nb max d'iterations */
        if ( Bb->NombreDeVariablesEntieresDuProbleme > 0 ) {
          if ( NoeudCourant == Bb->NoeudRacine && NumeroDePasse >= 2 ) {
					  # if FIN_COUPES_NOEUD_RACINE_SUR_TEMPS_MAX == OUI_PNE
              time( &HeureDeCalendrierCourant );
              Temps = difftime( HeureDeCalendrierCourant, HeureDeCalendrierDebut );
						
	            m = Bb->ToleranceDOptimalite * 0.01 * fabs( Pne->Critere );					
	            if ( m < VALEUR_ABSOLUE_VARIATION_CRITERE ) m = VALEUR_ABSOLUE_VARIATION_CRITERE;						
						  /* On continue si le critere decroit */
              if ( fabs( Pne->Critere - AncienneValeurDuCritere ) > m ) {
	              if ( Bb->AffichageDesTraces == OUI_PNE ) {
							    if ( Temps > LimiteTemps ) printf("..time limit for root cutting was reached but continuing because lower bound still increasing\n");
                }						
						    Temps = LimiteTemps - 1;
						  }												
			        if ( Temps > LimiteTemps ) {						
                CalculerDesCoupes = NON_PNE;
                /* Et pour pouvoir faire le strong branching */
                NumeroDePasse = DernierRoundDeCoupes;							
	              if ( Bb->AffichageDesTraces == OUI_PNE ) {							
							    printf("Root node cutting planes stopped because time limit was reached: computing time is %e , limit is %e\n",Temps,LimiteTemps);
							  }
						  }
						# else
						  /* On sort sur nombre max d'iterations */
              if ( NbIterationSimplexeCoupes > MxIterationsSimplexeCoupesNoeudRacine ) {							
	              m = Bb->ToleranceDOptimalite * 0.01 * fabs( Pne->Critere );					
	              if ( m < VALEUR_ABSOLUE_VARIATION_CRITERE ) m = VALEUR_ABSOLUE_VARIATION_CRITERE;						
						    /* On continue si le critere decroit */
                if ( fabs( Pne->Critere - AncienneValeurDuCritere ) > m ) {
	                if ( Bb->AffichageDesTraces == OUI_PNE ) {
							      printf("..iteration limit for root cutting was reached but continuing because lower bound still increasing\n");
                  }						
						    }
								else {
                  CalculerDesCoupes = NON_PNE;
                  /* Et pour pouvoir faire le strong branching */
                  NumeroDePasse = DernierRoundDeCoupes;							
	                if ( Bb->AffichageDesTraces == OUI_PNE ) {							
							      printf("Root node cutting planes stopped because iteration limit was reached: iteration count is %d, limit is %d\n",
										        NbIterationSimplexeCoupes,MxIterationsSimplexeCoupesNoeudRacine);
							    }								
								}
							}
							
            # endif						
	        }
        }	 
				
      }
      AncienneValeurDuCritere = Pne->Critere;
      /* On annule la prise en compte des coupes non saturees */
      if ( Bb->NoeudEnExamen == Bb->NoeudRacine ) PNE_ActualiserLesCoupesAPrendreEnCompte( Pne );
     
      goto ResolutionDuNoeud;
    }
  }

  /* Tri des coupes (celles d'avant et celles qui ont ete creees) */
  ReFaireUnCalculSansCalculerDesCoupes = NON; 
  YaUneSolutionSv = YaUneSolution;
  if ( YaUneSolution == NON && NumeroDePasse > 1 ) {
    /* Cela signifie que les coupes ajoutees on fait diverger le simplexe car elles etaient mal 
       fichues => on peut utiliser le resultat de la premiere passe */
    /* Ce qui aura pour effet de simplement de liberer les tableaux de coupes dans la pne, mais on n'en 
       stockera pas */
    YaUneSolution   = OUI; 
    YaUneSolutionSv = OUI;
    /* Si on est a une resolution qui a "echoue" apres l'ajout de coupes, on relance 
       le calcul en interdisant absolument le calcul des coupes. Ainsi on pourra beneficier du 
       strong branching pour le choix de la variable a instancier */
    if( Bb->SortieParDepassementDuCoutMax != OUI ) ReFaireUnCalculSansCalculerDesCoupes = OUI; 
    else YaUneSolutionSv = NON;
  }

  NbCoupesAlloc = Bb->NombreDeCoupesAjoutees + Bb->MajorantDuNombreDeCoupesAjouteesApresResolutionDuProblemeRelaxe;
  MargeAlloc    = NbCoupesAlloc + 1 /*1*/; 
  NbCoupesAlloc+= MargeAlloc;

  Bb->CoupeSaturee           = (char *) malloc( NbCoupesAlloc * sizeof(char) );
  Bb->CoupeSatureeAEntierInf = (char *) malloc( NbCoupesAlloc * sizeof(char) );  
  Bb->CoupeSatureeAEntierSup = (char *) malloc( NbCoupesAlloc * sizeof(char) );    
  if ( Bb->CoupeSaturee == NULL || Bb->CoupeSatureeAEntierInf == NULL || Bb->CoupeSatureeAEntierSup == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_ResoudreLeProblemeRelaxe\n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
  }
	
  PNE_TrierLesCoupesCalculees( Pne,
                       YaUneSolution,		       
                       Bb->BasesFilsDisponibles,
                       ReoptimisationDuNoeudApresCoupes,
                       Bb->CoupeSaturee,   
                       Bb->CoupeSatureeAEntierInf,   
                       Bb->CoupeSatureeAEntierSup   
                               );

  YaUneSolution = YaUneSolutionSv;
}
else {	
  Bb->NombreDeProblemesRelaxesResolus++;
  Bb->DernierNoeudResolu = 0;
  YaUneSolution      = NON; 
}

if ( ReFaireUnCalculSansCalculerDesCoupes == OUI ) {
  /* Il faut recuperer les informations sur les variables basiques complementaires du probleme
     avant l'ajout des nouvelles coupes: attention les tables de sauvegarde ne sont pas valables
     s'il s'agit du noeud racine */
  if ( Bb->NoeudEnExamen != Bb->NoeudRacine ) {
    for ( i = 0 ; i < Bb->NombreDeVariablesDuProbleme /*+ Bb->NombreDeContraintesDuProbleme*/ ; i++ ) {
      NoeudCourant->PositionDeLaVariable[i] = PositionDeLaVariableSv[i]; 
    }
    for ( i = 0 ; i < NbVarDeBaseComplementairesSv  ; i++ ) {
      NoeudCourant->ComplementDeLaBase[i] = ComplementDeLaBaseSv[i];  
    }
    NoeudCourant->NbVarDeBaseComplementaires = NbVarDeBaseComplementairesSv;
  }
  else {
    if ( Bb->BaseDisponibleAuNoeudRacine == OUI ) {
      NoeudCourant->BaseFournie = OUI_SPX;
      for ( i = 0 ; i < Bb->NombreDeVariablesDuProbleme ; i++ ) {
        NoeudCourant->PositionDeLaVariable[i] = Bb->NoeudRacine->PositionDeLaVariableSansCoupes[i]; 
      }
      for ( i = 0 ; i < Bb->NoeudRacine->NbVarDeBaseComplementairesSansCoupes ; i++ ) {
        NoeudCourant->ComplementDeLaBase[i] = Bb->NoeudRacine->ComplementDeLaBaseSansCoupes[i];  
      }
      NoeudCourant->NbVarDeBaseComplementaires = Bb->NoeudRacine->NbVarDeBaseComplementairesSansCoupes;
    }
    else {
      NoeudCourant->BaseFournie = NON_SPX;
    }
  }
  goto Debut; /* Juste pour pouvoir faire du strong branching */
} 

#if VERBOSE_BB
  if ( Bb->NombreDeVariablesEntieresDuProbleme > 0 ) {
    printf(" BasesFilsDisponibles %d \n",Bb->BasesFilsDisponibles);
    printf(" MinorantEspereAEntierInf %e \n",Bb->MinorantEspereAEntierInf);
    printf(" MinorantEspereAEntierSup %e \n",Bb->MinorantEspereAEntierSup);
  }
#endif

FinResolutionDuProblemeRelaxe:

Bb->NombreDeProblemesResolus++;
Bb->NombreDeProblemesResolusDepuisLaRAZDesCoupes++;

if(YaUneSolution == OUI) {
  NoeudCourant->NbValeursFractionnairesApresResolution = Pne->NombreDeVariablesAValeurFractionnaire;
}
else {  
  *SolutionEntiereTrouvee = NON;
  ValeurOptimale = PLUS_LINFINI;  
  #if VERBOSE_BB
    printf("Pas de solution au probleme relaxe du noeud \n");
  #endif
}

NoeudCourant->MinorantDuCritereAuNoeud    = ValeurOptimale;
NoeudCourant->LaSolutionRelaxeeEstEntiere = *SolutionEntiereTrouvee;

/*
printf("Nombre de variables entieres relaxees se trouvant sur une borne: %d \n",Bb->NombreDeVariablesFixeesEnSortie);
*/

time( &(Pne->HeureDeCalendrierCourant) );
TempsEcoule = difftime( Pne->HeureDeCalendrierCourant , Pne->HeureDeCalendrierDebut );
if ( TempsEcoule <= 0.0 ) TempsEcoule = 0.0;  
  
if ( Bb->NombreDeVariablesEntieresDuProbleme > 0 ) {
  if ( NoeudCourant->LaSolutionRelaxeeEstEntiere == OUI ) {
	  X = 0.;
	  if ( Bb->ProfondeurMoyenneDesSolutionsEntieres >= 0 ) {
      X = Bb->ProfondeurMoyenneDesSolutionsEntieres * Bb->NombreDeSolutionsEntieresTrouvees;
		}
		X += NoeudCourant->ProfondeurDuNoeud;	
	  Bb->NombreDeSolutionsEntieresTrouvees++;
		X /= Bb->NombreDeSolutionsEntieresTrouvees;
		Bb->ProfondeurMoyenneDesSolutionsEntieres = (int) ceil( X );
	}
  if ( NoeudCourant == Bb->NoeudRacine && Bb->AffichageDesTraces == OUI ) {
    if ( YaUneSolution == OUI ) {
      printf("\n");
      printf("** Root node, internal cost: %16.9e , elapsed time: %7d seconds\n",
	     NoeudCourant->MinorantDuCritereAuNoeud, (int) TempsEcoule);
      printf("\n");    
      printf("** Starting the tree search **\n");
    }
    else {
      printf("\n");
      printf("** No solution found, elapsed time: %7d seconds\n", (int) TempsEcoule);
      printf("\n");         
    }    
  }
}
else if ( Bb->AffichageDesTraces == OUI) {
  if ( YaUneSolution == OUI ) {
    printf("\n");
    printf("** Solution found, internal cost: %16.9e , elapsed time: %7d seconds\n",
	   NoeudCourant->MinorantDuCritereAuNoeud, (int) TempsEcoule);
    printf("\n");    
  }
  else {
    printf("\n");
    printf("** No solution found, elapsed time: %7d seconds\n", (int) TempsEcoule);
    printf("\n");         
  }
}

free( ComplementDeLaBaseSv );
free( PositionDeLaVariableSv );
ComplementDeLaBaseSv   = NULL;
PositionDeLaVariableSv = NULL;

/* Si on doit nettoyer toutes les coupes c'est qu'elles sont devenues merdiques et donc on relance sans les coupes */
if (  Bb->EnleverToutesLesCoupesDuPool == OUI ) {     
  if ( Bb->AffichageDesTraces == OUI ) {
		printf("Trying to clean the pool cuts for numerical stability reasons\n");
		/* Pour forcer le reaffichage de la legende */
		Bb->NombreDAffichages = CYCLE_DAFFICHAGE_LEGENDE;		
  }  
  DemandeExpresse = OUI;
  BB_NettoyerLesCoupes( Bb, DemandeExpresse );
  if ( RebouclageApresNettoyageDesCoupesFait == NON ) {  
    RebouclageApresNettoyageDesCoupesFait = OUI;
    goto DebutGeneral;
  }
}

/* Calcul de la taille du pool. S'il est trop grand on arrete le calcul des coupes */
if ( Bb->NbMaxDeCoupesCalculeesAtteint != OUI ) {
  {
    COUPE ** Coupe; int NumeroDeCoupe; int TailleDuPool;
    TailleDuPool = 0;
    Coupe = Bb->NoeudRacine->CoupesGenereesAuNoeud;
    for ( NumeroDeCoupe = 0 ; NumeroDeCoupe < Bb->NoeudRacine->NombreDeCoupesGenereesAuNoeud ; NumeroDeCoupe++ ) {
      TailleDuPool+= Coupe[NumeroDeCoupe]->NombreDeTermes * sizeof( int   );
      TailleDuPool+= Coupe[NumeroDeCoupe]->NombreDeTermes * sizeof( double );
    }
    if ( TailleDuPool >= TAILLE_MAXI_DU_POOL_DE_COUPES ) {
      if ( Bb->AffichageDesTraces == OUI ) {
        printf("Pool cuts is full\n");
      }
      Bb->NbMaxDeCoupesCalculeesAtteint = OUI;			
    }
  }
}

if ( YaUneSolution == OUI && ChoisirLaVariableAInstancier == 0 ) {
  if ( *SolutionEntiereTrouvee == NON ) {
	  printf("!!!!!!!! Attention Bug dans la partie Branche and Bound (ResoudreLeProblemeRelaxe) ChoisirLaVariableAInstancier = 0 et on va continuer !!!\n");
		/*exit(0);*/
	}
}

/* S'il y a une solution on tente l'heuristique */
if ( YaUneSolution == OUI && *SolutionEntiereTrouvee == NON ) {
  PasDeSolutionEntiereDansSousArbre = NON;
  PNE_HeuristiquePilotage( Pne, &PasDeSolutionEntiereDansSousArbre );
	if ( PasDeSolutionEntiereDansSousArbre == OUI_PNE ) YaUneSolution = NON;
}

return( YaUneSolution ); /* OUI s'il y a une solution, NON sinon */
}








