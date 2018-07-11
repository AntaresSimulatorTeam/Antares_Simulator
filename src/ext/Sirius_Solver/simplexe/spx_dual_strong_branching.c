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

   FONCTION: Comme son nom l'indique 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "pne_define.h"

# include "lu_fonctions.h"

/*----------------------------------------------------------------------------*/

void SPX_DualStrongBranching( 
                   PROBLEME_SPX * Spx     , 
                   /* La variable a brancher */ 
                   int   VariableSortante        , 
                   double CoutDeLaVariableSortante,
                   double NouvelleBorneMin        ,
                   double NouvelleBorneMax        ,  
	                 /* Sortie */
		               double * CoutReduitDeLaVariableBranchee,
                   int   * YaUneSolution,  
                   char   * TypeDeSolution,  
                   double * X,  
                   int   * PositionDeLaVariable,                   
                   int   * NbVarDeBaseComplementaires,                   
                   int   * ComplementDeLaBase,
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
/*int Var; */ int Cnt; int VariableSortanteSv; int   TypeDeSortieSv ; int NbMaxIterations;
char ControleFaitOptimumNonBorne           ; int   il; int ilMax ; char ArretDemande   ;
double XminSV     ; double XmaxSV          ; double XminEntreeSV   ; double DeltaCoutFixe; 
char ControlerAdmissibiliteDuale;
/* double S; int ic; int icMax; int NbCntrl; */

/* Ensuite Bs sera toujours remis a 0 des qu'on aura fini de s'en servir */
memset( (char *) Spx->Bs , 0 , Spx->NombreDeContraintes * sizeof( double ) );

CoutDeLaVariableSortante = 0; /* Pour ne pas avoir de warning a la compilation */

Spx->SeuilDePivotDual = VALEUR_DE_PIVOT_ACCEPTABLE;

Spx->StrongBranchingEnCours = OUI_SPX;
Spx->UtiliserLaLuUpdate     = NON_SPX;
Spx->FaireDuRaffinementIteratif = 0;

/* Le numero fourni ne peut pas etre dans la numerotation interne au simplexe car l'appelant n'y a
   pas acces */					  
Spx->VariableSortante = Spx->CorrespondanceVarEntreeVarSimplexe[VariableSortante];
VariableSortanteSv    = Spx->VariableSortante;
  
if ( Spx->PositionDeLaVariable[Spx->VariableSortante] == HORS_BASE_SUR_BORNE_INF ||		        
     Spx->PositionDeLaVariable[Spx->VariableSortante] == HORS_BASE_SUR_BORNE_SUP || 
     Spx->PositionDeLaVariable[Spx->VariableSortante] == HORS_BASE_A_ZERO ) {
  printf("Bug dans SPX_DualStrongBranching, la variable sortante %d n'est pas basique. ",Spx->VariableSortante); 
  if ( Spx->PositionDeLaVariable[Spx->VariableSortante] == HORS_BASE_SUR_BORNE_INF ) {
    printf("Elle est HORS_BASE_SUR_BORNE_INF\n"); 
  }
  else if ( Spx->PositionDeLaVariable[Spx->VariableSortante] == HORS_BASE_SUR_BORNE_SUP ) {
    printf("Elle est HORS_BASE_SUR_BORNE_SUP\n"); 
  }
  else if ( Spx->PositionDeLaVariable[Spx->VariableSortante] == HORS_BASE_A_ZERO ) {
    printf("elle est HORS_BASE_A_ZERO\n"); 
  }
  printf("Valeur calculee par le simplexe: %lf valeur min: %lf valeur max: %lf\n",
          Spx->X[Spx->VariableSortante],Spx->Xmin[Spx->VariableSortante],Spx->Xmin[Spx->VariableSortante]); 
  exit(0);					
}

XminEntreeSV = Spx->XminEntree[Spx->VariableSortante]; /* XminEntree n'a pas subi de scaling */
Spx->XminEntree[Spx->VariableSortante] = NouvelleBorneMin;

XminSV = Spx->Xmin[Spx->VariableSortante];
XmaxSV = Spx->Xmax[Spx->VariableSortante];

/* En comparant la valeur de X a ses nouvelles bornes, on en deduit le type de sortie de la base */
/* Scaling des bornes externes */
Spx->Xmin[Spx->VariableSortante] = NouvelleBorneMin;
Spx->Xmax[Spx->VariableSortante] = NouvelleBorneMax;
if ( Spx->TypeDeVariable[Spx->VariableSortante] != NON_BORNEE ) {
  Spx->Xmin[Spx->VariableSortante] = Spx->Xmin[Spx->VariableSortante] / Spx->ScaleX[Spx->VariableSortante];
}
if ( Spx->TypeDeVariable[Spx->VariableSortante] == BORNEE     ) { 
  Spx->Xmax[Spx->VariableSortante] = Spx->Xmax[Spx->VariableSortante] / Spx->ScaleX[Spx->VariableSortante];
}

if ( Spx->X[Spx->VariableSortante] >= Spx->Xmax[Spx->VariableSortante] )      Spx->SortSurXmaxOuSurXmin = SORT_SUR_XMAX;  
else if ( Spx->X[Spx->VariableSortante] <= Spx->Xmin[Spx->VariableSortante] ) Spx->SortSurXmaxOuSurXmin = SORT_SUR_XMIN;
else {
  printf("Bug dans SPX_StrongBranching, demande de branchement sur la variable %d or elle a deja une valeur entiere\n",
          VariableSortante);  
  exit(0);
}

if ( Spx->TypeDeVariable[Spx->VariableSortante] == BORNEE && 
     Spx->SortSurXmaxOuSurXmin                 == SORT_SUR_XMIN) {
  DeltaCoutFixe = ( Spx->C[Spx->VariableSortante] * XmaxSV ) / Spx->ScaleLigneDesCouts;
}
else DeltaCoutFixe = 0.;

TypeDeSortieSv = Spx->SortSurXmaxOuSurXmin;

/* On simule la translation des bornes */
Spx->Xmin[Spx->VariableSortante] = 0.;
Spx->Xmax[Spx->VariableSortante] = 0.;

if( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) {
  /* Faire un passage a XmaxEntree c'est imposer XminEntree = XmaxEntree . La translation des bornes ayant pour 
     effet de modifier le second membre, il faut mettre a jour BBarre */
  Cnt                         = Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante];
  Spx->BBarre[Cnt]             = ( XmaxSV - Spx->BBarre[Cnt] ) * -1.; 
  Spx->X[Spx->VariableSortante] = Spx->BBarre[Cnt];

  /* Dans les calculs qui suivent on est amene calculer entierement le vecteur BBarre. Il faut donc 
     tenir compte de la modification du second membre B qui decoulerait du fait que la variable 
     dont on simule l'instanciation se retrouve avec Xmin = Xmax */
  il    = Spx->Cdeb[Spx->VariableSortante];
  ilMax = il + Spx->CNbTerm[Spx->VariableSortante];
  while ( il < ilMax ) {
    Spx->B[Spx->NumeroDeContrainte[il]]-= Spx->ACol[il] * XmaxSV;
    il++;
  }
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

/* Inutile de calculer BBarre a la premiere iteration */
Spx->CalculerBBarre = NON_SPX; 

/* Inutile de calculer CBarre a la premiere iteration */
Spx->CalculerCBarre = NON_SPX;

SPX_InitialiserLeTableauDesVariablesHorsBase( Spx );
/* Car au depart CalculerBBarre est egal a NON_SPX */
SPX_InitialiserLesVariablesEnBaseAControler( Spx );

Spx->PhaseEnCours = PHASE_2;

while ( 1 ) {	

  Spx->Iteration++; 
  /*
  printf("Strong branching iteration de simplexe numero %d\n",Spx->Iteration);  
  */	
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
    /*SPX_CalculDuCout( Spx );*/
    SPX_CalculDuCoutSimplifie( Spx );  
    if ( Spx->Cout + DeltaCoutFixe > Spx->CoutMax ) {        
      *YaUneSolution  = NON_SPX;
      *TypeDeSolution = STRONG_BRANCHING_COUT_MAX_DEPASSE;			
      #if VERBOSE_SPX
        printf(" Fin strong branching par depassement du cout max \n"); 
      #endif			
      break;
    }
  }
  
  /* Choix de la variable qui quitte la base: attention, a la premiere iteration la variable sortante 
     a deja ete choisie */  
  if ( Spx->Iteration > 1 ) SPX_DualChoixDeLaVariableSortante( Spx );  
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

  if ( Spx->NombreDeVariablesATester <= 0 ) { 
    /* Mise a jour de l'indicateur: Spx->AdmissibilitePossible (on s'en sert plus tard) */
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
		
    *YaUneSolution  = NON_SPX;
		*TypeDeSolution = STRONG_BRANCHING_PAS_DE_SOLUTION;
    break;
  }

  if ( Spx->FactoriserLaBase == OUI_SPX ) {
    #if VERBOSE_SPX
      printf(" Fin strong branching par FactoriserLaBase = OUI \n"); 
    #endif		
    /* On ne peut pas s'arreter brutalement car il a pu y avoir des bound flip ce qui fait 
       que les valeurs des variables en base doivent etre recalculees */
    Spx->FactoriserLaBase = NON_SPX;
    ArretDemande          = OUI_SPX;
  }	
  else {
    SPX_CalculerABarreS( Spx );  /* On en a aussi besoin pour le steepest edge et pour les controles */
		/*SPX_VerifierABarreS( Spx );*/

		/* On fait toujours le changement de base meme s'il y a une imprecision */
    Spx->FaireChangementDeBase = OUI_SPX;
				
    /* Mise a jour des poids de la methode projected steepest edge */
    SPX_MajPoidsDualSteepestEdge( Spx );
    if ( Spx->FactoriserLaBase == OUI_SPX ) {
      #if VERBOSE_SPX
        printf(" Fin strong branching par FactoriserLaBase = OUI \n"); 
      #endif			
      Spx->FactoriserLaBase = NON_SPX;
      ArretDemande          = OUI_SPX;
    }
  }

	SPX_MettreAJourLesCoutsReduits( Spx );

	if ( Spx->FaireMiseAJourDeBBarre == OUI_SPX ) { 
    SPX_MettreAJourBBarre( Spx ); 
    Spx->CalculerBBarre = NON_SPX;
  }
  else Spx->CalculerBBarre = OUI_SPX;	
	  
  SPX_FaireLeChangementDeBase( Spx );

	/* Apres chaque chagement de base reussi on essaie de revenir au seuil de pivotage initial */
	if ( Spx->SeuilDePivotDual > VALEUR_DE_PIVOT_ACCEPTABLE ) {
    Spx->SeuilDePivotDual /= DIVISEUR_VALEUR_DE_PIVOT_ACCEPTABLE;
	  if ( Spx->SeuilDePivotDual < VALEUR_DE_PIVOT_ACCEPTABLE ) Spx->SeuilDePivotDual = VALEUR_DE_PIVOT_ACCEPTABLE;	
	}
	
  /* Si la factorisation s'est mal passee */
  if ( Spx->FactoriserLaBase == OUI_SPX ) ArretDemande = OUI_SPX;

}

*CoutReduitDeLaVariableBranchee = 0.0;
if ( *YaUneSolution == OUI_SPX ) {
  if ( Spx->PositionDeLaVariable[VariableSortanteSv] != EN_BASE_LIBRE ) {
    *CoutReduitDeLaVariableBranchee = fabs( Spx->CBarre[VariableSortanteSv] );
  }	
}

/* Maintenant on peut recuperer la valeur du critere correspondant a la solution courante ainsi 
   que la base puisqu'elle est duale realisable et qu'elle pourra etre reutilisee lors d'une optimisation 
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
  
Spx->Xmin[VariableSortanteSv] = XminSV;
Spx->Xmax[VariableSortanteSv] = XmaxSV;

Spx->XminEntree[VariableSortanteSv] = XminEntreeSV; 

if( TypeDeSortieSv == SORT_SUR_XMIN ) {
  il    = Spx->Cdeb[VariableSortanteSv];
  ilMax = il + Spx->CNbTerm[VariableSortanteSv];
  while ( il < ilMax ) {
    Spx->B[Spx->NumeroDeContrainte[il]]+= Spx->ACol[il] * XmaxSV;
    il++;
  }
}

#if VERBOSE_SPX
  printf(" Iteration de sortie du strong branching %d\n",Spx->Iteration); fflush(stdout);
#endif

/* On remet les donnees internes du simplexe dans l'etat initial */
il = Spx->NombreDeVariables * sizeof( double );
memcpy( (char *) Spx->X, (char *) Spx->XSV, il );
memcpy( (char *) Spx->CBarre, (char *) Spx->CBarreSV, il );
memcpy( (char *) Spx->C, (char *) Spx->Csv, il );
il = Spx->NombreDeVariables * sizeof( int );
memcpy( (char *) Spx->ContrainteDeLaVariableEnBase, (char *) Spx->ContrainteDeLaVariableEnBaseSV, il );
il = Spx->NombreDeVariables * sizeof( char );
memcpy( (char *) Spx->PositionDeLaVariable, (char *) Spx->PositionDeLaVariableSV, il );
memcpy( (char *) Spx->InDualFramework, (char *) Spx->InDualFrameworkSV, il );

il = Spx->NombreDeContraintes * sizeof( double );
memcpy( (char *) Spx->BBarre, (char *) Spx->BBarreSV, il );
memcpy( (char *) Spx->DualPoids, (char *) Spx->DualPoidsSV, il );
il = Spx->NombreDeContraintes * sizeof( int );
memcpy( (char *) Spx->VariableEnBaseDeLaContrainte, (char *) Spx->VariableEnBaseDeLaContrainteSV, il );
memcpy( (char *) Spx->CdebBase, (char *) Spx->CdebBaseSV, il );
memcpy( (char *) Spx->NbTermesDesColonnesDeLaBase, (char *) Spx->NbTermesDesColonnesDeLaBaseSV, il );

Spx->LastEta = -1;
Spx->NombreDeChangementsDeBase = 0;
Spx->Iteration                 = 0;

return;

}

