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

   FONCTION: Strong Branching pour les GUB.
             Remarque: que l'on instancie a gauche ou a droite, on
	     instancie toujours a 0
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "pne_define.h"

# include "lu_fonctions.h"

/*----------------------------------------------------------------------------*/

void SPX_DualStrongBranchingGUB( 
                   PROBLEME_SPX * Spx,
		   /* Les variables a brancher */
                   int     NombreDeVariablesABrancher ,
		   int   * NumerosDesVariablesABrancher,
		   double * XmaxSV, /* Tableau de sauvegarde dimension NombreDeVariablesABrancher */
	           /* Sortie */
                   int   * YaUneSolution             ,
                   char   * TypeDeSolution            ,
                   double * X                         ,  
                   int   * PositionDeLaVariable      ,                   
                   int   * NbVarDeBaseComplementaires,                   
                   int   * ComplementDeLaBase        ,
                   /* Donnees initiales du probleme (elles sont utilisees par SPX_RecupererLaSolution) */
                   int     NbVar_E, 
                   int   * TypeVar_E,  
                   int     NbContr_E,
                   /* Dans le but de recuperer les informations sur les coupes */
                   int     NombreDeContraintesCoupes,
		   int   * NbTermCoupes,		   
                   char   * PositionDeLaVariableDEcartCoupes 
                            )
{
int Var; int Cnt; int VarSpx ; int Nb; int NbMaxIterations; char ControleFaitOptimumNonBorne;
char ArretDemande ; int CntBase; char ControlerAdmissibiliteDuale;                  	

/* Ensuite Bs sera toujours remis a 0 des qu'on aura fini de s'en servir */
memset( (char *) Spx->Bs , 0 , Spx->NombreDeContraintes * sizeof( double ) );

Spx->SeuilDePivotDual = VALEUR_DE_PIVOT_ACCEPTABLE;

Spx->StrongBranchingEnCours = OUI_SPX;
Spx->UtiliserLaLuUpdate     = NON_SPX;

for ( Nb = 0 ; Nb < NombreDeVariablesABrancher ; Nb++ ) {

/* Le numero fourni ne peut pas etre dans la numerotation interne au simplexe car l'appelant n'y a
   pas acces */
  Var    = NumerosDesVariablesABrancher[Nb];
  VarSpx = Spx->CorrespondanceVarEntreeVarSimplexe[Var];
   
  XmaxSV  [Nb]     = Spx->Xmax[VarSpx];
  Spx->Xmax[VarSpx] = Spx->Xmin[VarSpx];
}

/* La base factorisee est la base courante et on s'arrete des qu'il faut factoriser
   la base */

Spx->LastEta = -1;
Spx->NombreDeChangementsDeBase = 0;

*YaUneSolution           = OUI_SPX;
*TypeDeSolution          = STRONG_BRANCHING_NON_DEFINI;
Spx->FactoriserLaBase    = NON_SPX;
Spx->ModifCoutsAutorisee = NON_SPX;
Spx->NbCyclesSansControleDeDegenerescence = (int) floor( 0.5 * Spx->CycleDeControleDeDegenerescence );
Spx->Iteration               = 0;
ControleFaitOptimumNonBorne = NON_SPX;
ArretDemande                = NON_SPX;
NbMaxIterations             = NOMBRE_DITERATIONS_DE_STRONG_BRANCHING;
if ( NbMaxIterations > CYCLE_DE_REFACTORISATION_DUAL - 1 ) NbMaxIterations = CYCLE_DE_REFACTORISATION_DUAL - 1;

Spx->NombreDeBornesAuxiliairesUtilisees = 0;

/* Inutile de calculer BBarre a la premiere iteration */
Spx->CalculerBBarre = OUI_SPX; 

/* Inutile de calculer CBarre a la premiere iteration */
Spx->CalculerCBarre = NON_SPX;

SPX_InitialiserLeTableauDesVariablesHorsBase( Spx ); 
/*SPX_InitMatriceDeStockageDesContraintesParLigne( Spx );*/

while ( 1 ) {	

  Spx->Iteration++;
  /*
  printf("Strong branching iteration de simplexe numero %d\n",Spx->Iteration);  
  */
  /* S'il y a des bornes auxiliaires, on essaie cycliquement de remettre les bornes initiales tout
	   en restant dual realisable */
	# ifdef UTILISER_BORNES_AUXILIAIRES
	if ( Spx->NombreDeBornesAuxiliairesUtilisees > 0 ) {
    if ( Spx->Iteration % CYCLE_POUR_SUPPRESSION_DES_BORNES_AUXILIAIRES == 0 ) {
		  SPX_DualSupprimerLesBornesAuxiliaires( Spx );
		}
	}
	# endif
	
  /* Calcul de BBarre c'est a dire B^{-1} * b */
  if ( Spx->CalculerBBarre == OUI_SPX ) {
    SPX_CalculerBBarre( Spx );
    Spx->CalculerBBarre = NON_SPX;
  }
  if ( Spx->Iteration >= NbMaxIterations || ArretDemande == OUI_SPX ) { 
    /* Clause de sortie */
    *YaUneSolution  = OUI_SPX;
    *TypeDeSolution = STRONG_BRANCHING_MXITER_OU_REFACT;
    #if VERBOSE_SPX
      if( Spx->Iteration >= NbMaxIterations ) printf(" Fin strong branching par nombre max d'iterations atteint\n"); 
      if( ArretDemande == OUI_SPX ) printf(" Fin strong branching par refactorisation demandee \n"); 
    #endif
    break;
  }

  /* Verification: si le cout courant depasse le cout max fourni, alors on
     arrete les calculs car le cout courant est un minorant du cout optimal.
     Remarque on peut tout a fait reprendre l'indicateur Spx->UtiliserCoutMax 
     et la valeur Spx->CoutMax utilisee a la resolution precedente */
  if ( Spx->UtiliserCoutMax == OUI_SPX && Spx->Iteration > 1 ) {  
    SPX_CalculDuCout( Spx );    
    if ( Spx->Cout > Spx->CoutMax ) {        
      *YaUneSolution = NON_SPX;      
      #if VERBOSE_SPX
        printf(" Fin strong branching par depassement du cout max \n"); 
      #endif
      break;
    }
  }
  /* Choix de la variable qui quitte la base */  
  SPX_DualChoixDeLaVariableSortante( Spx );  
  if ( Spx->VariableSortante < 0 ) {
    /* Optimalite atteinte */
    #if VERBOSE_SPX
      printf(" Fin strong branching par optimalite\n"); 
    #endif
    *YaUneSolution  = OUI_SPX;
    *TypeDeSolution = STRONG_BRANCHING_OPTIMALITE;
    break; /* Fin du while car optimum atteint */
  }
  
  SPX_DualCalculerNBarreR( Spx, OUI_SPX, &ControlerAdmissibiliteDuale );
  SPX_DualVerifierErBMoinsUn( Spx );
  
  if ( Spx->FactoriserLaBase == OUI_SPX ) { 
    #if VERBOSE_SPX
      printf(" Fin strong branching par FactoriserLaBase = OUI \n"); 
    #endif
    *YaUneSolution  = OUI_SPX;
    *TypeDeSolution = STRONG_BRANCHING_REFACTORISATION;
    break; 
  }

  /* SPX_DualCalculerCBarreSurNBarreR(); car c'est calcule dans SPX_DualCalculerNBarreR */
  if ( Spx->NombreDeVariablesATester <= 0 ) { 
    /* Mise a jour de l'indicateur: Spx->AdmissibilitePossible */
    SPX_DualConfirmerDualNonBorne( Spx );
  }

  /* Choix de la variable entrante */
  SPX_DualChoixDeLaVariableEntrante( Spx );
  if ( Spx->VariableEntrante < 0 ) {  
   if ( Spx->AdmissibilitePossible == OUI_SPX ) {  
      #if VERBOSE_SPX
        printf("Spx->AdmissibilitePossible = OUI_SPX => on considere quand meme qu'il y a une solution admissible\n"); 
      #endif
      *YaUneSolution  = OUI_SPX;
      *TypeDeSolution = STRONG_BRANCHING_OPTIMALITE;
      break; 
    }
    else { 
      #if VERBOSE_SPX
        printf("Spx->AdmissibilitePossible = NON_SPX\n"); 
      #endif
    }
 
    #if VERBOSE_SPX
      printf(" Fin strong branching par dual non borne \n"); 
    #endif
    *YaUneSolution = NON_SPX;
    break;
  }

  if ( Spx->FactoriserLaBase == OUI_SPX ) {
    #if VERBOSE_SPX
      printf(" Fin strong branching par FactoriserLaBase = OUI \n"); 
    #endif
    /* On ne peut pas s'arreter brutalement car il a pu y avoir des bound flip ce qui fait 
       que les valeurs des variables en base doivent etre recalculees */
    Spx->FactoriserLaBase = NON_SPX;
    ArretDemande         = OUI_SPX;
  }	
  else {
    SPX_CalculerABarreS( Spx );  /* On en a aussi besoin pour le steepest edge et pour les controles */
    SPX_VerifierABarreS( Spx ); 	
    CntBase = Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante];
    SPX_MettreAJourLesCoutsReduits( Spx );  
    if ( Spx->FaireMiseAJourDeBBarre == OUI_SPX ) { 
      SPX_MettreAJourBBarre( Spx ); 
      Spx->CalculerBBarre = NON_SPX;
    }
    else Spx->CalculerBBarre = OUI_SPX;
    
    /* Mise a jour des poids de la methode projected steepest edge */
    SPX_MajPoidsDualSteepestEdge( Spx );
    if ( Spx->FactoriserLaBase == OUI_SPX ) {
      #if VERBOSE_SPX
        printf(" Fin strong branching par FactoriserLaBase = OUI \n"); 
      #endif
      Spx->FactoriserLaBase = NON_SPX;
      ArretDemande         = OUI_SPX;
    }
  }
  
  SPX_FaireLeChangementDeBase( Spx );

	/* Apres chaque chagement de base reussi on essaie de revenir au seuil de pivotage initial */
	if ( Spx->SeuilDePivotDual > VALEUR_DE_PIVOT_ACCEPTABLE ) {
    Spx->SeuilDePivotDual /= DIVISEUR_VALEUR_DE_PIVOT_ACCEPTABLE;
	  if ( Spx->SeuilDePivotDual < VALEUR_DE_PIVOT_ACCEPTABLE ) Spx->SeuilDePivotDual = VALEUR_DE_PIVOT_ACCEPTABLE;	
	}
	
  /* Si la factorisation s'est mal passee */
  if ( Spx->FactoriserLaBase == OUI_SPX ) ArretDemande = OUI_SPX;

}

/* Maintenant on peut recuperer la valeur du critere correspondant a la solution courante ainsi 
   que la base puisqu'elle est duale realisable et qu'elle pourra servir lors d'une optimisation 
   complete */
   
SPX_FixerXEnFonctionDeSaPosition( Spx );

SPX_RecupererLaSolution(
                Spx,
                NbVar_E, 
                X,
                TypeVar_E,  
                NbContr_E,
                PositionDeLaVariable,	
                NbVarDeBaseComplementaires,
                ComplementDeLaBase
/*
                InDualFramework,
                DualPoids  
*/
                       );

if ( NombreDeContraintesCoupes > 0 ) {
  SPX_RecupererLaSolutionSurLesCoupes( Spx , NombreDeContraintesCoupes , NbTermCoupes , PositionDeLaVariableDEcartCoupes ); 
}

for ( Nb = 0 ; Nb < NombreDeVariablesABrancher ; Nb++ ) {
  Var    = NumerosDesVariablesABrancher[Nb];
  VarSpx = Spx->CorrespondanceVarEntreeVarSimplexe[Var];
  Spx->Xmax[VarSpx] = XmaxSV[Nb];
}

#if VERBOSE_SPX
  printf(" Iteration de sortie du strong branching %d\n",Spx->Iteration); fflush(stdout);
#endif

/* On remet les donnees internes du simplexe dans l'etat initial */

for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
  Spx->X                           [Var] = Spx->XSV[Var];
  Spx->PositionDeLaVariable        [Var] = Spx->PositionDeLaVariableSV[Var]; 
  Spx->CBarre                      [Var] = Spx->CBarreSV[Var];    
  Spx->InDualFramework             [Var] = Spx->InDualFrameworkSV[Var];         
  Spx->ContrainteDeLaVariableEnBase[Var] = Spx->ContrainteDeLaVariableEnBaseSV[Var];            
}

for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  Spx->BBarre                      [Cnt] = Spx->BBarreSV[Cnt];
  Spx->DualPoids                   [Cnt] = Spx->DualPoidsSV[Cnt];
  Spx->VariableEnBaseDeLaContrainte[Cnt] = Spx->VariableEnBaseDeLaContrainteSV[Cnt]; 
}

Spx->LastEta = -1;
Spx->NombreDeChangementsDeBase = 0;
Spx->Iteration                 = 0;

return;

}
