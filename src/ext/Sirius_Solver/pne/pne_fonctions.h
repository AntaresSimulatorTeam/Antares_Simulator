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
# ifndef FONCTIONS_PNE_DEJA_DEFINIES
/*******************************************************************************************/

# include "pne_definition_arguments.h" /* C'est ce header qui continet le protoype de PNE_Solveur */
# include "pne_define.h" /* Comme il contient aussi pne_constantes_externes.h il n'y a pas besoin
                            de l'inclure pne_constantes_externes.h dans l'appelant. cependant on
														le recommande comme ca l'utilisateur sera tente d'aller voir ce qu'il
														y a comme constantes externes ce qui n'est pas plus mal */
														
/*--------------------------------------------------------------------------------------------------*/

void PNE_SolveurProblemeReduit( PROBLEME_A_RESOUDRE * , CONTROLS * );

void PNE_ControleMacAdresse( void );

void PNE_SolveurCalculs( PROBLEME_A_RESOUDRE * , PROBLEME_PNE * );			       

void PNE_InitialiserLaPne( PROBLEME_PNE * , PROBLEME_A_RESOUDRE * );

void PNE_EnleverLesToutPetitsTermes( int * , int * , int * , double * , double * , double * , int , char );							 

void PNE_AjouterLaContrainteDeCoutMax( PROBLEME_PNE * );

void PNE_ClasserLesCoefficientsDesContraintesOrdreDecroissant( PROBLEME_PNE * );

void PNE_TranslaterLesBornes( PROBLEME_PNE * );			      

void PNE_DetecterLesGub( PROBLEME_PNE * );

void PNE_CompacterLaMatriceDesContraintes( PROBLEME_PNE * );

/* */
  
void PNE_SolvePbRlxPi( PROBLEME_PNE * , double * , int * );     

void PNE_SolvePbRlxSpxPrimal( PROBLEME_PNE * , double , int , int , int , int , int * , int * , 
                              int * , int * , double * , int * );

void PNE_SolvePbRlxSpxDual( PROBLEME_PNE * , char , double , int , int , int * , int * , int * , int * );

void PNE_CloneProblemeSpxDuNoeudRacine( PROBLEME_PNE * Pne , int * , int * , int * );

MATRICE_DE_CONTRAINTES * PNE_ConstruireMatriceDeContraintesDuSimplexeAuNoeudRacine( PROBLEME_PNE * , int * , int , int * );   

void PNE_LibererMatriceDeContraintesDuSimplexeAuNoeudRacine( MATRICE_DE_CONTRAINTES * );

void PNE_CalculerLaValeurDuCritere( PROBLEME_PNE * );

void PNE_DeterminerLesVariablesFractionnaires( PROBLEME_PNE * , int * );

void PNE_StrongBranching( PROBLEME_PNE * , double * , double * , int * , int   * , int * , 
                          int * , int * , int * , int * , char * , char );

void PNE_StrongBranchingClasserLeResultat( PROBLEME_PNE * , char    , char    , char    , double  , double, 
                                           double, double, double  , double  , double *, double *,
                                           int    , char *  , char *  , double  ,
                                           double *, double *,	  
                                           int *  , int *  , int *, int *, int  , int *, char *  ,	  
                                           int *  , int *  , int *, int *, int  , int *, char *  ,
                                           int * );
						  
char PNE_Gub( PROBLEME_PNE * , int * , int * , char *   , int * , int *   , char *   ,
                     double , double , double * , double , double * , double * ,
                     int * , int * , int *   , int * , int *   , int *   , int * );

void PNE_ChoixDesVariablesAInstancier( PROBLEME_PNE * , int * , int * , int ** , int * , int * , int ** );
				 				 
double PNE_ValeurOptimaleDuProblemeCourantAvantNouvellesCoupes( PROBLEME_PNE * );				            

void PNE_ClasserLesVariableFractionnaires( PROBLEME_PNE * , int , double );

void PNE_RecupererLaSolutionEtCalculerLeCritere( PROBLEME_PNE * , PROBLEME_A_RESOUDRE * );

void PNE_ConstruireLeChainageDeLaTransposee( PROBLEME_PNE * );

void PNE_CalculerLesCoupes( PROBLEME_PNE * );

void PNE_EnrichirLeProblemeCourantAvecUneCoupe( PROBLEME_PNE * , char , /*int ,*/ int , double , double , double * , int * ); 

void PNE_TrierLesCoupesCalculees( PROBLEME_PNE * , int , int , char , char * , char * , char * );

void PNE_ActualiserLesCoupesAPrendreEnCompte( PROBLEME_PNE * );

void PNE_CalculerLesCoupesDIntersection( PROBLEME_PNE * );

void PNE_Gomory( PROBLEME_PNE * );

void PNE_CalculerUneGomoryEnVariablesMixtes( PROBLEME_PNE * , int , double );

void PNE_NormaliserUnCoupe( double * , double * , int , double );
						  
void PNE_InsererUneContrainte( PROBLEME_PNE * , int , double * , int * , double , char , char );

void PNE_RecupererLeProblemeInitial( PROBLEME_PNE * );

void PNE_RechercheDesContraintesSaturees( PROBLEME_PNE * , int , int * );

void PNE_BranchAndBoundSolvePbRlx( PROBLEME_PNE * , char , int , char * , char , int * , int * , char * , double * , int * ,
                                   int * , int * , int * , int * , double * , int * , int * , int * , double * , double * ,	 
                                   double * , double * , int * , int * , int * , int * , int * , int * , int * );  																	 

char PNE_ReducedCostFixing( PROBLEME_PNE * , int * );

void PNE_ArchivagesPourReducedCostFixingAuNoeudRacine( PROBLEME_PNE * , int * , double * , double );

void PNE_ReducedCostFixingAuNoeudRacine( PROBLEME_PNE * );

void PNE_RelanceDuSimplexeAuNoeudRacine(  PROBLEME_PNE * , int * );

void PNE_FixationDesVariablesEntieresSurCritere( PROBLEME_PNE * );

void  PNE_CalculerLesRestrictionsDeBornes( PROBLEME_PNE * , int * , char * , char );
					
void PNE_ArchiverLaSolutionCourante( PROBLEME_PNE * ); 

void PNE_RestituerLaSolutionArchivee( PROBLEME_PNE * ); 

void PNE_AllocProbleme( PROBLEME_PNE * , int , int * , int * , double * , double * , int , int * , int * , int * , double * );

void PNE_AllocationsPourLePostSolve( PROBLEME_PNE * );      

void PNE_AllocTablesDeSubstitution( PROBLEME_PNE * );

void PNE_CleanPostSolve( PROBLEME_PNE * );     

void PNE_AugmenterLeNombreDeVariables( PROBLEME_PNE * );

void PNE_AugmenterLeNombreDeContraintes( PROBLEME_PNE * );

void PNE_AugmenterLaTailleDeLaMatriceDesContraintes( PROBLEME_PNE * );

void PNE_AllocCoupes( PROBLEME_PNE * );

void PNE_AugmenterLeNombreDeCoupes( PROBLEME_PNE * );

void PNE_AugmenterLaTailleDeLaMatriceDesCoupes( PROBLEME_PNE * );
         
void PNE_LibereProbleme( PROBLEME_PNE * );

void PNE_LireJeuDeDonneesMPS( void );

void PNE_PrendreEnCompteLesContraintesRangeMPS( void );

void PNE_EcrireJeuDeDonneesMPS( PROBLEME_PNE * , PROBLEME_A_RESOUDRE * );

void PNE_EcrirePresolvedMPS( PROBLEME_PNE * );       

void PNE_CreerHashCodeContrainteMPS( void /*int **/ );

void PNE_CreerHashCodeVariableMPS( void /*int **/ );

int PNE_HashCode( char * , int );

void PNE_RechercheDuNumeroDeContrainteMPS( /*int , int ,*/ int * , char * );

void PNE_RechercheDuNumeroDeVariableMPS( /*int , int ,*/ int * , char * );

void PNE_ClasserVariablesDansContraintesMPS( void );

/* */

void PNE_ClasserVariablesDansContraintes(  PROBLEME_PNE *  );

double PNE_SRand( double );
               
double PNE_Rand( double );	   

/* Heuristiques */

void PNE_HeuristiquePilotage( PROBLEME_PNE * , char * );

void PNE_Heuristique( PROBLEME_PNE * , double , int , int * , int , int * );

void PNE_HeuristiqueArrondis( PROBLEME_PNE * , char * );

MATRICE_DE_CONTRAINTES * PNE_HeuristiqueConstruireMatriceDeContraintes( PROBLEME_PNE * );

void PNE_HeuristiqueLibererMatriceDeContraintes( MATRICE_DE_CONTRAINTES * );

char PNE_HeuristiqueEvaluerTailleDuProbleme( PROBLEME_PNE * );

void PNE_HeuristiqueArchivageSolutionEtMajBranchAndBound( PROBLEME_PNE * , double );

char PNE_HeuristiqueResolutionBranchAndBoundReduit( PROBLEME_PNE * , MATRICE_DE_CONTRAINTES * );

char PNE_HeuristiqueRINS( PROBLEME_PNE * );

char PNE_HeuristiqueVariablesEntieresFixees( PROBLEME_PNE * );

void PNE_HeuristiqueRechercheEnProfondeur( PROBLEME_PNE * , char * );


/*                     */

void PNE_AnalyseInitialeDesBornesVariables( PROBLEME_PNE * );

void PNE_DetecterLesContraintesMixtes( PROBLEME_PNE * );

void PNE_DetecterLesVariablesBigM( PROBLEME_PNE * );
 
void PNE_AnalyseInitialeDesKnapsack( PROBLEME_PNE * );

void PNE_CoverKnapsackSimple( PROBLEME_PNE * );

void PNE_KnapsackSurCombinaisonsComplexesDeContraintes( PROBLEME_PNE * );

void PNE_KnapsackSurCombinaisonsDeContraintes( PROBLEME_PNE * , int , int , int * , double * , double , char * , char * );
		
void PNE_GreedyCoverKnapsack( PROBLEME_PNE * , int , int , int * , double * , double , char , char * ,
                              char , double , int , int * , char * , double * );
															
void PNE_FreeTasGreedyCoverKnapsack( char * );

int PNE_MajorantKnapsack( int , int * , double * , double , char );
  
void PNE_KnapsackCoeffEntiers( int , double * , double * , double * );

int PNE_PartitionTriRapide( double * , int * , int , int , char );

void PNE_TriRapide( double * , int * , int , int , char );

void PNE_ReclasserCsurA( int , char * , int * , double * , int , int * , double * , double * , int * );

void PNE_RechercherUneCouvertureMinimale( int , double * , double * , int * , char * , double ,
                                          char * , double * , int * , double * , int * , double * , int * ,
																					double * , int * , char );
																					
void PNE_SequenceIndependantCoverKnapsackLifting( int , double * , double * , double , int * , double * , int * , double * ,
                                                  double * , char * , double * , int * , char * );																					

double PNE_ProgrammationDynamiqueKnapsackSeparation( int , double * , double * , double );

void PNE_KnapsackLifterLaVariableContinue( double * , int , double * , double , int , int * , double * ,
                                           double , double, double * );

void PNE_Knapsack_0_1_AvecVariableContinue( PROBLEME_PNE * , int , int * , double * , double , double , int ,
																						int * , char * , double * );																										 
																				 
double PNE_G_de_D( double , double );
double PNE_TesterUneMIR( int , int * , double * , double * , double , double , double , double ,
												 double * , char );
												 
void PNE_TesterUneMIRpourUneValeurDeDelta( PROBLEME_PNE * , double , int , double , int * , double * , double , double * , double , 
																					 double * , double * , double * , char * , double * , char );

char PNE_C_MIR( PROBLEME_PNE * , int , int * , double * , double * , double * , double );

void PNE_SyntheseEtStockageMIR( PROBLEME_PNE * , int , int * , double * , double , double ,
																int , int * , char * , double * );

char PNE_BoundSubstitution( PROBLEME_PNE * ,int , int , int * , double * , double , int * , int * ,
                            double * , double * , double * , double * , int * , int * , char * , double * , char * );

char PNE_Agregation( PROBLEME_PNE * , char * , int * , int * , int * , double * , double * , char *  , char * , int * , int * , int * , int *  );

void PNE_MIRMarchandWolsey( PROBLEME_PNE * );

void PNE_MIRMarchandWolseySurContrainteConcateeGrapheDeConflits( PROBLEME_PNE * , double * , int * , char * , char * , double * , int * ,
																																 char * , double * , int * , char * , char * );

void PNE_SubstitutionVarContinuesPourK( PROBLEME_PNE * );

void PNE_CalculerUneKnapsackSurGomoryOuIntersection( PROBLEME_PNE * , double * , int * , double , int , double );

/*********************** Node presolve et probing ********************************/

void PNE_NodePresolve( PROBLEME_PNE * , int * );

void PNE_PresolveSimplifie( PROBLEME_PNE * , char * , char , int * );
void PNE_PresolveSimplifieVariableProbing( PROBLEME_PNE * , int * , char * );
void PNE_PresolveSimplifieContraintesDeBornesVariables( PROBLEME_PNE * , int * , char * );

PROBLEME_PNE * PNE_AllocPnePbReduit( PROBLEME_PNE * , int * );

void PNE_ProbingNodePresolveAlloc( PROBLEME_PNE * , char * );

void PNE_InitBorneInfBorneSupDesVariables( PROBLEME_PNE * );

void PNE_ProbingModifierLaMatriceDesContraintes( PROBLEME_PNE * , PROBING_OU_NODE_PRESOLVE * );

void PNE_AmeliorerLesCoefficientsDesVariablesBinaires( PROBLEME_PNE * , void * , char );

void PNE_CalculMinEtMaxDesContraintes( PROBLEME_PNE * , int * );

char PNE_DeterminerForcingConstraint( PROBLEME_PNE * , PROBING_OU_NODE_PRESOLVE * , int , char , char , char , double , double , double );										

void PNE_NodePresolveInitBornesDesContraintes( PROBLEME_PNE * , int * );

void PNE_NodePresolveMajBminBmax( PROBLEME_PNE * , int * , int * , int * , double * , double * , double * ,
                                  double * , double * , int , double , char );

void PNE_NodePresolveGrapheDeConflit( PROBLEME_PNE * , int * , int * , int * , double * , double * , double * ,
																			double * , double * , double * , double * , int , double , int *  );

void PNE_NodePresolveAppliquerContraintesDeBornesVariables( PROBLEME_PNE * , int * );								

void PNE_CalculMinContrainte( PROBLEME_PNE * , double , double , int , double * );																	

void PNE_CalculMaxContrainte( PROBLEME_PNE * , double , double , int , double * );

void PNE_MajIndicateursDeBornes( PROBLEME_PNE * , double * , double * , char * , char * , double , int , char , char );

void PNE_ModifierLaBorneDUneVariable( PROBLEME_PNE * , int , char , char , double , char , double ,
                                      double * , char * , char * , int * );																			
																			
void PNE_CalculXiXs( PROBLEME_PNE * , double , int , int , char * , char * , double *  , double * );

void PNE_CalculXiXsContrainteAUneSeuleVariable( PROBLEME_PNE * , int * , int , int , int , double * , int * ,
																								char * , double * , char * , char * , double * , double * );

void PNE_ProbingNodePresolveFixerVariablesSurCritere( PROBLEME_PNE * , char * );

void PNE_VariableProbing( PROBLEME_PNE * );
void PNE_VariableProbingSauvegardesDonnees( PROBLEME_PNE * );
void PNE_VariableProbingReinitDonnees( PROBLEME_PNE * );
void PNE_VariableProbingPreparerInstanciation( PROBLEME_PNE * , int , double );
void PNE_VariableProbingFixerUneVariableInstanciee( PROBLEME_PNE * , PROBING_OU_NODE_PRESOLVE * , int , double );
void PNE_VariableProbingRazContraintesAAnalyser( PROBLEME_PNE * , PROBING_OU_NODE_PRESOLVE * );
void PNE_VariableProbingRazCoupesDeProbingPotentielles( PROBING_OU_NODE_PRESOLVE * );

void PNE_PostProbing( PROBLEME_PNE * );

void PNE_ProbingInitVariablesAInstancierApresLePresolve( PROBLEME_PNE * , PROBING_OU_NODE_PRESOLVE * );

void PNE_ProbingInitVariablesAInstancier( PROBLEME_PNE * , PROBING_OU_NODE_PRESOLVE * );

void PNE_ProbingMajVariablesAInstancier( PROBLEME_PNE * , PROBING_OU_NODE_PRESOLVE * );

void PNE_ProbingMajFlagVariablesAInstancier( PROBLEME_PNE * , int , double );

void PNE_ProbingMajBminBmax( PROBLEME_PNE * , int , double , char );

void PNE_VariableProbingAppliquerLeConflictGraph( PROBLEME_PNE * , int , double , char , char );
																																			
void PNE_AnalyseListeDeContraintes( PROBLEME_PNE * );

void PNE_InitListeDesContraintesAExaminer( PROBLEME_PNE * , int , double , char );

void PNE_MajConflictGraph( PROBLEME_PNE * , int , double );

void PNE_AllocConflictGraph( PROBLEME_PNE * );

void PNE_ReallocConflictGraph( PROBLEME_PNE * );

void PNE_CliquesConflictGraph( PROBLEME_PNE * );

void PNE_TransformerCliquesEnEgalites( PROBLEME_PNE * );

void PNE_ArchiverMaxClique( PROBLEME_PNE * , int , int * );

void PNE_ExtendConflictGraph( PROBLEME_PNE * );

void PNE_ConflictGraphFixerVariables( PROBLEME_PNE * );

void PNE_ConflictGraphSupprimerUnArc( int , int , int * , int * , int * );

void PNE_ConflictGraphSupprimerUnNoeud( int , int * , int * , int * );

void PNE_DetectionDesCliquesViolees( PROBLEME_PNE * );

void PNE_DetectionDesCoupesDeProbingViolees( PROBLEME_PNE * );

void PNE_DetectionKnapsackNegligeesViolees( PROBLEME_PNE * );

void PNE_DetectionGomoryNegligeesViolees( PROBLEME_PNE * );

void PNE_ControleCliquesAvantResolutionProblemeRelaxe( PROBLEME_PNE * , int * );

void PNE_ReducedCostFixingNoeudRacineConflictGraph( PROBLEME_PNE * , int , double );

void PNE_ReducedCostFixingConflictGraph( PROBLEME_PNE * , int , double , double * , double , double * , double * , double * ,
                                         int * , int * , int * , int * , int * );

void PNE_ConflictGraphFixerLesNoeudsVoisinsDunNoeud( PROBLEME_PNE * , int );

void PNE_CreerLesCoupesDeProbing( PROBLEME_PNE * , PROBING_OU_NODE_PRESOLVE * );

void PNE_ProbingInitDetectionDesBornesVariables( PROBLEME_PNE * );

void PNE_ProbingCloseDetectionDesBornesVariables( PROBLEME_PNE * );

void PNE_ProbingConstruireContraintesDeBornes( PROBLEME_PNE * , PROBING_OU_NODE_PRESOLVE * , double * , double * );

void PNE_DetectionDesContraintesDeBorneVariableViolees( PROBLEME_PNE * );

void PNE_CreerUneCoupeDeProbing( PROBLEME_PNE * , int , double , int , char , int , double );

void PNE_CreerUneCoupeKNegligee( PROBLEME_PNE * , int );

void PNE_CreerUneCoupeGNegligee( PROBLEME_PNE * , int );

void PNE_TwoStepMirSurContrainte( PROBLEME_PNE * );

void PNE_TwoStepMirSurTableau( PROBLEME_PNE * , int , double );

void PNE_RechercheSymetries( PROBLEME_PNE * , int , int * );

void PNE_PostSolve( PROBLEME_PNE * , PROBLEME_A_RESOUDRE * );

void PNE_PostSolveVariablesSubstituees( PROBLEME_PNE * , PROBLEME_A_RESOUDRE * , int , char );

void PNE_PostSolveVariablesColineaires( PROBLEME_PNE * , int );

void PNE_PostSolveContraintesSingleton( PROBLEME_PNE * , int , double * );

void PNE_PostSolveForcingConstraints( PROBLEME_PNE * , PROBLEME_A_RESOUDRE * , void * , int );

void PNE_PostSolveContraintesColineaires( PROBLEME_PNE * , PROBLEME_A_RESOUDRE * , int );

void PNE_ChangerLesTypesDeVariables( PROBLEME_PNE * Pne );

void PNE_BranchAndBoundIntermediare( PROBLEME_PNE * , double );

void PNE_PostSolveSiUniquementPresolve( PROBLEME_PNE * , PROBLEME_A_RESOUDRE * );

/*
void PNE_NodeDumalgeMendelsonMatriceDesContraintes( PROBLEME_PNE * , double * , double * ,
						  																			char * , char * , char * , int * );
																									 
void * PNE_DumalgeFactoriserMatrice( PROBLEME_PNE * , void ** , int , int , int , int , int , int ,
                                     void * , void * , int * , int * , char * , char , char );

void PNE_DumalgeResoudrePrimal( PROBLEME_PNE * , void * , void * , int , int , int , int , int , int ,
                                void * , void * , int * , int * , double * , char * , char ,
		                            double * , double * , char * , char * , char * );
*/

double PNE_Round( double , double );

void PNE_MiseAJourSeuilCoupes( PROBLEME_PNE * , char , double * );
void PNE_MiseAJourDesSeuilDeSelectionDesCoupes( PROBLEME_PNE * );

/* En test: pour detecter les contraintes d'egalite sur des entiere infaisables */
void PNE_DetectionContraintesEntieresInfaisable( PROBLEME_PNE * );

/*******************************************************************************************/
# define FONCTIONS_PNE_DEJA_DEFINIES	
# endif
# ifdef __cplusplus
  }
# endif		














