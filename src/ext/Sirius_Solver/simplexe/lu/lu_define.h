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
# ifndef LU_DEFINE_DEJA_DEFINI
/*****************************************************************************/		

# include "lu_sys.h"	        
# include "lu_constantes_externes.h"           
# include "lu_definition_arguments.h"               
# include "lu_constantes_internes.h"  
                         
/*****************************************************************************/
/* Super lignes dans le cas de la factorisation des matrices non symetriques: en test
   utilise pour diminuer le temps passe dans scan ligne */
   
typedef struct {
int     NumeroDeLaSuperLigne; 
int     ScannerLaSuperLigne; /* Vaut OUI ou NON */
int     NombreDeLignesDeLaSuperLigne;  
int   * NumerosDesLignesDeLaSuperLigne; 
int   * IndexDuTermeDiagonal;
/* Modele de la ligne */ 
int     NombreDeTermes; /* Correspond aussi au nombre de colonnes de la super ligne */
int   * IndiceColonne;
int     Capacite; /* C'est le nombre de colonnes qu'on peut mettre dans la super ligne */
/* Pointeur sur les colonnes de la super ligne */
double * ElmColonneDeSuperLigne;
/* Colonnes de la super ligne */
int     CapaciteDesColonnes; /* C'est le nombre de termes qu'on peut mettre dans chaque colonne d'une super ligne */
} SUPER_LIGNE_DE_LA_MATRICE; 

/*****************************************************************************/

typedef struct {
/* Pour les outils de gestion memoire */
void * Tas;
/*     */
int   IncrementDallocationDeL;
int   IncrementDallocationDeU;
/* Contexte de la factorisation */
char    ContexteDeLaFactorisation; /* Vaut:
                                    * LU_SIMPLEXE         si la factorisation est utilisee par le simplexe
                                    * LU_POINT_INTERIEUR  si la factorisation est utilisee par le point interieur
                                    * LU_GENERAL          si la factorisation est utilisee dans les autres cas
                                   */    
char    UtiliserLesSuperLignes;            /* Vaut OUI_LU ou NON_LU */
char    LaMatriceEstSymetriqueEnStructure; /* Vaut OUI_LU ou NON_LU */
/* Remarque: prevu pour les matrices carrees seulement */
int     NombreDeTermesDeLaMatriceAFactoriser; 
int     MargePourCreationDeTermesLignes;   
int     MargePourCreationDeTermesColonnes;

int     Rang;  /* Matrice supposee de rang plein => Rang = nombre de colonnes */
double  RangAuCarre;
double  RangAuCarrePlus1;	     

int     MaxScan; 	         

double  PivotMin;        /* Plus petite valeur de pivot acceptable */
double  SeuilDePivotage; /* Seuil d'acceptabilite d'un pivot par rapport au plus grand terme de la ligne */
int     NombreDeChangementsDeSeuilDePivotage; 

/* Pour le pivotage de Markowitz */
char   * LigneRejeteeTemporairementPourPivotage; 
char   * ColonneRejeteeTemporairementPourPivotage; 						       		 											  
double * AbsDuPlusGrandTermeDeLaLigne; 
int      PlusPetitNombreDeTermesDesLignes;
int      PlusPetitNombreDeTermesDesColonnes;

/* Pour la factorisation en matrice pleine */ 
char   FactoriserEnMatricePleine;
double ValeurDuPlusGrandTerme; 
int    LigneDuPlusGrandTerme;    
int    ColonneDuPlusGrandTerme;    
 
/* Stockage par ligne de la matrice active */
int   LIndexLibre; 
int   LDernierIndexLibrePossible;
int   DerniereLigne;
int   LIncrementDallocation;
int * LignePrecedente;
int * LigneSuivante;

int * Ldeb;
int * LNbTerm;
int * LDernierPossible;
int * LIndiceColonne;
double * Elm;

/* Chainage par colonne de la matrice active */					     
int   CIndexLibre;
int   CDernierIndexLibrePossible;
int   DerniereColonne;
int   CIncrementDallocation;
int * ColonnePrecedente;
int * ColonneSuivante;

int * Cdeb;
int * CNbTerm;
int * CNbTermMatriceActive;
int * CDernierPossible;
int * CIndiceLigne;

/* Pointeur vers la super ligne si la ligne est dans une super ligne */
  char MatricePleineDansUneSeuleSuperLigne;
  char LaMatriceEstPleine;
  SUPER_LIGNE_DE_LA_MATRICE ** SuperLigneDeLaLigne;       
  SUPER_LIGNE_DE_LA_MATRICE ** SuperLigneAScanner;
  SUPER_LIGNE_DE_LA_MATRICE ** SuperLigne;
  int      NombreDeSuperLignes;
  int    * PoidsDesColonnes; 
  /* Hash code */ 
  int HashModuloSize;
  /* Valable pour les lignes et les super lignes */
  unsigned int * HashCodeLigne;
  unsigned int * HashCodeSuperLigne;
  
  int * HashModuloPrem;   
  int * HashNbModuloIdentiques;
    
  char * TypeDeClassementHashCodeAFaire;
  int  * HashModuloSuiv;
  int  * HashModuloPrec;
  /* Pour le declenchement */
  char   OnDeclenche;
  int    NbFois;
  int    CycleDeBase;
  int    Cycle;
  int    NombreDePassagesDansSuperLignes;
  double SeuilDeVariationDeBase;
  double SeuilDeVariation;
  double TauxPrec;
  int   SeuilNombreDeSuperLigneAGrouper;
  int   SeuilNombreDeLignesAGrouper;
/* Fin super lignes */

/* Pour le chainage des lignes en fonction du nombre de termes */		  
int   * PremLigne; 						 		     
int   * SuivLigne; 				        
int   * PrecLigne; 	         			      

/* Pour le chainage des colonne en fonction du nombre de termes */
int   * PremColonne;
int   * SuivColonne; 
int   * PrecColonne;  

/* Termes de la matrice active */     
int     TailleAlloueeLignes; 
int     PremierEmplacementDisponibleLigne; 
int     TailleAlloueeColonnes; 
int     PremierEmplacementDisponibleColonne; 

/* Infos de travail */  
double * W;
char   * Marqueur;
/*
 double * ValeurDesTermesDeLaColonnePivot;
 int    * LigneDesTermesDeLaColonnePivot;  
*/
int     NbFillIn; /* Nombre de termes crees par la factorisation */
int     Kp;       /* Etape de la factorisation */

char    ExclureLesEchecsMarkowitz; 
char    LuUpdateEnCours;

int     CompteurExclusionMarkowitz; 

/* Permutations */
char  FaireDuPivotageDiagonal;           /* OUI_LU ou NON_LU */
char  LaMatriceEstSymetrique;            /* OUI_LU ou NON_LU */
int * OrdreLigne;  
int * InverseOrdreLigne;   
int * OrdreColonne;  
int * InverseOrdreColonne; 

/* Le triangle inferieur stocke par colonne */
int      IndexLibreDeL; 
int      DernierIndexLibreDeL; 
int    * NbTermesParColonneDeL; 
int    * CdebParColonneDeL; 
double * ElmDeL;		        					  
int    * IndiceLigneDeL;
int    * IndexKpDeUouL;   /* Utilise que par le point interieur */

/* Le triangle superieur stocke par ligne */ 
int      IndexLibreDeU; 
int      DernierIndexLibreDeU; 
int    * NbTermesParLigneDeU; 
int    * LdebParLigneDeU; 	   
double * ElmDeU;
int    * IndiceColonneDeU;
int    * CapaciteParLigneDeU;

char     SecondMembreCreux; /* Vaut OUI_LU ou NON_LU */

/* Le triangle inferieur stocke par ligne */
int    * LdebParLigneDeL; 
int    * NbTermesParLigneDeL; 
int    * IndiceColonneParLigneDeL;    
double * ElmDeLParLigne;		        					  
   
/* Le triangle superieur stocke par colonne */
int      IndexLibreDeUParColonne;				    
int      DernierIndexLibreDeUParColonne;	   	   
int    * CdebParColonneDeU;			     		   
int    * NbTermesParColonneDeU;	  
int    * IndiceLigneParColonneDeU;
int    * CapaciteParColonneDeU;
double * ElmDeUParColonne;
/* */
int   * StockageColonneVersLigneDeU;
/* */
int   * StockageLigneVersColonneDeU;
/* */
int     LimiteUpdatePourRefactorisation;
/* */

/* Vecteur temporaire pour les resolutions */
double * SolutionIntermediaire;

/* Pour l'hyper creux (simplexe uniquement) */
char *   NoeudDansLaliste;
int  *   ListeDesNoeuds;
int      TailleAlloueeDePseudoPile;
int  *   PseudoPile;

double   RatioMoyen;
int      NombreDeRatios;
int      NbTermesAuDepart;

double   RatioMoyenTransposee;
int      NombreDeRatiosTransposee;
int      NbTermesAuDepartTransposee;

/* En cas d'anomalie */			   
int    AnomalieDetectee;			      
jmp_buf Env; 
				       
/* En cas de matrice singuliere */
int   EtapeSinguliere;              /* Numero de l'etape de la factorisation a laquelle la matrice est singuliere */
int * NumeroDeTriangleDeLaVariable; /* Permet de savoir si la variable est indeterminee ou si elle se trouve dans un triangle factorise */

/* Permutation pour la LU update */
int * OrdreUcolonne;
int * InverseOrdreUcolonne;

int    NombreDeVecteursHAlloues;								   
int    NombreDElementsHAlloues;								   

int     NombreDeLuUpdates;	        

int    * HDeb;
int    * HLigne;
int    * HNbTerm;
int    * HIndiceColonne; 
double * HValeur; 
int     IndexHLibre;

double * ValeurElmSpike; 
int    * IndicesLignesDuSpike;
int      NbTermesNonNulsDuSpike;
char     SauvegardeDuResultatIntermediaire; /* Vaut OUI_LU ou NON_LU */

int     MxTermesCreesLuUpdate;  
int     NbTermesCreesLuUpdate;

/* Fin des donnees pour la LU_update */		    

/* Pour le Scaling s'il est demande */
char     FaireScaling; 
char     ScalingEnPuissanceDe2;
double * ScaleX;
double * ScaleB;

/* Pour le pivotage diagonal s'il est demande */
double * AbsValeurDuTermeDiagonal;

/* Les seuils */
char   UtiliserLesValeursDePivotNulParDefaut; /* Vaut OUI_LU ou NON_LU ( valeur conseillee: OUI_LU ) */                                 
double ValeurDuPivotMin;         /* Si UtiliserLesValeursDePivotNulParDefaut est egal a NON_LU, alors le module */
double ValeurDuPivotMinExtreme;  /* de factorisation utilise ces valeurs de pivot min */
int    NombreMaxDeChangementsDeSeuilDePivotage;

/* Pour la refactorisation (specifique point interieur) */
int    *  DebutInfosAdressesQueKpModifie; /* Nombre de lignes/colonnes */
double ** AdresseDeUModifie;
double ** AdresseUHaut;
int    *  DebutInfosLignesQueKpModifie; /* Nombre de lignes/colonnes */
int    *  NombreDeLignesQueKpModifie;   /* Nombre de lignes/colonnes */
double ** AdresseUGauche;
int    *  NombreDeTermesParLigneQueKpModifie;
int       PremierKpParCalculClassique;  

/* Stockage du pointeur vers la matrice d'entree */
MATRICE_A_FACTORISER * MatriceEntree;

/* Pour le raffinement iteratif */
double * SecondMembreSV; 
double * SolutionSV;

/* Pour la mise au point des super lignes */
int NbScanSuperLignes;  
int NbLignesSuperLignes;
int NbScanMin; 
int NombreDeTermes;

/* Pour la regularisation dans le cas du point interieur */
double   ValeurDeRegularisation;
char     OnARegularise;
char     OnPeutRegulariser;
double * TermeDeRegularisation;

} MATRICE;

/*******************************************************************************************/
# define LU_DEFINE_DEJA_DEFINI 	
# endif
# ifdef __cplusplus
  }
# endif    




