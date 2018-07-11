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

   FONCTION: Resolution de Min c x sous contrainte Ax = b par un  
             simplexe (forme revisee du simplexe) en matrices  
             creuses
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"
   
# include "spx_fonctions.h"
# include "spx_define.h"

/*----------------------------------------------------------------------------*/

void SPX_SimplexeCalculs( PROBLEME_SIMPLEXE * Probleme , PROBLEME_SPX * Spx )
{
/* Le probleme d'entree */
int Contexte; int NombreMaxDIterations; 
double * C_E ; double * X_E; double * Xmin_E; double * Xmax_E; int NbVar_E; int * TypeVar_E; 
int NbContr_E; int * Mdeb_E; int * NbTerm_E; int * Indcol_E; double * A_E; char * TypeDeContrainte_E; double * B_E;
int  ChoixDeLAlgorithme; int BaseDeDepartFournie_E; 
int * PositionDeLaVariable_E; int * NbVarDeBaseComplementaires_E; int * ComplementDeLaBase_E;
int * ExistenceDUneSolution; int LibererMemoireALaFin;    
double CoutMax; int UtiliserCoutMax;
int NombreDeContraintesCoupes; double * BCoupes; char * PositionDeLaVariableDEcartCoupes; 
int * MdebCoupes; int * NbTermCoupes; int * NuvarCoupes; double * ACoupes; 
double * CoutsMarginauxDesContraintes; 
double * CoutsReduits; int Var;
/*                     */
int Cnt; int i; double UnSurScaleLigneDesCouts; double C; double u; int il_E; int il_EMx; int Cnt_E;
int * CorrespondanceCntEntreeCntSimplexe; double * Pi; double * ScaleB; char * CorrectionDuale;
/*int * CorrespondanceVarEntreeVarSimplexe;*/

/*----------------------------------------------------------------------------------------------------*/  

/*printf("Debut SPX_SimplexeCalculs\n");*/

Contexte             = Probleme->Contexte;
NombreMaxDIterations = Probleme->NombreMaxDIterations; 
 
C_E       = Probleme->CoutLineaire;
X_E       = Probleme->X;   
Xmin_E    = Probleme->Xmin;
Xmax_E    = Probleme->Xmax;
NbVar_E   = Probleme->NombreDeVariables;
TypeVar_E = Probleme->TypeDeVariable;

NbContr_E          = Probleme->NombreDeContraintes;
Mdeb_E             = Probleme->IndicesDebutDeLigne;
NbTerm_E           = Probleme->NombreDeTermesDesLignes;
Indcol_E           = Probleme->IndicesColonnes;
A_E                = Probleme->CoefficientsDeLaMatriceDesContraintes;
TypeDeContrainte_E = Probleme->Sens;
B_E                = Probleme->SecondMembre;

/*ChoixDeLAlgorithme    = Probleme->ChoixDeLAlgorithme;*/
ChoixDeLAlgorithme = SPX_DUAL;

BaseDeDepartFournie_E  = Probleme->BaseDeDepartFournie;
PositionDeLaVariable_E = Probleme->PositionDeLaVariable;
NbVarDeBaseComplementaires_E = &Probleme->NbVarDeBaseComplementaires;
ComplementDeLaBase_E         = Probleme->ComplementDeLaBase;

ExistenceDUneSolution = &Probleme->ExistenceDUneSolution;
LibererMemoireALaFin  = Probleme->LibererMemoireALaFin;

/* Pour tenir compte de petites erreurs pendant l'algorithme on augment CoutMax */
C = 0.0; /* Car la marge est ajoutee ensuite */
CoutMax         = Probleme->CoutMax + C;
UtiliserCoutMax = Probleme->UtiliserCoutMax;

NombreDeContraintesCoupes        = Probleme->NombreDeContraintesCoupes;
BCoupes                          = Probleme->BCoupes;
PositionDeLaVariableDEcartCoupes = Probleme->PositionDeLaVariableDEcartCoupes;
MdebCoupes                       = Probleme->MdebCoupes;
NbTermCoupes                     = Probleme->NbTermCoupes;
NuvarCoupes                      = Probleme->NuvarCoupes;
ACoupes                          = Probleme->ACoupes;

CoutsMarginauxDesContraintes = Probleme->CoutsMarginauxDesContraintes;
CoutsReduits                 = Probleme->CoutsReduits;

Spx->AffichageDesTraces = Probleme->AffichageDesTraces;

Spx->NbCycles = 0;

Spx->TypeDePricing = (char) Probleme->TypeDePricing;
if ( Spx->TypeDePricing != PRICING_DANTZIG && Spx->TypeDePricing != PRICING_STEEPEST_EDGE ) {
  printf("TypeDePricing pas correctement renseigne\n");
  exit(0);
}

Spx->FaireDuScalingSPX = (char) Probleme->FaireDuScaling;
if ( Spx->FaireDuScalingSPX != OUI_SPX && Spx->FaireDuScalingSPX != NON_SPX ) {
  printf("FaireDuScalingSPX pas correctement renseigne\n");
  exit(0);
}
Spx->StrategieAntiDegenerescence = (char) Probleme->StrategieAntiDegenerescence;
if ( Spx->StrategieAntiDegenerescence != AGRESSIF && Spx->StrategieAntiDegenerescence != PEU_AGRESSIF ) {
  printf("StrategieAntiDegenerescence pas correctement renseigne\n");
  exit(0);
}

Spx->DureeMaxDuCalcul = Probleme->DureeMaxDuCalcul;
if ( Spx->DureeMaxDuCalcul > 0. ) SPX_InitDateDebutDuCalcul( Spx );

Spx->ExplorationRapideEnCours = NON_SPX;

/*----------------------------------------------------------------------------------------------------*/  

#if VERBOSE_SPX
  printf("Entree dans le simplexe: \n");
  printf("      nombre de variables  :  %d\n",NbVar_E);
  printf("      nombre de contraintes:  %d\n",NbContr_E);
  fflush(stdout);
#endif

*ExistenceDUneSolution = NON_SPX; /* Precaution. En realite la mise a jour est faite au moment
                                     du return c'est pour ca qu'il n'en faut qu'un seul */ 

Spx->YaUneSolution   = OUI_SPX;
Spx->PremierSimplexe = NON_SPX;

if ( Spx->StrategieAntiDegenerescence == AGRESSIF ) {
  Spx->CycleDeControleDeDegenerescence = CYCLE_DE_CONTROLE_DE_DEGENERESCENCE_AGRESSIF;
}
else {
  Spx->CycleDeControleDeDegenerescence = CYCLE_DE_CONTROLE_DE_DEGENERESCENCE_PEU_AGRESSIF;
}

if      ( Contexte == BRANCH_AND_BOUND_OU_CUT       ) Spx->Contexte = BRANCH_AND_BOUND_OU_CUT;
else if ( Contexte == BRANCH_AND_BOUND_OU_CUT_NOEUD ) Spx->Contexte = BRANCH_AND_BOUND_OU_CUT_NOEUD;
else if ( Contexte == SIMPLEXE_SEUL                 ) Spx->Contexte = SIMPLEXE_SEUL;
else {
  printf("Erreur dans les donnees du simplexe, l'indicateur de contexte d'utilisation est mal initialisé:\n");
  printf("       les valeurs acceptées sont BRANCH_AND_BOUND_OU_CUT, BRANCH_AND_BOUND_OU_CUT_NOEUD et SIMPLEXE_SEUL.\n");
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

if      ( BaseDeDepartFournie_E == OUI_SPX ) Spx->LaBaseDeDepartEstFournie = OUI_SPX;
else if ( BaseDeDepartFournie_E == NON_SPX ) Spx->LaBaseDeDepartEstFournie = NON_SPX;
else if ( BaseDeDepartFournie_E == UTILISER_LA_BASE_DU_PROBLEME_SPX ) {
  if ( NombreDeContraintesCoupes > 0 ) {
    printf("Erreur dans les donnees du simplexe, l'indicateur de fourniture de base n est pas correctement initialise \n");
    printf("Vous demandez a utiliser la base courante du probleme simplexe mais vous demandez aussi de modifier le probleme\n");
    printf("en y ajoutant des coupes. C'est incompatible car la dimension de la base change.\n");
    Spx->AnomalieDetectee = OUI_SPX;
    longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
  }
  Spx->LaBaseDeDepartEstFournie = UTILISER_LA_BASE_DU_PROBLEME_SPX;
	/* Dans ce cas, on force le contexte a etre BRANCH_AND_BOUND_OU_CUT_NOEUD ce qui permet a l'appelant
	   de mettre SIMPLEXE_SEUL et de faire du hot start d'un facon differente de celle du branch and bound
		 ainsi c'est moins destabilisant pour lui que de mettre BRANCH_AND_BOUND_OU_CUT_NOEUD alors qu'il
		 n'en fait pas */
	Contexte = BRANCH_AND_BOUND_OU_CUT_NOEUD;	 
	Spx->Contexte = BRANCH_AND_BOUND_OU_CUT_NOEUD;	 
}
else {
  printf("Erreur dans les donnees du simplexe, l'indicateur de fourniture de base n est pas initialise \n");
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

if ( ChoixDeLAlgorithme == SPX_PRIMAL )    { 
  Spx->AlgorithmeChoisi = SPX_PRIMAL;
  printf("Algorithme primal a valider, ne pas l utiliser\n");
  exit(0);
}
else if ( ChoixDeLAlgorithme == SPX_DUAL ) Spx->AlgorithmeChoisi = SPX_DUAL;
else {
  printf("Erreur dans les donnees du simplexe, l argument qui precise l'algorithme choisi est incorrect \n");
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

Spx->FlagStabiliteDeLaFactorisation       = 0;  
Spx->ProblemeDeStabiliteDeLaFactorisation = NON_SPX;

/* Pour etre certain de ne pas utiliser a tort les infos sur les coupes. Ainsi en dehors d'un contexte de branch and bound
   avec possibilités de coupes, l'appelant n'a pas a initialiser NombreDeContraintesCoupes */
if ( Spx->Contexte == SIMPLEXE_SEUL || Spx->Contexte == BRANCH_AND_BOUND_OU_CUT ) NombreDeContraintesCoupes = 0;

if ( Spx->Contexte == SIMPLEXE_SEUL || Spx->Contexte == BRANCH_AND_BOUND_OU_CUT ) {
  SPX_AllouerProbleme( Spx , NbVar_E , NbContr_E , Mdeb_E , NbTerm_E );
  SPX_ConstruireLeProbleme( Spx , C_E , X_E , Xmin_E     , Xmax_E   , NbVar_E  , TypeVar_E , 
                            NbContr_E , Mdeb_E     , NbTerm_E , Indcol_E , A_E       , 
                            TypeDeContrainte_E     , B_E                  , 
                            PositionDeLaVariable_E , *NbVarDeBaseComplementaires_E   , 
                            ComplementDeLaBase_E   , CoutMax , UtiliserCoutMax );			    
}
else {
  SPX_ModifierLeProbleme( Spx , C_E, X_E, Xmin_E, Xmax_E, NbVar_E, TypeVar_E,  
	                  CoutMax  , UtiliserCoutMax );
  if ( Spx->LaBaseDeDepartEstFournie != UTILISER_LA_BASE_DU_PROBLEME_SPX ) {
    /* Si on a choisi d'utiliser la base du probleme simplexe courant, alors il est inutile de la reconstruire.
       De plus, on ne peut pas ajouter de coupes (donc de contraintes) sinon le probleme change et la base
       n'a meme plus la bonne dimension */
    SPX_ConstruireLaBaseDuProblemeModifie( Spx , NbVar_E, PositionDeLaVariable_E, *NbVarDeBaseComplementaires_E,
                                           ComplementDeLaBase_E );
																					 
    /* Les coupes */
    SPX_AjouterLesCoupes( Spx , NombreDeContraintesCoupes, PositionDeLaVariableDEcartCoupes,  
                          MdebCoupes               , NbTermCoupes                    ,   
                          NuvarCoupes              , ACoupes                         ,       
                          BCoupes ); 

    SPX_CompleterLaBaseDuProblemeModifie( Spx , NbVar_E, NbContr_E , PositionDeLaVariable_E ); 

    /* Maintenant il faut faire le chainage de la transposee */
		/* Pour l'instant: si on fait de la relaxation de contraintes on refait systeùatiquement le chainage a cause
		   de Spx->CNbTermSansCoupes */
    if ( NombreDeContraintesCoupes > 2 * Spx->NombreDeContraintesDuProblemeSansCoupes ) {
      SPX_ChainageDeLaTransposee( Spx , COMPACT );
    }
    else {
      SPX_ModifierLeChainageDeLaTransposee( Spx );
    }
  }
  else {
    Spx->LaBaseDeDepartEstFournie = OUI_SPX;
    /* Reinitialisation des corrections duales meme si on repart de la base du probleme SPX */
    CorrectionDuale = Spx->CorrectionDuale;
    for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) CorrectionDuale[i] = NOMBRE_MAX_DE_PERTURBATIONS_DE_COUT;    				
  }
}

if ( Spx->YaUneSolution == NON_SPX ) goto FinDuSimplexe;

SPX_BruitageInitialDesCouts( Spx );

SPX_AjusterTolerancesVariablesEntieres( Spx );

SPX_InitialiserLeTableauDesVariablesHorsBase( Spx );

Spx->UtiliserLaLuUpdate = OUI_SPX /*OUI_SPX*/;       
Spx->UtiliserLaBaseReduite = NON_SPX;
Spx->ForcerUtilisationDeLaBaseComplete = 0;
Spx->NombreDeFactorisationsDeBaseReduite = 0;
Spx->NombreDeReactivationsDeLaBaseReduite = 0;

Spx->NombreDeBasesCompletesSuccessives = NB_DE_BASES_COMPLETES_SUCCESSIVES;
Spx->NombreDeBasesReduitesSuccessives = NB_DE_BASE_REDUITE_SUCCESSIVES_SANS_PRISE_EN_COMPTE_DES__VIOLATIONS;
Spx->NombreDinfaisabilitesSiBaseReduite = Spx->NombreDeContraintes;
Spx->NbEchecsReductionNombreDinfaisabilitesSiBaseReduite = 0;
Spx->InitBaseReduite = OUI_SPX;

for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) Spx->PositionHorsBaseReduiteAutorisee[Var] = OUI_1_FOIS;

Spx->Iteration = 0;
Spx->NombreDeChangementsDeBase = 0;
Spx->NombreMaxDIterations      = NOMBRE_MAX_DITERATIONS; 
if ( Spx->NombreMaxDIterations <= ( 5 * Spx->NombreDeContraintes ) ) {
  Spx->NombreMaxDIterations = 5 * Spx->NombreDeContraintes;
}
/* Prise en compte de la valeur fournie si elle est valide */
if ( NombreMaxDIterations > 0 ) Spx->NombreMaxDIterations = NombreMaxDIterations;

if ( Spx->AlgorithmeChoisi == SPX_PRIMAL ) Spx->CycleDeRefactorisation = CYCLE_DE_REFACTORISATION; 
else                                       Spx->CycleDeRefactorisation = CYCLE_DE_REFACTORISATION_DUAL;

/*Spx->LastEta               = -1;*/
Spx->RemplissageMaxDeLaFPI = (int) floor( 0.6 * Spx->NombreDeContraintes * Spx->CycleDeRefactorisation ) + 1;

#if VERBOSE_SPX
  printf("Dans le simplexe: \n");
  printf("      nombre de variables  :  %d\n",Spx->NombreDeVariables);
  printf("      nombre de contraintes:  %d\n",Spx->NombreDeContraintes); fflush(stdout);
#endif

Spx->StrongBranchingEnCours = NON_SPX;

/* Factoriser la premiere base */
Spx->BaseInversibleDisponible = NON_SPX;
SPX_FactoriserLaBase( Spx );

if ( Spx->YaUneSolution == NON_SPX ) goto FinDuSimplexe;

/* Et c'est parti dans les iterations: que la force soit avec nous ! */
if ( Spx->AlgorithmeChoisi == SPX_PRIMAL ) {
  return;
}
else {
  SPX_DualSimplexe( Spx );
}

#if VERBOSE_SPX
  printf("Fin du simplexe a l iteration %d\n",Spx->Iteration);
#endif

# ifdef UTILISER_BORNES_AUXILIAIRES
  /* Controle: il ne doit JAMAIS rester de bornes auxilaires */
	if ( Spx->NombreDeBornesAuxiliairesUtilisees != 0 ) {
	  printf("BUG, le nombre de bornes auxiliaires doit etre nul en fin de simplexe: %d\n",Spx->NombreDeBornesAuxiliairesUtilisees);
		exit(0);
	}	
# endif

FinDuSimplexe:

if ( Spx->YaUneSolution == OUI_SPX ) {

  /* Recalcul systematique des variables duales car elle ne sont pas mise a jour a chaque iteration */
  /* Calcul de Pi = c_B * B^{-1} */
	
  SPX_CalculerPi( Spx );
	
  /* On initialise les valeurs des variables en fonction de leur position */
  SPX_FixerXEnFonctionDeSaPosition( Spx );	
  
  /* Dans le cas d'un contexte de Branch And Bound ou de Branch And Cut on sauvegarde les donnees 
     qui seront necessaires pour faire du strong branching voire des coupes de Gomory. Si la base 
     ne vient pas d'être factorisée, on la factorise. */
  if ( Spx->Contexte != SIMPLEXE_SEUL ) SPX_SauvegardesBranchAndBoundAndCut( Spx );
  
  /* Ecretage Xmin Xmax avant recuperation de la solution */
  /*  
  for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) {
    if ( Spx->OrigineDeLaVariable[i] != NATIVE ) continue;
    if ( Spx->TypeDeVariable[i] == BORNEE_INFERIEUREMENT ) { 
      if ( Spx->X[i] < 0. ) {
        printf("Variables internes ecretage var %d a xmin %lf car x = %lf Position %d\n",
	        i,Spx->Xmin[i],Spx->X[i],Spx->PositionDeLaVariable[i]); 
        if ( Spx->X[i] < -0.001 ) exit(0);
        Spx->X[i] = 0.;
      }
    }
    else if ( Spx->TypeDeVariable[i] == BORNEE ) {
      if ( Spx->X[i] < 0. ) {
        printf("Variables internes ecretage var %d a xmin %lf car x = %lf  Position %d\n",
	        i,Spx->Xmin[i],Spx->X[i],Spx->PositionDeLaVariable[i]);
        if ( Spx->X[i] < -0.001 ) exit(0);
        Spx->X[i] = 0.;
      }
      else if ( Spx->X[i] > Spx->Xmax[i] ) { 
        printf("Variables internes ecretage var %d a xmax %lf car x = %lf Position %d\n",
	        i,Spx->Xmax[i],Spx->X[i],Spx->PositionDeLaVariable[i]);
        if ( Spx->X[i] > Spx->Xmax[i] + 0.001 ) exit(0);
        Spx->X[i] = Spx->Xmax[i];
      }
    }
  }
  */ 
	
	/* Traces: verification de l'admissibilite primale dans les variables du simplexe */
	/*
	{ int il; int ilMax; double Seuil; double S;
	  printf("Attention verification admissibilite primale systematique\n");
	  Seuil = 1.e-5;
    for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
      il = Spx->Mdeb[Cnt];
	  	ilMax = il + Spx->NbTerm[Cnt];
	  	S = 0.0;
		  while ( il < ilMax ) {
        S += Spx->A[il] * Spx->X[Spx->Indcol[il]];    
        il++;
		  }
		  if ( fabs( Spx->B[Cnt] - S ) / ( Spx->ScaleB[Cnt] * Spx->SupXmax ) > Seuil ) {
        printf("1- Probleme d'admissibilite primale sur la contrainte %d \n",Cnt);
			  printf("B = %e S = %e ecart = %e\n",Spx->B[Cnt],S,fabs( Spx->B[Cnt] - S ));
			  exit(0);
		  }
		  if ( fabs( Spx->B[Cnt] - S ) > Seuil ) {
        printf("2- Probleme d'admissibilite primale sur la contrainte %d \n",Cnt);
			  printf("B = %e S = %e ecart = %e\n",Spx->B[Cnt],S,fabs( Spx->B[Cnt] - S ));
			  exit(0);
		  }		
	  }
  }
	*/
	
	/* */
  SPX_RecupererLaSolution( Spx, NbVar_E, X_E, TypeVar_E   , NbContr_E , PositionDeLaVariable_E , 
                           NbVarDeBaseComplementaires_E , ComplementDeLaBase_E /* , InDualFramework_E , DualPoids_E */ );
  if ( Spx->Contexte != SIMPLEXE_SEUL && NombreDeContraintesCoupes > 0 ) {
    SPX_RecupererLaSolutionSurLesCoupes( Spx , NombreDeContraintesCoupes, NbTermCoupes, PositionDeLaVariableDEcartCoupes ); 
  }	

	/* Recuperation des couts marginaux et des couts reduits */
  /* Afin d'avoir les couts reduits sur les variables fixes en entree */
	if ( CoutsReduits != NULL ) memcpy( (char *) CoutsReduits, (char *) C_E, NbVar_E * sizeof( double ) );
  
  UnSurScaleLigneDesCouts = 1. / Spx->ScaleLigneDesCouts;
  CorrespondanceCntEntreeCntSimplexe = Spx->CorrespondanceCntEntreeCntSimplexe;
  Pi     = Spx->Pi;
  ScaleB = Spx->ScaleB;	
  /* Les contraintes */
  for ( Cnt_E = 0 ; Cnt_E < NbContr_E ; Cnt_E++ ) {
    Cnt = CorrespondanceCntEntreeCntSimplexe[Cnt_E];
    if ( Cnt < 0 && CoutsMarginauxDesContraintes != NULL ) {
      CoutsMarginauxDesContraintes[Cnt_E] = 0.0;
      continue;
    }
    u = UnSurScaleLigneDesCouts * Pi[Cnt] * ScaleB[Cnt];
		if ( 	CoutsMarginauxDesContraintes != NULL ) CoutsMarginauxDesContraintes[Cnt_E] = u;
		if ( CoutsReduits != NULL ) {
      il_E = Mdeb_E[Cnt_E];
      il_EMx = il_E + NbTerm_E[Cnt_E];
      while ( il_E < il_EMx ) {
        CoutsReduits[Indcol_E[il_E]]-= u * A_E[il_E];
        il_E++;
      }
		}
  }	
  /* Les coupes */
  /* Par convention, si la partie branch and bound veut desactiver certaines coupes sans les renumeroter,
     il lui suffit de mettre a 0 les valeurs de NbTermCoupes pour les coupes en question. Il est donc necessaire
     de faire le traitement special qui suit lorsque l'on retourne les valeurs des variables duales */
  if ( CoutsMarginauxDesContraintes != NULL ) {		
    for ( Cnt = Spx->NombreDeContraintesDuProblemeSansCoupes ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
      CoutsMarginauxDesContraintes[Cnt] = UnSurScaleLigneDesCouts * Pi[Cnt] * ScaleB[Cnt];
    }
	}
	
	if ( CoutsReduits != NULL ) {	
	  Cnt = Spx->NombreDeContraintesDuProblemeSansCoupes;  
    for ( i = 0 ; i < NombreDeContraintesCoupes ; i++ ) {    
      if ( NbTermCoupes[i] > 0 ) {		
        if ( CoutsMarginauxDesContraintes != NULL ) u = CoutsMarginauxDesContraintes[Cnt];
			  else u = UnSurScaleLigneDesCouts * Pi[Cnt] * ScaleB[Cnt];			
        il_E = MdebCoupes[i];
        il_EMx = il_E + NbTermCoupes[i];
        while ( il_E < il_EMx ) {							
          CoutsReduits[NuvarCoupes[il_E]]-= u * ACoupes[il_E];
          il_E++;
        }      
        Cnt++;
		  }
		}
	}
			
  /* Precaution pour eviter les petites derives */
	/*CorrespondanceVarEntreeVarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe;*/
  for ( i = 0 ; i < NbVar_E ; i++ ) {
	  /*if ( CorrespondanceVarEntreeVarSimplexe[i] < 0 ) continue;*/	
    if ( TypeVar_E[i] != VARIABLE_FIXE && TypeVar_E[i] != VARIABLE_NON_BORNEE ) {		
      /* Pour les tests */
      /*
      if ( X_E[i] < Xmin_E[i] ) {
        printf("ecretage var %d a xmin %lf car x = %lf\n",i,Xmin_E[i],X_E[i]) ; 
        if ( X_E[i] < Xmin_E[i] - 0.0001 ) exit(0);
      }
      if ( X_E[i] > Xmax_E[i] ) {
        printf("ecretage var %d a xmax %lf car x = %lf\n",i,Xmax_E[i],X_E[i]);
        if ( X_E[i] > Xmax_E[i] + 0.0001 ) exit(0);
      }
      */
      /* Fin tests */			
      if ( X_E[i] < Xmin_E[i] ) { 
	      X_E[i] = Xmin_E[i];
      }
      else if ( X_E[i] > Xmax_E[i] ) {
        X_E[i] = Xmax_E[i];
      }			
    }
  }
	
goto PasDeTraces;

printf(" Lancement d'une verification d'admissiblite par le simplexe lui-meme ... a n utiliser que pour la mise au point\n");

/* Traces */
{
  int Cnt_E; int il; int ilMax; double S; int OnSort; int OnEcrit; double Seuil; double Ec; int CntSpx;
	
	Seuil = 1.e-5;
		
  for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) {

    if ( Spx->OrigineDeLaVariable[i] != BASIQUE_ARTIFICIELLE ) continue; 

    if( Spx->PositionDeLaVariable[i] != HORS_BASE_SUR_BORNE_INF ) {
      /* La variable est en base. La base doit etre degeneree et la variable sur borne inf. */
      if (Spx->PositionDeLaVariable[i] != EN_BASE_SUR_BORNE_INF ) { 
       /* printf("*** Pas de solution car variable de base %d valeur %lf EN_BASE Xmax %lf \n",
                i,Spx->BBarre[Spx->ContrainteDeLaVariableEnBase[i]],Spx->Xmax[i]); */  
        if ( Spx->Xmax[i] > 1.1 * SEUIL_DE_DEGENERESCENCE ) { 
          printf("*** Pas de solution car variable de base %d valeur %lf EN_BASE Xmax %lf \n",
                  i,Spx->BBarre[Spx->ContrainteDeLaVariableEnBase[i]],Spx->Xmax[i]); 
          Spx->AnomalieDetectee = OUI_SPX;
          longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
        }
      }
      else {
        Cnt = Spx->ContrainteDeLaVariableEnBase[i];
        Cnt_E = Spx->CorrespondanceCntSimplexeCntEntree[Cnt];
        /* printf("*** Variable de base %d valeur %lf EN_BASE sur borne inf contrainte %d type %c\n",
                 i,Spx->X[i],Cnt_E,TypeDeContrainte_E[Cnt_E]); */ 
      }
    }
  }
  OnSort = NON_SPX;
  for ( Cnt_E = 0 ; Cnt_E < NbContr_E ; Cnt_E++ ) {
    il = Mdeb_E[Cnt_E];
    ilMax = il + NbTerm_E[Cnt_E];
    S = 0.;  
    OnEcrit = NON_SPX;
    while ( il < ilMax ) {
      S+= A_E[il] * X_E[Indcol_E[il]];
      il++;
    }
    if ( TypeDeContrainte_E[Cnt_E] == '=' ) { 
      if ( fabs( B_E[Cnt_E] - S ) > Seuil ) { 
        printf("  Erreur admissibilite contrainte %d type = B %e Calcul %e depassement %e ",Cnt_E,B_E[Cnt_E],S,fabs( B_E[Cnt_E] - S ));
	      if ( Spx->CorrespondanceCntEntreeCntSimplexe[Cnt_E] >= 0 ) {
	        printf(" ScaleB %e B_spx %e ",Spx->ScaleB[Spx->CorrespondanceCntEntreeCntSimplexe[Cnt_E]],
                   B_E[Cnt_E]*Spx->ScaleB[Spx->CorrespondanceCntEntreeCntSimplexe[Cnt_E]]);
	      }
	      printf("\n");
        OnSort  = OUI_SPX;
        OnEcrit = OUI_SPX;
      }
    } 
    if ( TypeDeContrainte_E[Cnt_E] == '<' ) { 
      if ( S > B_E[Cnt_E] + Seuil ) { 
        printf("  Erreur admissibilite contrainte %d type < B %e Calcul %e depassement %e ",Cnt_E,B_E[Cnt_E],S,fabs( B_E[Cnt_E] - S ));
	      if ( Spx->CorrespondanceCntEntreeCntSimplexe[Cnt_E] >= 0 ) {
	        printf(" ScaleB %e ",Spx->ScaleB[Spx->CorrespondanceCntEntreeCntSimplexe[Cnt_E]]);
	      }
	      printf("\n");	
        OnSort  = OUI_SPX;
        OnEcrit = OUI_SPX;
      }
    }
    if ( TypeDeContrainte_E[Cnt_E] == '>' ) { 
      if ( S < B_E[Cnt_E] - Seuil ) { 
        printf("  Erreur admissibilite contrainte %d type > B %e Calcul %e depassement %e ",Cnt_E,B_E[Cnt_E],S,fabs( B_E[Cnt_E] - S ));
	      if ( Spx->CorrespondanceCntEntreeCntSimplexe[Cnt_E] >= 0 ) {
	       printf(" ScaleB %e ",Spx->ScaleB[Spx->CorrespondanceCntEntreeCntSimplexe[Cnt_E]]);
	      }
	      printf("\n");	
        OnSort  = OUI_SPX;
        OnEcrit = OUI_SPX;
      }
    }
    if( OnEcrit == OUI_SPX ) {
      CntSpx = Spx->CorrespondanceCntEntreeCntSimplexe[Cnt_E];
		  printf("B[Cnt] %e Contrainte Spx numero %d NbTermes spx %d ScaleB %e\n",Spx->B[CntSpx],CntSpx,Spx->NbTerm[CntSpx],Spx->ScaleB[CntSpx]);

      il = Spx->Mdeb[CntSpx];
		  ilMax = il + Spx->NbTerm[CntSpx];
		  S = 0.0;
		  while ( il < ilMax ) {
			  printf("Valeur de la variable SPX %e  xmin %e  xmax %e  scale %e\n",
				        Spx->X[Spx->Indcol[il]] / Spx->ScaleX[Spx->Indcol[il]],
				        Spx->Xmin[Spx->Indcol[il]],Spx->Xmax[Spx->Indcol[il]],Spx->ScaleX[Spx->Indcol[il]]);

			  printf("VarSpx %d X ecart scaleB %e\n",Spx->Indcol[il],Spx->X[Spx->Indcol[il]] / ( Spx->ScaleX[Spx->Indcol[il]] * Spx->ScaleB[CntSpx] ));
								
        S += Spx->A[il] * ( Spx->X[Spx->Indcol[il]] / Spx->ScaleX[Spx->Indcol[il]] );    
        il++;
		  }
      printf("Ecart sur Cnt Spx %d \n",CntSpx);
		  S /= Spx->ScaleB[CntSpx];
			printf("B = %e S = %e ecart = %e   B/ScaleB = %e  ScaleB = %e\n",Spx->B[CntSpx]/Spx->ScaleB[CntSpx],S,
			        fabs( (Spx->B[CntSpx]/Spx->ScaleB[CntSpx]) - S ),Spx->B[CntSpx]/Spx->ScaleB[CntSpx],Spx->ScaleB[CntSpx]);			
			
		  Ec = 0.0;	
      il = Mdeb_E[Cnt_E];
      ilMax = il + NbTerm_E[Cnt_E];
      while ( il < ilMax ) {
        printf(" Entree: Var %d X %e Xmin %e Xmax %e A %e ",Indcol_E[il],X_E[Indcol_E[il]],Xmin_E[Indcol_E[il]],Xmax_E[Indcol_E[il]],A_E[il]);
	      if ( Spx->CorrespondanceVarEntreeVarSimplexe[Indcol_E[il]] >= 0 ) {
				
          i = Spx->CorrespondanceVarEntreeVarSimplexe[Indcol_E[il]];
					S = Spx->X[i] / Spx->ScaleX[i]; /* Valeur SPX */

					printf("\n Simplexe: Var %d X %e SeuilViolation %e A %e Xmin %e  Xmax %e Position %d ScaleX %e \n",
					           i,
										 S,
										 Spx->SeuilDeViolationDeBorne[i],
										 A_E[il]*Spx->ScaleB[Spx->CorrespondanceCntEntreeCntSimplexe[Cnt_E]]*Spx->ScaleX[i],
					           Spx->Xmin[i],
					           Spx->Xmax[i],
										 Spx->PositionDeLaVariable[i],
                     Spx->ScaleX[i]);

          if ( TypeVar_E[Indcol_E[il]] == VARIABLE_FIXE ) printf("Var_E est FIXE\n");
					else if ( TypeVar_E[Indcol_E[il]] == VARIABLE_BORNEE_DES_DEUX_COTES ) printf("Var_E est VARIABLE_BORNEE_DES_DEUX_COTES\n");
					else if ( TypeVar_E[Indcol_E[il]] == VARIABLE_BORNEE_INFERIEUREMENT ) printf("Var_E est VARIABLE_BORNEE_INFERIEUREMENT\n");
					else if ( TypeVar_E[Indcol_E[il]] == VARIABLE_BORNEE_SUPERIEUREMENT ) printf("Var_E est VARIABLE_BORNEE_SUPERIEUREMENT\n");
					else if ( TypeVar_E[Indcol_E[il]] == VARIABLE_NON_BORNEE ) printf("Var_E est VARIABLE_NON_BORNEE\n");
					else printf("Variable de type inconnu\n");

          if ( Spx->TypeDeVariable[i] == BORNEE ) printf("VarSpx est BORNEE\n");
					else if ( Spx->TypeDeVariable[i] == BORNEE_INFERIEUREMENT )  printf("VarSpx est BORNEE_INFERIEUREMENT\n");
					else if ( Spx->TypeDeVariable[i] == BORNEE_SUPERIEUREMENT )  printf("VarSpx est BORNEE_SUPERIEUREMENT\n");
 					else if ( Spx->TypeDeVariable[i] == NON_BORNEE )  printf("VarSpx est NON_BORNEE\n");
					else printf("Variable Spx de type inconnu\n");        					
															 					
					printf("produit A*X = %e  produit A*X du simplexe = %e  ecart = %e\n",
					        A_E[il] * X_E[Indcol_E[il]], A_E[il]*Spx->ScaleX[i] * S,(A_E[il] * X_E[Indcol_E[il]])-(A_E[il]*Spx->ScaleX[i] * S) );
									
          Ec += X_E[Indcol_E[il]] - (S*Spx->ScaleX[i]);
					
					if ( S < Spx->Xmin[i] - Spx->SeuilDeViolationDeBorne[i] ||
					     S > Spx->Xmax[i] + Spx->SeuilDeViolationDeBorne[i] ) {
            printf("Violation de borne sur la variable simplexe %d\n",Spx->CorrespondanceVarEntreeVarSimplexe[Indcol_E[il]]);
            printf("X %e\n",S);
            printf("Xmin %e\n",Spx->Xmin[Spx->CorrespondanceVarEntreeVarSimplexe[Indcol_E[il]]]);
            printf("Xmax %e\n",Spx->Xmax[Spx->CorrespondanceVarEntreeVarSimplexe[Indcol_E[il]]]);					 
					}
									 
	      }
	      printf("\n");
        il++;
      }
			printf("Ec = %e\n",Ec);
    }
  }
  
  if( OnSort == OUI_SPX ) {
	  /* Constitution d'un fichier MPS avant l'exit */
    SPX_EcrireProblemeSpxAuFormatMPS( Spx );
    exit(0); 
    /*
    Spx->AnomalieDetectee = OUI_SPX;
    longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); 
    */
  }
  
printf(" CONTROLE D AMISSIBILITE EN SORTIE DE SIMPLEXE SATISFAISANT\n");
}
/* Fin traces */		      

}
else {
  #if VERBOSE_SPX
    printf("Pas de solution \n");
  #endif
  /* On recupere quand meme les resultats sur les coupes. Pourquoi (au contraire, c'est idiot) ? */
  /*
  if ( Spx->Contexte != SIMPLEXE_SEUL && NombreDeContraintesCoupes > 0 ) {
    SPX_RecupererLaSolutionSurLesCoupes( Spx , PositionDeLaVariableDEcartCoupes );  
  }
  */
}

PasDeTraces:

*ExistenceDUneSolution = Spx->YaUneSolution; 

if ( LibererMemoireALaFin == OUI_SPX ) SPX_LibererProbleme( Spx );

return;

}
