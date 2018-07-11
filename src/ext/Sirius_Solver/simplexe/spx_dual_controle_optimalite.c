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

   FONCTION: Controle de l'optimalite
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# define VERBOSE_SPX 0

/*----------------------------------------------------------------------------*/

void SPX_DualControlerOptimalite( PROBLEME_SPX * Spx, int * NbControlesFinaux,
                                  char * ControleSolutionFait )
{
int AdmissibiliteRestauree; int Marge; char CoutsReduitsAJour; int NombreDeCoutsBruitess;
char OnVientDeFactoriser; int Var; char * CorrectionDuale; double * C; double * Csv;

#if VERBOSE_SPX 
  printf("SPX_DualControlerOptimalite a l iteration %d\n", Spx->Iteration);
#endif

Spx->YaUneSolution = NON_SPX;
Spx->ModifCoutsAutorisee = NON_SPX;

if ( Spx->NombreDeChangementsDeBase > 0 ) CoutsReduitsAJour = NON_SPX;
else CoutsReduitsAJour = OUI_SPX;

/* On verifie qu'on peut faire encore des iterations si necessaire */
/* Attention une augmentation inconsidere de NombreMaxDIterations peut conduire le simplexe a ne
   jamais se terminer */
if ( *NbControlesFinaux < NOMBRE_MAX_DE_CONTROLES_FINAUX ) {  
  Marge = (int) ceil( 0.5 * Spx->NombreDeContraintes );
  if ( Spx->Iteration < Spx->NombreMaxDIterations ) {
	  if ( Spx->NombreMaxDIterations - Marge < Spx->Iteration ) Spx->NombreMaxDIterations = Spx->Iteration + Marge;
  }
  else Spx->NombreMaxDIterations = Spx->Iteration + Marge;
}

/* Optimum atteint: si il y a eu degenerescence, on restaure les couts */
if ( Spx->LesCoutsOntEteModifies == OUI_SPX ) {
  /* On remet les vrais couts */	
  /*memcpy( (char *) Spx->C, (char *) Spx->Csv, Spx->NombreDeVariables * sizeof( double ) );*/
	NombreDeCoutsBruitess = 0;
	CorrectionDuale = Spx->CorrectionDuale;
	C = Spx->C;
	Csv = Spx->Csv;
	for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
    if ( CorrectionDuale[Var] == 0 ) NombreDeCoutsBruitess++;
		C[Var] = Csv[Var];
	}
	 if ( Spx->AffichageDesTraces == OUI_SPX ) {
		printf("NombreDeCoutsBruitess %d  NombreDeVariables %d  ratio %e\n",
		NombreDeCoutsBruitess,Spx->NombreDeVariables,(float) NombreDeCoutsBruitess / (float) Spx->NombreDeVariables);
	 }

  Spx->LesCoutsOntEteModifies = NON_SPX;
  CoutsReduitsAJour = NON_SPX;

  if ( NombreDeCoutsBruitess > 0.1 * Spx->NombreDeVariables ) {
	  if ( Spx->AffichageDesTraces == OUI_SPX )
		  printf("On relance la base reduite\n");
	  Spx->ForcerUtilisationDeLaBaseComplete = 0;
	  Spx->InitBaseReduite = OUI_SPX;
	}

  if ( Spx->AffichageDesTraces == OUI_SPX ) {
    if ( Spx->ProblemePneDeSpx == NULL ) printf("Not far from optimality ..seems so ..\n");
	}
	
}

/* Controle d'admissibilite duale */
if ( *NbControlesFinaux < NOMBRE_MAX_DE_CONTROLES_FINAUX ) { /* C'est pour eviter un cyclages pour des question d'epsilon */
  /* Avant de verifier l'admissibilite duale il est preferable de calculer exactement les
	   couts reduits pour eviter les derives dues a la mise a jour */
	OnVientDeFactoriser = NON_SPX;
  if ( Spx->NombreDeChangementsDeBase > 0 && Spx->ExplorationRapideEnCours == NON_SPX ) {		
	  OnVientDeFactoriser = OUI_SPX;
    SPX_FactoriserLaBase( Spx );	
    if ( Spx->ChoixDeVariableSortanteAuHasard == OUI_SPX ) {
      *NbControlesFinaux = *NbControlesFinaux + 1; /* Incrementer le compteur car sinon il y a risque cyclage */					
		  /* Probleme: la refactorisation n'a pas marche */
      #if VERBOSE_SPX 
        printf( "SPX_DualControlerOptimalite: NbControlesFinaux %d ChoixDeVariableSortanteAuHasard: OUI\n",
				         *NbControlesFinaux);  
      #endif					
      return;
    }
    CoutsReduitsAJour = NON_SPX;
  }
  if ( CoutsReduitsAJour == NON_SPX ) {	
	  /* Recalcul des couts reduits car ils peuvent etre imprecis */				
    SPX_CalculerPi( Spx );             
    SPX_CalculerLesCoutsReduits( Spx );
    CoutsReduitsAJour = OUI_SPX;
    Spx->CalculerCBarre = NON_SPX;
	}	
  *NbControlesFinaux = *NbControlesFinaux + 1;			
  SPX_VerifierAdmissibiliteDuale( Spx , &AdmissibiliteRestauree );
  if ( AdmissibiliteRestauree == OUI_SPX ) { 
    #if VERBOSE_SPX 
      printf( "SPX_DualControlerOptimalite: admissibilite duale restauree, on refait des iterations duales\n");  
    #endif				
    Spx->CalculerBBarre = OUI_SPX;		
		/* La solution n'etait pas optimale, on repart dans les iterations */
		return;
  }
	/* Si la base vient d'etre factorisee, on relance une recherche de variables sortantes au cas ou
	   la solution n'etait pas primale admissible */
	if ( OnVientDeFactoriser == OUI_SPX ) {	
    Spx->CalculerBBarre = OUI_SPX;
		return;
	}		 
}

/* A ce stade, soit on a depasse le nombre max de controles finaux, soit tout est OK */
if ( *ControleSolutionFait == NON_SPX ) {
  /* Si la date de factorisation precedente est trop ancienne on refactorise la base */
  if ( Spx->NombreDeChangementsDeBase >= (int) (0.5 * Spx->CycleDeRefactorisation) ) { 
    #if VERBOSE_SPX 
      printf("Factorisation de la base pour affiner la solution\n");
    #endif		
    /* Bizarement, si on refactorise apres un nombre de changements de bases trop petit ca peut dire qu'il n'y a pas de solution ensuite */
    SPX_FactoriserLaBase( Spx );     
    Spx->CalculerBBarre      = OUI_SPX; 
    Spx->CalculerCBarre      = OUI_SPX;  /* Attention il faut recalculer Pi quand meme pour avoir les variables duales */
    *ControleSolutionFait    = OUI_SPX;
    Spx->CycleDeRefactorisation = (int) (0.5 * Spx->CycleDeRefactorisation);
		/* 2 car c'est decremente apres Debut */
		Spx->FaireDuRaffinementIteratif = 2;		
		return;		
  }
}		

if ( CoutsReduitsAJour == NON_SPX ) {
  /* On affine quand meme les couts reduits pour le reduced cost fixing du branch and bound */
  SPX_CalculerPi( Spx );             
  SPX_CalculerLesCoutsReduits( Spx );
  Spx->CalculerCBarre = NON_SPX;

  /* Controle de l'admissibilite duale: si la solution n'est pas duale admissible, on sort sans solution */
  SPX_VerifierAdmissibiliteDuale( Spx , &AdmissibiliteRestauree );
  if ( AdmissibiliteRestauree == OUI_SPX ) { 
    #if VERBOSE_SPX 
      printf( "SPX_DualControlerOptimalite: solution primale admissible mais non duale adminissible\n");  
    #endif

    /* Il faut recalculer BBarre puisqu'on a change des bornes */
    Spx->CalculerBBarre = OUI_SPX; 
    /*Spx->CalculerCBarre = OUI_SPX;*/
		
		return;
  }
}

#if VERBOSE_SPX 
  printf( "SPX_DualControlerOptimalite: fin de phase 2 atteint en %d iterations\n",Spx->Iteration-1);
#endif
	 
Spx->YaUneSolution = OUI_SPX;

return;
}


