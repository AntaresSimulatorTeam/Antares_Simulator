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
# ifndef DEFINITIONS_PNE_FAITES
/*******************************************************************************************/

# include "pne_sys.h"
# include "pne_constantes_externes.h"
# include "pne_constantes_internes.h"

/*******************************************************************************************************/

typedef struct{
int     NombreDeContraintesAllouees;
int     TailleAlloueePourLaMatriceDesContraintes;
/*----------------------------------------*/
int     NombreDeContraintes;
/* Le second membre des contraintes */ 
double * B;    /* Le sens est toujours <= */
char   * PositionDeLaVariableDEcart; /* EN_BASE si la contrainte n'est pas saturee
                                        HORS_BASE_SUR_BORNE_INF si elle est saturee */
char   * PositionDeLaVariableDEcartAGauche; /* Pour le strong branching */
char   * PositionDeLaVariableDEcartADroite; /* Pour le strong branching */
/*----------------------------------------*/
/* Chainage de la matrice des contraintes */
int   * Mdeb;
int   * NbTerm;
int   * Nuvar; 
double * A;
char * TypeDeCoupe;
} COUPES_DUN_PROBLEME_RELAXE;

/*******************************************************************************************************/

typedef struct{
char     Type;                /* 'G' gomory, 'L' lift and project, 'K' knapsack, 'M' Mir */
int     IndexDansCliques;     /* -1 si ce n'est pas une clique */
int     IndexDansCoupesDeProbing;     /* -1 si ce n'est pas une coupe de probing */
int     IndexDansContraintesDeBorneVariable;     /* -1 si ce n'est pas une contrainte de borne variable qui vient du variable probing */
int     IndexDansKNegligees;     /* -1 si ce n'est pas une knapsack negligee */
int     IndexDansGNegligees;     /* -1 si ce n'est pas une coupe gomry negligee */
int     NumeroDeLaContrainte; /* C'est le numero de la contrainte qui correspond a la coupe dans le probleme relaxe */
int     NombreDeTermes;       /* Nombre de coefficients non nuls dans la coupe */
double * Coefficient;         /* Coefficient de la contrainte */
int   * IndiceDeLaVariable;   /* Indices des variables qui interviennent dans la coupe */
double   SecondMembre;        /* La coupe est toujours dans le sens <= SecondMembre */
double   Distance;            /* Distance de la solution courante */
} COUPE_CALCULEE;

/*******************************************************************************************************/
# define VALIDITE_A_DETERMINER 10 /* Pour BminValide et BmaxValide */
typedef struct {
int Faisabilite;
int VariableInstanciee;
double ValeurDeLaVariableInstanciee;

/* Zone de travail */
/* Variables */
char * BorneInfConnue;
char * BorneSupConnue;
double * ValeurDeBorneInf;
double * ValeurDeBorneSup;
/* Contraintes */
double * Bmin;
double * Bmax;

/* Zone de sauvegarde */
char * BorneInfConnueSv;
char * BorneSupConnueSv;
double * ValeurDeBorneInfSv;
double * ValeurDeBorneSupSv;
double * BminSv;
double * BmaxSv;

int NbVariablesModifiees;
int * NumeroDeVariableModifiee;
char * VariableModifiee;

int NbContraintesModifiees;
int * NumeroDeContrainteModifiee;
int * NbFoisContrainteModifiee;

int NombreDeContraintesAAnalyser;
int NbParcours;

int IndexCourantContraintesAAnalyser;
int IndexLibreContraintesAAnalyser;

int SeuilNbTermesMatrice;
int * NumeroDeContrainteAAnalyser;
char * ContrainteAAnalyser;
/*int * IndexContrainteAAnalyser;*/

/*int Next_NombreDeContraintesAAnalyser;*/
/*int * Next_NumeroDeContrainteAAnalyser;*/
/*int * Next_IndexContrainteAAnalyser;*/

char * BminValide;
char * BmaxValide;

/* Variables fixees */
int NombreDeVariablesFixees;
int * NumeroDesVariablesFixees;

/* Pour pouvoir remplacer les contraintes concernees par une leur equivalent en coupe de probing */
int * NumeroDeCoupeDeProbing; 

int NombreDeVariablesFixeesDansLeProbing;

int NbCntCoupesDeProbing;
int * NumCntCoupesDeProbing; /* Attention il faudra liberer ca ensuite */
char * FlagCntCoupesDeProbing; /* Attention il faudra liberer ca ensuite */

int NbVarAInstancier;
int * NumVarAInstancier; /* Attention il faudra liberer ca ensuite */
char * FlagVarAInstancier; /* Attention il faudra liberer ca ensuite */

/* Tableaux de travail */
char * Buffer; /* C'est l'adresse du buffer qu'il faudra liberer */
} PROBING_OU_NODE_PRESOLVE;

typedef struct {
int TailleAllouee;
int NbNoeudsDuGraphe;
int Pivot;
int * First;
int * Adjacent;
int * Next;
int NbEdges;
int NbEdgesLast;
char Full; /* OUI_PNE ou NON_PNE */
int IncrementDAllocation;
} CONFLICT_GRAPH;

typedef struct {
int NbCliquesAllouees;
int TailleCLiquesAllouee;
int NombreDeCliques;
int * First;
int * NbElements;
int * Noeud;
/* */
char * LaCliqueEstDansLePool; /* OUI_PNE ou NON_PNE */
char * CliqueDeTypeEgalite; /* OUI_PNE ou NON_PNE */
int * NumeroDeCliqueDuNoeud;
/* */
char Full; /* OUI_PNE ou NON_PNE */
} CLIQUES;

typedef struct {
int IndexLibre;
int NombreDeCoupesDeProbingAlloue;
int NombreDeCoupesDeProbing;
double * SecondMembre;
int * First;
int * NbElements;
int TailleCoupesDeProbingAllouee;
int * Colonne;
double * Coefficient;
/* */
char * LaCoupDeProbingEstDansLePool; /* OUI_PNE ou NON_PNE */
/* */
char Full; /* OUI_PNE ou NON_PNE */
} COUPES_DE_PROBING;

typedef struct {
int IndexLibre;
int NombreDeContraintesDeBorneAlloue;
int NombreDeContraintesDeBorne;
double * SecondMembre;
int * First;
int TailleContraintesDeBorneAllouee;
int * Colonne;
double * Coefficient;
/* */
char * LaContrainteDeBorneVariableEstDansLePool; /* OUI_PNE ou NON_PNE */
/* */
char Full; /* OUI_PNE ou NON_PNE */
} CONTRAITES_DE_BORNE_VARIABLE;

typedef struct {
int IndexLibre;
int NombreDeCoupesAllouees;
int NombreDeCoupes;
double * SecondMembre;
int * First;
int * NbElements;
int TailleCoupesAllouee;
int * Colonne;
double * Coefficient;
/* */
char * LaCoupeEstDansLePool; /* OUI_PNE ou NON_PNE */
/* */
char Full; /* OUI_PNE ou NON_PNE */
} COUPES_K_NEGLIGEES;

typedef struct {
int IndexLibre;
int NombreDeCoupesAllouees;
int NombreDeCoupes;
double * SecondMembre;
int * First;
int * NbElements;
int TailleCoupesAllouee;
int * Colonne;
double * Coefficient;
/* */
char * LaCoupeEstDansLePool; /* OUI_PNE ou NON_PNE */
/* */
char Full; /* OUI_PNE ou NON_PNE */
} COUPES_G_NEGLIGEES;

typedef struct {
  int NombreDeContraintes;
	int * IndexDebut;
	int * NombreDeTermes;
	double * SecondMembre;
	char * Sens;
	int * Colonne;
	double * Coefficient;
  /* Pour la base de depart: uniquement pour le clone du simplexe au noeud racine */
	int BaseDeDepartFournie;
	double * X;
	double * CoutsReduits;
  int * PositionDeLaVariable;
  int * PositionDeLaVariableSV;
  int   NbVarDeBaseComplementaires;
  int   NbVarDeBaseComplementairesSV;
  int * ComplementDeLaBase;    	
  int * ComplementDeLaBaseSV;    	
} MATRICE_DE_CONTRAINTES;

/*******************************************************************************************************/
/* Donnees supplementaires pour le pilotage si appele par le branch ans bound lui-meme */
/* La structure n'existe que dans le cas d'un appel interne. Sinon le pointeur "Controls" est NULL */
typedef struct {
  void * Pne;     /* Donne le probleme PNE pere si le probleme courant est lui meme appele par le solveur */
  void * PneFils; /* Donne le probleme PNE fils si le probleme courant est lui meme appele par le solveur */
  void * Presolve; /* Donne le probleme PRESOLVE du PNE fils si le probleme courant est lui meme appele par le solveur */
	int PresolveUniquement; /* OUI_PNE ou NON_PNE */
	int FaireDuVariableProbing; /* OUI_PNE ou NON_PNE */
	int RechercherLesCliques; /* OUI_PNE ou NON_PNE */
} CONTROLS;

typedef struct {
/* Pour les outils de gestion memoire */
void * Tas;

/*--------------------------------------------*/
/*              Pour le pilotage              */
/* Lorsque le solveur s'appelle lui-meme */
CONTROLS * Controls;
char FaireHeuristiqueRINS;
char StopHeuristiqueRINS;
int NombreDeSolutionsHeuristiqueRINS;
int NombreDEchecsSuccessifsHeuristiqueRINS;
int NombreDeRefusSuccessifsHeuristiqueRINS;
int NombreDeReactivationsSansSuccesHeuristiqueRINS;

char FaireHeuristiqueFixation;
char StopHeuristiqueFixation;
int NombreDeSolutionsHeuristiqueFixation;
int NombreDEchecsSuccessifsHeuristiqueFixation;
int NombreDeRefusSuccessifsHeuristiqueFixation;
int NombreDeReactivationsSansSuccesHeuristiqueFixation;

char FaireHeuristiqueFractionalDive;
char StopHeuristiqueFractionalDive;
int NombreDeSolutionsHeuristiqueFractionalDive;
int NombreDEchecsSuccessifsHeuristiqueFractionalDive;
int NombreDeRefusSuccessifsHeuristiqueFractionalDive;
int NombreDeReactivationsSansSuccesHeuristiqueFractionalDive;

/*                                       */
char   FaireDuPresolve;
int    TempsDExecutionMaximum; /* En secondes */
char   AffichageDesTraces;
int    NombreMaxDeSolutionsEntieres;
double ToleranceDOptimalite;

char SolveurPourLeProblemeRelaxe;

double DureeDuPremierSimplexe;
  
/*--------------------------------------------*/

int   * CorrespondanceVarEntreeVarNouvelle; 
char   * VariableAInverser;

/* Variables eliminees dont il faut recalculer la valeur */
int     NombreDeVariablesElimineesSansValeur;
int   * VariableElimineeSansValeur; 

int     NombreDeVariablesTrav;
int     NombreDeVariablesNonFixes;
int   * NumeroDesVariablesNonFixes;
int     NombreDeVariablesEntieresNonFixes;
char    YaQueDesVariablesEntieres; /* OUI_PNE ou NON_PNE */
int     NombreDeVariablesEntieresTrav;
int   * NumerosDesVariablesEntieresTrav;
int   * TypeDeVariableTrav;
double * SeuilDeFractionnalite; /* On pourrait compacter les variables entieres plutot que d'utiliser TypeDeVariableTrav et SeuilDeFractionnalite en parallele */
char *  VariableBinaireBigM; /* Vaut OUI_PNE ou NON_PNE */
int   * TypeDeBorneTrav;
int   * TypeDeBorneTravSv;

double * UTrav;
double * S1Trav;
double * S2Trav;
double * UmaxTrav;
double * UmaxTravSv;
double * UminTrav;
double * UminTravSv;
double * UmaxEntree;
double * UminEntree;
double * LTrav;
double * CoutsReduits;
/*----------------------------------*/
/* Pour le reduced cost fixing */
double   CritereAuNoeudRacine;
double   MxCoutReduitAuNoeudRacineFoisDeltaBornes;
double * CoutsReduitsAuNoeudRacine;
int * PositionDeLaVariableAuNoeudRacine;

/*
int NombreDeVariablesHorsBase;
int * NumeroDeVariableCoutReduit;
double * CoutsReduitsAuNoeudRacineFoisDeltaBornes;
*/
/*----------------------------------*/
int     NombreDeContraintesTrav;
int     NombreDeContraintesDInegalite;
/*----------------------------------*/
/* Le second membre des contraintes */ 
double * BTrav;
char   * SensContrainteTrav;
int    * ContrainteSaturee;
char   * ContrainteActivable; /* Utilisables que si presence de variables entieres OUI_PNE ou NON_PNE */
/*----------------------------------*/
/* Utilise dans le presolve */
double   PlusGrandTerme;
double   PlusPetitTerme;
/*----------------------------------*/
int   * CorrespondanceCntPneCntEntree;
/* En resultat les variables duales des contraintes */    
double * VariablesDualesDesContraintesTrav;
double * VariablesDualesDesContraintesTravEtDesCoupes;
int      TailleAlloueeVariablesDualesDesContraintesTravEtDesCoupes;
/*----------------------------------------*/
/* Chainage de la matrice des contraintes */ 
int   * MdebTrav;
int   * NbTermTrav;   
int   * NuvarTrav; 
double * ATrav;
/*---------------------------------------------------------*/
int   NombreDeGub;
int * NumeroDeContrainteDeLaGub;
int * ValeurDInstanciationPourLaGub;
/*---------------------------------------------------------*/
/* Chainage de la transposee de la matrice des contraintes */
char ChainageTransposeeExploitable; /* OUI_PNE ou NON_PNE */
int * CdebTrav;
int * CNbTermTrav;
int * CsuiTrav;
int * NumContrainteTrav;
/*---------------------------------------------------------*/
/* Tailles allouees                                        */
int NombreDeVariablesAllouees;
int NombreDeContraintesAllouees;
int TailleAlloueePourLaMatriceDesContraintes;  
/*---------------------------------------------------------*/

int VariableLaPlusFractionnaire;
/* Pour faire des branchements de type Gub */
int   DimBranchementGub;
int   ValeurAGauche;
int   NbVarGauche;
int * PaquetDeGauche;
int   ValeurADroite;
int   NbVarDroite;
int * PaquetDeDroite;
/*                                         */
int NumeroDeLaVariableFractionnaire;
int LaContrainteDisjointQuelqueChose;

int   NombreDeVariablesAValeurFractionnaire;
int * LaVariableAUneValeurFractionnaire;
double NormeDeFractionnalite;  

int   PremFrac;  /* Pour le classement des variables entieres dont */
int * SuivFrac;  /* la valeur est fractionnaire */

/*-----------------------------------*/
/*     Pour le strong branching      */
char     FaireDuStrongBranching;  /* Vaut OUI_PNE ou NON_PNE */ 
double * UStrongBranching;

/*-----------------------------------*/
/*           Pour les coupes         */
COUPES_DUN_PROBLEME_RELAXE Coupes;

char    ResolutionDuNoeudReussie;
int 	  NombreDeCoupesCalculees;            /* Nombre de coupes calculees au probleme courant */
int     NombreDeCoupesCalculeesNonEvaluees; /* C'est le nombre de coupes qui a ete calcule apres resolution du noeud */ 
int     NombreDeCoupesAjouteesAuRoundPrecedent;
double  DernierTauxParCoupe;
double  DbleNbIterLim;

int NbGDuCycle;
int NbIDuCycle;
int NbKDuCycle;
int NbGInsere;
int NbIInsere;
int NbKInsere;
COUPE_CALCULEE ** CoupesCalculees; /* Pointeur sur les coupes calculees au probleme courant */

int     NombreDeK;
double  SommeViolationsK;
double  SeuilDeViolationK;

int     NombreDeMIR_MARCHAND_WOLSEY;
double  SommeViolationsMIR_MARCHAND_WOLSEY;
double  SeuilDeViolationMIR_MARCHAND_WOLSEY;

int     NombreDeCliques;
double  SommeViolationsCliques;
double  SeuilDeViolationCliques;

int     NombreDImplications;
double  SommeViolationsImplications;
double  SeuilDeViolationImplications;

int     NombreDeBornesVariables;
double  SommeViolationsBornesVariables;
double  SeuilDeViolationBornesVariables;

int     NombreDeCoupesAjoute;

double * Coefficient_CG;
int   * IndiceDeLaVariable_CG;

double * ValeurLocale; /* Dimension nombre de variable et dont la validite n'est garantie que localement */
int    * IndiceLocal; /* Dimension nombre de variable et dont la validite n'est garantie que localement */

char * ContrainteKnapsack;

int *  CntDeBorneSupVariable; /* Pour chaque variable, numero de la contrainte qui decrit la borne variable */
                              /* Vaut -1 si pas de borne variable */
int *  CntDeBorneInfVariable;
char * ContrainteMixte;
char   CalculDeMIRmarchandWolsey;
int    NbEvitementsDeCalculsMIRmarchandWolsey;
int    NbEchecsConsecutifsDeCalculsMIRmarchandWolsey;
int    ProfondeurMirMarchandWolseTrouvees;
int *  FoisCntSuccesMirMarchandWolseyTrouvees; 

/*-----------------------------------*/

double ValeurOptimaleDuProblemeCourantAvantNouvellesCoupes;

/*-----------------------------------*/

char    YaDesVariablesEntieres;  /* Vaut OUI_PNE ou NON_PNE */
char    YaDesBigM;  /* Vaut OUI_PNE ou NON_PNE */
int     CestTermine;  
int     YaUneSolution; /* Vaut OUI_PNE ou NON_PNE */
int     YaUneSolutionEntiere; /* Vaut OUI_PNE ou NON_PNE */
double   CoutOpt;       /* Cout de la solution entiere stockee */
double   Z0;
double * UOpt;	        /* Vecteur contenant la solution optimale */
int     NumeroDeLaContrainteDeCoutMax;
int     NumeroDeLaVariableDEcartPourCoutMax;

/*-----------------------------------*/

int PremierIndexLibre; /* Premier index que l'on peut utiliser dans la matrice
                          des contraintes */

/*-------------------------------------------------------------------------*/

PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve;
CONFLICT_GRAPH * ConflictGraph;
CLIQUES * Cliques;
COUPES_DE_PROBING * CoupesDeProbing;
CONTRAITES_DE_BORNE_VARIABLE * ContraintesDeBorneVariable;
time_t HeureDeCalendrierDebutCliquesEtProbing;
time_t HeureDeCalendrierCourantCliquesEtProbing;
double TempsEcoule;
char ArreterCliquesEtProbing;

/*-------------------------------------------------------------------------*/

COUPES_K_NEGLIGEES * CoupesKNegligees;
COUPES_G_NEGLIGEES * CoupesGNegligees;

/*-------------------------------------------------------------------------*/
/* Informations utiles pour les operations de postsovle                    */

int NombreDOperationsDePresolve; 
int TailleTypeDOperationDePresolve;
char * TypeDOperationDePresolve;
int * IndexDansLeTypeDOperationDePresolve;

/*-------------------------------------------------------------------------*/
/* Ceci est a affiner. Ce sont des vecteurs qui permettent de retrouver les
   variables qui ont ete substituees dans le presolve. Dans un premier temps
	 on reserve une place equivalente a la taille de la matrice puis une fois
	 qu'on a fini on fait un realloc pour recuperer de la place */

int IndexLibreVecteurDeSubstitution;
int NbVariablesSubstituees;
int * NumeroDesVariablesSubstituees;
double * CoutDesVariablesSubstituees;  /* Je pense qu'on peut s'en passer */
int * ContrainteDeLaSubstitution;
double * ValeurDeLaConstanteDeSubstitution;
int * IndiceDebutVecteurDeSubstitution;
int * NbTermesVecteurDeSubstitution;
double * CoeffDeSubstitution;
int * NumeroDeVariableDeSubstitution;

/* Cas des colonnes colineaires qui correspondent a des variables de cout identique.
   Dans le presolve on cree cree une variable equivalente combinaison lineaire
	 des vraibles concernees: nouvelle variable = variable1 + ValeurDeNu * variable 2.
   En sortie il faut retrouver la valeur de chacune de ces variables. */
int NbCouplesDeVariablesColineaires;
int * PremiereVariable;
double * XminPremiereVariable;
double * XmaxPremiereVariable;

int * DeuxiemeVariable;
double * XminDeuxiemeVariable;
double * XmaxDeuxiemeVariable;

double * ValeurDeNu;

/* Cas des ligne singleton: il faut pouvoir restituer la valeur de la variable duale
   de la contrainte */
int NbLignesSingleton;
int * NumeroDeLaContrainteSingleton;
int * VariableDeLaContrainteSingleton;
double * SecondMembreDeLaContrainteSingleton;

/* Cas des forcing constraints: il faut pouvoir restituer la valeur de la variable duale
   de la contrainte */
int NbForcingConstraints;
int * NumeroDeLaForcingConstraint;

/* Cas des contraintes colineaires : il faut pouvoir restituer la valeur de la variable duale
   de la contrainte */
int NbSuppressionsDeContraintesColineaires;
int * ContrainteConservee;
int * ContrainteSupprimee;

/* Contraintes inactives */
int NombreDeContraintesInactives;
int * NumeroDesContraintesInactives;
	 
/*-------------------------------------------------------------------------*/

char   PrioriteDansSpxAuxVariablesSortantesEntieres; /* OUI_PNE ou NON_PNE */
void * ProblemeSpxDuSolveur;
void * ProblemeSpxDuNoeudRacine;
MATRICE_DE_CONTRAINTES * MatriceDesContraintesAuNoeudRacine;
void * ProblemeBbDuSolveur;
void * ProblemePrsDuSolveur;

/* Pour les temps */
time_t HeureDeCalendrierDebut;
time_t HeureDeCalendrierCourant;

/*-------------------------------------------------------------------------*/
int    AnomalieDetectee;
jmp_buf Env; 
/*-------------------------------------------------------------------------*/

double Critere;

} PROBLEME_PNE;
 
/*******************************************************************************************/
# define DEFINITIONS_PNE_FAITES	 
# endif  
# ifdef __cplusplus
  }
# endif		
