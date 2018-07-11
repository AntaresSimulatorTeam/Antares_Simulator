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

   FONCTION: Factorisation LU de la base 
               
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_constantes_externes.h"
# include "lu_definition_arguments.h"

# include "lu_define.h"
# include "lu_fonctions.h"

# define COEFF_MULTIPLICATEUR_DU_SEUIL  2.0 /*4.0*/
# define SEUIL_MARKOWITZ_MAX            0.2 /*0.5*/
# define NOMBRE_DE_FOIS_MODIF_SEUIL     2   /*3*/
# define SEUIL_INVERSE_ASSEZ_PLEIN      0.25

# define DEBUG OUI_SPX

# define TRACES 0

# define VERBOSE_SPX 0

# define COMPILE_DM 0
# if COMPILE_DM == 1
# include "cs.h"

/*----------------------------------------------------------------------------*/

void SPX_TestDM( PROBLEME_SPX * Spx, MATRICE_A_FACTORISER * Matrice )
{
int i; int ilAi; int il; int ilMax; csd * Csd; csi seed;
int Cb; int C1; int C2; int C3; 
int R1; int R2; int R3; int Rb;
cs * A;

A = (cs *) malloc( sizeof( cs ) );
A->nzmax = 0;
for ( i = 0 ; i < Matrice->NombreDeColonnes ; i++ ) A->nzmax += Matrice->NbTermesDesColonnes[i];
A->n = Matrice->NombreDeColonnes;
A->m = Matrice->NombreDeColonnes;
A->p = (csi *) malloc( ( A->n + 1 ) * sizeof( csi ) );
A->i = (csi *) malloc( A->nzmax * sizeof( csi ) );
A->x = (double *) malloc( A->nzmax * sizeof( double ) );
A->nz = -1; /* compressed-col */

ilAi = 0;
for ( i = 0 ; i < Matrice->NombreDeColonnes ; i++ ) {
	il = Matrice->IndexDebutDesColonnes[i];
	ilMax = il + Matrice->NbTermesDesColonnes[i];
  A->p[i] = ilAi;		
  while ( il < ilMax ) {
    A->i[ilAi] = Matrice->IndicesDeLigne[il];
		A->x[ilAi] = Matrice->ValeurDesTermesDeLaMatrice[il];
    ilAi++;
		il++;
	}
}
A->p[A->n] = ilAi;

printf("cs_dmperm \n");

seed = 0;
Csd = cs_dmperm( A, seed );

printf("Nombre de blocs: %d   rang %d  iteration %d\n",(int) Csd->nb,Matrice->NombreDeColonnes,Spx->Iteration);
for ( i = 0 ; i < (int) Csd->nb ; i++ ) {
  if ( Csd->s[i+1] - Csd->s[i] == 1 ) continue;
	printf("bloc %d de %d a %d   taille %d\n",i,(int) Csd->s[i],(int) Csd->s[i+1],(int) Csd->s[i+1] - (int) Csd->s[i]);

}

/*
Cb = Csd->cc[1];
printf("Nombre d'elements de CBarre : %d\n",Cb);
C1 = Csd->cc[2]-Csd->cc[1];
printf("Nombre d'elements de C1 : %d\n",C1);
C2 = Csd->cc[3]-Csd->cc[2];
printf("Nombre d'elements de C2 : %d\n",C2);
C3 = Csd->cc[4]-Csd->cc[3];
printf("Nombre d'elements de C3 : %d\n",C3);

printf("\n");
R1 = Csd->rr[1];
printf("Nombre d'elements de R1 : %d\n",R1);
R2 = Csd->rr[2]-Csd->rr[1];
printf("Nombre d'elements de R2 : %d\n",R2);
R3 = Csd->rr[3]-Csd->rr[2];
printf("Nombre d'elements de R3 : %d\n",R3);
Rb = Csd->rr[4]-Csd->rr[3];
printf("Nombre d'elements de RBarre : %d\n",Rb);
*/
return;
}
# endif

/*----------------------------------------------------------------------------*/

void SPX_FactoriserLaBase( PROBLEME_SPX * Spx )						    
{
int Cnt; int Var  ; int CodeRetour; char OnRetesteSiPivotNul; int FaireScalingLU; 
char SeuilPivotMarkowitzParDefaut    ; double Coeff; double NbTrm; int NbTrmCol;
int * CdebBase; int * NbTermesDesColonnesDeLaBase; int * Cdeb; int * CNbTerm;
int * VariableEnBaseDeLaContrainteSV; int * VariableEnBaseDeLaContrainte;   
char * PositionDeLaVariableSV; char * PositionDeLaVariable; double SeuilInverseDense;
int * ContrainteDeLaVariableEnBaseSV; int * ContrainteDeLaVariableEnBase;
char RestaurerAdmissibiliteDuale; int Iteration; int NombreMaxDIterations; double N2;
int * IndicesDeLigneDesTermesDeLaBase; double * ValeurDesTermesDesColonnesDeLaBase;
int r; int * NumeroDeContrainte; double * ACol; int RangDeLaMatriceFactorisee;
int * ColonneDeLaBaseFactorisee; MATRICE_A_FACTORISER Matrice; int * CdebProblemeReduit;
int * CNbTermProblemeReduit; 

/*
printf("------- Factorisation de la base iteration %d ------ CoefficientPourLaValeurDePerturbationDeCoutAPosteriori %e \n",
        Spx->Iteration,Spx->CoefficientPourLaValeurDePerturbationDeCoutAPosteriori);
*/

OnRetesteSiPivotNul          = OUI_SPX;
SeuilPivotMarkowitzParDefaut = OUI_LU;
RestaurerAdmissibiliteDuale  = NON_SPX;

Debut: 

CdebBase = Spx->CdebBase;
NbTermesDesColonnesDeLaBase = Spx->NbTermesDesColonnesDeLaBase;

Cdeb    = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;   
NumeroDeContrainte = Spx->NumeroDeContrainte;
ACol = Spx->ACol;

if ( Spx->ForcerUtilisationDeLaBaseComplete == 0 ) {  
  SPX_OrdonnerLesContraintesPourLaBase( Spx );
  if ( Spx->UtiliserLaBaseReduite == NON_SPX ) {
	  Spx->RangDeLaMatriceFactorisee = Spx->NombreDeContraintes;
		Spx->ForcerUtilisationDeLaBaseComplete = 1;
    SPX_InitialiserLeTableauDesVariablesHorsBase( Spx );
	}
}
else {
  Spx->RangDeLaMatriceFactorisee = Spx->NombreDeContraintes;
  Spx->UtiliserLaBaseReduite = NON_SPX;
	Spx->NombreDeFactorisationsDeBaseReduite = 0;
	# if TRACES == 1
    printf("\nRangDeLaMatriceFactorisee %d  (base complete) NombreDeFactorisationsDeBaseReduite %d  ModifCoutsAutorisee %d\n",
	          Spx->RangDeLaMatriceFactorisee,Spx->NombreDeFactorisationsDeBaseReduite,Spx->ModifCoutsAutorisee);
	# endif
	/*
	if ( Spx->NombreDeReactivationsDeLaBaseReduite < NB_MAX_DE_REACTIVATIONS_DE_LA_BASE_REDUITE ) {
	  Spx->ForcerUtilisationDeLaBaseComplete--;
	  if ( Spx->ForcerUtilisationDeLaBaseComplete <= 0 ) {
			Spx->ForcerUtilisationDeLaBaseComplete = 0;
			Spx->NombreDeReactivationsDeLaBaseReduite++;
		}
	}
	*/
}

VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {

  RangDeLaMatriceFactorisee = Spx->RangDeLaMatriceFactorisee;

  N2 = (double) RangDeLaMatriceFactorisee * (double) RangDeLaMatriceFactorisee;
  SeuilInverseDense = SEUIL_INVERSE_ASSEZ_PLEIN * N2;

  N2 = (double) RangDeLaMatriceFactorisee * (double) RangDeLaMatriceFactorisee;
  SeuilInverseDense = SEUIL_INVERSE_ASSEZ_PLEIN * N2;
	
  ColonneDeLaBaseFactorisee = Spx->ColonneDeLaBaseFactorisee;
	
	CdebProblemeReduit = Spx->CdebProblemeReduit;
	CNbTermProblemeReduit = Spx->CNbTermProblemeReduit;

  ValeurDesTermesDesColonnesDeLaBase = Spx->ValeurDesTermesDesColonnesDuProblemeReduit;
  IndicesDeLigneDesTermesDeLaBase = Spx->IndicesDeLigneDesTermesDuProblemeReduit;

  NbTrm = 0.0;
  for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) {
		Var = VariableEnBaseDeLaContrainte[ColonneDeLaBaseFactorisee[r]];
    CdebBase[r] = CdebProblemeReduit[Var];
	  NbTrmCol = CNbTermProblemeReduit[Var];
    NbTermesDesColonnesDeLaBase[r] = NbTrmCol;		
	  NbTrm += (double) NbTrmCol;
  }

}   
else {

  N2 = (double) Spx->NombreDeContraintes * (double) Spx->NombreDeContraintes;
  SeuilInverseDense = SEUIL_INVERSE_ASSEZ_PLEIN * N2;

  /* On a deja sous la main le chainage de la transposee mais pour toutes les colonnes 
     de la transposee. Il s'agit donc de preparer une nouvelle table des indices debut 
     pour les seules colonnes de la base */
  ValeurDesTermesDesColonnesDeLaBase = Spx->ACol;
  IndicesDeLigneDesTermesDeLaBase = Spx->NumeroDeContrainte;
		 
  NbTrm = 0.0;
  for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
    Var = VariableEnBaseDeLaContrainte[Cnt];		
    CdebBase[Cnt] = Cdeb[Var];
	  NbTrmCol = CNbTerm[Var];
    NbTermesDesColonnesDeLaBase[Cnt] = NbTrmCol;
	  NbTrm += (double) NbTrmCol;
  }
}

if ( NbTrm > SeuilInverseDense ) Spx->InverseProbablementDense = OUI_SPX;
else Spx->InverseProbablementDense = NON_SPX;

if ( NbTrm < TAUX_DE_REMPLISSAGE_POUR_BASE_HYPER_CREUSE * N2 ) Spx->TypeDeCreuxDeLaBase = BASE_HYPER_CREUSE;
else if ( NbTrm == N2 ) Spx->TypeDeCreuxDeLaBase = BASE_PLEINE;
else Spx->TypeDeCreuxDeLaBase = BASE_CREUSE;

if ( Spx->TypeDeCreuxDeLaBase == BASE_HYPER_CREUSE ) SPX_InitialiserLesIndicateursHyperCreux( Spx );

if ( Spx->MatriceFactorisee != NULL ) {
  LU_LibererMemoireLU( (MATRICE *) Spx->MatriceFactorisee );
  Spx->MatriceFactorisee = NULL;
}

/* Attention. On ne peut pas faire de LU_Update si on fait du scaling car les coefficients
   de scaling ne sont plus valables */

FaireScalingLU = NON_LU;
if ( Spx->FaireScalingLU > 0 && Spx->UtiliserLaLuUpdate == NON_SPX ) {
  FaireScalingLU = OUI_LU;
}

/* Si on a deja un seuil de pivot (cas de la recuperation de la derniere base inversible) on
   le prend */
if ( SeuilPivotMarkowitzParDefaut == OUI_LU ) {
  if ( Spx->FlagStabiliteDeLaFactorisation == 1 ) {
    if ( Spx->NombreDeChangementsDeBase == 0 ) {
      /* Si le nombre changements de base etait nul, on augmente le seuil dans une limite */
      SeuilPivotMarkowitzParDefaut = NON_LU;
      Spx->ValeurDuPivotMarkowitz   = COEFF_MULTIPLICATEUR_DU_SEUIL * PREMIER_SEUIL_DE_PIVOTAGE;
      if ( Spx->ValeurDuPivotMarkowitz > SEUIL_MARKOWITZ_MAX ) Spx->ValeurDuPivotMarkowitz = SEUIL_MARKOWITZ_MAX;
      Spx->ProblemeDeStabiliteDeLaFactorisation = OUI_SPX;
      Spx->FlagStabiliteDeLaFactorisation       = 0;
    }
    else {
      Spx->FlagStabiliteDeLaFactorisation = 0;
    }
  }
  else if ( Spx->ProblemeDeStabiliteDeLaFactorisation == OUI_SPX ) {
    SeuilPivotMarkowitzParDefaut = NON_LU;
    Coeff = pow( (1./COEFF_MULTIPLICATEUR_DU_SEUIL) , (1./(double) NOMBRE_DE_FOIS_MODIF_SEUIL) );
    Spx->ValeurDuPivotMarkowitz   = Coeff * Spx->ValeurDuPivotMarkowitz;
    if ( Spx->ValeurDuPivotMarkowitz < PREMIER_SEUIL_DE_PIVOTAGE) {
      SeuilPivotMarkowitzParDefaut = OUI_LU;
      Spx->ValeurDuPivotMarkowitz   = PREMIER_SEUIL_DE_PIVOTAGE;
      Spx->ProblemeDeStabiliteDeLaFactorisation = NON_SPX;
    }
  }
  /* Pas besoin mais juste pour la coherence */
  if ( SeuilPivotMarkowitzParDefaut == OUI_LU ) Spx->ValeurDuPivotMarkowitz = PREMIER_SEUIL_DE_PIVOTAGE;  
}

Matrice.ContexteDeLaFactorisation  = LU_SIMPLEXE;
Matrice.UtiliserLesSuperLignes     = NON_LU;
Matrice.ValeurDesTermesDeLaMatrice = ValeurDesTermesDesColonnesDeLaBase;
Matrice.IndicesDeLigne		         = IndicesDeLigneDesTermesDeLaBase;
Matrice.IndexDebutDesColonnes	     = CdebBase;
Matrice.NbTermesDesColonnes	       = NbTermesDesColonnesDeLaBase;
Matrice.NombreDeColonnes	         = Spx->RangDeLaMatriceFactorisee;
Matrice.FaireScalingDeLaMatrice    = NON_LU /*FaireScalingLU*/; 
Matrice.UtiliserLesValeursDePivotNulParDefaut = OUI_LU;
Matrice.SeuilPivotMarkowitzParDefaut = SeuilPivotMarkowitzParDefaut;
Matrice.ValeurDuPivotMarkowitz       = Spx->ValeurDuPivotMarkowitz;
Matrice.FaireDuPivotageDiagonal      = NON_LU;
Matrice.LaMatriceEstSymetrique       = NON_LU;
Matrice.LaMatriceEstSymetriqueEnStructure = NON_LU;


# if COMPILE_DM == 1
SPX_TestDM( Spx, &Matrice );
# endif

Spx->MatriceFactorisee = LU_Factorisation( &Matrice );

if ( Spx->MatriceFactorisee == NULL ) {
  Spx->AnomalieDetectee = SPX_ERREUR_INTERNE;
  longjmp( Spx->EnvSpx, Spx->AnomalieDetectee );
}

CodeRetour = Matrice.ProblemeDeFactorisation;

Spx->FaireScalingLU--;
if ( Spx->FaireScalingLU < 0 ) Spx->FaireScalingLU = 0;

if ( Matrice.ProblemeDeFactorisation != 0 ) {
  if ( CodeRetour != MATRICE_SINGULIERE ) {
    #if VERBOSE_SPX
      printf(" Erreur dans la factorisation LU du simplexe, numero d'erreur %d \n",CodeRetour); 
    #endif
    Spx->AnomalieDetectee = OUI_SPX;
    longjmp( Spx->EnvSpx, Spx->AnomalieDetectee );
  }  
  /* Cas d'une matrice singuliere */
  if ( Spx->BaseInversibleDisponible == OUI_SPX && OnRetesteSiPivotNul == OUI_SPX ) { 
    #if VERBOSE_SPX
      printf("Base non inversible, on repart de la derniere base inversible disponible Iteration %d\n",Spx->Iteration);
    #endif          
    Spx->ProblemeDeStabiliteDeLaFactorisation = NON_SPX;

    SeuilPivotMarkowitzParDefaut = NON_LU;
    Spx->ValeurDuPivotMarkowitz  = Spx->ValeurDuPivotMarkowitzSV;
    /* Recuperation de la base inversible */
    for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
      Spx->VariableEnBaseDeLaContrainte[Cnt] = Spx->VariableEnBaseDeLaContrainteSV[Cnt];
    }
		
	  RestaurerAdmissibiliteDuale = NON_SPX;
    for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
      Spx->ContrainteDeLaVariableEnBase[Var] = Spx->ContrainteDeLaVariableEnBaseSV[Var];            
      Spx->PositionDeLaVariable        [Var] = Spx->PositionDeLaVariableSV[Var];
			if ( Spx->PhaseEnCours == PHASE_1 ) continue;
			if ( Spx->StrongBranchingEnCours == OUI_SPX ) {
			  printf("Attention probleme dans SPX_Factorisation: on essaie de factoriser alors qu'on est en strong branching\n");
				continue;
			}
			if ( RestaurerAdmissibiliteDuale == OUI_SPX ) continue;
		  /* Pour les types de bornes modifiables, il faut tester la position des variables pour voir
		     si elle est en accord ou non. En effet, il a pu y avoir des modifications de types de bornes. Si elle n'est
		     pas en accord on fait une phase 1 */			
			if ( Spx->TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {
			  if ( Spx->PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) RestaurerAdmissibiliteDuale = OUI_SPX;
			}
			else if ( Spx->TypeDeVariable[Var] == NON_BORNEE ) {
			  if ( Spx->PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF ||
				     Spx->PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) RestaurerAdmissibiliteDuale = OUI_SPX;
			}
    }

	  /* Il faut restaurer l'admissibilite duale si l'on utilise les bornes auxiliaires */
    # ifdef UTILISER_BORNES_AUXILIAIRES   
		  RestaurerAdmissibiliteDuale = OUI_SPX;
			/* On enleve toutes les bornes auxiliaires car elles seront retablies si necessaire */
      for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
        if ( Spx->StatutBorneSupCourante[Var] != BORNE_NATIVE ) SPX_SupprimerUneBorneAuxiliaire( Spx, Var );					
      }
			if ( Spx->NombreDeBornesAuxiliairesUtilisees != 0 ) {
			  printf("Attention probleme dans SPX_Factorisation: le nombre de bornes auxiliaires n'est pas nul (valeur = %d)\n",
				        Spx->NombreDeBornesAuxiliairesUtilisees);			
			}			
    # endif
				
    /* Precaution: RAZ du Steepest Edge */
    SPX_InitDualPoids( Spx );
		
    /* Infos concernant les variables hors base */
    SPX_InitialiserLeTableauDesVariablesHorsBase( Spx );		
		
    /* Initialisation des parametres de choix au hasard des variables sortantes */

		Spx->NombreDeChoixFaitsAuHasard = 0;
		Spx->FaireChangementDeBase = NON_SPX;		
    Spx->ChoixDeVariableSortanteAuHasard = OUI_SPX;
    
		Spx->NombreMaxDeChoixAuHasard = (int) (0.5 * Spx->CycleDeRefactorisation);
    if ( Spx->NombreMaxDeChoixAuHasard < 10 ) Spx->NombreMaxDeChoixAuHasard = 10;

		/* On augmente le seuil dual de pivotage */
		Spx->SeuilDePivotDual = COEFF_AUGMENTATION_VALEUR_DE_PIVOT_ACCEPTABLE * VALEUR_DE_PIVOT_ACCEPTABLE;		
						
    #if VERBOSE_SPX
      printf("Iteration %d nombre de choix de pivot au hasard a faire parmi les choix acceptables: %d\n",
			        Spx->Iteration,Spx->NombreMaxDeChoixAuHasard);
    #endif
		
    /* On refait la factorisation */
    OnRetesteSiPivotNul = NON_SPX; /* Mais si on retombe sur un pivot nul alors c'est grave */
		
    goto Debut;    
  }
  else {
    /* Pas de base inversible disponible */
    Spx->AnomalieDetectee = OUI_SPX;
    if ( CodeRetour == MATRICE_SINGULIERE ) { 
      #if VERBOSE_SPX
        printf(" Erreur dans la factorisation LU du simplexe, base non inversible et aucune base inversible disponible\n"); 
      #endif
      Spx->AnomalieDetectee = SPX_MATRICE_DE_BASE_SINGULIERE;
    }		
    longjmp( Spx->EnvSpx, Spx->AnomalieDetectee );
  }
}

/* Stockage de la derniere base inversible */

Spx->BaseInversibleDisponible = OUI_SPX;

Spx->ValeurDuPivotMarkowitzSV = Spx->ValeurDuPivotMarkowitz;

VariableEnBaseDeLaContrainteSV = Spx->VariableEnBaseDeLaContrainteSV;
VariableEnBaseDeLaContrainte   = Spx->VariableEnBaseDeLaContrainte;
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  VariableEnBaseDeLaContrainteSV[Cnt] = VariableEnBaseDeLaContrainte[Cnt];
}

PositionDeLaVariableSV = Spx->PositionDeLaVariableSV;
PositionDeLaVariable   = Spx->PositionDeLaVariable;
ContrainteDeLaVariableEnBaseSV = Spx->ContrainteDeLaVariableEnBaseSV;
ContrainteDeLaVariableEnBase   = Spx->ContrainteDeLaVariableEnBase;

for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
  PositionDeLaVariableSV        [Var] = PositionDeLaVariable[Var];
  ContrainteDeLaVariableEnBaseSV[Var] = ContrainteDeLaVariableEnBase[Var];            
}

Spx->LastEta = -1;
Spx->NombreDeChangementsDeBase = 0;

Spx->FactoriserLaBase = NON_SPX; /* Repositionnement */

Spx->CalculerBBarre = OUI_SPX;  
Spx->CalculerCBarre = OUI_SPX;

if ( RestaurerAdmissibiliteDuale == OUI_SPX ) {
  Iteration            = Spx->Iteration;
  NombreMaxDIterations = Spx->NombreMaxDIterations;
  
  #if VERBOSE_SPX
    printf("Probleme de factorisation, restauration de l'admissibilite duale necessaire a l'iteration %d\n",Spx->Iteration); 
  #endif 
		
  /* Restauration des couts: normalement ca a deja ete fait sauf s'il n'y a pas eu de degenerescence duale */
	memcpy( (char *) Spx->C, (char *) Spx->Csv, Spx->NombreDeVariables * sizeof( double ) );
  for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) Spx->CorrectionDuale[Var] = NOMBRE_MAX_DE_PERTURBATIONS_DE_COUT;    
    
  SPX_DualPhase1Simplexe( Spx );
	
	Spx->PhaseEnCours = PHASE_2;
  
  Spx->Iteration             = Iteration;
  Spx->NombreMaxDIterations  = NombreMaxDIterations;
  Spx->CalculerBBarre        = OUI_SPX; 
  Spx->CalculerCBarre        = OUI_SPX;
	Spx->FaireChangementDeBase = NON_SPX;
  
  if ( Spx->LaBaseEstDualeAdmissible == NON_SPX ) {
    /* Echec */
    Spx->YaUneSolution = NON_SPX;
    Spx->Iteration     = 10 * Spx->NombreMaxDIterations;
  }
	else {
	  /* On augmente le seuil dual de pivotage car on est tombe sur un pivot nul auparavant */
    /* Et on fait quelques tirages au hasard pour eviter de retomber sur le probleme */
	  Spx->NombreDeChoixFaitsAuHasard = 0;
	  Spx->FaireChangementDeBase = NON_SPX;		
    Spx->ChoixDeVariableSortanteAuHasard = OUI_SPX;    
		Spx->NombreMaxDeChoixAuHasard = (int) (0.5 * Spx->CycleDeRefactorisation);
    if ( Spx->NombreMaxDeChoixAuHasard < 10 ) Spx->NombreMaxDeChoixAuHasard = 10;

		/* On augmente le seuil dual de pivotage */
		Spx->SeuilDePivotDual = COEFF_AUGMENTATION_VALEUR_DE_PIVOT_ACCEPTABLE * VALEUR_DE_PIVOT_ACCEPTABLE;		
  }						
		
}

return;
}

