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

   FONCTION: Gradient conjugue pour affiner la resolution.
	           Le point initial est la resolution du systeme regularise
						 ou non.
						 
       
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

# include "lu_constantes_externes.h"
# include "lu_definition_arguments.h"

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef PI_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pi_memoire.h"
# endif

/*--------------------------------------------------------------------------------*/

void PI_GradientConjugue( PROBLEME_PI * Pi )
{
int ic; int icMax; int il; int ilMax; int Var; int Cnt; int i; double NormeGradientF;
int NombreDeVariables; int NombreDeContraintes; int Iteration; int IterationMax;

int * Cdeb; int * CNbTerm; int * NumeroDeContrainte; 
int * Mdeb; int * NbTerm; int * Indcol; 
double * Dk; double * ADk; double * B; double * Xk;
double DktADk; double X;

double GradientFtDk; double GradientFtADk;
double * GradientF; 

double tk; double Betak;
double * ACol; double * A; double * DeltaU; double * DeltaLambda;

char * TypeDeVariable; double * Qpar2; double * UnSurUkMoinsUmin; double * UnSurUmaxMoinsUk;
double * S1; double * S2; double * RegulVar; double * RegulContrainte; double * U; double * U0;
double * Lambda; double * Lambda0;

NombreDeVariables   = Pi->NombreDeVariables;
NombreDeContraintes = Pi->NombreDeContraintes;

TypeDeVariable   = Pi->TypeDeVariable;
Qpar2            = Pi->Qpar2;
UnSurUkMoinsUmin = Pi->UnSurUkMoinsUmin;
UnSurUmaxMoinsUk = Pi->UnSurUmaxMoinsUk;
S1 = Pi->S1;
S2 = Pi->S2;

U       = Pi->U;
U0      = Pi->U0;
Lambda  = Pi->Lambda;
Lambda0 = Pi->Lambda0;

RegulVar        = Pi->RegulVar;
RegulContrainte = Pi->RegulContrainte;

Cdeb               = Pi->Cdeb;
CNbTerm            = Pi->CNbTerm;
ACol               = Pi->ACol;
NumeroDeContrainte = Pi->NumeroDeContrainte;

Mdeb   = Pi->Mdeb;
NbTerm = Pi->NbTerm;
A      = Pi->A;
Indcol = Pi->Indcol;

DeltaU      = Pi->DeltaU;
DeltaLambda = Pi->DeltaLambda;

Dk  = (double *) malloc( (NombreDeVariables + NombreDeContraintes ) * sizeof( double ) );
ADk = (double *) malloc( (NombreDeVariables + NombreDeContraintes ) * sizeof( double ) );
B   = (double *) malloc( (NombreDeVariables + NombreDeContraintes ) * sizeof( double ) );
Xk  = (double *) malloc( (NombreDeVariables + NombreDeContraintes ) * sizeof( double ) );

GradientF = (double *) malloc( (NombreDeVariables + NombreDeContraintes ) * sizeof( double ) );

Iteration = 0;
IterationMax = 10;

/* Initialiser B a partir de la sauvegarde du second membre qui a ete faite dans PI_ResolutionSystemeAugmente */
memcpy( (char *) B, (char *) Pi->SecondMembreAffine, ( NombreDeVariables + NombreDeContraintes ) * sizeof( double ) );

/* On enleve le terme de regularisation */
for ( i = 0 ; i < NombreDeVariables ; i++ ) {  
  B[i]+= RegulVar[i] * ( U[i] - U0[i] );
}  
for ( i = 0 ; i < NombreDeContraintes ; i++ ) {
  B[NombreDeVariables + i]+= RegulContrainte[i] * ( Lambda[i] - Lambda0[i] );
}


/* Initialiser Xk a la solution du systeme */
for ( Var = 0 ; Var < NombreDeVariables ; Var++ )   Xk[Var] = DeltaU[Var];
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) Xk[NombreDeVariables + Cnt] = DeltaLambda[Cnt];

/* Initialiser Dk a la solution du systeme */
/* Calcul de GradientFk */
for ( i = 0 ; i < NombreDeVariables + NombreDeContraintes ; i++ ) GradientF[i] = -B[i];

for ( Var = 0 ; Var < NombreDeVariables; Var++ ) {
  X = 0.0;
  /* Partie Hessien */
  if ( TypeDeVariable[Var] == BORNEE ) {		
    X = Qpar2[Var] + ( UnSurUkMoinsUmin[Var] * S1[Var] ) + ( UnSurUmaxMoinsUk[Var] * S2[Var] );
  }
  else if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {		
    X = Qpar2[Var] + ( UnSurUkMoinsUmin[Var] * S1[Var] );
  }
  else if ( TypeDeVariable[Var] == BORNEE_SUPERIEUREMENT ) {			
    X = Qpar2[Var] + ( UnSurUmaxMoinsUk[Var] * S2[Var] );
  }
  else if ( TypeDeVariable[Var] == NON_BORNEE ) { 
    X = Qpar2[Var];				
  }
  /* On ne met pas le terme de regularisation */
	
	X*= Xk[Var];
	
  /* Partie Contraintes */
  ic    = Cdeb[Var];
  icMax = ic + CNbTerm[Var];
  while ( ic < icMax ) {
    X+= ACol[ic] * Xk[NombreDeVariables+NumeroDeContrainte[ic]];
    ic++;
  }
	GradientF[Var]+= X;
}
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  X = 0.0;
	
  /* On ne met pas le terme de regularisation */
	
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
    /* Partie contrainte */
    X+= A[il] * Xk[Indcol[il]];	
    il++;
	}
	GradientF[NombreDeVariables+Cnt]+= X;
}

NormeGradientF = 0.0;
for ( i = 0 ; i < NombreDeVariables + NombreDeContraintes ; i++ ) {
  NormeGradientF+= GradientF[i] * GradientF[i];
}
NormeGradientF = sqrt( NormeGradientF );
printf("Norme du gradient %e\n",NormeGradientF);

for ( i = 0 ; i < NombreDeVariables + NombreDeContraintes ; i++ ) Dk[i] = -GradientF[i];

NouvelleIteration:

X = 0.0 ;
for ( i = 0 ; i < NombreDeVariables + NombreDeContraintes ; i++ ) {
  X+= Dk[i] * Dk[i];
}
printf("  Norme de Dk %e\n",sqrt(X));

/* Calcul de GradientFtDk */
GradientFtDk = 0.0;
for ( i = 0 ; i < NombreDeVariables + NombreDeContraintes ; i++ ) {
  GradientFtDk+= GradientF[i] * Dk[i];
}
printf("  GradientFtDk %e\n",GradientFtDk);
/* Calcul de ADk */
for ( Var = 0 ; Var < NombreDeVariables; Var++ ) {
  X = 0.0;
  /* Partie Hessien */
  if ( TypeDeVariable[Var] == BORNEE ) {		
    X = Qpar2[Var] + ( UnSurUkMoinsUmin[Var] * S1[Var] ) + ( UnSurUmaxMoinsUk[Var] * S2[Var] );
  }
  else if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {		
    X = Qpar2[Var] + ( UnSurUkMoinsUmin[Var] * S1[Var] );
  }
  else if ( TypeDeVariable[Var] == BORNEE_SUPERIEUREMENT ) {			
    X = Qpar2[Var] + ( UnSurUmaxMoinsUk[Var] * S2[Var] );
  }
  else if ( TypeDeVariable[Var] == NON_BORNEE ) { 
    X = Qpar2[Var];				
  }
	/* On ne met pas le terme de regularisation */	
	X*= Dk[Var];		
  /* Partie Contraintes */
  ic    = Cdeb[Var];
  icMax = ic + CNbTerm[Var];
  while ( ic < icMax ) {
    X+= ACol[ic] * Dk[NombreDeVariables+NumeroDeContrainte[ic]];
    ic++;
  }
	ADk[Var] = X;
}
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  X = 0.0;
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
    /* Partie contrainte */
    X+= A[il] * Dk[Indcol[il]];	
    il++;
	}
	ADk[NombreDeVariables+Cnt] = X;
}

/* Calcul de DktADk */
DktADk = 0.0;
for ( i = 0 ; i < NombreDeVariables + NombreDeContraintes ; i++ ) {
  DktADk+= Dk[i] * ADk[i];
}
printf("  DktADk  %e\n",DktADk );

/* Calcul de tk */
tk = -GradientFtDk / DktADk;
tk = 1.e-10/( 1 + ( Iteration * Iteration) );
printf("  tk %e\n",tk );

/* Calcul de Xk a k+1 */
for ( i = 0 ; i < NombreDeVariables + NombreDeContraintes ; i++ ) {
  Xk[i]+= tk * Dk[i];
}

/* Calcul de GradientF k+1 */
for ( i = 0 ; i < NombreDeVariables + NombreDeContraintes ; i++ ) GradientF[i] = -B[i];
for ( Var = 0 ; Var < NombreDeVariables; Var++ ) {
  X = 0.0;
  /* Partie Hessien */
  if ( TypeDeVariable[Var] == BORNEE ) {		
    X = Qpar2[Var] + ( UnSurUkMoinsUmin[Var] * S1[Var] ) + ( UnSurUmaxMoinsUk[Var] * S2[Var] );
  }
  else if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {		
    X = Qpar2[Var] + ( UnSurUkMoinsUmin[Var] * S1[Var] );
  }
  else if ( TypeDeVariable[Var] == BORNEE_SUPERIEUREMENT ) {			
    X = Qpar2[Var] + ( UnSurUmaxMoinsUk[Var] * S2[Var] );
  }
  else if ( TypeDeVariable[Var] == NON_BORNEE ) { 
    X = Qpar2[Var];				
  }
	
  /* On ne met pas le terme de regularisation */
	
	X*= Xk[Var];	
	
  /* Partie Contraintes */
  ic    = Cdeb[Var];
  icMax = ic + CNbTerm[Var];
  while ( ic < icMax ) {
    X+= ACol[ic] * Xk[NombreDeVariables+NumeroDeContrainte[ic]];
    ic++;
  }
	GradientF[Var]+= X;
}
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  X = 0.0;

  /* On ne met pas le terme de regularisation */
	
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
    /* Partie contrainte */
    X+= A[il] * Xk[Indcol[il]];	
    il++;
	}
	GradientF[NombreDeVariables+Cnt]+= X;
}

NormeGradientF = 0.0;
for ( i = 0 ; i < NombreDeVariables + NombreDeContraintes ; i++ ) {
  NormeGradientF+= GradientF[i] * GradientF[i];
}
NormeGradientF = sqrt( NormeGradientF );
printf("Norme du gradient %e\n",NormeGradientF);

/* Calcul de GradientFkPlus1ADk */
GradientFtADk = 0.0;
for ( i = 0 ; i < NombreDeVariables + NombreDeContraintes ; i++ ) {
  GradientFtADk+= GradientF[i] * ADk[i];
}
printf("  GradientFkPlus1ADk %e\n",GradientFtADk );

/* Calcul de Betak */
Betak = GradientFtADk / DktADk;
Betak = 0.0;
printf("  Betak %e\n", Betak);

/* Calcul de Dk a k+1 */
for ( i = 0 ; i < NombreDeVariables + NombreDeContraintes ; i++ ) {
  Dk[i] = -GradientF[i] + ( Betak * Dk[i] );
}

if ( Iteration < IterationMax ) {
  Iteration++;
  goto NouvelleIteration;
}

exit(0);

/* Recuperation du resultat */
for ( Var = 0 ; Var < NombreDeVariables ; Var++ )   DeltaU[Var] = Xk[Var];
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) DeltaLambda[Cnt] = Xk[NombreDeVariables + Cnt];

free( Dk );
free( ADk );
free( B );
free( GradientF );
free( Xk );

return;
}
