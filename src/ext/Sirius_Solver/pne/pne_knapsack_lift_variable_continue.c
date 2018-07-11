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

   FONCTION: Apres constitution d'une coupe de knapsack sur la restriction
	           aux variables entieres d'une knapsack avec variable continue,
						 on lifte cette variable continue.
							 S'applique aux mixed knapsack avec une variable continue:
							 a^T y < b + s 

   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define Zero 1.e-9

double PNE_ProgrammationDynamiqueMaxKnapsack( int , double * , double * , double , char * );
																						 
/*------------------------------------------------------------------------*/
/* Resolution du probleme   Max c^T . y   s.c   a^T y >= b */
/* y binaire, a entier > 0, b entier > 0, c reel */
/* Methode: programmation dynamique */

double PNE_ProgrammationDynamiqueMaxKnapsack( int NombreDeVariables, double * C, double * A, double B0double,
                                              /* En retour */
																							char * Y
																						 )
{
int Var; int i; double Z; int B1; int AdeVar; double CdeVar; double * Bellman; int B0; double Opt; double Sec;
int B0max; int E1; int E2; int B; char * Commande; int EtatOpt; char * Init;

/* Ci-dessous un probleme pour tester la programmation dynamique */
/*
if ( NombreDeVariables >= 6 ) {
  B0double = 178;
	C[0] = 1.;
	C[1] = 1.;
	C[2] = 1./4.;
	C[3] = 1./2.;
	C[4] = 0;
	C[5] = 1.;
	
  A[0] = 45;
  A[1] = 46;
  A[2] = 79;
  A[3] = 54;
  A[4] = 53;
  A[5] = 125;
	
	NombreDeVariables = 6;
}
*/
EtatOpt = -1;

B0 = (int) B0double;
/* Calcul de la valeur max du second membre */
B0max = 0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  B0max += (int) A[Var];
	/*printf("C[%d] = %e  A = %d   (%e)  B0 %d B0max %d\n",Var,C[Var],(int)A[Var],A[Var],B0,B0max);*/
}
B1 = B0max + 1;

i = (B0max + 1) * NombreDeVariables;
i++;
Bellman = (double *) malloc( i * sizeof( double ) );
Init = (char *) malloc( i * sizeof( char ) );
Commande = (char *) malloc( i * sizeof( char ) );
for ( B = 0 ; B < i ; B++ ) Init[B] = NON_PNE;
Z = -LINFINI_PNE;

E1 = 0;
/* Variable 0 */
Var = 0;
AdeVar = (int) A[Var];
CdeVar = C[Var];
/* i est l'etat de depart, j est l'etat d'arrivee */
for ( i = 0 ; i <= B0max ; i++ ) {

		if ( Init[E1] == OUI_PNE ) {
		  printf("Bug l'etat %d est deja initialise \n",E1);
		  exit(0);
		}

  /* Variable a 1 => on change de niveau */
	/* B est le niveau atteint pour le second membre */	
  B = i + AdeVar;
	if ( B >= B0 && B <= B0max ) {
	  /* On atteint une solution realisable */
    Bellman[E1] = CdeVar;
	  Commande[E1] = 1;								
		Init[E1] = OUI_PNE;
	}	
	/* Variable a 0 => on ne change pas de niveau */
  B = i;
	if ( B >= B0 ) {	
    if ( Init[E1] == OUI_PNE ) {
      if ( 0 > Bellman[E1] ) {
		    Bellman[E1] = 0;
	      Commande[E1] = 0;											
		  }
	  }
	  else {
		  Bellman[E1] = 0;
	    Commande[E1] = 0;											
		  Init[E1] = OUI_PNE;
	  }
	}
	E1++;	
}
Var++;

for ( ; Var < NombreDeVariables ; Var++ ) {
  AdeVar = (int) A[Var];
	CdeVar = C[Var];
	/* i est l'etat de depart, j est l'etat d'arrivee */
  for ( i = 0 ; i <= B0max ; i++ ) {	
	  /* normalement l'etat n'est pas initialise */
		if ( Init[E1] == OUI_PNE ) {
		  printf("Bug Variable %d etat %d est deja initialise \n",Var,E1);
		  exit(0);
		}		
    /* Variable a 1 => on change de niveau */
    B = i + AdeVar;		
		if ( B <= B0max ) {
		  E2 = E1 - B1 + AdeVar;
      if ( Init[E2] == OUI_PNE ) {
			  Bellman[E1] = CdeVar + Bellman[E2];
		    Commande[E1] = 1;				
			  Init[E1] = OUI_PNE;
			  /*printf("Bellman %e B %d  Var %d\n",Bellman[E1],B,Var);*/
			}	   
    }		
		/* Variable a 0 => on ne change pas de niveau */
    B = i;
		E2 = E1 - B1;
    if ( Init[E2] == OUI_PNE ) {
			if ( Init[E1] == OUI_PNE ) {
        if ( Bellman[E2] > Bellman[E1] ) {
          Bellman[E1] = Bellman[E2];
		      Commande[E1] = 0;										
			  }
			}
      else {
        Bellman[E1] = Bellman[E2];
		    Commande[E1] = 0;										
			  Init[E1] = OUI_PNE;
			}			
		}		
		if ( Var == NombreDeVariables - 1 ) {
		  if ( i == 0 ) {
		    if ( Init[E1] == NON_PNE ) {
		      printf("Bug Variable %d etat non initialise \n",E1);
		      exit(0);
		    }				
			  Z = Bellman[E1];
				EtatOpt = E1;
				break;
			}
		}
		E1++;
  }
}

if ( Y != NULL ) {
  /* On rebalaie en avant pour avoir la solution optimale */
  Opt = 0;
  Sec = 0;
  E1 = EtatOpt;
  Var = NombreDeVariables - 1;
  i = 0;
  Y[Var] = Commande[E1];
  Opt += C[Var] * Y[Var];
  Sec += A[Var] * Y[Var];
  while ( Var > 0 ) {
	  E2 = E1 - B1 + ( (int) A[Var] * (int) Commande[E1] );
	  i += (int) A[Var] * (int) Commande[E1];
    Var--;
    Y[Var] = Commande[E2];
    Opt += C[Var] * Y[Var];
    Sec += A[Var] * Y[Var];	
	  E1 = E2;
  }

  i += (int) A[0] * (int) Y[0];
  /*
  printf(" Z %e  niveau atteint %d\n",Z,i);
  printf(" Opt %e  sec %e\n",Opt,Sec);
  */

  if ( Opt != Z ) {
    printf("Ereur programmation dynamique !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	  exit(0);
  }
}

free( Bellman );
free( Init );
free( Commande );

printf(" Z %e \n",Z);

return( Z );
}

/*----------------------------------------------------------------------------*/

void PNE_KnapsackLifterLaVariableContinue( /* Un vecteur de travail */
																					 double * C,
                                           /* La knapsack */
                                           int NbTermesKnapsack,
																					 double * aKnapsack,
																					 double bKnapsack,
																					 /* La coupe sur knapsack */
																					 int NbTermesCoupeKnapsack,
																					 int * varCoupeKnapsack,
																					 double * piCoupeKnapsack,
																					 double pi0CoupeKnapsack,
																					 /* La valeur prise par s pour le calcul de la coupe sur knapsack */
                                           double sBarre,
																				   /* En retour le coeff de la variable continue liftee */
																					 double * Gamma
																				 )
{
double GammaLB; double X; int i; double CoutConstant; double Xi; double sOpt;
char * Y;

printf("Valeur de sBarre %e\n",sBarre);

Y = (char *) malloc( NbTermesKnapsack * sizeof( char ) );

/* Calcul du produit scalaire a^T . Pi */
memset( (char *) C, 0 , NbTermesKnapsack * sizeof( double ) );
for ( i = 0 ; i < NbTermesKnapsack ; i++ ) C[i] = aKnapsack[i];
X = 0.0;
for ( i = 0 ; i < NbTermesCoupeKnapsack ; i++ ) X += C[varCoupeKnapsack[i]] * piCoupeKnapsack[i];

X -= bKnapsack + sBarre;
if ( fabs( X ) < Zero ) X = 1.0;
GammaLB = ( piCoupeKnapsack[0] - pi0CoupeKnapsack ) / X;
if ( GammaLB < 0.0 ) GammaLB = 1.0;

/* Resolution du probleme knapsack */
ResolutionKnapsack:
/* Constitution du vecteur des couts des variables entieres */
for ( i = 0 ; i < NbTermesKnapsack ; i++ ) C[i] = -GammaLB * aKnapsack[i];
for ( i = 0 ; i < NbTermesCoupeKnapsack ; i++ ) C[varCoupeKnapsack[i]] += piCoupeKnapsack[i];
/* Resolution du probleme knapsack : Max du critere sous contraint aKnapsack * Y > bKnapsack */
CoutConstant = pi0CoupeKnapsack + GammaLB * bKnapsack;

Xi = PNE_ProgrammationDynamiqueMaxKnapsack( NbTermesKnapsack, C, aKnapsack, bKnapsack,
                                            /* En retour */
																					  Y );
Xi -= CoutConstant;

if ( Xi >= 0.0 ) {
  /* La coupe n'est pas valide pour toutes les valeurs de s avec cette valeur de GammaLB, on l'augmente */
  X = 0.0;
  for ( i = 0 ; i < NbTermesCoupeKnapsack ; i++ ) X += piCoupeKnapsack[i] * (double) Y[varCoupeKnapsack[i]];
  X -= pi0CoupeKnapsack;
	sOpt = -bKnapsack;
  for ( i = 0 ; i < NbTermesKnapsack ; i++ ) sOpt += aKnapsack[i] * (double) Y[i];
  if ( fabs( sOpt ) < Zero ) return; /* Prevoir qu'on arrete */
  GammaLB = X / ( sOpt - sBarre );
  goto ResolutionKnapsack;
}

/* En mettant 0 on doit la detecter violee */

*Gamma = GammaLB;

printf("Valeur de Gamma %e\n",GammaLB);

free( Y );

return;
}

/*----------------------------------------------------------------------------*/
/* Recherche d'un coupe de knapsack pour la partie binaire puis lifting de la
   variable continue.
	 Appele par PNE_MIRMarchandWolsey */
	 
void PNE_Knapsack_0_1_AvecVariableContinue( PROBLEME_PNE * Pne,
                                            int NombreDeVariablesBinaires, 
																						int * NumeroDeLaVariableBinaire,
                                            double * CoeffDeLaVariableBinaire,
																						double b,
								                            double ValeurDeLaVariableContinue,
												                    int NombreDeVariablesSubstituees,
																						int * NumeroDesVariablesSubstituees,
												                    char * TypeDeSubsitution,
																						double * CoefficientDeLaVariableSubstituee				
																					)						
{
char CouvertureTrouvee; double SecondMembre; double sBarre; 
char RendreLesCoeffsEntiers; char Mixed_0_1_Knapsack;

Mixed_0_1_Knapsack = OUI_PNE;

RendreLesCoeffsEntiers = OUI_PNE;
sBarre = ValeurDeLaVariableContinue;
SecondMembre = b + sBarre;
CouvertureTrouvee = NON_PNE;

/* Les variables binaires ne sont pas encore complementees. Elles le seront dans GreedyCoverKnapsack.
   La variable continue a ete creee. */
PNE_GreedyCoverKnapsack( Pne, 0, NombreDeVariablesBinaires, NumeroDeLaVariableBinaire, CoeffDeLaVariableBinaire,
                         SecondMembre, RendreLesCoeffsEntiers, &CouvertureTrouvee,
												 Mixed_0_1_Knapsack, 
                         sBarre,													 
												 NombreDeVariablesSubstituees, NumeroDesVariablesSubstituees,
												 TypeDeSubsitution, CoefficientDeLaVariableSubstituee												 
											 );

											 
return;
}
