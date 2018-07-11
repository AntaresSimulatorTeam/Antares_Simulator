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
# ifndef FONCTIONS_BB_DEJA_DEFINIES
/*-----------------------------------------------------------------------------------------*/
		
# include "bb_define.h"

int    BB_BranchAndBound( void * , int , int , double , int , int , int , char , int * );

int    BB_BranchAndBoundCalculs( BB * , int , int , double ,int , int , int , char , int * );

NOEUD * BB_AllouerUnNoeud( BB * , NOEUD * , int , int , int , int , int * , double );

void    BB_DesallouerUnNoeud( BB * , NOEUD * );

void    BB_DesallocationPartielleDUnNoeud( NOEUD * );       

void    BB_BranchAndBoundAllouerProbleme( BB * );

void    BB_BranchAndBoundDesallouerProbleme( BB * );

int    BB_ResoudreLeProblemeRelaxe( BB * , NOEUD * , int * );
  
char    BB_ArchiverToutesLesCoupesLorsDuTri( BB * );

void    BB_AfficherLesTraces( BB * , NOEUD * );

void    BB_LeverLeFlagPourEnleverToutesLesCoupes( BB * );

void    BB_LeverLeFlagDeSortieDuSimplexeParDepassementDuCoutMax( BB * );

int    BB_ChoixDesVariablesAInstancier( BB * , NOEUD * , int * , int * , int ** , int * , int * , int ** );

void    BB_EliminerLesNoeudsSousOptimaux( BB * );

void    BB_SupprimerTousLesDescendantsDUnNoeud( BB * , NOEUD * );

int    BB_BalayageEnProfondeur( BB * , NOEUD * , int );

NOEUD * BB_NoeudPereSuivantDansRechercheEnProfondeur( NOEUD ** );                       

NOEUD * BB_RemonterDansRechercheEnProfondeur( NOEUD * , int );                        

void    BB_CreerLesNoeudsFils( BB * , NOEUD * );
                       
void    BB_BalayageEnLargeur( BB * , NOEUD * , int );

void    BB_FaireUneRechercheEnProfondeurDansUneRechercheEnLargeur( BB * );

void    BB_NettoyerLArbre( BB * , int * , NOEUD * );
	       
void    BB_NettoyerLArbreDeLaRechercheEnProfondeur( BB * , NOEUD * , NOEUD * , int );	 /* Obsolete */

int    BB_ExaminerUnNoeudEnProfondeur( BB * , NOEUD * , int * /*, int , NOEUD ** , int , NOEUD ***/ );

void    BB_RechercherLeMeilleurMinorant( BB * , char );

NOEUD * BB_RechercherLeNoeudLeMoinsFractionnaire( BB * , char );
 
NOEUD * BB_RechercherLeNoeudLeAvecMeilleurRatioFractionnaire( BB * );

void    BB_EvaluerEnProfondeurLesNoeudsCritiques( BB * );
				      							     
/* Coupes */
 
void    BB_StockerUneCoupeGenereeAuNoeud( BB * , int , double * , int * , double , /*int ,*/ char );

void    BB_InsererLesCoupesDansLeProblemeCourant( BB * , NOEUD * );

void    BB_RechercherLesCoupesViolees( BB * , double * );

void    BB_NettoyerLesCoupes( BB * , char );

void    BB_DemanderUneNouvelleResolutionDuProblemeRelaxe( BB * );

/*        */  

void    BB_BestFirst( BB * );

void    BB_EvaluerLesDeuxFilsDuMeilleurMinorant( BB * , NOEUD * );

/*        */

void BB_ControlerLesCoupesNonInsereesInitialement( BB * , char * );

/*----------------- Specifique pour l'exploration rapide en profondeur --------------------*/
void BB_LibererLaBaseSimplexeDuNoeud( NOEUD * );
/*
void BB_RemettreLesDonneesAvantInstanciation( BB * , PROBLEME_SPX * , int * , NOEUD * );
void BB_InstancierLaVariableDansLeSimplexe( BB * , PROBLEME_SPX * , int * , NOEUD * );
NOEUD * BB_EvaluationRapideDUnNoeud( BB * , PROBLEME_SPX * , PROBLEME_PNE * , int * );
*/
NOEUD * BB_PreparerExplorationRapideEnProfondeur( BB * , NOEUD * );
void BB_ExplorationRapideEnProfondeur( BB * );

/*-----------------------------------------------------------------------------------------*/
# define FONCTIONS_BB_DEJA_DEFINIES
# endif
# ifdef __cplusplus
  }
# endif
 
