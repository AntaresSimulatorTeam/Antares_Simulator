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

   FONCTION: Determination de l'ordre des contraintes pour la factorisation
	           de la partie de la base qui concerne des colonnes a plus
						 d'un terme.

                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"  

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"
   
# define TRACES 0  

# define MAX_RANG_BASE_REDUITE 0.66 /* i.e. 2/3 */

# define COEFF_POUR_RANG_BASE_REDUITE 0.5

# define COEFF_MULT_TOLERANCE_PRIMALE 1.  
    
# define TRAVAILLER_AVEC_LA_BASE_REDUITE OUI_SPX /*OUI_SPX*/

int SPX_PartitionTriRapideViolation( double * , int * , int , int );
void SPX_TriRapideViolations( double * , int * , int , int );

/*----------------------------------------------------------------------------*/

void SPX_TestPassageBaseReduiteBaseComplete( PROBLEME_SPX * Spx )
{
int NombreDInfaisabilites; char TenterBaseReduite; int Seuil1; int Seuil2;

SPX_ControleDesVariablesBasiquesHorsBaseReduite( Spx, &NombreDInfaisabilites );

TenterBaseReduite = OUI_SPX;

Seuil1 = 100;
Seuil2 = 100;

if ( NombreDInfaisabilites >= 0 ) {
  if      ( Spx->NombreDeContraintes < 100000 ) Seuil2 = 0.01 * Spx->NombreDeContraintes;
  else if ( Spx->NombreDeContraintes < 200000 ) Seuil2 = 0.02 * Spx->NombreDeContraintes;
  else if ( Spx->NombreDeContraintes < 300000 ) Seuil2 = 0.03 * Spx->NombreDeContraintes;
  else if ( Spx->NombreDeContraintes < 400000 ) Seuil2 = 0.04 * Spx->NombreDeContraintes;
  else Seuil2 = 0.05 * Spx->NombreDeContraintes;
}
else NombreDInfaisabilites = -NombreDInfaisabilites;

# if TRACES == 1
  printf("NombreDInfaisabilites = %d iteration %d  ModifCoutsAutorisee  %d\n",NombreDInfaisabilites,Spx->Iteration,Spx->ModifCoutsAutorisee);
# endif

if ( NombreDInfaisabilites < 100 || NombreDInfaisabilites < Seuil2 ) {
  if ( NombreDInfaisabilites == 0 ) TenterBaseReduite = NON_SPX;
  else if ( Spx->Iteration > 1000 ) {
	  /* On reprend la matrice complete */  
		TenterBaseReduite = NON_SPX;
	}
}

# if TRACES == 1
  if ( TenterBaseReduite == NON_SPX ) printf("On passe en base complete\n");
# endif

if ( Spx->IterationDeConstructionDeLaBaseReduite > 0 ) {
  /*
  if ( Spx->Iteration - Spx->IterationDeConstructionDeLaBaseReduite > 0.66 * Spx->RangDeLaMatriceFactorisee ) {
    TenterBaseReduite = NON_SPX;
  }
	*/
	
  if ( Spx->Iteration - Spx->IterationDeConstructionDeLaBaseReduite > 0.5 * (Spx->NombreDeContraintes-Spx->RangDeLaMatriceFactorisee) ) {
    TenterBaseReduite = NON_SPX;
  }
		
}

if ( TenterBaseReduite == OUI_SPX ) {
  # if TRACES == 1
     printf("    On refactorise avec possibilite de base reduite \n");						
  #endif
  Spx->InitBaseReduite = OUI_SPX;
  Spx->YaUneSolution = OUI_SPX; /* Car c'est positionne a NON_SPX en cas de probleme */
  SPX_FactoriserLaBase( Spx );
  if ( Spx->YaUneSolution == NON_SPX ) {
    #if VERBOSE_SPX
      printf("Base non inversible ou probleme pour trouver une base duale admissible\n");
    #endif
    return;
	}	
}
else {
  /* La base reduite est optimale et admissible */
  # if TRACES == 1
    printf("    On refactorise la base complete\n");
  #endif

	Spx->UtiliserLaBaseReduite = NON_SPX;
  SPX_InitialiserLeTableauDesVariablesHorsBase( Spx );
	
  Spx->ForcerUtilisationDeLaBaseComplete = Spx->NombreDeBasesCompletesSuccessives;
  Spx->NombreDeBasesCompletesSuccessives++;				
  Spx->YaUneSolution = OUI_SPX; /* Car c'est positionne a NON_SPX en cas de probleme */
  SPX_FactoriserLaBase( Spx );
  if ( Spx->YaUneSolution == NON_SPX ) {
    #if VERBOSE_SPX
      printf("Base non inversible ou probleme pour trouver une base duale admissible\n");
    #endif
    return;
	}
  SPX_InitDualPoids( Spx ); /* Car les poids de la base reduite ne sont plus bons */

	Spx->CoefficientPourLaValeurDePerturbationDeCoutAPosteriori = 1.0;
		
}
Spx->CalculerBBarre = OUI_SPX; 
Spx->CalculerCBarre = OUI_SPX;
return;
}

/*----------------------------------------------------------------------------*/

int SPX_PartitionTriRapideViolation( double * Violation, int * NumeroDeVariable, int Deb, int Fin )
{
int Compt; double Pivot; int i; int Var; double Viol;

Compt = Deb;
Var   = NumeroDeVariable[Deb];
Pivot = Violation[Deb];

/* Ordre decroissant */
for ( i = Deb + 1 ; i <= Fin ; i++) {
  if ( Violation[i] > Pivot) {
    Compt++;		
    Viol = Violation[Compt];
    Violation[Compt] = Violation[i];
    Violation[i] = Viol;		
    Var = NumeroDeVariable[Compt];
    NumeroDeVariable[Compt] = NumeroDeVariable[i];
    NumeroDeVariable[i] = Var;		
  }
}
Viol = Violation[Compt];
Violation[Compt] = Violation[Deb];
Violation[Deb] = Viol;		
Var = NumeroDeVariable[Compt];
NumeroDeVariable[Compt] = NumeroDeVariable[Deb];
NumeroDeVariable[Deb] = Var;
return(Compt);
}

/*----------------------------------------------------------------------------*/

void SPX_TriRapideViolations( double * Violation, int * NumeroDeVariable, int Debut, int Fin )
{
int Pivot;
if ( Debut < Fin ) {
  Pivot = SPX_PartitionTriRapideViolation( Violation, NumeroDeVariable, Debut, Fin );  
  SPX_TriRapideViolations( Violation, NumeroDeVariable, Debut  , Pivot-1 );
  SPX_TriRapideViolations( Violation, NumeroDeVariable, Pivot+1, Fin );
}
return;
}

/*----------------------------------------------------------------------------*/
/*  Initialisation  */
void SPX_ControleDesVariablesBasiquesHorsBaseReduite( PROBLEME_SPX * Spx, int * NombreDInfaisabilites )
{
int * ColonneDeLaBaseFactorisee; int Colonne; int Var; int * VariableEnBaseDeLaContrainte;
double * BBarre; int r; double * SeuilDeViolationDeBorne; char * PositionHorsBaseReduiteAutorisee;
int NbInfaisabilites; char * OrigineDeLaVariable; char * TypeDeVariable; int NbInfMax;
int * NumeroDeVariable; double * Violation; int NombreDeRevalidations; double * C;
int NouvelleDimensionDeLaBaseReduite;

/* Calcul des valeurs pour la partie hors base reduite */
SPX_CalculerBBarreAHorsReduite( Spx );
ColonneDeLaBaseFactorisee = Spx->ColonneDeLaBaseFactorisee;
VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;
PositionHorsBaseReduiteAutorisee = Spx->PositionHorsBaseReduiteAutorisee;
BBarre = Spx->BBarre;
SeuilDeViolationDeBorne = Spx->SeuilDeViolationDeBorne;
OrigineDeLaVariable = Spx->OrigineDeLaVariable;
TypeDeVariable = Spx->TypeDeVariable;
C = Spx->C;

Violation = (double *) malloc( Spx->NombreDeVariables * sizeof( double ) );
NumeroDeVariable = (int *) malloc( Spx->NombreDeVariables * sizeof( int ) );
if ( Violation == NULL || NumeroDeVariable == NULL ) {
  free( Violation ); free( NumeroDeVariable );
	*NombreDInfaisabilites = 0; /* Afin de passer en base complete */
	return;
}

NombreDeRevalidations = 0;
for ( r = 0 ; r < Spx->RangDeLaMatriceFactorisee ; r++ ) {
  Colonne = ColonneDeLaBaseFactorisee[r];
  Var = VariableEnBaseDeLaContrainte[Colonne];	
	if ( OrigineDeLaVariable[Var] == ECART && C[Var] == 0 ) { 
	  if ( PositionHorsBaseReduiteAutorisee[Var] != NON_2_FOIS ) NombreDeRevalidations++;
	}		
}

/* Classement des N premieres violations */
NbInfaisabilites = 0;
for ( r = Spx->RangDeLaMatriceFactorisee ; r < Spx->NombreDeContraintes ; r++ ) {
  Colonne = ColonneDeLaBaseFactorisee[r];
	Var = VariableEnBaseDeLaContrainte[Colonne];
	if ( PositionHorsBaseReduiteAutorisee[Var] == OUI_1_FOIS || PositionHorsBaseReduiteAutorisee[Var] == OUI_2_FOIS ) {
	  if ( BBarre[Colonne] < -(COEFF_MULT_TOLERANCE_PRIMALE * SeuilDeViolationDeBorne[Var]) ) {
			Violation[NbInfaisabilites] = fabs( BBarre[Colonne] );
      NumeroDeVariable[NbInfaisabilites] = Var;
      NbInfaisabilites++;			
	  }
	  else if ( BBarre[Colonne] > Spx->Xmax[Var] + (COEFF_MULT_TOLERANCE_PRIMALE * SeuilDeViolationDeBorne[Var]) ) {
			Violation[NbInfaisabilites] = fabs( BBarre[Colonne] - Spx->Xmax[Var] );
      NumeroDeVariable[NbInfaisabilites] = Var;
      NbInfaisabilites++;					
	  }			
	}
}
*NombreDInfaisabilites = NbInfaisabilites;

SPX_TriRapideViolations( Violation, NumeroDeVariable, 0, NbInfaisabilites - 1 );

NbInfaisabilites = 0;

if      ( Spx->NombreDeContraintes < 100000 ) NbInfMax = 0.01 * Spx->NombreDeContraintes;
else if ( Spx->NombreDeContraintes < 200000 ) NbInfMax = 0.02 * Spx->NombreDeContraintes;
else if ( Spx->NombreDeContraintes < 300000 ) NbInfMax = 0.03 * Spx->NombreDeContraintes;
else if ( Spx->NombreDeContraintes < 400000 ) NbInfMax = 0.04 * Spx->NombreDeContraintes;
else NbInfMax = 0.05 * Spx->NombreDeContraintes;

for ( NbInfaisabilites = 0 ; NbInfaisabilites < *NombreDInfaisabilites && NbInfaisabilites < NbInfMax ; NbInfaisabilites++ ) {	
  Var = NumeroDeVariable[NbInfaisabilites];	
	if ( PositionHorsBaseReduiteAutorisee[Var] == OUI_1_FOIS ) {
		if ( OrigineDeLaVariable[Var] == ECART || TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {
			PositionHorsBaseReduiteAutorisee[Var] = /*NON_2_FOIS*/ NON_1_FOIS;
		}
		else PositionHorsBaseReduiteAutorisee[Var] = NON_2_FOIS;		
	}
	else PositionHorsBaseReduiteAutorisee[Var] = NON_2_FOIS;	
}

if ( *NombreDInfaisabilites <= Spx->NombreDinfaisabilitesSiBaseReduite || Spx->NbEchecsReductionNombreDinfaisabilitesSiBaseReduite < 2 ) {
  /* Seulement si on arrive a faire baisser le nombre d'infaisabilites */
  /* Tant que la nouvelle dimension de la base reduite reste petite, on arrete pas le princing dual partiel */
  if ( *NombreDInfaisabilites > 100 ) {
    NouvelleDimensionDeLaBaseReduite = Spx->RangDeLaMatriceFactorisee + NbInfaisabilites - NombreDeRevalidations;
    if ( NouvelleDimensionDeLaBaseReduite < COEFF_POUR_RANG_BASE_REDUITE * Spx->NombreDeContraintes ) {
      *NombreDInfaisabilites = -(*NombreDInfaisabilites);
    }
  }
}
if ( fabs( *NombreDInfaisabilites ) > Spx->NombreDinfaisabilitesSiBaseReduite ) Spx->NbEchecsReductionNombreDinfaisabilitesSiBaseReduite++;

Spx->NombreDinfaisabilitesSiBaseReduite = fabs( *NombreDInfaisabilites );

free( NumeroDeVariable );
free( Violation );
return;
}

/*----------------------------------------------------------------------------*/
/*  Initialisation  */
void SPX_OrdonnerLesContraintesPourLaBase( PROBLEME_SPX * Spx )
{
int NombreDeContraintes; int * OrdreColonneDeLaBaseFactorisee; int * ColonneDeLaBaseFactorisee;
int Colonne; int * OrdreLigneDeLaBaseFactorisee; int * LigneDeLaBaseFactorisee; int MaxRang;
int Var; char * OrigineDeLaVariable; int Ligne; int i; int RangDeLaMatriceFactorisee;
int OrdreNonNative; int * Cdeb; int * NumeroDeContrainte; int AncienOrdreDeLigne; int DerniereLigne;
int * VariableEnBaseDeLaContrainte;	int * IndexDansContrainteASurveiller; int NombreDeContraintesASurveiller;
int Iteration; double * BBarre; int r; char VariableDansBaseReduite; char * PositionDeLaVariable;
int NombreDeVariablesDecartBasiques; char * PositionHorsBaseReduiteAutorisee; double * C; 
char ControlerAdmissibiliteDuale; char * CorrectionDuale; int * CNbTerm; char * TypeDeVariable;
 
BBarre = Spx->BBarre;		
VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;
PositionDeLaVariable = Spx->PositionDeLaVariable;
C = Spx->C;

# if TRAVAILLER_AVEC_LA_BASE_REDUITE == NON_SPX
  Spx->RangDeLaMatriceFactorisee = Spx->NombreDeContraintes;
  Spx->UtiliserLaBaseReduite = NON_SPX;
  Spx->UtiliserLaLuUpdate = OUI_SPX;   
  return;
# endif

if ( Spx->Iteration > Spx->NombreDeContraintes && 0 ) {
  Spx->RangDeLaMatriceFactorisee = NombreDeContraintes;
	if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) SPX_InitDualPoids( Spx );	
  Spx->UtiliserLaBaseReduite = NON_SPX;
  Spx->UtiliserLaLuUpdate = OUI_SPX;   
  return;	
}

if ( Spx->InitBaseReduite == NON_SPX ) {	
  return;
}

NombreDeContraintes = Spx->NombreDeContraintes;
OrigineDeLaVariable = Spx->OrigineDeLaVariable;
			
MaxRang = (int) ( (ceil) (MAX_RANG_BASE_REDUITE * NombreDeContraintes) );

NombreDeVariablesDecartBasiques = 0;
for ( Var = Spx->NombreDeVariablesNatives ; Var < Spx->NombreDeVariables ; Var++ ) {
  if ( PositionDeLaVariable[Var] == EN_BASE_LIBRE ) {
	  if ( OrigineDeLaVariable[Var] == ECART ) NombreDeVariablesDecartBasiques++;
	}
}

if ( NombreDeContraintes - NombreDeVariablesDecartBasiques > MaxRang ) {
  Spx->RangDeLaMatriceFactorisee = NombreDeContraintes;
	if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) SPX_InitDualPoids( Spx );
  Spx->UtiliserLaBaseReduite = NON_SPX;
  Spx->UtiliserLaLuUpdate = OUI_SPX;	
  return;	
}

/* Prevoir une exclusion d'emblee basee sur MaxRang s'il n'y a pas assez de variables d'ecart */
TypeDeVariable = Spx->TypeDeVariable;
Cdeb = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;
NumeroDeContrainte = Spx->NumeroDeContrainte;

IndexDansContrainteASurveiller = Spx->IndexDansContrainteASurveiller;
NombreDeContraintesASurveiller = Spx->NombreDeContraintesASurveiller;
Iteration = Spx->Iteration;

OrdreColonneDeLaBaseFactorisee = Spx->OrdreColonneDeLaBaseFactorisee;
ColonneDeLaBaseFactorisee = Spx->ColonneDeLaBaseFactorisee;

OrdreLigneDeLaBaseFactorisee = Spx->OrdreLigneDeLaBaseFactorisee;
LigneDeLaBaseFactorisee = Spx->LigneDeLaBaseFactorisee;

PositionHorsBaseReduiteAutorisee = Spx->PositionHorsBaseReduiteAutorisee;

/* Permutation des colonnes */
/* Permutation des lignes. On utilise le fait que les variables d'ecart et artificielles sont classees
   a la fin de la contrainte dans le stockage par ligne */	 

RangDeLaMatriceFactorisee = 0;
OrdreNonNative = NombreDeContraintes - 1;

for ( i = 0 ; i < NombreDeContraintes ; i++ ) {
  OrdreLigneDeLaBaseFactorisee[i] = i;
	LigneDeLaBaseFactorisee[i] = i;
}

CorrectionDuale = Spx->CorrectionDuale;
ControlerAdmissibiliteDuale = NON_SPX;   
 
for ( Colonne = 0 ; Colonne < NombreDeContraintes ; Colonne++ ) {

  Var = VariableEnBaseDeLaContrainte[Colonne];
	
	VariableDansBaseReduite = NON_SPX;
	if ( OrigineDeLaVariable[Var] != ECART ) { 
    /* On regarde si la variable peut faire office de variable d'ecart */	  
    if ( CNbTerm[Var] == 1 && C[Var] == 0 /*&& OrigineDeLaVariable[Var] != BASIQUE_ARTIFICIELLE*/ ) {
	    if ( PositionHorsBaseReduiteAutorisee[Var] == NON_1_FOIS || PositionHorsBaseReduiteAutorisee[Var] == NON_2_FOIS  ) VariableDansBaseReduite = OUI_SPX;			
	    else {
			  if ( PositionHorsBaseReduiteAutorisee[Var] == OUI_1_FOIS ) VariableDansBaseReduite = NON_SPX;
				else if ( PositionHorsBaseReduiteAutorisee[Var] == OUI_2_FOIS ) {
	        if ( Spx->BBarre[Colonne] > -Spx->SeuilDeViolationDeBorne[Var] &&
					     Spx->BBarre[Colonne] < Spx->Xmax[Var] + Spx->SeuilDeViolationDeBorne[Var] ) VariableDansBaseReduite = NON_SPX;
          else VariableDansBaseReduite = OUI_SPX;					
				}
			}
	    if ( PositionHorsBaseReduiteAutorisee[Var] == NON_1_FOIS ) PositionHorsBaseReduiteAutorisee[Var] = OUI_2_FOIS;	    			  
	  }  
	  else VariableDansBaseReduite = OUI_SPX;				
  }
	else {
	  /* C'est une variable d'ecart */
	  if ( PositionHorsBaseReduiteAutorisee[Var] == NON_1_FOIS || PositionHorsBaseReduiteAutorisee[Var] == NON_2_FOIS || C[Var] != 0.0 ) VariableDansBaseReduite = OUI_SPX;
	  if ( PositionHorsBaseReduiteAutorisee[Var] == NON_1_FOIS && C[Var] == 0 ) PositionHorsBaseReduiteAutorisee[Var] = OUI_2_FOIS;
	}	
	
	/* Si la variable est native ou violee on la met dans la partie reduite pour qu'elle puisse sortir
	   de la base si necessaire */
	if ( VariableDansBaseReduite == OUI_SPX ) {	
	  OrdreColonneDeLaBaseFactorisee[Colonne] = RangDeLaMatriceFactorisee;		
	  ColonneDeLaBaseFactorisee[RangDeLaMatriceFactorisee] = Colonne;			
		RangDeLaMatriceFactorisee++;		
	}
	else {		
	  /* Variable non native (ecart ou basique artificielle) */
	  OrdreColonneDeLaBaseFactorisee[Colonne] = OrdreNonNative;
    ColonneDeLaBaseFactorisee[OrdreNonNative] = Colonne;

    DerniereLigne = LigneDeLaBaseFactorisee[OrdreNonNative];
		
		Ligne = NumeroDeContrainte[Cdeb[Var]];
    AncienOrdreDeLigne = OrdreLigneDeLaBaseFactorisee[Ligne];
		
    LigneDeLaBaseFactorisee[OrdreNonNative] = Ligne;
	  OrdreLigneDeLaBaseFactorisee[Ligne] = OrdreNonNative;
		
    OrdreLigneDeLaBaseFactorisee[DerniereLigne] = AncienOrdreDeLigne;
    LigneDeLaBaseFactorisee[AncienOrdreDeLigne] = DerniereLigne;

    /* Test: la contrainte est negligee. Si a la fin elle est violee on elle passera a NON_2_FOIS et ne sera plus
		         jamais relaxee */
    /*PositionHorsBaseReduiteAutorisee[Var] = OUI_1_FOIS;*/					 
    /* Fin test */		
						
		OrdreNonNative--;		
		
	}
}

/* Il faut reinitialiser les poids car les contraintes ont pu changer */
SPX_InitDualPoids( Spx );

SPX_InitialiserLesIndicateursHyperCreux( Spx );

Spx->RangDeLaMatriceFactorisee = RangDeLaMatriceFactorisee;
Spx->UtiliserLaBaseReduite = OUI_SPX;
Spx->UtiliserLaLuUpdate = OUI_SPX;
Spx->NombreDeBasesReduitesSuccessives--;

Spx->InitBaseReduite = NON_SPX;

if ( Spx->RangDeLaMatriceFactorisee > MaxRang ) {
  Spx->RangDeLaMatriceFactorisee = NombreDeContraintes;
  Spx->UtiliserLaBaseReduite = NON_SPX;
  Spx->UtiliserLaLuUpdate = OUI_SPX;
  return;	
}

/* Construction du stockge de la transposee de la matrice reduite */
SPX_ConstructionDeLaMatriceReduite( Spx );

SPX_InitialiserLeTableauDesVariablesHorsBase( Spx );

Spx->IterationDeConstructionDeLaBaseReduite = Spx->Iteration;

/* Il est indispensable que les couts des variables d'ecart basiques hors base reduite soient nuls car on ne recalcule pas 
   la variable duale des contraintes associees puisqu'on suppose qu'elles sont nulles */
   
/* Pas certain que ce soit utile */
for ( r = RangDeLaMatriceFactorisee ; r < NombreDeContraintes ; r++ ) {
  BBarre[r] = 0;	
}

/*printf("\nRangDeLaMatriceFactorisee %d  NombreDeContraintes %d Iteration %d\n\n",Spx->RangDeLaMatriceFactorisee,NombreDeContraintes,Spx->Iteration);*/

# if TRACES == 1
  printf("SPX_OrdonnerLesContraintesPourLaBase: RangDeLaMatriceFactorisee = %d\n",Spx->RangDeLaMatriceFactorisee);
# endif

return;
}

/*----------------------------------------------------------------------------*/

