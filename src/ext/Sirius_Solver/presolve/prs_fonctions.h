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

# include "prs_define.h"

void PRS_AllocationsStructure( PRESOLVE * );

void PRS_AllocTablesDeSubstitution( void * ); /* En realite c'est un type PROBLEME_PNE mais il n'est pas connu dans ce fichier */

void PRS_LiberationStructure( PRESOLVE * );

void PRS_AmeliorerLesBornes( PRESOLVE * , int * );

void PRS_AmeliorerBorneSurVariable( PRESOLVE * , int , int * );

void PRS_MettreAJourLesBornesDUneVariable( PRESOLVE * , int , char , double , int , char , double , int , char * );

void PRS_VariablesDeBornesIdentiques( PRESOLVE * , int , double , double , char * );

void PRS_VariablesDualesEtCoutsReduits( PRESOLVE * , int * );

void PRS_MajVariableDuale( PRESOLVE * , int , double , double , char , int * );

void PRS_AmeliorerOuCalculerCoutsReduits( PRESOLVE * , int , int * );

void PRS_AmeliorerBornesDesVariablesDuales( PRESOLVE * , int , char , double , char ,double , char , int *  );

void PRS_BornerLesVariablesDualesNonBornees( PRESOLVE * , int * );

void PRS_CalculeBorneSurVariableDualeEnFonctionDeLaVariable( PRESOLVE * , int , int , char , char * , double * , char * , double * );

void PRS_MettreAJourLesBornesDUneVariableDuale( PRESOLVE * , int , char , double , char , double , char * );

void PRS_VariablesDualesDeBornesIdentiques( PRESOLVE * , int , double , double , char * );

void PRS_CalculerLesBornesDeToutesLesContraintes( PRESOLVE * , int * );
  
void PRS_AmeliorerLesCoefficientsDesVariablesBinaires( PRESOLVE * );

void PRS_AmeliorerCoeffDesContraintesDeBornesVariables( PRESOLVE * , int * );

void PRS_CalculerLesBornesDuneContrainte( PRESOLVE * , int , char , int * , int * );

void PRS_CalculeBorneSurVariableEnFonctionDeLaContrainte( PRESOLVE * , int , int , char * , double * , char * , double * );

void PRS_ContraintesToujoursInactives( PRESOLVE * , int * );

void PRS_SubstituerVariableDansCntDInegalite( PRESOLVE * , int     , int   , char   , double , int , int  * ,
					      double * , char * , double , char * , double * , int * );

void PRS_TesterLaDominanceDesContraintes( PRESOLVE * , char * , char * );

void PRS_TesterLaRedondanceDUneContrainte( PRESOLVE * , char *   , double   , double , int , int,
                                           char *     , double * , double * ,double * );

void PRS_EnleverLesContraintesInactives( PRESOLVE * );

void PRS_FixerVariablesSurCritere( PRESOLVE * , int * );

void PRS_Presolve( void * );

void PRS_SingletonsSurLignes( PRESOLVE * , int * );

void PRS_SingletonsSurColonnes( PRESOLVE * , int * );

char PRS_ChoisirLaBorneLaPlusContraignantePourLesVariablesDuales( int , int , double , double , double , double , char ,
                                                                  char , char , char );									 

char PRS_AjusterLeTypeDeBorne( char , char , double , double , double , double , char , char , char , char , char , char );

void PRS_DesactiverContrainte( PRESOLVE * , int );

void PRS_SubstituerVariables( PRESOLVE * , int * );

char PRS_TestSubstituerUneVariable( PRESOLVE * , int ,int , double , double , int );

void PRS_SubstituerUneVariable( PRESOLVE * , int ,int , double , double );

void PRS_ClasserVariablesDansContraintes( PRESOLVE * );
																	 
void PRS_SupprimerLesContraintesAvecQueDesVariablesFixees( PRESOLVE * );

void PRS_BornerLesVariablesNonBornees( PRESOLVE * , int * );

void PRS_TenterUnCalculDeBorne( PRESOLVE * , int , int , int * );

void PRS_VariablesHorsContraintes( PRESOLVE * , int * );

void PRS_CombinaisonDeContraintes( PRESOLVE * , int * );

void PRS_ContraintesColineaires( PRESOLVE * , int * );

void PRS_ColonnesColineaires( PRESOLVE * , int * );

void PRS_ConstruireLeGrapheAuFormatDIMACS( PRESOLVE * );

void PRS_SubstituerLesVariablesNonBornees( PRESOLVE * );

void PRS_FixerUneVariableAUneValeur( PRESOLVE * , int , double );

/* Pour le test BTF */
void PRS_BTF( PRESOLVE * );
void PRS_DumalgeMendelson( PRESOLVE * );
void PRS_DumalgeMendelsonPourLaMatriceDesContraintes( PRESOLVE * );
void * PRS_DumalgeFactoriserMatrice( PRESOLVE * , void ** , int , int , int , int , int , int ,																																											
                                     void * , void * , int * , int * , char * , char , char );
																					

void PRS_DumalgeResoudrePrimal( PRESOLVE * ,
                                void * ,
																void * ,                                  
                                          int ,
																					int ,																					
 	                                        int ,
																				 	int ,					   
																					int ,
																					int ,																																											
                                          void * ,
																					void * ,
																					int * ,
																					int * ,
																					double * ,
																				  char * ,
																					char  
		                                 );																		
																					
void * PRS_DumalgeMendelsonResoudreSystemeTranspose( PRESOLVE * ,
                                          int , int , int , int , int , int ,	void * , void * , int * ,
																					int * , int * , int * , double * , char * , char , char );

void PRS_DumalgeResoudreDual( PRESOLVE * , void * , void * , int , int , int , int , int ,
															int , void * , void *, int * , int * , double * , char * );
																					
																					
/* Fin Test */

/* */






















