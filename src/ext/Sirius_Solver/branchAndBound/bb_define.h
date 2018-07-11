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
# ifdef __cplusplus
  extern "C" 
	{ 
# endif 
# ifndef DEFINITIONS_BB_FAITES  
/*******************************************************************************************/

# include "bb_sys.h"

#define VERBOSE_BB   0

# define BB_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE 
  # undef BB_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE
													    
#define	PLUS_LINFINI 1.0e+80    
   
#define OUI 1				 	  			  	       
#define NON 0    
   
#define FILS_GAUCHE 0      
#define FILS_DROIT  1

#define BORNE_SUP 1
#define BORNE_INF 2

#define ARRET_CAR_TEMPS_MAXIMUM_ATTEINT 2  
#define BB_ERREUR_INTERNE               3  

#define A_EVALUER   1
#define EVALUE      2 /* Le noeud a ete evalue */
#define A_REJETER   3 /* Le noeud a ete evalue mais il n'y a pas de solution au probleme relaxe ou bien */
                      /* le noeud a ete evalue mais lui-meme et son sous-arbre sont sous-optimaux car le */
                      /* cout de la solution du probleme relaxe est superieur au cout de la meilleure */
                      /* solution trouvee */

#define	PAS_DE_VARIABLES_A_INSTANCIER -1
#define	VARIABLES_A_INSTANCIER         1
   
#define RECHERCHER_LE_PLUS_PETIT                  2
#define RECHERCHER_LE_PLUS_PETIT_GAP_PROPORTIONNEL 3
 
#define PROFONDEUR_PURE                 1
#define MOINS_DE_VALEURS_FRACTIONNAIRES 2
#define NORME_FRACTIONNAIRE_MINIMALE    3

#define NOMBRE_DEVALUATIONS_DU_MEILLEUR_MINORANT 10 /*5*/   

#define	TOLERANCE_OPTIMALITE 0.0000001 /*0.0001*/ /* Pour supprimer une branche il faut que le cout de la solution soit superieur 
				       au cout de la meilleure solution + une petite marge pour tenir compte du fait 
				       que l'optimum est toujours entache d'un petite erreur quand-meme */

#define PROFONDEUR_TOUT_SEUL    1
#define LARGEUR_TOUT_SEUL       2
#define PROFONDEUR_DANS_LARGEUR 3

#define	COEFFICIENT_POUR_LE_CALCUL_DE_LA_PROFONDEUR_LIMITE  0.20 /*0.05*/
#define	PROFONDEUR_MIN_PENDANT_LA_RECHERCHE_EN_LARGEUR      10 /*10*/ 
#define	PROFONDEUR_MAX_PENDANT_LA_RECHERCHE_EN_LARGEUR      100 /* Pas de limite 1000000 */ 
#define NOMBRE_MIN_DE_PROBLEMES_PROFONDEUR_DANS_LARGEUR     15  

#define CYCLE_POUR_RECHERCHE_EN_PROFONDEUR  2/*5*/  /* Dans une recherche en largeur, a chaque fois qu'on a evalue ce 
                                                       nombre de problemes, on part dans une recherche en profondeur */

#define MAX_COUPES_PAR_PROBLEME_RELAXE  0.5 /*0.2*/ /*0.5*/ /* Coefficient a appliquer sur le nombre de contraintes du probleme 
                                                               pour calculer le nombre max. de coupes que l'on s'autorise a 
				     	                                                 ajouter dans un probleme relaxe */
#define NOMBRE_MAX_DE_COUPES  10000   /*5000*/ 
#define NOMBRE_MOYEN_MINIMUM_DE_COUPES_UTILES  5 
						       
#define TAILLE_MAXI_DU_POOL_DE_COUPES    500000000 /* En octets */ 

#define CYCLE_DINFORMATIONS  10 /* toutes les 10 secondes */
#define CYCLE_DINFORMATIONS_EN_NOMBRE_DE_PROBLEMES  100 
#define CYCLE_DAFFICHAGE_LEGENDE  15 

#define NON_INITIALISE 1000000

/* Donnees simplexe pour l'exploration rapide en profondeur */
typedef struct{
char   * PositionDeLaVariable;
char   * InDualFramework; 
int   * ContrainteDeLaVariableEnBase;
double * DualPoids;
int   * VariableEnBaseDeLaContrainte;
} BASE_SIMPLEXE;
													  
typedef struct{
/* Attention: toutes les coupes sont de type "inferieur ou egal" */
char     UtiliserLaCoupe    ; /* Information variable qui vaut OUI ou NON selon l'arborscence du noeud en 
                                 lequel on veut ajouter une coupe */
char     CoupeRencontreeDansLArborescence; /* Information variable qui vaut OUI ou NON selon l'arborscence du noeud en 
                                              lequel on veut ajouter une coupe */
char     CoupeExamineeAuNoeudCourant;
char     CoupeSaturee       ; /* Information variable qui vaut OUI_PNE ou NON_PNE selon l'arborscence du noeud en 
                                 lequel on veut ajouter une coupe */ 
/*int     VariableCause      ;*/ /* Numero de la variable pour laquelle on fait la coupe ( < 0 si c'est pour  
                                 pour une autre raison */
char     TypeDeCoupe        ; /* Type de coupe: 
                                 G si coupe de Gomory
				 K si sac a dos ( Knapsack )
				 I si coupe d intersection 
				 L si lift and project 
				 I si implication suite au "probing" de certains variables */
char     CoupeRacine        ; /* Vaut OUI si la coupe a ete generee au noeud racine */			      
int     NombreDeTermes     ; /* Nombre de coefficients non nuls dans la coupe */
double * Coefficient        ; /* Coefficient de la contrainte */
int   * IndiceDeLaVariable ; /* Indices des variables qui interviennent dans la coupe */
double   SecondMembre       ; /* La coupe est toujours dans le sens <= SecondMembre */
} COUPE;

typedef struct{  
/* */
int   ProfondeurDuNoeud; /* Le noeud racine a la profondeur 0 */
int   StatutDuNoeud    ; /* A_EVALUER  EVALUE  A_REJETER  */
int   NoeudTerminal    ; /* OUI ou NON */
/* */  
int   NombreDeVariablesEntieresInstanciees  ;
char * ValeursDesVariablesEntieresInstanciees; /* vaut '0' ou '1' */
int * IndicesDesVariablesEntieresInstanciees;
/* */
double MinorantPredit;	 
/* */  
double MinorantDuCritereAuNoeud   ;    
int   LaSolutionRelaxeeEstEntiere;
int   NbValeursFractionnairesApresResolution;
double NormeDeFractionnalite;
/* */
COUPE ** CoupesGenereesAuNoeud        ; /* Tableau de points sur les coupes generees au noeud */
int     NombreDeCoupesGenereesAuNoeud;
int     NombreDeG;
int     NombreDeI;
int     NombreDeK;
/* */
int * NumeroDesCoupeAjouteeAuProblemeCourant; 	 
/* */
int   NombreDeCoupesExaminees;
int * NumeroDesCoupesExaminees;     
char * LaCoupeEstSaturee;        /* C'est un resultat de la resolution du probleme relaxe */
/* */
int   NombreDeCoupesViolees ; /* Elles n'etaient donc pas incluses dans le probleme */
int * NumeroDesCoupesViolees;     
/* */
void * NoeudAntecedent; /* Adresse de la structure du noeud antecedent: inutilise si noeud racine */
void * NoeudSuivantGauche; 
void * NoeudSuivantDroit;
/* */
int     BaseFournie; 			     
int     IndiceDeLaNouvelleVariableInstanciee; 
int *   PositionDeLaVariable; 		     
int     TailleComplementDeBase; 	     
int     NbVarDeBaseComplementaires; 	     
int *   ComplementDeLaBase;
/* Pour pouvoir disposer d'une base de depart sans coupe dans le cas du nettoyage des
   coupes (valable uniquement pour le noeud racine */
int * PositionDeLaVariableSansCoupes; 		     
int   NbVarDeBaseComplementairesSansCoupes; 	     
int * ComplementDeLaBaseSansCoupes;   
/* Pour le reduced cost fixing */
int      NombreDeBornesModifiees;
int *    NumeroDeLaVariableModifiee;
char *   TypeDeBorneModifiee;
double * NouvelleValeurDeBorne;
/* */
/* Pour l'exploration rapide en profondeur dans le simplexe */
BASE_SIMPLEXE * BaseSimplexeDuNoeud;
/* */
} NOEUD;  

typedef struct{
/* Pour les outils de gestion memoire */
void * Tas;

NOEUD  * NoeudRacine;
char     BaseDisponibleAuNoeudRacine;

double * ValeursOptimalesDesVariables;		    
double * ValeursOptimalesDesVariablesEntieres;	   
double   CoutDeLaMeilleureSolutionEntiere;
NOEUD  * NoeudDeLaMeilleureSolutionEntiere;
int     ProfondeurMoyenneDesSolutionsEntieres;
int     UtiliserCoutDeLaMeilleureSolutionEntiere;
int     ProfondeurMaxiSiPlongeePendantUneRechercheEnLargeur;

double   ValeurDuMeilleurMinorant;
double   ValeurDuMeilleurPremierMinorant;
double   ValeurDuMeilleurDernierMinorant;
int     NombreDEvaluationDuMeilleurMinorant;
NOEUD *  NoeudDuMeilleurMinorant;

double * ValeursCalculeesDesVariablesPourLeProblemeRelaxeCourant;
double * ValeursCalculeesDesVariablesEntieresPourLeProblemeRelaxeCourant;

/*          */

int   * NumerosDesVariablesEntieresDuProbleme;	    
int     NombreDeVariablesEntieresDuProbleme;
int     NombreDeVariablesDuProbleme;
int     NombreDeContraintesDuProbleme;	      

char     ArreterLesCalculs;	      
int     TempsDexecutionMaximum;	      
char     ForcerAffichage;	      
char     AffichageDesTraces;
int     NombreMaxDeSolutionsEntieres;
double   ToleranceDOptimalite;
						      
char     EnleverToutesLesCoupesDuPool;
char    SolutionEntiereTrouveeParHeuristique;

NOEUD  * NoeudEnExamen;
				  
int     NombreDeProblemesRelaxesResolus;
int     NombreDeProblemesResolus;
int     NombreDeProblemesResolusDepuisLaRAZDesCoupes;
int     NombreDeSolutionsEntieresTrouvees;
int     NbProbPourLaPremiereSolutionEntiere; /* Nombre de problemes resolus pour trouver la 
                                                 premiere solution entiere */
int     NombreTotalDeCoupesDuPoolUtilisees;
int     NombreTotalDeGDuPoolUtilisees;
int     NombreTotalDeIDuPoolUtilisees;
int     NombreTotalDeKDuPoolUtilisees;

/* Variable a instancier suite a la resolution d'un probleme relaxe */ 
int     VariableProposeePourLInstanciation; 
double   MinorantEspereAEntierInf;
double   MinorantEspereAEntierSup;

int   * PositionDeLaVariableAEntierInf; 
int     NbVarDeBaseComplementairesAEntierInf;    
int   * ComplementDeLaBaseAEntierInf;    
int   * PositionDeLaVariableAEntierSup; 
int     NbVarDeBaseComplementairesAEntierSup;    
int   * ComplementDeLaBaseAEntierSup;             
int     BasesFilsDisponibles; 

/* Etat de saturation des coupes */
int    NombreDeCoupesAjoutees;
char * CoupeSaturee;   
char * CoupeSatureeAEntierInf;   
char * CoupeSatureeAEntierSup;   

int   NombreMaxDeCoupesParProblemeRelaxe;

/* Indicateur solution ameliorante trouvee ou non (utilise pendant le balayage
   en largeur) */
int     SolutionAmelioranteTrouvee;
int     NumeroDeProblemeDeLaSolutionAmeliorante;
								   
/* Listes de noeuds a explorer */
int     NbNoeuds1_PNE_BalayageEnProfondeur;  
NOEUD ** Liste1_PNE_BalayageEnProfondeur;

int     NbNoeuds1_PNE_NettoyerLArbreDeLaRechercheEnProfondeur;
NOEUD ** Liste1_PNE_NettoyerLArbreDeLaRechercheEnProfondeur;
int     NbNoeuds2_PNE_NettoyerLArbreDeLaRechercheEnProfondeur;
NOEUD ** Liste2_PNE_NettoyerLArbreDeLaRechercheEnProfondeur;

int     NbNoeuds1_PNE_BalayageEnLargeur;
NOEUD ** Liste1_PNE_BalayageEnLargeur;
int     NbNoeuds2_PNE_BalayageEnLargeur;
NOEUD ** Liste2_PNE_BalayageEnLargeur;

int     NbNoeuds1_PNE_EliminerLesNoeudsSousOptimaux;
NOEUD ** Liste1_PNE_EliminerLesNoeudsSousOptimaux;
int     NbNoeuds2_PNE_EliminerLesNoeudsSousOptimaux;
NOEUD ** Liste2_PNE_EliminerLesNoeudsSousOptimaux;

int     NbNoeuds1_PNE_SupprimerTousLesDescendantsDUnNoeud;
NOEUD ** Liste1_PNE_SupprimerTousLesDescendantsDUnNoeud;
int     NbNoeuds2_PNE_SupprimerTousLesDescendantsDUnNoeud;
NOEUD ** Liste2_PNE_SupprimerTousLesDescendantsDUnNoeud;

NOEUD * DernierNoeudResolu;

char    ComplementDeBaseModifie;
int    MajorantDuNombreDeCoupesAjouteesApresResolutionDuProblemeRelaxe;
char    CalculerDesCoupesDeGomory;
char    CalculerDesCoupes;
char    ControlerLesCoupesNonInclusesPourUnNouvelleResolution;

char    TypeDExplorationEnCours;  /* Valeurs possible: 
                                     PROFONDEUR_TOUT_SEUL    
                                     LARGEUR_TOUT_SEUL       
                                     PROFONDEUR_DANS_LARGEUR */
char    EvaluerLesFilsDuMeilleurMinorant;
double  EcartBorneInf;

int    AnomalieDetectee;
jmp_buf EnvBB;

char    SortieParDepassementDuCoutMax;
int    TailleTableau;
/* Un noeud est ferme lorsque ses 2 fils ont ete evalues ou bien lorsqu'il est terminal */
int    NbNoeudsOuverts; 
  
/* Infos pour les affichages */
int    TempsDuDernierAffichage;
int    NombreDeProblemesDepuisLeDernierAffichage;
int    NombreDAffichages;
 
char   NbMaxDeCoupesCalculeesAtteint;
int    NombreDeNoeudsEvaluesSansCalculdeCoupes;
double EcartBorneInfALArretDesCoupes;
int    NombreMaxDeCoupes; 
int    NombreMoyenMinimumDeCoupesUtiles;

/* Pour calculer la profondeur moyenne d'elagage (pruning) */
int    NombreDeNoeudsElagues;
int    AveragePruningDepth;
int    SommeDesProfondeursDesSolutionsAmeliorantes;
int    NombreDeSolutionsAmeliorantes;
int    SommeDesProfondeursDElaguage;
int    NombreDeSimplexes;
int    SommeDuNombreDIterations;

int    AverageG;
int    AverageI;
int    AverageK;

/*   */
void * ProblemePneDuBb;

} BB;

/*******************************************************************************************/
# define DEFINITIONS_BB_FAITES	 
# endif  
# ifdef __cplusplus
  }
# endif
