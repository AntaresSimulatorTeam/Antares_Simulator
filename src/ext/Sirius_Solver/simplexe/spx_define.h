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
# ifndef DEFINITIONS_SPX_FAITES
/*******************************************************************************************/

# include "spx_sys.h" 
# include "spx_constantes_externes.h"  
# include "spx_constantes_internes.h"  

/* Pour les coupes d'intersection */
typedef struct{
int      NombreDeTermes;
int    * NumeroDeVariableMatrice;
double * ProduitScalaire; 
} LIGNE_DE_PRODUITS_SCALAIRES;

typedef struct{
/* Les variables continues */
int     NombreDeTermes;
int *   NumeroDeVariableSpx;
double * Coefficient;  
double   SecondMembre;
} LIGNE_DE_MATRICE;

typedef struct{
double *                       Vecteur;
char   *                       LaVariableSpxEstEntiere; /* OUI_SPX ou NON_SPX */
double *                       B;
LIGNE_DE_PRODUITS_SCALAIRES ** LigneDeProduitScalaire;
LIGNE_DE_MATRICE **            LigneDeMatrice;
double *                       NormeAvantReduction;
int                            NombreDeVariables;
double *                       XmaxSv;

double   AlphaI0;
char   * TSpx;
double * CoeffSpx;
} DONNEES_POUR_COUPES_DINTERSECTION; 

/* Pour les Gomory */
typedef struct{
char   * T;
double * Coeff; 
double * B;
char   * LaVariableSpxEstEntiere; /* OUI_SPX ou NON_SPX */
double * XmaxSv;
} DONNEES_POUR_COUPES_DE_GOMORY;   

/* Pour la mini exploration */
typedef struct{
char   * PositionDeLaVariable;
char   * InDualFramework; 
int   * ContrainteDeLaVariableEnBase;
double * DualPoids;
int   * VariableEnBaseDeLaContrainte;
} BASE_INSTANCIATION; 

typedef struct{
void * NoeudPere;
void * Fils_0;
void * Fils_1;
BASE_INSTANCIATION * BaseDuNoeud;
int VariableSortieDeLaBase;
char TypeDeSortieDeLaBase;
int Profondeur;
} NOEUD_INSTANCIATION; 

/* Pour le probleme Spx */
typedef struct {
/* Pour les outils de gestion memoire */
void * Tas;

/*------------------------------------------------------------------------*/	
int   NbCycles;
char   AffichageDesTraces;
char   TypeDePricing; /* PRICING_DANTZIG ou PRICING_STEEPEST_EDGE */
char   FaireDuScalingSPX;
char   StrategieAntiDegenerescence; /* AGRESSIF ou PEU_AGRESSIF */
int   CycleDeControleDeDegenerescence;

char   EcrireLegendePhase1;
char   EcrireLegendePhase2;    
int   Contexte;
int   AlgorithmeChoisi;
int   LaBaseDeDepartEstFournie;
int   YaUneSolution;
int   NombreMaxDIterations;
double DureeMaxDuCalcul;
int   CycleDeRefactorisation; 
double UnSurRAND_MAX;

int FaireDuRaffinementIteratif;
char ChoixDeVariableSortanteAuHasard; 
char BaseInversibleDisponible;
int NombreMaxDeChoixAuHasard;
int NombreDeChoixFaitsAuHasard;  
char AdmissibilitePossible;
char InverseProbablementDense;
char ToleranceSurLesVariablesEntieresAjustees; /* OUI_SPX ou NON_SPX */

/*----------------------      Tailles allouees      -----------------------*/
int    NombreDeVariablesAllouees;
int    NombreDeContraintesAllouees;
int    NbTermesAlloues;
/*---------------------- Les variables du probleme ------------------------*/
char    PresenceDeVariablesDeBornesIdentiques;
int     NombreDeVariables;
int     NombreDeVariablesACoutNonNul;
int     NombreDeVariablesNatives;
int     NombreDeVariablesDuProblemeSansCoupes;
int *   NumeroDesVariablesACoutNonNul;
double * C;                                  /* Dimension nombre de variables */
double * Csv;                                /* Dimension nombre de variables */         
double * X;                                  /* Dimension nombre de variables */
double * Xmin;                               /* Dimension nombre de variables */
double * Xmax;                               /* Dimension nombre de variables */
char   * TypeDeVariable;                     /* 3 valeurs possibles:  
                                                BORNEE , BORNEE_INFERIEUREMENT , NON_BORNEE */
char   * OrigineDeLaVariable;                /* 3 valeurs possibles:  
                                                NATIVE , ECART , BASIQUE_ARTIFICIELLE */	 
/* Bornes auxiliaires:
1- Pour l'instant seul la borne sup est utilisee.
2- Pour l'instant seul le type BORNE_AUXILIAIRE_PRESOLVE est utilise.
3- Elles ne s'appliquent pas aux variables non bornees. */
int     NombreDeBornesAuxiliairesUtilisees;
int     IterationPourBornesAuxiliaires;
double  CoeffMultiplicateurDesBornesAuxiliaires;  
char   * StatutBorneSupCourante;   /* BORNE_NATIVE, BORNE_AUXILIAIRE_PRESOLVE, BORNE_AUXILIAIRE_FICTIVE */
double * BorneSupAuxiliaire;
char   * StatutBorneSupAuxiliaire; /* BORNE_AUXILIAIRE_PRESOLVE, BORNE_AUXILIAIRE_FICTIVE, BORNE_AUXILIAIRE_INVALIDE */
/* Fin bornes auxilaires */

int   * CntVarEcartOuArtif;   
double * XEntree;                            /* Dimension nombre de variables */
double * XminEntree;                         /* Dimension nombre de variables */
double * XmaxEntree;                         /* Dimension nombre de variables */
double * SeuilDeViolationDeBorne;            /* Dimension nombre de variables */

double * SeuilDAmissibiliteDuale1; /* Une valeur par variable pour tenir compte  du scaling */
double * SeuilDAmissibiliteDuale2; /* Une valeur par variable pour tenir compte  du scaling */

double * ScaleX;                             /* Dimension nombre de variables */    
double   ScaleLigneDesCouts;             
int   * CorrespondanceVarEntreeVarSimplexe; /* Dimension nombre de variables d'entree */
int   * CorrespondanceVarSimplexeVarEntree; /* Dimension nombre de variables d'entree + Nombre de contraintes d'entree */

/*------------------------------------------------------------------------*/
/*---------------------- Matrice des contraintes -------------------------*/
int     NombreDeContraintes;
int     NombreDeContraintesDuProblemeSansCoupes;
double * B;                                  /* Dimension nombre de contraintes */
double * BAvantTranslationEtApresScaling;    /* Dimension nombre de contraintes */
double * ScaleB;                             /* Dimension nombre de contraintes */
int   * Mdeb;                               /* Dimension nombre de contraintes */ 
int   * NbTerm;                             /* Dimension nombre de contraintes */
double * A;                                  /* Dimension nombre de termes */ 
int   * Indcol;                             /* Dimension nombre de termes */
int   * CorrespondanceCntSimplexeCntEntree; /* Dimension nombre de contraintes */
int   * CorrespondanceCntEntreeCntSimplexe; /* Dimension nombre de contraintes */
/*-------------------------------------------------------------------------*/
/*---- Matrice des contraintes avec uniquement les variables hors base ----*/
int * IndexDansLaMatriceHorsBase;
int * MdebHorsBase;
int * NbTermHorsBase;
double * AHorsBase;
int * IndcolHorsBase;
int * InverseIndexDansLaMatriceHorsBase;
 
/*------------------------------------------------------------------------*/
double   ValeurMoyenneDuSecondMembre; /* Pour construire des bornes sup artificielles */
double   PlusGrandTermeDeLaMatrice;
double   PlusPetitTermeDeLaMatrice;
double   RapportDeScaling;
double   CoutMoyen;
double   EcartDeBornesMoyen;
double   PerturbationMax;
/*---------------- Transposee de la matrice des contraintes --------------*/
char     StockageParColonneSauvegarde;  
int *   Cdeb ;                       /* Dimension nombre de variables */
int *   Csui ;                       /* Dimension nombre de termes */
int *   CNbTerm;	              /* Dimension nombre de variables */
int *   CNbTermSansCoupes;	      /* Dimension nombre de variables */
int *   CNbTermesDeCoupes;	      /* Dimension nombre de variables */
double * ACol;                        /* Dimension nombre de termes */
int *   NumeroDeContrainte;          /* Dimension nombre de termes */
int *   CdebBase;                    /* Dimension nombre de contraintes */
int *   NbTermesDesColonnesDeLaBase; /* Dimension nombre de contraintes */
/*-------------------------- Eta matrices ---------------------------------*/
int     LastEta;
int     RemplissageMaxDeLaFPI; 
int   * EtaDeb;          /* Nombre de changements de base avant refactorisation */
int   * EtaNbTerm;       /* Nombre de changements de base avant refactorisation */
int   * EtaColonne;      /* Nombre de changements de base avant refactorisation */
int   * EtaIndiceLigne;  /* Nombre de chgt de base avant refactorisation * nb contraintes */
double * EtaMoins1Valeur; /* Nombre de chgt de base avant refactorisation * nb contraintes */
/*------------------------------------------------------------------------*/ 
/*------------------- Zone des donnees de travail ------------------------*/

int   Iteration;       
time_t HeureDeCalendrierDebut;
int   NbCyclesSansControleDeDegenerescence;
int   PhaseEnCours;            /* PHASE_1 ou PHASE_2 */   

int   ChangementDeBase;  
int   VariableEntrante;
double DeltaXSurLaVariableHorsBase;
int   VariableSortante;
int   SortSurXmaxOuSurXmin;
int   NombreDeChangementsDeBase; 
char   StrongBranchingEnCours;
char   PremierSimplexe;
char   BBarreAEteCalculeParMiseAJour; /* Vaut OUI_SPX ou NON_SPX */  
char   CBarreAEteCalculeParMiseAJour; /* Vaut OUI_SPX ou NON_SPX */  

/* Tableau de travail */
int * T; /* dimensionne au nombre de variables (i.e. toujours > au nombre de contraintes, toujours -1 quand on en a besoin */

/* Tableaux de travail */
int     NbABarreSNonNuls;        /* Utile que dans le cas TypeDeStockageDeABarreS = COMPACT_SPX */
int   * CntDeABarreSNonNuls;     /* Utile que dans le cas TypeDeStockageDeABarreS = COMPACT_SPX */
double * ABarreS;                 /* Dimension nombre de contraintes */

double * Bs;        /* Dimension nombre de contraintes */
double * BBarre;    /* Dimension nombre de contraintes */
char     CalculerBBarre; 
char     FaireMiseAJourDeBBarre;
int     BuffNbBoundFlip;
int     NbItBoundFlip;
int     NbBoundFlipIterationPrecedente;

int     NbBoundFlip;
int *   BoundFlip; /* Dimension nombre de variables: (Numero de variable + 1) si passe Xmin vers Xmax
                                                     -(Numero de variable + 1) si passe Xmax vers Xmin */
double   ABarreSCntBase;      

/* Donnees concernant la base */
double * Pi;                           /* Dimension nombre de contraintes */
double * CBarre;                       /* Dimension nombre de variables */ 
char *   PositionDeLaVariable;         /* Dimension nombre de variables */
int *   ContrainteDeLaVariableEnBase; /* Dimension nombre de variables */
int *   VariableEnBaseDeLaContrainte; /* Dimension nombre de contraintes */
int *   NombreDeVariablesHorsBaseDeLaContrainte; /* Dimension nombre de contraintes */

/* Tableaux pour le pricing */
int NombreDeContraintesASurveiller;
int   * IndexDansContrainteASurveiller;   /* Dimensionne au nombre de contraintes */
int   * NumerosDesContraintesASurveiller; /* Dimensionne au nombre de contraintes */
double * ValeurDeViolationDeBorne;         /* Au carre. Dimensionne au nombre de contraintes */

/* Variables hors base */  
int     NombreDeVariablesHorsBase;
int *   NumerosDesVariablesHorsBase; /* Dimension: variables - nombre de contraintes */
double * NBarreR;                     /* Dimension: variables */
int *   IndexDeLaVariableDansLesVariablesHorsBase; /* Dimension: nombre de variables */

/* Cas hyper creux */
int     NombreDeValeursNonNullesDeNBarreR;
int *   NumVarNBarreRNonNul;      /* Dimension: variables - nombre de contraintes */
/*            */

/* Donnees specifiques a l'algorithme dual */
double   SeuilDePivotDual;
int     NombreDeVariablesATester;
char     PremierPassage;
char     FaireTriRapide;
int   * NumeroDesVariableATester;
double * CBarreSurNBarreR;              
double * CBarreSurNBarreRAvecTolerance;

char     TypeDeStockageDeErBMoinsUn;
int     NbTermesNonNulsDeErBMoinsUn;
int   * IndexTermesNonNulsDeErBMoinsUn;
double * ErBMoinsUn;

double   DeltaPiSurLaVariableEnBase;

char     CalculerCBarre;

double   SommeDesInfaisabilitesPrimales;          
double   Cout;              /* Valeur du cout de la solution */
double   CoutMax;           /* Valeur du cout au dessus de laquelle on arrete les calculs (utilite: branch and bound) */
int     UtiliserCoutMax;   /* Vaut OUI_SPX si on desire faire le test par rapport a CoutMax, et NON_SPX si on ne veut 
                               pas utiliser cette possibilite */
double   PartieFixeDuCout;  /* Partie du cout qui est due aux variables dont la valeur est fixe. C'est calculé en 
                               entree du solveur */

char     LeSteepestEdgeEstInitilise;
char *   InDualFramework; /* Pour la methode dual devex ou steepest edge: dimension nombre de variables */
double * DualPoids      ; /* Pour la methode dual devex ou steepest edge: dimension nombre de contraintes */
double * Tau            ; /* Pour la methode dual devex ou steepest edge: dimension nombre de contraintes */

char   LesCoutsOntEteModifies; 
char   ModifCoutsAutorisee;
double CoefficientPourLaValeurDePerturbationDeCoutAPosteriori;
char * CorrectionDuale;  

/* Les zones memoire ci-dessous peuvent etre liberees des la fin de phase 1 de l'algorithme dual mais
   on ne le fait pas afin de pouvoir utiliser ces zones comme tableaux temporaires si necessaire */
char     LaBaseEstDualeAdmissible;
int     NbInfaisabilitesDualesALaPremiereIteration;
int     NbInfaisabilitesDuales;
double   SommeDesInfaisabilitesDuales;                                    
double * V;                        /* Dimension nombre de contraintes */ 
char *   FaisabiliteDeLaVariable;  /* Dimension nombre de variables */
                    
/* Fin des zones memoires specifiques a la fin de phase 1 de l'algorithme dual */

/* Sauvegardes pour le branch and bound (utile pour le choix de la variable a instancier) */
double * XSV; 	                         /* Dimension nombre de variables */
double   ValeurDuPivotMarkowitzSV;       /* Le seuil Markowitz qui a servi a factoriser la derniere base
                                            inversible */
char   * PositionDeLaVariableSV;         /* Dimension nombre de variables */
double * CBarreSV; 	                 /* Dimension nombre de variables */
char   * InDualFrameworkSV;	         /* Dimension nombre de variables */
int   * ContrainteDeLaVariableEnBaseSV; /* Dimension nombre de variables */

double * BBarreSV;                       /* Dimension nombre de contraintes */
double * DualPoidsSV;                    /* Dimension nombre de contraintes */
int   * VariableEnBaseDeLaContrainteSV; /* Dimension nombre de contraintes */

int   * CdebBaseSV;                    /* Dimension nombre de contraintes */
int   * NbTermesDesColonnesDeLaBaseSV; /* Dimension nombre de contraintes */

/*-------------------------------------------------------------------------*/
  
char  UtiliserLaLuUpdate;
int  FaireScalingLU;
char  FactoriserLaBase; /* Indicateur positionne a OUI_SPX en cas de derive de la forme produit de l'inverse */
char  FaireChangementDeBase;

/* Precaution pour la stabilite des calculs. Lorsqu'un probleme de stabilite des calculs est detecte et
   qu'on demande une factorisation, on augmente le seuil du pivot de Markowitz. On le decremente
   ensuite tant qu'on ne rencontre pas de probleme de stabilite numerique */
char   ProblemeDeStabiliteDeLaFactorisation; /* Vaut OUI_SPX ou NON_SPX */
char   FlagStabiliteDeLaFactorisation;       /* Vaut 0 ou 1 */
double ValeurDuPivotMarkowitz;

/*-------------------------------------------------------------------------*/

/* Pour le pilotage de l'hyper creux */
char TypeDeStockageDeABarreS; /* Vaut COMPACT_SPX ou VECTEUR_SPX */
char TypeDeStockageDeNBarreR; /* Vaut COMPACT_SPX ou VECTEUR_SPX */

char TypeDeCreuxDeLaBase; /* Vaut BASE_HYPER_CREUSE ou BASE_CREUSE ou BASE_PLEINE */

char CalculErBMoinsUnEnHyperCreux;       /* OUI_SPX au depart */
char CalculErBMoinsEnHyperCreuxPossible; /* OUI_SPX au depart */
int CountEchecsErBMoins;                /* 0 au depart */
int AvertissementsEchecsErBMoins;       /* 0 au depart */
int NbEchecsErBMoins;                   /* 0 au depart */

char CalculABarreSEnHyperCreux;         /* OUI_SPX au depart */
char CalculABarreSEnHyperCreuxPossible; /* OUI_SPX au depart */
int CountEchecsABarreS;                /* 0 au depart */
int AvertissementsEchecsABarreS;       /* 0 au depart */
int NbEchecsABarreS;                   /* 0 au depart */

char CalculTauEnHyperCreux;         /* OUI_SPX au depart */
char CalculTauEnHyperCreuxPossible; /* OUI_SPX au depart */
int CountEchecsTau;                /* 0 au depart */
int AvertissementsEchecsTau;       /* 0 au depart */
int NbEchecsTau;                   /* 0 au depart */

/*-------------------------------------------------------------------------*/
    
void * MatriceFactorisee;
int RangDeLaMatriceFactorisee;
int NombreDeFactorisationsDeBaseReduite;
int ForcerUtilisationDeLaBaseComplete; /* OUI_SPX ou NON_SPX */
int NombreDeReactivationsDeLaBaseReduite;
int NombreDeBasesReduitesSuccessives;
int NombreDeBasesCompletesSuccessives;
int NombreDinfaisabilitesSiBaseReduite;
int NbEchecsReductionNombreDinfaisabilitesSiBaseReduite;
char InitBaseReduite;
char * PositionHorsBaseReduiteAutorisee; /* OUI_SPX ou NON_SPX */
                                      
char UtiliserLaBaseReduite; /* OUI_SPX ou NON_SPX */
int  IterationDeConstructionDeLaBaseReduite;
int  ProchaineIterationDeReinitDesCouts;

int * OrdreColonneDeLaBaseFactorisee; /* Dimension nombre de contraintes */
int * ColonneDeLaBaseFactorisee; /* Dimension nombre de contraintes */
int * OrdreLigneDeLaBaseFactorisee; /* Dimension nombre de contraintes */
int * LigneDeLaBaseFactorisee; /* Dimension nombre de contraintes */

int * CdebProblemeReduit;    /* Dimension nombre de variables */
int * CNbTermProblemeReduit; /* Dimension nombre de variables */
double * ValeurDesTermesDesColonnesDuProblemeReduit;
int * IndicesDeLigneDesTermesDuProblemeReduit;
int NbElementsAllouesPourLeProblemeReduit;

double * AReduit; /* Dimension nombre de contraintes toujours a 0 quand on en a besoin */
int * IndexAReduit; /* Dimension nombre de contraintes */

int * Marqueur;  /* Dimension nombre de contraintes toujours a -1 quand on en a besoin */

/*-------------------------------------------------------------------------*/
int    AnomalieDetectee;
jmp_buf EnvSpx; 			   
/*-------------------------------------------------------------------------*/

/* Pour les coupes d'intersection */
DONNEES_POUR_COUPES_DINTERSECTION * DonneesPourCoupesDIntersection;
char CoupesDintersectionAllouees; /* Vaut OUI_SPX ou NON_SPX */

/* Pour les coupes coupes de Gomory */
DONNEES_POUR_COUPES_DE_GOMORY * DonneesPourCoupesDeGomory;

/*---------------------------------*/
/*         Pour la PNE             */
void * ProblemePneDeSpx; /* Mis a jour par la PNE */

char   ExplorationRapideEnCours; /* OUI_SPX ou NON_SPX */

/*------------------------------------------*/
/* Pour utiliser le tirage aleatoire de pne */
double A1;

/*------------------------------------------*/

} PROBLEME_SPX; 
  

/*******************************************************************************************/
# define DEFINITIONS_SPX_FAITES
# endif  
# ifdef __cplusplus
  }
# endif




