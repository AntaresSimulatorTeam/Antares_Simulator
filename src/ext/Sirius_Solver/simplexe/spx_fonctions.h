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
# ifndef FONCTIONS_SPX_DEJA_DEFINIES
/*-----------------------------------------------------------------------------------------*/

# include "spx_definition_arguments.h"
# include "spx_define.h"

/* En cours pour tests heuristique 
spx_dual_sortir_les_variables_binaires.c


*/

/*----------------------------- Fonctions du simplexe ------------------------------*/

PROBLEME_SPX * SPX_Simplexe( PROBLEME_SIMPLEXE * , PROBLEME_SPX * );  
			  
void SPX_SimplexeCalculs( PROBLEME_SIMPLEXE * , PROBLEME_SPX * );
 
void SPX_AllouerProbleme( PROBLEME_SPX * , int , int , int * , int * );

void SPX_AugmenterLeNombreDeVariables( PROBLEME_SPX * );
 
void SPX_AugmenterLeNombreDeContraintes( PROBLEME_SPX * );

void SPX_AugmenterLaTailleDeLaMatriceDesContraintes(  PROBLEME_SPX * );

void SPX_LibererProbleme( PROBLEME_SPX * );

void SPX_InitDateDebutDuCalcul( PROBLEME_SPX * );
   
void SPX_ControleDuTempsEcoule( PROBLEME_SPX * );

/*---------------------------------------------------------------------------------*/

void SPX_ConstruireLeProbleme( PROBLEME_SPX * ,
                        double * , double * , double * , double * , int     , int * ,
                        int     , int   * , int   * , int   * , double * , char * ,
                        double * , int   * , int     , int   * , double   , int );

void SPX_ModifierLeProbleme( PROBLEME_SPX * , double * , double * , double * , double * ,
                             int , int * , double , int );

void SPX_OrdonnerMatriceDesContraintes( int , int * , int * , int * , double * );
														 
void  SPX_ConstruireLaBaseDuProblemeModifie( PROBLEME_SPX * , int , int * , int ,  
                                             int * );
																						 			     
void SPX_CompleterLaBaseDuProblemeModifie( PROBLEME_SPX * , int , int , int * );

void SPX_ModifierLeVecteurCouts( PROBLEME_SPX * , double * , int );

void SPX_ModifierLeVecteurSecondMembre( PROBLEME_SPX * , double * , char * , int );

void SPX_InitialiserLeNombreDesVariablesHorsBaseDesContraintes( PROBLEME_SPX * );

void SPX_MettreAJourLeNombreDesVariablesHorsBaseDesContraintes( PROBLEME_SPX * ); 

void SPX_InitialiserLeTableauDesVariablesHorsBase( PROBLEME_SPX * );

void SPX_MajDuTableauDesVariablesHorsBase( PROBLEME_SPX * );

void SPX_InitialiserLesVariablesEnBaseAControler( PROBLEME_SPX * ); 

void SPX_MajDesVariablesEnBaseAControler( PROBLEME_SPX * , int , int * , int * );
 		
void SPX_AjouterLesCoupes( PROBLEME_SPX * , int , char * , int * , int * , int * , double * , double * ); 

void SPX_MettreLaContrainteSousFormeStandard( PROBLEME_SPX * , int ); 
	    
void SPX_RecupererLaSolution( PROBLEME_SPX * , int , double * , int * , int , int * , int * , int * );

void SPX_RecuperationSimplifieeDeLaSolutionPourStrongBranching( PROBLEME_SPX * , int , int * , int * , int * );

void SPX_RecupererLaSolutionSurLesCoupes( PROBLEME_SPX * , int , int * , char * ); 

void SPX_ChainageDeLaTransposee( PROBLEME_SPX * , int );

void SPX_ModifierLeChainageDeLaTransposee( PROBLEME_SPX * );
                   
void SPX_TranslaterLesBornes( PROBLEME_SPX * ); 

void SPX_CalculerLeScaling( PROBLEME_SPX * );      
  
void SPX_ArrondiEnPuissanceDe2( double * );

void SPX_Scaling( PROBLEME_SPX * );

void SPX_AjusterTolerancesVariablesEntieres( PROBLEME_SPX * );

void SPX_UnScaling( PROBLEME_SPX * );    
    
void SPX_VerifierAdmissibiliteDuale( PROBLEME_SPX * , int * );

void SPX_ReinitialiserLesCoutsNatifSiCestPossible( PROBLEME_SPX * );

void SPX_BruitageInitialDesCouts( PROBLEME_SPX * );

void SPX_FixerXEnFonctionDeSaPosition( PROBLEME_SPX * );

void SPX_CalculDuCout( PROBLEME_SPX * );

void SPX_CalculDuCoutSimplifie( PROBLEME_SPX * );

void SPX_ResoudreUBEgalC( PROBLEME_SPX * , char , double * , int * , int * , char * , char );

void SPX_ResolutionDeSystemeTransposee( PROBLEME_SPX * , char , double * , int * , int * , char * , char , char );

void SPX_AppliquerLesEtaVecteursTransposee( PROBLEME_SPX * , double * , int * , int * , char , char );

void SPX_CalculerErBMoins1( PROBLEME_SPX * , char );

void SPX_CalculerErBMoins1AvecBaseComplete( PROBLEME_SPX * , char );

void SPX_CalculerErBMoins1AvecBaseReduite( PROBLEME_SPX * , char );

void SPX_TenterRestaurationCalculErBMoinsEnHyperCreux( PROBLEME_SPX * );

void SPX_TenterRestaurationCalculABarreSEnHyperCreux( PROBLEME_SPX * );

void SPX_ResoudreBYegalA( PROBLEME_SPX * , char , double * , int * , int * , char * , char , char , char );

void SPX_ResolutionDeSysteme( PROBLEME_SPX * , char , double * , int * , int * , char * , char , char , char );

void SPX_AppliquerLesEtaVecteurs( PROBLEME_SPX * , double * , int * , int * , char , char );

void SPX_CalculerPi( PROBLEME_SPX * );

void SPX_CalculerPiAvecBaseReduite( PROBLEME_SPX * );

void SPX_CalculerPiAvecBaseComplete( PROBLEME_SPX * );
					 
void SPX_MettreAJourPi( void /* PROBLEME_SPX * */ );

void SPX_CalculerLesCoutsReduits( PROBLEME_SPX * );   

void SPX_MettreAJourLesCoutsReduits( PROBLEME_SPX * );

void SPX_CalculerBBarre( PROBLEME_SPX * );

void SPX_CalculerBBarreAvecBaseReduite( PROBLEME_SPX * );

void SPX_CalculerBBarreAvecBaseComplete( PROBLEME_SPX * );

void SPX_CalculerBBarreAHorsReduite( PROBLEME_SPX * );
		 
void SPX_MettreAJourBBarre( PROBLEME_SPX * );

void SPX_MettreAJourBBarreAvecBaseReduite( PROBLEME_SPX * , double * , int * , int * , char * );

void SPX_MettreAJourBBarreAvecBaseComplete( PROBLEME_SPX * , double * , int * , int * , char * );

void SPX_FactoriserLaBase( PROBLEME_SPX * );

void SPX_CalculerABarreS( PROBLEME_SPX * );

void SPX_CalculerABarreSAvecBaseReduite( PROBLEME_SPX * , char * , char * , char * , char * );

void SPX_CalculerABarreSAvecBaseComplete( PROBLEME_SPX * , char * , char * , char * , char * );

void SPX_VerifierABarreS( PROBLEME_SPX * );
void SPX_VerifierABarreSAvecBaseReduite( PROBLEME_SPX * );
void SPX_VerifierABarreSAvecBaseComplete( PROBLEME_SPX * );

void SPX_FaireLeChangementDeBase( PROBLEME_SPX * );
 
void SPX_MettreAJourLaBase( PROBLEME_SPX * , int * );

void SPX_InitialiserLesIndicateursHyperCreux( PROBLEME_SPX * );

/*------------------------ Specifique algorithme dual -----------------------------*/

void SPX_DualSimplexe( PROBLEME_SPX * );

void SPX_DualControlerOptimalite( PROBLEME_SPX * , int * , char * );

void SPX_DualControleDualNonBorne( PROBLEME_SPX * , char * );

void SPX_DualConstruireUneCrashBase( PROBLEME_SPX * );

void SPX_DualCreerVariableDeBase( PROBLEME_SPX * , int );

void SPX_DualConstruireLaBaseInitiale( PROBLEME_SPX * , int , int * , int , int * , char * );

void SPX_DualReconstruireUneBase( PROBLEME_SPX * , char * );

/* */
            void SPX_DualPhase1Simplexe( PROBLEME_SPX * );  
            void SPX_DualPhase1PositionnerLesVariablesHorsBase( PROBLEME_SPX * );
            void SPX_DualPhase1UtiliserLesBornesAuxiliaires( PROBLEME_SPX * );							
            void SPX_DualPhase1CalculerV( PROBLEME_SPX * );
            void SPX_DualPhase1ChoixDeLaVariableSortante( PROBLEME_SPX * );
            void SPX_DualPhase1ChoixDeLaVariableSortanteAuHasard( PROBLEME_SPX * );
            void SPX_DualPhase1TestDuRatio( PROBLEME_SPX * );
            void SPX_DualPhase1ChoixDeLaVariableEntrante( PROBLEME_SPX * );
            double SPX_CalculerUneBorneAuxiliaire( PROBLEME_SPX * , int );							
/* */

void SPX_DualChoixDeLaVariableSortante( PROBLEME_SPX * );

void SPX_DualChoixDeLaVariableSortanteAuHasard( PROBLEME_SPX * );

void SPX_DualCalculerNBarreR( PROBLEME_SPX * , char , char * );

void SPX_DualCalculerNBarreRStandard( PROBLEME_SPX * );

void SPX_DualCalculerNBarreRHyperCreux( PROBLEME_SPX * );

void SPX_DualConfirmerDualNonBorne( PROBLEME_SPX * );
 
void SPX_DualVerifierErBMoinsUn( PROBLEME_SPX * );  

void SPX_DualComparerABarreSEtNBarreR( PROBLEME_SPX * ); 

void SPX_DualChoixDeLaVariableEntrante( PROBLEME_SPX * );  

void SPX_DualTestDuRatio( PROBLEME_SPX * , int * );

void SPX_InitDualPoids( PROBLEME_SPX * );

void SPX_ResetReferenceSpace( PROBLEME_SPX * ); 

void SPX_MajPoidsDualSteepestEdge( PROBLEME_SPX * );

void SPX_DualSteepestEdgeResolutionBaseReduite( PROBLEME_SPX * , double * , char , char * , int * , char * , char * );

void SPX_DualSteepestEdgeResolutionAvecBaseComplete( PROBLEME_SPX * , double * , char , char * , int * , char * , char * );

void SPX_DualSteepestGestionIndicateursHyperCreux( PROBLEME_SPX * , char , char , char , char * , char * );

void SPX_DualSteepestControleDuPoidsDeCntBase( PROBLEME_SPX * , double , char * );

void SPX_DualSteepestVariablesDeBornesIdentiques( PROBLEME_SPX * , double * );
																									
void SPX_TenterRestaurationCalculTauEnHyperCreux( PROBLEME_SPX * );

void SPX_DualSupprimerLesBornesAuxiliaires( PROBLEME_SPX * );

void SPX_DualControlerLesBornesAuxiliaires( PROBLEME_SPX * , char * );

void SPX_DualRepositionnerLesBornes( PROBLEME_SPX * , char * );

void SPX_SupprimerUneBorneAuxiliaire( PROBLEME_SPX * , int );

/*----------------- Specifique utilitaires branch and bound --------------------*/

void SPX_DualEpurerLaBaseSimplexe( PROBLEME_SPX * , int * , char * , char * );
									       
void SPX_SauvegardesBranchAndBoundAndCut( PROBLEME_SPX * );

void SPX_DualStrongBranching( PROBLEME_SPX * , int , double , double , double , double * ,
                              int * , char * , double * , int * , int * , int * , int , 
                              int * , int , int , int * , char * );																													
		 
void SPX_DualStrongBranchingGUB( PROBLEME_SPX *  ,
                                 int   , int * , double * , 
                                 int * , char * , double * , int * , int * , int * ,
                                 int   , int * , int     ,
                                 int   , int * , char * );
				 
/*----------------- Specifique pour les coupes de Gomory --------------------*/

int SPX_PreparerLeCalculDesGomory( PROBLEME_SPX * , int , int * ); 

void SPX_TerminerLeCalculDesGomory( PROBLEME_SPX * );

void SPX_CalculerUneCoupeDeGomory( PROBLEME_SPX * , int ,double , double , double , double , double ,
                                   int * , double * , int * , double * , char * );

void SPX_CalculMIRPourCoupeDeGomoryOuIntersection( PROBLEME_SPX * , double , double , double , double ,
			                                             double , double , double * , char * , double * , char * ,			 
                                                   int * , double * , int * , double * , char * );																	 

/*----------------- Specifique pour 2 MIR cuts (a revoir car eput etre ameliore --------------------*/

void SPX_GetTableauRow( PROBLEME_SPX * , int , double , int * , double * , int * , double * , char * );

/*----------------- Specifique pour les coupes d'intersection --------------------*/

void SPX_InitCoupesDIntersection( PROBLEME_SPX * , char * , double * );

void SPX_TerminerLeCalculDesCoupesDIntersection( PROBLEME_SPX * );
 
void SPX_AllocLignePourCoupesDIntersection( PROBLEME_SPX * );

void SPX_MatriceCoupesDIntersection( PROBLEME_SPX * );

double SPX_CalculDeLaNouvelleNorme( LIGNE_DE_PRODUITS_SCALAIRES * , LIGNE_DE_PRODUITS_SCALAIRES * , int , int , double );

void SPX_MajMatricePourCoupesDIntersection( PROBLEME_SPX * , int , int , double , double ); 
																					 
void SPX_ReductionDesNormesPourCoupesDIntersection( PROBLEME_SPX * );

int SPX_NombrePotentielDeCoupesDIntersection( PROBLEME_SPX * );

void SPX_CalculerUneCoupeDIntersection( PROBLEME_SPX * , int , double , double , double , double , double ,			   																				
                                        int * , double * , int * , double * , char * );																																								

/*----------------- Specifique pour l'exploration rapide en profondeur --------------------*/
void Spx_ModifierLesDonneesSurInstanciation( PROBLEME_SPX * , int , int );
void Spx_RemettreLesDonneesAvantInstanciation( PROBLEME_SPX * , int , int );
void SPX_SauvegarderLaBaseDeDepart( PROBLEME_SPX * , void * ); 
void SPX_InitialiserLaBaseDeDepart( PROBLEME_SPX * , void * );
void Spx_CreationNoeudsFils( PROBLEME_SPX * , void * , void * , int );
void SPX_ChoisirLaVariableAInstancier( PROBLEME_SPX * , void * ,int * , int * );
void SPX_RecupererLaSolutionSiExplorationRapide( PROBLEME_SPX * , void * , void * , void * , int * );
					     
/*-----------------------------------------------------------------------------------------*/

/* Pour le simplexe generalise de Metrix */
       		   
PROBLEME_SPX * SPX_SimplexeGeneralise( PROBLEME_SIMPLEXE * , PROBLEME_SPX * , char , double , int * ,
                                       char * , int * , int * , double * , char , char );    

void SPX_ClasserDesVariables( double * , int , int , int * , int , int * , int * , char );

																			 
/*-----------------------------------------------------------------------------------------*/
/* En test: pivotages supplementaires pour obtenir une solution entiere par pivotages degeneres */

void SPX_DualPivotagesComplementaires( PROBLEME_SPX * );

/*-----------------------------------------------------------------------------------------*/
/* Heuristique */
void SPX_HeuristiqueArrondis( PROBLEME_SPX * , int * , int , double * , int * , int , int * , 
									            int * , int * , int , int , int * , double * , double * );
void SPX_RAZHeuristiqueArrondis( PROBLEME_SPX * );  

/*-----------------------------------------------------------------------------------------*/

void SPX_EcrireProblemeSpxAuFormatMPS( PROBLEME_SPX * );

void SPX_EcrireProblemeAuFormatMPS( PROBLEME_SIMPLEXE );

void SPX_EcrireJeuDeDonneesLineaireAuFormatMPS(int , int * , double * , double * , double * , int , double * , char * , int * , int * ,	double * , int * );
								 
/*-----------------------------------------------------------------------------------------*/

void SPX_OrdonnerLesContraintesPourLaBase( PROBLEME_SPX * );

void SPX_ConstructionDeLaMatriceReduite( PROBLEME_SPX * );					    

void SPX_TestPassageBaseReduiteBaseComplete( PROBLEME_SPX * );

void SPX_ControleDesVariablesBasiquesHorsBaseReduite( PROBLEME_SPX * , int * );

void SPX_VerifierLesVecteursDeTravail( PROBLEME_SPX * );

/*-----------------------------------------------------------------------------------------*/

void SPX_InitMatriceHorsBase( PROBLEME_SPX * );

void SPX_MettreAJourLaMatriceHorsBase( PROBLEME_SPX * );

/*-----------------------------------------------------------------------------------------*/

# define FONCTIONS_SPX_DEJA_DEFINIES
# endif
# ifdef __cplusplus
  }
# endif


   







