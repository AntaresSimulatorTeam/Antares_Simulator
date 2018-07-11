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
# ifndef DEFINITIONS_PROBLEME_PI_FAITES	
/*******************************************************************************************/

# include "pi_constantes_externes.h"

# define VERBOSE_PI    0

# define PI_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE 
# undef PI_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE
	
# define MPCC NON_PI
				
# define BORNEE                1  /* La variable est bornee des deux cotes */	 
# define BORNEE_INFERIEUREMENT 2  /* La variable n'est bornee qu'inferieurement */	 
# define BORNEE_SUPERIEUREMENT 3  /* La variable n'est bornee que superieurement. 
                                     Rq: en entree un traitement supplementaire la transforme en bornee inferieurement
			                               de telle sorte qu'en cours d'algorithme il n'y a plus que des variables 
				                          	 bornees inferieurement */	 
# define NON_BORNEE            4  /* La variable n'est ni inferieurement ni superieurement bornee */

# define ZERO          1.e-15 /*1.e-14*/
# define VZER          ( 1.1 * ZERO )
  
# define VALEUR_DE_REGULARISATION     1.e-9  /*1.e-8*/
# define VALEUR_DU_PIVOT_MIN          (1.e-3 * VALEUR_DE_REGULARISATION)  /*1.e-10*/
# define VALEUR_DU_PIVOT_MIN_EXTREME  (1.e-3 * VALEUR_DE_REGULARISATION)  /*1.e-10*/

# define LINFINI_PI      1.e+80 
 
# define MUK_MIN         0.0    /*1.e-12*/  /* 1.e-9 */
# define MUK_MAX         1.e+20 /* Pas de limite (presque) */ /*1.e+9*/    

# define TOLERANCE_ADMISSIBILITE_PAR_DEFAUT     1.e-5 /*1.e-6*/  
# define TOLERANCE_STATIONNARITE_PAR_DEFAUT     1.e-5 /*1.e-5*/  
# define TOLERANCE_COMPLEMENTARITE_PAR_DEFAUT   1.e-5 /*1.e-5*/  

# define CONDITIONNEMENT_MIN   100.    
# define CONDITIONNEMENT_MAX   1000.  

# define NOMBRE_MAX_DITERATION 300 /*200*/

# define INCREMENT_DALLOCATION 4096 

# define AFFINE         1   
# define CENTRALISATION 2
    
#ifndef COMPACT
  # define COMPACT 1 
#endif

#ifndef CREUX 
  # define CREUX   2
#endif

# define BON     1 
# define APPROX  2
# define PARFAIT 3
# define RANGE       1.e+8
# define RANGE_MIN   1.e+6
# define RANGE_MAX   1.e+10
# define SEUIL_PARFAIT 1.e-8
# define SEUIL_APPROX  1.e-6

# define ITERATION_DEBUT_RAFINEMENT_ITERATIF 6 /* Il faut mettre un nombre pair */

typedef struct {
/* La matrice est rangee par colonne */
int     NombreDElementsAlloues;
double * Elm;
int   * Indl;
int   * Ideb;
int   * Nonu;
int     NombreDeColonnes;
/* Resultat de la factorisation: les triangles L et U */
int     NbElementsDeU;
/* Triangle U par Ligne */
int   * IndexDebutDesLignesDeU; 
int   * NbTermesDesLignesDeU;    
double * ValeurDesTermesDeU;     
double * ValeurDesTermesDeU_SV;    
int   * IndicesDeColonneDeU;
/* Emplacement du terme diagonal dans la factorisee */
int   * IndexDuTermeDiagonalDansU; 
/* Table de travail utilisee pour les resolutions de systeme et les factorisations */ 
double * Sec;
} MATRICE_PI;
 

typedef struct {
/* Pour les outils de gestion memoire */
void * Tas;

MATRICE_PI * MatricePi;
  
int   NombreDeVariables;   
int   NombreDeContraintes;
int   NombreDeContraintesSV;
int * CorrespondanceVarEntreeVarPi;   
int * CorrespondanceCntEntreeCntPi;

/*-------------------------------------------------------------------------*/
int    AnomalieDetectee;
jmp_buf Env; 
/*-------------------------------------------------------------------------*/
/* Coefficients de la fonction objectifs et leur chainage */
double * Q;
double * L;
double * Qpar2;  /* c'est 2 * Q */

/* Chainage de la matrice des contraintes */
int   NbTermesAllouesPourA;
double * A;
double * B;
int * Mdeb;
int * NbTerm;    
int * Indcol;   

/* Chainage de la matrice transposee des contraintes **/
int * Cdeb;
int * CNbTerm;
double * ACol;
int   * NumeroDeContrainte;
int   * Csui;

/* Chainage de la matrice A * AT par colonne */
/*double * MoinsDMoins1;
int     NbTermesAllouesPourAFoisATranspose;
int *   CDebFormeNormale;
int *   CNbTermFormeNormale;
int *   LigneFormeNormale;
double * AFormeNormale;
*/

/* Les variables */ 
double * U;      
double * U0;    
double * Umax;    
double * Umin;
double * Alpha; /* Coeff de la matrice diagonale de changement de variable (utilite en test) */
double * Alpha2; /* Alpha au carre */
char *   TypeDeVariable; /* Bornée, non bornée etc... */
/* Pour la penalisation des contraintes de complementarite (norme l infini) */
char *   VariableBinaire; /* OUI_PI ou NON_PI */
int     NombreDeVariablesBinaires; 
int *   NumeroDeLaContrainteDeComplementarite;
double   Rho; /* Coefficient de penalisation de la variable Xi */
int     NumeroDeLaVariableXi;
/*               */
double * S1;    
double * S2;   
double * Lambda;
double * Lambda0;

/* Pour le scaling */ 
double * ScaleU;
double * ScaleB;
double   ScaleLigneDesCouts;
double   ScaleLigneDesU;
char     PrendreEnCompteLesCoutsLineairesDansLeScaling;
 
/* Pour les iterations du calcul variables */
double SeuilDOptimalite;
double SeuilDAdmissibilite;
double SeuilDeStationnarite;
 
double Smin; 
double G;  
double Ro;
 
double * DeltaU;
double * DeltaLambda;
double * DeltaS1;    
double * DeltaS2;
double * UkMoinsUmin;	   
double * UmaxMoinsUk;  
double * UnSurUkMoinsUmin;
double * UnSurUmaxMoinsUk;
double * DeltaUDeltaS1; 
double * DeltaUDeltaS2; 

double * SecondMembre;
double * SecondMembreAffine;

double Gamma;
char   Resolution;
double Range;

int ComplementariteAtteinte;
int RealisabiliteAtteinte;
int StationnariteAtteinte;
int NumeroDIteration; 
 
int     TypeDIteration;
double   Muk;
double * MukIteration;	 
double   MukPrecedent;

double * MaxOptimalite;
double * MaxContrainte;
double * MaxStationnarite;
double * IterationTetakP;
double * IterationTetakD;

double * RegulVar;
double * RegulContrainte;

int ArretSta;
int ArretCnt;
int ArretOpt;
int Traces;

int YaUneSolution;

void * MatriceFactorisee;
void * Matrice; /* C'est la matrice a factoriser */

} PROBLEME_PI;

/*******************************************************************************************/
# define DEFINITIONS_PROBLEME_PI_FAITES	 
# endif  
# ifdef __cplusplus
  }
# endif
