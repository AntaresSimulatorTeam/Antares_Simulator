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

   FONCTION: Calcul d'une borne sur le sac a dos
   
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_definition_arguments.h"
# include "pne_fonctions.h"
# include "pne_constantes_externes.h"	       						 
# include "pne_define.h"

int PNE_KnapsackProgrammationDynamique( int , int * , double * , double );

/*------------------------------------------------------------------------*/
/* Resolution du probleme de separation pour les couvertures de sac a dos.
   On calcul le min du critere, la contrainte a respecter est du type >.
	 Min Sigma cj.xj s.c.: Sigma aj.xj > B0 (inegalite stricte) et xj binaire */ 

double PNE_ProgrammationDynamiqueKnapsackSeparation( int NombreDeVariables, double * C, double * A, double B0double )
{
int Var; int i; double Z; int B1; int AdeVar; double CdeVar; double * Bellman; int B0; 
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
      if ( 0 < Bellman[E1] ) {
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
	  /* normalement l'etat E1 n'est pas initialise */
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
        if ( Bellman[E2] < Bellman[E1] ) {
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

free( Bellman );
free( Init );
free( Commande );

printf(" Z %e \n",Z);

return( Z );
}

/*-----------------------------------------------------------------------*/
/* Resolution du knapsack par programmation dynamique. Remplace le calcul
   des majorants employes dans le up et down lifting */
/* Max cx s.c. ax <= b */
int PNE_KnapsackProgrammationDynamique( int NombreDeVariables, int * C, double * A, double B0 )
{
int Var; int i; int Z; int B1; int AdeVar; int CdeVar; int * Bellman_T1; int * Bellman_T2;
  
B1 = (int) B0 + 1;

Bellman_T1 = (int *) malloc( B1 * sizeof( int ) );
Bellman_T2 = (int *) malloc( B1 * sizeof( int ) );

memset( (char *) Bellman_T2, 0, B1 * sizeof( int ) );
memset( (char *) Bellman_T1, 0, B1 * sizeof( int ) );
	
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  AdeVar = (int) A[Var];
	CdeVar = (int) C[Var];
  for ( i = 0 ;  i < B1 ; i++ ) {  
    /* variable a 0 */
    if ( Bellman_T2[i] > Bellman_T1[i] ) Bellman_T1[i] = Bellman_T2[i];
    /* variable a 1 */
		if ( i+AdeVar < B1 ) {
      if ( CdeVar + Bellman_T2[i+AdeVar] > Bellman_T1[i] ) Bellman_T1[i] = CdeVar + Bellman_T2[i+AdeVar];
		}
  }
	memcpy( (char *) Bellman_T2, (char *) Bellman_T1, B1 * sizeof( int ) );
}
Z = Bellman_T2[0];

free( Bellman_T1 );
free( Bellman_T2 );

return( Z );
}

/*-----------------------------------------------------------------------*/

int PNE_MajorantKnapsack( int NombreDeVariables, int * C, double * A, double B0,
                          char LesCoeffSontEntiers )  
{
int Var; double X; int Z; int U_Dantzig; int U0; int U1; char U0_Valide; char U1_Valide;
char SolEntiere; int sigma0_s; int sigma1_s; double Marge; double B; int s;
int U; char U_Valide; int Zdyn;
# if UTILISER_AUSSI_LA_PROGRAMMATION_DYNAMIQUE == OUI_PNE
  char ProgDyn;
# endif

Zdyn = -1;
# if UTILISER_AUSSI_LA_PROGRAMMATION_DYNAMIQUE == OUI_PNE
  if ( B0 <= SEUIL_POUR_PROGRAMMATION_DYNAMIQUE ) {
		ProgDyn = OUI_PNE;
		if ( LesCoeffSontEntiers != OUI_PNE ) ProgDyn = NON_PNE;
		if ( ProgDyn == OUI_PNE ) {
      /*printf("Programmation dynamique B0 %e\n",B0);*/
      Z = PNE_KnapsackProgrammationDynamique( NombreDeVariables, C, A, B0 );
		  Zdyn = Z;
		  /*return( Z);*/
	  }
  }
# endif

Marge = 1.e-8;
SolEntiere = NON_PNE;
/* Pour eviter les warning de compilation */
LesCoeffSontEntiers = NON_PNE;

/* Recherche de l'element critique s */
B = B0;
Z = 0;
X = 0.0;
s = 0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {	
  B -= A[Var];
  Z += C[Var];	
  if ( B < 0.0 ) {
    /* On a empile la variable en trop */
		/* L'item critique est Var */
    s = Var;
    B += A[s];  
    Z -= C[s];
    X = B / A[s];
		if ( fabs(X) < Marge ) {
			SolEntiere = OUI_PNE;
		}
		else if ( fabs( 1. - X ) < Marge ) {
			Z += C[s];
			SolEntiere = OUI_PNE;
		}			
	}	
}
if ( SolEntiere == OUI_PNE ) {
  if ( Zdyn >= 0 && /*Zdyn < Z*/ Zdyn != Z ) {
	  printf("SolEntiere prog dyn Zdyn %d Z %d\n",Zdyn,Z);
	}
  return( Z );
}

/* Borne de Dantzig */
U_Dantzig = Z + (int) floor( (double) C[Var] * X );
U0 = U_Dantzig;
U1 = U_Dantzig;
U = U_Dantzig;

/* A ce stade, Z est la valeur de la fonction cout apres avoir empile les item jusqu'a s-1 compris */
/* sigma0_s est l'item critique lorsqu'on fait Xs = 0 */
U0_Valide = NON_PNE;
for ( Var = s + 1 ; Var < NombreDeVariables ; Var++ ) {	
  B -= A[Var];
  Z += C[Var];	
  if ( B < 0.0 ) {
		/* L'item critique est Var */
    sigma0_s = Var;
    B += A[sigma0_s];  
    Z -= C[sigma0_s];
	  X = B / A[sigma0_s];
		/* Borne de Fayard et Plateau */
		U0_Valide = OUI_PNE;
		if ( fabs(X) < Marge ) U0 = Z;
		else if ( fabs( 1. - X ) < Marge ) U0 = Z + C[sigma0_s];
		else U0 = Z + (int) floor( C[sigma0_s] * X );
	}
}

/* sigma1_s est l'item critique lorsqu'on fait Xs = 1  sigma0_s est forcement toujours avant s */
B = B0 - A[s];
Z = C[s];
U1_Valide = NON_PNE;
if ( B > 0.0 ) {
  for ( Var = 0 ; Var < s ; Var++ ) {	
    B -= A[Var];
    Z += C[Var];	
    if ( B < 0.0 ) {
		  /* L'item critique est Var */
      sigma1_s = Var; 
      B += A[sigma1_s];  
      Z -= C[sigma1_s];
	    X = B / A[sigma1_s];			  
		  /* Borne de Hudson */
		  U1_Valide = OUI_PNE;
		  if ( fabs(X) < Marge ) U1 = Z;
		  else if ( fabs( 1. - X ) < Marge ) U1 = Z + C[sigma1_s];
		  else U1 = Z + (int) floor( C[sigma1_s] * X );			
	  }
  }
}

U_Valide = NON_PNE;
if ( U0_Valide == OUI_PNE && U1_Valide == OUI_PNE ) {
  U_Valide = OUI_PNE;	
  U = U0;
  if ( U < U1 ) U = U1;	  
}

Z = U_Dantzig ;
if ( U_Valide == OUI_PNE ) {
  if ( U <= Z ) {
	  /*printf("Borne de Dantzig rejetee U %d U_Dantzig %d\n",U,U_Dantzig);*/
	  Z = U;
	}
	/*
	else {
	  if ( U_Valide == OUI_PNE ) printf("Borne de Dantzig pas rejetee U %d U_Dantzig %d  U0_Valide %d U1_Valide %d s %d\n",U,U_Dantzig,U0_Valide,U1_Valide,s);
	}
	*/
}

/*
if ( Zdyn >= 0 && Zdyn != Z ) {
	printf(" prog dyn Zdyn %d Z %d\n",Zdyn,Z);
}
*/

return( Z );

}
