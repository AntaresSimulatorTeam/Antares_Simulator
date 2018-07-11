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

   FONCTION: Calcul de NBarre pour la variable sortante
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "pne_fonctions.h"

# define RANDOM_MIN 1.e-10
# define RANDOM_MAX 1 /*0.5*/ /*1.e-7*/

# define COEFF_MULT_DE_LA_PERTURBATION 10.

# define SEUIL_DE_BRUITAGE_FIXE NON_SPX
# define SEUIL_DE_BRUITAGE (10.*SEUIL_ADMISSIBILITE_DUALE_2)

# define TENIR_COMPTE_DES_BORNES_POUR_LA_VALEUR_DE_BRUITAGE OUI_SPX
# define VARIATION_MAX_DU_COUT_SUR_CHANGEMENT_DE_BORNE_1 /*50*/ 100
# define VARIATION_MIN_DU_COUT_SUR_CHANGEMENT_DE_BORNE_1 1

# define VARIATION_MAX_DU_COUT_SUR_CHANGEMENT_DE_BORNE_2 1  

# define BORNE_AUXILAIRE 1000.

# define BRUITAGE_SUR_VARIABLES_BORNEES_DES_DEUX_COTES OUI_SPX

# define EPS_HARRIS_MAX SEUIL_ADMISSIBILITE_DUALE_2 /*(2*SEUIL_ADMISSIBILITE_DUALE_2)*/

/*----------------------------------------------------------------------------*/
void SPX_TenterRestaurationCalculErBMoinsEnHyperCreux( PROBLEME_SPX * Spx )
{
int Cnt; double * ErBMoinsUn; int Count;
	
if ( Spx->CountEchecsErBMoins == 0 ) {
  if ( Spx->Iteration % CYCLE_TENTATIVE_HYPER_CREUX == 0 ) {
		Spx->NbEchecsErBMoins    = SEUIL_REUSSITE_CREUX;
		Spx->CountEchecsErBMoins = SEUIL_REUSSITE_CREUX + 2;
	}
}
if ( Spx->CountEchecsErBMoins == 0 ) return;

Spx->CountEchecsErBMoins--;		
/* On compte le nombre de termes non nuls du resultat */
ErBMoinsUn = Spx->ErBMoinsUn;
Count = 0;
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) if ( ErBMoinsUn[Cnt] != 0.0 ) Count++;
if ( Count < 0.1 * Spx->NombreDeContraintes ) Spx->NbEchecsErBMoins--;
if ( Spx->NbEchecsErBMoins <= 0 ) {
  # if VERBOSE_SPX
    printf("Remise en service de l'hyper creux pour le calcul de la ligne pivot, iteration %d\n",Spx->Iteration);
	# endif
  Spx->AvertissementsEchecsErBMoins = 0;
  Spx->CountEchecsErBMoins = 0;
	Spx->CalculErBMoinsUnEnHyperCreux = OUI_SPX;
}
else if ( Spx->CountEchecsErBMoins <= 0 ) {
  Spx->CountEchecsErBMoins = 0;
  if ( Spx->CalculErBMoinsUnEnHyperCreux == NON_SPX ) Spx->AvertissementsEchecsErBMoins++;
  if ( Spx->AvertissementsEchecsErBMoins >= SEUIL_ABANDON_HYPER_CREUX ) {
    # if VERBOSE_SPX
      printf("Arret prolonge de l'hyper creux pour le calcul de la ligne pivot, iteration %d\n",Spx->Iteration);
	  # endif
		Spx->CalculErBMoinsEnHyperCreuxPossible = NON_SPX;
	}
}

return;
}
/*----------------------------------------------------------------------------*/

void SPX_DualCalculerNBarreR( PROBLEME_SPX * Spx, char CalculDeCBarreSurNBarre, char * ControlerAdmissibiliteDuale )
{
int Var; double S; char PositionDeLaVariable_x; double X; int i; char * PositionDeLaVariable; 
double * NBarreR; int SortSurXmaxOuSurXmin; double * C; double * CBarre; char CalculEnHyperCreux;
double * CBarreSurNBarreR; double * CBarreSurNBarreRAvecTolerance; int * NumeroDesVariableATester;
char * CorrectionDuale; char FaireTestDegenerescence; double NBarreR_x; int j; double SeuilDePivot;
double UnSurNBarreR; int iLimite; int * NumerosDesVariables; double EpsHarris; int NbDeg0; int NbDeg;
double MinAbsCBarreNonDeg; double Delta; char ToutModifier; int Iter; double SeuilModifCout;
double * SeuilDAmissibiliteDuale; double Alpha; double * Csv; int * CNbTerm; double Random;
double B1; double B2; char * TypeDeVariable; double * Xmax; double * Xmin; double Xmx;
double PerturbationMax; double EpsHarrisMax;

Spx->AdmissibilitePossible = NON_SPX;
*ControlerAdmissibiliteDuale = NON_SPX;

if ( Spx->TypeDeCreuxDeLaBase == BASE_HYPER_CREUSE && Spx->CalculErBMoinsUnEnHyperCreux == OUI_SPX &&
     Spx->FaireDuRaffinementIteratif <= 0 ) { 
  CalculEnHyperCreux = OUI_SPX;
  Spx->TypeDeStockageDeErBMoinsUn = COMPACT_SPX;	
}
else {
	CalculEnHyperCreux = NON_SPX;
  Spx->TypeDeStockageDeErBMoinsUn = VECTEUR_SPX;	
}

SPX_CalculerErBMoins1( Spx, CalculEnHyperCreux );

/* Si on est pas en hyper creux, on essaie d'y revenir */
if ( Spx->CalculErBMoinsUnEnHyperCreux == NON_SPX ) {
  if ( Spx->CalculErBMoinsEnHyperCreuxPossible == OUI_SPX ) {
    SPX_TenterRestaurationCalculErBMoinsEnHyperCreux( Spx );
	}
}   

if ( Spx->TypeDeStockageDeErBMoinsUn == COMPACT_SPX ) { 
  SPX_DualCalculerNBarreRHyperCreux( Spx );	
}
else {
  SPX_DualCalculerNBarreRStandard( Spx );  
}

if ( CalculDeCBarreSurNBarre == NON_SPX ) return;
  
SeuilDePivot = Spx->SeuilDePivotDual;

PositionDeLaVariable   = Spx->PositionDeLaVariable;  
NBarreR                = Spx->NBarreR;   
SortSurXmaxOuSurXmin   = Spx->SortSurXmaxOuSurXmin;

C      = Spx->C;
Csv    = Spx->Csv;
CBarre = Spx->CBarre;

CNbTerm = Spx->CNbTerm;

CBarreSurNBarreR              = Spx->CBarreSurNBarreR;
CBarreSurNBarreRAvecTolerance = Spx->CBarreSurNBarreRAvecTolerance;
NumeroDesVariableATester      = Spx->NumeroDesVariableATester;

CorrectionDuale = Spx->CorrectionDuale;

FaireTestDegenerescence = NON_SPX;
if ( Spx->NbCyclesSansControleDeDegenerescence >= Spx->CycleDeControleDeDegenerescence ) {
  Spx->NbCyclesSansControleDeDegenerescence = 0;
  if ( Spx->ModifCoutsAutorisee == OUI_SPX ) FaireTestDegenerescence = OUI_SPX;
}
else {
  Spx->NbCyclesSansControleDeDegenerescence++;
}

/***************************** Preparation du test du ratio ***********************************/
Iter = 1;

SeuilDAmissibiliteDuale = Spx->SeuilDAmissibiliteDuale1;

Spx->NombreDeVariablesATester = 0;

j = 0;

if ( Spx->TypeDeStockageDeNBarreR == VECTEUR_SPX ) {
  iLimite = Spx->NombreDeVariablesHorsBase;
	NumerosDesVariables = Spx->NumerosDesVariablesHorsBase;
}
else {
  iLimite = Spx->NombreDeValeursNonNullesDeNBarreR;
	NumerosDesVariables = Spx->NumVarNBarreRNonNul;
}

TypeDeVariable = Spx->TypeDeVariable;

/* Indicateurs utilises pour la degenerescence */
NbDeg0 = 0;
NbDeg  = 0;
MinAbsCBarreNonDeg = LINFINI_SPX;

EpsHarrisMax = EPS_HARRIS_MAX;

for ( i = 0 ; i < iLimite ; i++ ) {
   
  Var = NumerosDesVariables[i];	
  if ( fabs( NBarreR[Var] ) < SeuilDePivot ) continue; 
    
  NBarreR_x = NBarreR[Var];
  PositionDeLaVariable_x = PositionDeLaVariable[Var];
  /* Classement des CBarreSurNBarreR qui vont servir au choix de la variables
     entrante dans l'algorithme dual.

  Si la variable sortante sort sur XMIN
  ------------------------------------- 
  Cas 1 - la variable hors base est sur BORNE INF: 
          alors son CBarre est >= 0 
          on est interesses que par les valeurs de NBarreR < 0 
          le rapport CBarre / NBarreR est <= 0
  Cas 2 - la variable hors base est sur BORNE SUP:
          alors son CBarre est <= 0 
          on est interesses que par les valeurs de NBarreR > 0 
          le rapport CBarre / NBarreR est <= 0
  Cas 3 - la variable hors base est sur HORS_BASE_A_ZERO (i.e. non bornée):
          alors son CBarre est = 0 quel que soit le signe de NBarreR,
					la variable devra entrer en base 
                                                 
  Si la variable sortante sort sur XMAX
  ------------------------------------- 
  Cas 1 - la variable hors base est sur BORNE INF: 
          alors son CBarre est >= 0 
          on est interesses que par les valeurs de NBarreR > 0 
          les rapport CBarre / NBarreR est >= 0
  Cas 2 - la variable hors base est sur BORNE SUP:
          alors son CBarre est <= 0 
          on est interesses que par les valeurs de NBarreR < 0 
          le rapport CBarre / NBarreR est >= 0
  Cas 3 - la variable hors base est sur HORS_BASE_A_ZERO (i.e. non bornée):
          alors son CBarre est = 0 quel que soit le signe de NBarreR,
					la variable devra entrer en base 
  */
  if ( SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) {
    /* Pour les variables autres que non bornees on est interesses que par les rapports negatifs */
    if ( PositionDeLaVariable_x == HORS_BASE_SUR_BORNE_INF ) {
      /* Si la variable est HORS_BASE_SUR_BORNE_INF et si NBarreR est positif, la variable ne 
         quittera pas la base */
		  if ( NBarreR_x > 0.0 ) continue;
		}		
    else if ( PositionDeLaVariable_x == HORS_BASE_SUR_BORNE_SUP ) {
	    /* Si la variable est HORS_BASE_SUR_BORNE_SUP et si NBarreR est negatif, la variable ne 
         quittera pas la base */
		  if ( NBarreR_x < 0.0 ) continue;
		}
	  /* Si on arrive la c'est que c'est une variable non bornee */
  }
  else { /* La variable sort sur XMAX */
    /* Pour les variables autres que non bornees on est interesses que par les rapports positifs */
    if ( PositionDeLaVariable_x == HORS_BASE_SUR_BORNE_INF ) {
      /* Si la variable est HORS_BASE_SUR_BORNE_INF et si NBarreR est negatif, la variable ne 
         quittera pas la base */
		  if ( NBarreR_x < 0.0 ) continue;
		}	
    else if ( PositionDeLaVariable_x == HORS_BASE_SUR_BORNE_SUP ) {
      /* Si la variable est HORS_BASE_SUR_BORNE_INF et si NBarreR est positif, la variable ne 
         quittera pas la base */
		  if ( NBarreR_x > 0.0 ) continue;
		}
	  /* Si on arrive la c'est que c'est une variable non bornee */
  }

  /* Calcul de CBarre sur NBarreR */
  UnSurNBarreR = 1. / NBarreR_x;	
  X = CBarre[Var] * UnSurNBarreR;
	
  if ( COEFF_TOLERANCE_POUR_LE_TEST_DE_HARRIS * SeuilDAmissibiliteDuale[Var] > EpsHarrisMax ) EpsHarris = EpsHarrisMax * UnSurNBarreR;
  else 	EpsHarris = COEFF_TOLERANCE_POUR_LE_TEST_DE_HARRIS * SeuilDAmissibiliteDuale[Var] * UnSurNBarreR;	
	
  if ( PositionDeLaVariable_x == HORS_BASE_SUR_BORNE_INF ) {		
    /* Le cout reduit est positif (sauf s'il y a des infaisabilites duales) */				
    if ( SortSurXmaxOuSurXmin != SORT_SUR_XMIN ) {		  
      CBarreSurNBarreR             [j] = X;
      CBarreSurNBarreRAvecTolerance[j] = X + EpsHarris;			
    }
		else {		  
      CBarreSurNBarreR             [j] = -X;
      CBarreSurNBarreRAvecTolerance[j] = -(X + EpsHarris);			 			
		}		
    NumeroDesVariableATester[j] = i;				
    j++;

    if ( FaireTestDegenerescence == OUI_SPX ) {
		  # if SEUIL_DE_BRUITAGE_FIXE == OUI_SPX
			  SeuilModifCout = SEUIL_DE_BRUITAGE;				     
			# else
			  SeuilModifCout = COEFF_SEUIL_POUR_MODIFICATION_DE_COUT *  SeuilDAmissibiliteDuale[Var];				     
			# endif

      # if BRUITAGE_SUR_VARIABLES_BORNEES_DES_DEUX_COTES == OUI_SPX			
	      if ( CBarre[Var] < SeuilModifCout ) {
			    NbDeg0++;
	        if ( CorrectionDuale[Var] != 0 ) NbDeg++;
		    }
		    else if ( fabs( CBarre[Var] ) < MinAbsCBarreNonDeg ) MinAbsCBarreNonDeg = fabs( CBarre[Var] );
			# else
				if ( TypeDeVariable[Var] != BORNEE ) {
	        if ( CBarre[Var] < SeuilModifCout ) {
			      NbDeg0++;
	          if ( CorrectionDuale[Var] != 0 ) NbDeg++;
		      }
		      else if ( fabs( CBarre[Var] ) < MinAbsCBarreNonDeg ) MinAbsCBarreNonDeg = fabs( CBarre[Var] );				
				}			
			# endif
			
	  }
		
  }
  else if ( PositionDeLaVariable_x == HORS_BASE_SUR_BORNE_SUP ) {	
    /* Le cout reduit est negatif (sauf s'il y a des infaisabilites duales) */		
    if ( SortSurXmaxOuSurXmin != SORT_SUR_XMIN ) {		  
      CBarreSurNBarreR             [j] = X;
      CBarreSurNBarreRAvecTolerance[j] = X - EpsHarris;									
		}
		else {		  
      CBarreSurNBarreR[j] = -X;	
      CBarreSurNBarreRAvecTolerance[j] = -(X - EpsHarris);			    			
		}		
    NumeroDesVariableATester[j] = i; 		
    j++;

	  if ( FaireTestDegenerescence  == OUI_SPX ) {
		  # if SEUIL_DE_BRUITAGE_FIXE == OUI_SPX
			  SeuilModifCout = SEUIL_DE_BRUITAGE;				     	
			# else		
			  SeuilModifCout = COEFF_SEUIL_POUR_MODIFICATION_DE_COUT *  SeuilDAmissibiliteDuale[Var];				     
			# endif

      # if BRUITAGE_SUR_VARIABLES_BORNEES_DES_DEUX_COTES == OUI_SPX					
	      if ( CBarre[Var] > -SeuilModifCout ) {
			    NbDeg0++;
	        if ( CorrectionDuale[Var] != 0 ) NbDeg++;
		    }
		    else if ( fabs( CBarre[Var] ) < MinAbsCBarreNonDeg ) MinAbsCBarreNonDeg = fabs( CBarre[Var] );
			# else
				if ( TypeDeVariable[Var] != BORNEE ) {
	        if ( CBarre[Var] > -SeuilModifCout ) {
			      NbDeg0++;
	          if ( CorrectionDuale[Var] != 0 ) NbDeg++;
		      }
		      else if ( fabs( CBarre[Var] ) < MinAbsCBarreNonDeg ) MinAbsCBarreNonDeg = fabs( CBarre[Var] );							
        }
			# endif
			
	  }
		
  }
  else {
    /* On est dans la cas d'une variable non bornee */      
    /* Comme on fait entrer en priorite les variables non bornees, des qu'on en trouve une 
       on arrete */		
    CBarreSurNBarreR             [0] = 0.;
    CBarreSurNBarreRAvecTolerance[0] = 0.;
    NumeroDesVariableATester     [0] = i;		   
		Spx->NombreDeVariablesATester = 1;		
    return;	  				
  }
		
}

Spx->NombreDeVariablesATester = j;

if ( FaireTestDegenerescence == NON_SPX ) return;

/* S'il y a risque de degenerescence on modifie les couts */

ToutModifier = NON_SPX;
if ( NbDeg == 0 ) {	
  if ( NbDeg0 == 0 ) return;  
	else ToutModifier = OUI_SPX;
}


if ( NbDeg == 0 ) return;

/* Rq PerturbationMax peut etre caclule une fois pour toutes */
PerturbationMax = Spx->PerturbationMax;
if ( PerturbationMax > VARIATION_MAX_DU_COUT_SUR_CHANGEMENT_DE_BORNE_1 ) PerturbationMax = VARIATION_MAX_DU_COUT_SUR_CHANGEMENT_DE_BORNE_1;
else if ( PerturbationMax > VARIATION_MIN_DU_COUT_SUR_CHANGEMENT_DE_BORNE_1 ) PerturbationMax = VARIATION_MIN_DU_COUT_SUR_CHANGEMENT_DE_BORNE_1;

TypeDeVariable = Spx->TypeDeVariable;
Xmax = Spx->Xmax;
Xmin = Spx->Xmin;

Spx->LesCoutsOntEteModifies = OUI_SPX;

S = Spx->CoefficientPourLaValeurDePerturbationDeCoutAPosteriori * COEFF_MULT_DE_LA_PERTURBATION * VALEUR_PERTURBATION_COUT_A_POSTERIORI;

if ( ToutModifier == NON_SPX ) {
  Delta = S / (double) NbDeg;
  /* Plus grande des corrections inferieure a un seuil */   
  if ( NbDeg * Delta > MinAbsCBarreNonDeg ) Delta = MinAbsCBarreNonDeg / NbDeg;
  /* Correction pas trop petite quand-meme */
  if ( NbDeg * Delta < SEUIL_ADMISSIBILITE_DUALE_2 ) Delta = SEUIL_ADMISSIBILITE_DUALE_2 / NbDeg;		
}
else {
  Delta = S / (double) NbDeg0;
  /* Plus grande des corrections inferieure a un seuil */  
  if ( NbDeg0 * Delta > MinAbsCBarreNonDeg ) Delta = MinAbsCBarreNonDeg / NbDeg0;
  /* Correction pas trop petite quand-meme */
  if ( NbDeg0 * Delta < SEUIL_ADMISSIBILITE_DUALE_2 ) Delta = SEUIL_ADMISSIBILITE_DUALE_2 / NbDeg0;	
}

S = Delta;
Spx->A1 = PNE_Rand( Spx->A1 ); /* Nombre aleatoire entre 0 et 1 */
Random = RANDOM_MIN;
Random += Spx->A1 * ( RANDOM_MAX - RANDOM_MIN );
Alpha = 1. + Random;

/* Maintenant on modifie les couts */
B1 = 0.1 * SEUIL_ADMISSIBILITE_DUALE_1;
B2 = 0.1 * SEUIL_ADMISSIBILITE_DUALE_1;

for ( j = 0 ; j < Spx->NombreDeVariablesATester ; j++ ) {

  Var = NumerosDesVariables[NumeroDesVariableATester[j]];

  # if BRUITAGE_SUR_VARIABLES_BORNEES_DES_DEUX_COTES == NON_SPX					
	  if ( TypeDeVariable[Var] == BORNEE ) continue;
  # endif
				
	# if SEUIL_DE_BRUITAGE_FIXE == OUI_SPX
    SeuilModifCout = SEUIL_DE_BRUITAGE;
  # else
    SeuilModifCout = COEFF_SEUIL_POUR_MODIFICATION_DE_COUT * SeuilDAmissibiliteDuale[Var];
  # endif
	
  if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF ) {
	  if ( CBarre[Var] < SeuilModifCout ) {

		  if ( ToutModifier == NON_SPX ) {  
		    if ( CorrectionDuale[Var] == 0 ) {  
			    /* Rendre au minimum dual admissible */
					/* Si la correction demandee tend a revenir vers le cout initial on l'accepte afin retablir le cout initial */
				  /*
					if ( CBarre[Var] < 0 ) {
            X = -CBarre[Var];
					  X += B1 * Alpha;
						if ( Csv[Var] > C[Var] ) {
						  if ( C[Var] + X > Csv[Var] ) X = Csv[Var] - C[Var];													
              C[Var] += X; 
              CBarre[Var] += X;
						}
				  }
					*/
			    continue;
				}
			}			

			/* Rend au minimum dual admissible sauf si la variable est bornee car il suffira de la changer de borne
				 pour la rendre dual admissible */			
			if ( CBarre[Var] < 0 && TypeDeVariable[Var] != BORNEE ) {
			  X = -CBarre[Var];
				if ( X > S ) {
				  S = X;				
			  	/* Comme on arriverait sur 0 on repousse un peu plus */
				  S += B2;
				}				
			}

      # if TENIR_COMPTE_DES_BORNES_POUR_LA_VALEUR_DE_BRUITAGE == OUI_SPX
        /* Si la variable est bornee on limite le bruitage */
				if ( TypeDeVariable[Var] == BORNEE ) Xmx = Xmax[Var];
				else Xmx = BORNE_AUXILAIRE;				
				if ( CorrectionDuale[Var] != 0 || 1 ) {				
          if ( S * Alpha * ( Xmx - Xmin[Var] ) > PerturbationMax ) {					
            S = PerturbationMax / ( Alpha * ( Xmx - Xmin[Var] ) );						
					}          					
				}
				else if ( S * Alpha * ( Xmx - Xmin[Var] ) > VARIATION_MAX_DU_COUT_SUR_CHANGEMENT_DE_BORNE_2 ) {				
          S = VARIATION_MAX_DU_COUT_SUR_CHANGEMENT_DE_BORNE_2 / ( Alpha * ( Xmx - Xmin[Var] ) );						
				}				
      # endif
						
      C[Var] += S * Alpha; 
      CBarre[Var] += S * Alpha;

		  CorrectionDuale[Var] -= 1;				
	    if ( CorrectionDuale[Var] <= 0 ) CorrectionDuale[Var] = 0;			
			S += Delta;	
						
			X = CBarre[Var] / NBarreR[Var];
			
      if ( COEFF_TOLERANCE_POUR_LE_TEST_DE_HARRIS * SeuilDAmissibiliteDuale[Var] > EpsHarrisMax ) EpsHarris = EpsHarrisMax  / NBarreR[Var];
      else 	EpsHarris = COEFF_TOLERANCE_POUR_LE_TEST_DE_HARRIS * SeuilDAmissibiliteDuale[Var] / NBarreR[Var];
	
      if ( SortSurXmaxOuSurXmin != SORT_SUR_XMIN ) {			
        CBarreSurNBarreR             [j] = X;
        CBarreSurNBarreRAvecTolerance[j] = X + EpsHarris;
      }
		  else {
        CBarreSurNBarreR             [j] = -X;
        CBarreSurNBarreRAvecTolerance[j] = -(X + EpsHarris);
		  }					
		}
	}
	else if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) {
	  if ( CBarre[Var] > -SeuilModifCout ) {
				
		  if ( ToutModifier == NON_SPX ) {				
		    if ( CorrectionDuale[Var] == 0 ) {
			    /* Rendre au minimum dual admissible */
					/* Si la correction demandee tend a revenir vers le cout initial on l'accepte afin retablir le cout initial */
	        /*
					if ( CBarre[Var] > 0 ) {
            X = CBarre[Var];
					  X += B1 * Alpha;
						if ( Csv[Var] < C[Var] ) {
              if ( C[Var] - X < Csv[Var] ) X = C[Var] - Csv[Var];																			
              C[Var] -= X;  
              CBarre[Var] -= X;
						}
			    }
					*/
			    continue;
			  }
			}
			
			/* Rend au minimum dual admissible */
			if ( CBarre[Var] > 0 ) {
			  X = CBarre[Var];
				if ( X > S ) {
				  S = X;
				  /* Comme on arriverait sur 0 on repousse un peu plus */
				  S += B2;
				}				
			}

      # if TENIR_COMPTE_DES_BORNES_POUR_LA_VALEUR_DE_BRUITAGE == OUI_SPX
        /* On limite le bruitage */
				if ( TypeDeVariable[Var] == BORNEE ) Xmx = Xmax[Var];
				else Xmx = BORNE_AUXILAIRE;				
				if ( CorrectionDuale[Var] != 0 || 1 ) {				
          if ( S * Alpha * ( Xmx - Xmin[Var] ) > PerturbationMax ) {					
            S = PerturbationMax / ( Alpha * ( Xmx - Xmin[Var] ) );						
					}
				}
				else if ( S * Alpha * ( Xmx - Xmin[Var] ) > VARIATION_MAX_DU_COUT_SUR_CHANGEMENT_DE_BORNE_2 ) {					
          S = VARIATION_MAX_DU_COUT_SUR_CHANGEMENT_DE_BORNE_2 / ( Alpha * ( Xmx - Xmin[Var] ) );											    
				}				        			
      # endif			
												
      C[Var] -= S * Alpha;  
      CBarre[Var] -= S * Alpha;

		  CorrectionDuale[Var] -= 1;				
	    if ( CorrectionDuale[Var] <= 0 ) CorrectionDuale[Var] = 0;						
			S += Delta;
			
			X = CBarre[Var] / NBarreR[Var];				 			
			
      if ( COEFF_TOLERANCE_POUR_LE_TEST_DE_HARRIS * SeuilDAmissibiliteDuale[Var] > EpsHarrisMax ) EpsHarris = EpsHarrisMax  / NBarreR[Var];
      else 	EpsHarris = COEFF_TOLERANCE_POUR_LE_TEST_DE_HARRIS * SeuilDAmissibiliteDuale[Var] / NBarreR[Var];
			
      if ( SortSurXmaxOuSurXmin != SORT_SUR_XMIN ) {
        CBarreSurNBarreR             [j] = X;
        CBarreSurNBarreRAvecTolerance[j] = X - EpsHarris;		
		  }
		  else {
        CBarreSurNBarreR             [j] = -X;	
        CBarreSurNBarreRAvecTolerance[j] = -(X - EpsHarris);			
		  }								
		}
	}					     	
}

return;
}


