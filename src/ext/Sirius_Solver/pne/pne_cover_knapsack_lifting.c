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

   FONCTION: Lifting d'une couverture de sac a dos par la methode
	           "Sequence independent lifting of cover inequalities" 
	           Gu, Nemhauser, Savelsbergh
             Integer Prog. and Combinatorial Opt, 4th Int'l
             IPCO Conference Proceedings, Copenhagen, Denmark,
             May 1995, pgs 452-461.
             Rq: je n'ai plus l'article mais on peut trouver la 
             methode dans " Sequence Independent Lifting in Mixed Integer 
             Programming" des memes auteur.

						 Plusieurs chose ne sont pas au point:
						 le downlifting des ariables dans C2 et le
						 calcul du majorant knapsack. En ce qui concerne le
						 calcul du majorant knapsack, le probleme vient du fait
						 que les coefficients de la couverture ne sont pas
						 toujours des entiers.
	 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"  

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define COMPILER 0

# define TRACES NON_PNE

# define Epsilon 1.e-8

double PNE_SequenceIndependentMajorantKnapsack( int , double * , double * , double ); 
void PNE_SequenceIndependentReclasserCsurA( int , char * , int * , double * , double , double * , double * , double * , int * );

/*----------------------------------------------------------------------------*/

double PNE_SequenceIndependentMajorantKnapsack( int NombreDeVariables, double * C, double * A, double B0 )  
{
int Var; double X; double Z; double U_Dantzig; double U0; double U1; char U0_Valide; char U1_Valide;
char SolEntiere; int sigma0_s; int sigma1_s; double Marge; double B; int s;
int U; char U_Valide; 

Marge = 1.e-8;
SolEntiere = NON_PNE;

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
if ( SolEntiere == OUI_PNE ) return( Z );

/* Borne de Dantzig */
U_Dantzig = Z + ( C[Var] * X );

return( U_Dantzig );

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
		else U0 = Z + ( C[sigma0_s] * X );
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
		  else U1 = Z + ( C[sigma1_s] * X );			
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
	  Z = U;
	}
}

return( Z );

}
/*----------------------------------------------------------------------------*/

void PNE_SequenceIndependentReclasserCsurA( int VariableLiftee, char * Z, int * Nb, double * CsurA,
                                            double Co, double * C, double * A, double * A0, int * NumVar )
{
int iSupprime; int NbNew; double CdivA; int i;
NbNew = *Nb;
Z[VariableLiftee] = 1;
CdivA = (double) Co / A0[VariableLiftee];
iSupprime = NbNew;	
for ( i = 0 ; i < NbNew ; i++ ) {
  if ( CsurA[i] < CdivA ) {
    iSupprime = i;
    break;
	}
}	
for( i = NbNew ; i > iSupprime ; i-- ) {
  CsurA[i] = CsurA[i-1];
  C[i] = C[i-1];
  A[i] = A[i-1];
  NumVar[i] = NumVar[i-1];		
}	
C[iSupprime] = Co;
A[iSupprime] = A0[VariableLiftee];
CsurA[iSupprime] = CdivA;	
NumVar[iSupprime] = VariableLiftee;
NbNew++;
*Nb = NbNew;
return; 
}

# if COMPILER == 1

/*----------------------------------------------------------------------------*/
/* Finalement on constate que le sequence independent lifting donne de moins
   bons resultats */
void PNE_SequenceIndependantCoverKnapsackLifting(
                  /* La contrainte de sac a dos */
                  int NombreDeVariables, /* Nombre de variables de la contrainte de sac a dos */
									double * ValeurDeX,
                  double * A0, /* Les coefficients de la contrainte de sac a dos */
                  double b, /* Second membre de la contrainte de sac a dos */
									/* La couverture */
                  int * NombreDeVariablesDeLaCouvertureAuDepart, 
                  double * CoefficientsDeLaCouverture, /* Coefficients de la couverture */
									int *  VariableBooleenneDeLaCouverture, /* Contient les numeros de variables de la couverture */
									double * SecondMembreDeLaCouverture,
                  double * A, /* Contient les coeff (dans la contrainte A0) des variables de la couverture */
                  char * Z, /* Contient des infos pour savoir si on doit lifter une variable */
									double * CsurA,
									int * Ordre,
                  char * CouvertureTrouvee )
{
double * Mu; double Lambda; double Somme; double * MuMoinsLambda; int Var; int h;
double Aj; double Co; double * Rho; int i; int NbVarAjoutees; int j; double Xi;
int NombreDeVariablesDeLaCouverture; char TypeTri; double * C; int * Num;

NbVarAjoutees = 0;
NombreDeVariablesDeLaCouverture = *NombreDeVariablesDeLaCouvertureAuDepart;

/* Mu est dimensionnes au nombre de variables de la couverture + 1 */
/* Rho est dimmensionne au nombre de variables de la couverture */
/* On met NombreDeVariables car on s'en resert a la fin */

Mu = (double *) malloc( ( NombreDeVariables + 1 ) * sizeof( double) );
MuMoinsLambda = (double *) malloc( ( NombreDeVariables + 1 ) * sizeof( double) );
Rho = (double *) malloc( ( NombreDeVariables + 1 ) * sizeof( double) );

/* Les coefficients A des variables de la couverture sont supposes positifs et classes dans 
l'ordre decroissant */

/* Mu[i] = Somme pour j = 0 a j = i-1 A[j] attention: dans l'article on va jusqu'a j=i mais
   c'est parce que la numerotation des variables commence a 1 or en C la numerotation 
   commence a 0.
   On a Mu[0] = 0 ; Mu[1] = A[0] ; Mu[2] = A[0] + A[1] ;
   Pour s'en rappeler il faut se dire que Mu[2] c'est la somme pour 2 variables */
   
/* Calcul de Lambda: le depassement de la couverture (cover excess) */
Somme = 0.0;
for ( i = 0 ; i < NombreDeVariablesDeLaCouverture ; i++ ) Somme += A[i];
Lambda = Somme - b;
if ( Lambda < Epsilon ) return;

Mu[0] = 0.0;
MuMoinsLambda[0] = -Lambda;
for ( i = 1 ; i < NombreDeVariablesDeLaCouverture + 1 ; i++ ) {
  Mu[i] = Mu[i-1] + A[i-1];
  MuMoinsLambda[i] = Mu[i] - Lambda;
}

/* On controle si f(z) est superaditive */
if ( MuMoinsLambda[1] < A[1] || 1 ) goto FNonSuperAditive;
/* f(z) est superaditive */ 
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( Z[Var] != 0 ) continue; /* La variable est dans la couverture */
  Aj = A0[Var];	
  /* La variable doit etre liftee car elle est dans C2 */
  if ( Aj <= MuMoinsLambda[1] - Epsilon ) {
    /* Le coeff de lifting vaut 0 */
    continue;	
  }	
  for ( h = 2 ; h < NombreDeVariablesDeLaCouverture + 1 ; h++ ) {	
    if ( Aj <= MuMoinsLambda[h] - Epsilon ) {  
      /* Le coefficient de lifting de la variable Var est h */ 
      Co = h - 1;
			# if TRACES == OUI_PNE
			  printf("valeur du coeff lifte : %e VariableLiftee %d\n",Co,Var);
      # endif
			CoefficientsDeLaCouverture[NombreDeVariablesDeLaCouverture + NbVarAjoutees] = Co;
			VariableBooleenneDeLaCouverture[NombreDeVariablesDeLaCouverture + NbVarAjoutees] = Var;
			NbVarAjoutees++;
      break;
    }
  }	
}
goto FinLifting;

FNonSuperAditive:
/* On utilise une autre fonction g qui est superaditive */  
Rho[0] = Lambda;
Rho[NombreDeVariablesDeLaCouverture] = 0.0; 
for ( i = 1 ; i < NombreDeVariablesDeLaCouverture ; i++ ) {
  if ( A[i] - MuMoinsLambda[1] > 0 ) Rho[i] = A[i] - MuMoinsLambda[1];
  else Rho[i] = 0.0;
}

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( Z[Var] != 0 ) continue; /* La variable est dans la couverture */
  Aj = A0[Var];	
  for ( h = 0 ; h < NombreDeVariablesDeLaCouverture ; h++ ) {
    if ( Aj <= MuMoinsLambda[h+1] - Epsilon ) {   
      if ( h > 0 ) {
        Co = h; /* Coeff de la variable liftee */								
		 	  # if TRACES == OUI_PNE
			    printf("1- valeur du coeff lifte : %e VariableLiftee %d\n",Co,Var);
        # endif
        CoefficientsDeLaCouverture[NombreDeVariablesDeLaCouverture + NbVarAjoutees] = Co;
			  VariableBooleenneDeLaCouverture[NombreDeVariablesDeLaCouverture + NbVarAjoutees] = Var;
				NbVarAjoutees++;
      }
      break;
    }
    else if ( Aj <= MuMoinsLambda[h+1] + Rho[h+1] - Epsilon ) {
      Co = h+1;
      Co -= ( MuMoinsLambda[h+1] + Rho[h+1]-  Aj ) / Rho[1];			
		 	# if TRACES == OUI_PNE
			  printf("2- valeur du coeff lifte : %e VariableLiftee %d\n",Co,Var);
      # endif
      CoefficientsDeLaCouverture[NombreDeVariablesDeLaCouverture + NbVarAjoutees] = Co;
			VariableBooleenneDeLaCouverture[NombreDeVariablesDeLaCouverture + NbVarAjoutees] = Var;
			NbVarAjoutees++;
      break;
    }       
  }   
}

FinLifting:

# if TRACES == OUI_PNE
  printf("NbVarAjoutees %d\n",NbVarAjoutees);
# endif

NombreDeVariablesDeLaCouverture += NbVarAjoutees;

/* On downlift les variables dans C2 */
/* Classement de C/A dans l'ordre decroissant */
for ( i = 0 ; i < NombreDeVariablesDeLaCouverture ; i++ ) {
	CsurA[i] = CoefficientsDeLaCouverture[i] / A0[VariableBooleenneDeLaCouverture[i]];
  Ordre[i] = i;
}  
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( Z[Var] != 2 ) continue;
  b += A0[Var];
}
TypeTri = DECROISSANT;
PNE_TriRapide( CsurA, Ordre, 0, NombreDeVariablesDeLaCouverture - 1, TypeTri);

C = (double *) MuMoinsLambda;
Num = (int *) Rho;
memcpy( (char *) C, (char *) CoefficientsDeLaCouverture, NombreDeVariablesDeLaCouverture * sizeof( double ) );
memcpy( (char *) Num, (char *) VariableBooleenneDeLaCouverture, NombreDeVariablesDeLaCouverture * sizeof( int ) );

for ( i = 0 ; i < NombreDeVariablesDeLaCouverture ; i++ ) {
  j = Ordre[i];
	CoefficientsDeLaCouverture[i] = C[j];	
  VariableBooleenneDeLaCouverture[i] = Num[j];
	A[i] = A0[VariableBooleenneDeLaCouverture[i]];
	CsurA[i] = CoefficientsDeLaCouverture[i] / A[i];
}

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( Z[Var] != 2 ) continue;	
	/* La variable est dans C2 */
	Xi = PNE_SequenceIndependentMajorantKnapsack( NombreDeVariablesDeLaCouverture, CoefficientsDeLaCouverture, A, b );
	Co = Xi - *SecondMembreDeLaCouverture;
  /* Lifting impossible: les ensembles ont ete mal choisis, tant pis */	  
	if ( Co <= 0 ) goto Fin; 	
	if ( Co > 0 ) {
	  /* Insertion de la variable et modification du Classement de C/A */
    PNE_SequenceIndependentReclasserCsurA ( Var, Z, &NombreDeVariablesDeLaCouverture, CsurA, Co,
		                                        CoefficientsDeLaCouverture, A, A0, VariableBooleenneDeLaCouverture );		
	  *SecondMembreDeLaCouverture = *SecondMembreDeLaCouverture + Co;
	}
}

*NombreDeVariablesDeLaCouvertureAuDepart = NombreDeVariablesDeLaCouverture;

/* Controle du depassement */
Somme = 0.0;
for ( i = 0 ; i < NombreDeVariablesDeLaCouverture ; i++ ) {
  Somme += CoefficientsDeLaCouverture[i] * ValeurDeX[VariableBooleenneDeLaCouverture[i]];  
}
if ( Somme > *SecondMembreDeLaCouverture + Pne->SeuilDeViolationK ) {
  # if TRACES == OUI_PNE
    printf("Couverture liftee depasse b:  Somme = %e  SecondMembreDeLaCouverture = %e  ecart %e\n",
	          Somme,*SecondMembreDeLaCouverture, Somme - *SecondMembreDeLaCouverture );
  # endif
	*CouvertureTrouvee = OUI_PNE;
}
else {
  # if TRACES == OUI_PNE
    printf("Couverture liftee ne viole pas b: Somme = %e  SecondMembreDeLaCouverture = %e \n",
	          Somme, *SecondMembreDeLaCouverture);
  # endif
}

Fin:

free( Mu );
free( MuMoinsLambda );
free( Rho );

return;
}

# endif
