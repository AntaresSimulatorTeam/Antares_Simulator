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
# ifndef LU_FONCTIONS_DEJA_DEFINI                             
/*****************************************************************************/
		
# include "lu_define.h"
# include "lu_definition_arguments.h"

/*****************************************************************************/
/*----------------------------- Factorisation LU ------------------------------*/

MATRICE * LU_Factorisation( MATRICE_A_FACTORISER * );	           

void LU_FactorisationCalculs( MATRICE * Matrice , MATRICE_A_FACTORISER * ); 

void LU_Refactorisation( MATRICE * , double * , int , double * , double , char * , char * );

void LU_InitTableauxANull( MATRICE * );

void LU_AllouerLU( MATRICE * , double * , int * , int * , int * , int * );

void LU_ConstruireProbleme( MATRICE * Matrice , double * , int * , int * , int * , int , int , int );
														
void LU_AugmenterLaTailleDeLaMatriceActive( MATRICE * , int , int );

void LU_AugmenterLaTailleDeLaMatriceActiveParColonne( MATRICE * , int , int ); 
  
void LU_AugmenterLaTailleDuTriangleL( MATRICE * , int );       
 
void LU_AugmenterLaTailleDuTriangleU( MATRICE * , int );

void LU_AugmenterLaTailleDuTriangleUParColonne( MATRICE * , int );

void LU_SelectionDuPivot( MATRICE * , int * , int * );  

void LU_SelectionDuPivotDiagonal( MATRICE * , int * , int * );

double LU_TrouverPlusGrandTermeDeLigne( MATRICE * , int );
				    
void LU_EliminationDUneLigne( MATRICE * , int , int );

void LU_EliminationDUneLigneVersionTurbo( MATRICE * , int , int );

void LU_ScanLigne( MATRICE * , int , int , int , double , int , char * , char * );

void LU_ClasserToutesLesLignesEtToutesLesColonnes( MATRICE * );  

void LU_ClasserUneLigne( MATRICE * , int , int ); 

void LU_DeClasserUneLigne( MATRICE * , int , int ); 
							     
void LU_ClasserUneColonne( MATRICE * , int , int ); 

void LU_DeClasserUneColonne( MATRICE * , int , int );

void LU_SupprimerTermesInutilesDansColonne( MATRICE * , int , int );
							    
void LU_InitMinMarkowitzDeColonne( MATRICE * , int , int * , double * );

void LU_PlusGrandTermeDeLaLigne( MATRICE * , int , int , int * );	    
          
void LU_InitMinMarkowitzDeLigne( MATRICE * , int , int * , double * );

/* Gestion des super lignes */
void LU_InitPourSuperLignes( MATRICE * );    

void LU_DeclenchementEventuelRechercheSuperLignes( MATRICE * );	  

void LU_CreerLesHashCode( MATRICE * );  

void LU_ClasserLesElementsEnFonctionDuHashCode( MATRICE * );

void LU_MajClassementDesElementsEnFonctionDuHashCode( MATRICE * );

void LU_ClasserEnFonctionDuHashCode( MATRICE * , int );

void LU_DeClasserEnFonctionDuHashCode( MATRICE * , int );

void LU_RechercherUnGroupeDeLignes( MATRICE * , int * , int * , int , int , int ,
                                           int * T  , int * , int * ); 				

void LU_DetectionSuperLignes( MATRICE * , char * );

void LU_ScanSuperLigne( MATRICE * , SUPER_LIGNE_DE_LA_MATRICE * , int , int , double * , unsigned int * );											
								 
void LU_ScanSuperLigneCasSymetrique( MATRICE * , SUPER_LIGNE_DE_LA_MATRICE * , int , int , double * , unsigned int * );
																					 			       
SUPER_LIGNE_DE_LA_MATRICE * LU_CreerUneSuperLigne( MATRICE * , int * , int , int * , int );

void LU_FusionnerDeuxSuperLignes( MATRICE * , int * , int , int );

void LU_AjouterUneLigneDansUneSuperLigne( MATRICE * , SUPER_LIGNE_DE_LA_MATRICE * , int * , int );

void LU_SupprimerUneLigneDansUneSuperLigne( MATRICE * , SUPER_LIGNE_DE_LA_MATRICE * , int , int );

void LU_SupprimerUnTermeDansUneSuperLigne( MATRICE * , SUPER_LIGNE_DE_LA_MATRICE * , int , int );

void LU_RecopierUneLigneDeSuperLigneDansLigne( MATRICE * , SUPER_LIGNE_DE_LA_MATRICE * , int );

void LU_AugmenterCapaciteDesColonnesDeSuperLigne( MATRICE * , SUPER_LIGNE_DE_LA_MATRICE * , int );

void LU_DiminuerCapaciteDesColonnesDeSuperLigne( MATRICE * , SUPER_LIGNE_DE_LA_MATRICE * );

void LU_AugmenterCapaciteDeSuperLigne( MATRICE * , SUPER_LIGNE_DE_LA_MATRICE * , int );

void LU_MettreTouteLaMatriceDansUneSuperLigne( MATRICE * Matrice );

/*    */
					   
/* Pour le switch vers le pivotage de Markowitz en cas de pivot nul sur une factorisation
   par pivotage diagonal */ 
void LU_SwitchVersMarkowitz( MATRICE * );
void LU_ClasserLesColonnesRestantes( MATRICE * );
/*    */
 
/* Mode matrice pleine */
void LU_ScanLigneMatricePleine ( MATRICE * , int , int , double ); 
void LU_EliminationDUneLigneMatricePleine( MATRICE * , int , int );
void LU_SelectionDuPivotMatricePleine( MATRICE * , int * , int * );
void LU_RechercherLePlusGrandTermeDeLaMatriceActive( MATRICE * );				     
/*    */

/* Mode matrice symetrique */ 
void LU_EliminationDUneLigneCasSymetrique( MATRICE * , int , int );
void LU_ScanLigneCasSymetrique( MATRICE * , int , int , int , double , int , char * ); 
/*    */

void LU_LibererLesTableauxTemporaires( MATRICE * );
								        
void LU_LibererMemoireLU( MATRICE * ); 
							    
void LU_LuSolv( MATRICE * , double * , int * , MATRICE_A_FACTORISER * , int , double );	      
void LU_LuSolvTriangleL( MATRICE * , double * );
void LU_LuSolvTriangleU( MATRICE * , double * );
		
void LU_LuSolvTransposee( MATRICE * , double * , int * , MATRICE_A_FACTORISER * , int , double );
void LU_LuSolvTriangleUTransposee( MATRICE * , double * );
void LU_LuSolvTriangleLTransposee( MATRICE * , double * );

/*  Resolution cas hyper creux */
void LU_AllocEtInitHyperCreux( MATRICE * , char * );
char LU_RecusionSurTriangle_1( MATRICE * , int * , int , int * , int * , int * , int * , int * );
char LU_RecusionSurTriangle_2( MATRICE * , int * , int , int * , int * , int * , int * );

char LU_LuSolvTriangleLSecondMembreHyperCreux( MATRICE * , int * , int * , double * );
void LU_LuSolveHSecondMembreHyperCreux( MATRICE * , int * , int * );
char LU_LuSolvTriangleUSecondMembreHyperCreux( MATRICE * , double * , int * , int * , char );

char LU_LuSolvTriangleUTransposeeSecondMembreHyperCreux( MATRICE * , int * , int * , double * );
void LU_LuSolveHTransposeeSecondMembreHyperCreux( MATRICE * , int * , int * );
char LU_LuSolvTriangleLTransposeeSecondMembreHyperCreux( MATRICE * , double * , int * , int * );

void LU_LuSolvSecondMembreHyperCreux( MATRICE * , double * , int * , int * , char , char , char * );
void LU_LuSolvTransposeeSecondMembreHyperCreux( MATRICE * , double * , int * , int * , char , char * );
void LU_MettreSolutionSousFormeHyperCreux( MATRICE * , double * , int * , int * , double * , char , char );

/* Fin cas hyper creux */

void LU_LuSolvLuUpdate( MATRICE * , double * , int * , char , char , MATRICE_A_FACTORISER * , 
		                    int , double );
void LU_LuSolvTransposeeLuUpdate( MATRICE * , double * , int * , char , MATRICE_A_FACTORISER * ,
		                              int , double );

void LU_UpdateLuSpikePret( MATRICE * , int , int * );
void LU_UpdateLuEliminations( MATRICE * , int , int , int  , int * , int * , char * , double * );

void LU_LuSolveH( MATRICE * );
void LU_LuSolveHTransposee( MATRICE * );
															
void LU_ChainageParLigneDeULUGeneral( MATRICE * );
void LU_ChainageParLigneDeU( MATRICE * );
void LU_ChainageParColonneDeU( MATRICE * );
void LU_ChainageParLigneDeL( MATRICE * , char * );
void LU_ChainageParColonneDeUHorsSimplexe( MATRICE * );

/* Variables indeterminees en cas de singularite */

int * LU_InconnuesIndeterminees( MATRICE * , int * , int * );

int * LU_Indeterminees( MATRICE * , int * , char , char , char );
					        			        
/* Pour le scaling s'il est demande */
void LU_CalculerLeScaling( MATRICE * );
void LU_ArrondiEnPuissanceDe2( double * );
void LU_Scaling( MATRICE * );
void LU_ScalingSecondMembre( MATRICE * , double * );
void LU_ScalingSecondMembreTranspose( MATRICE * , double * );
void LU_UnScaling( MATRICE *  , double * );     
void LU_UnScalingTranspose( MATRICE * , double * );

/* Pour les refactorisation pour le point interieur */
void   LU_StockageDeUDansLOrdreCroissantDesIndices( MATRICE * );
void   LU_RefactorisationSimulation( MATRICE * , char * , int );
double LU_RefactorisationControleDuPivot( MATRICE * , double , double * , int , int , char * );
void   LU_RefactorisationScanLigne( int , int , double , char * , double * , double * , int * );
void LU_RefactorisationScanLignePlein( int , int , double , double * , double * , int * );

/* Pour les refactorisations non symetriques */
void LU_EliminationDUneLigneRefactorisationNonSymetrique( MATRICE * , int * );
void LU_InitTrianglesRefactorisationNonSymetrique( MATRICE * ,double  * , int * , int * , int * , int * );
void LU_RefactorisationNonSymetrique( MATRICE * , MATRICE_A_FACTORISER * ); 	    					  
void LU_RefactorisationLL( MATRICE * , double  * , int * , int * , int * , int * );
												 
/* Pour les refactorisations symetriques */
void LU_RefactorisationSymetrique( MATRICE * , MATRICE_A_FACTORISER * );

/*******************************************************************************************/
# define LU_FONCTIONS_DEJA_DEFINI 	
# endif
# ifdef __cplusplus
  }
# endif    
