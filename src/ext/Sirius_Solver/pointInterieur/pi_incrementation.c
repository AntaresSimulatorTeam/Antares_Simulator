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
/***********************************************************************************

   FONCTION: Incrementation des variables 

   PI_CalculDesTeta , PI_Incrementation
                   
   AUTEUR: R. GONZALEZ

************************************************************************************/

# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

/*------------------------------------------------------------------------*/
/*                 Calcul des pas primaux et duaux                        */ 

void PI_CalculDesTeta( PROBLEME_PI * Pi , double * TetakP , double * TetakD )
{
int i;
double Marge       ; double Z           ; double Sm; 
double UmaxMoinsUkM; double UkMoinsUminM; double Xs;

int NombreDeVariables; double * UmaxMoinsUk; double * UkMoinsUmin;
char * TypeDeVariable; double * DeltaU; double * S1; double * S2;
double * DeltaS1; double * DeltaS2; double Smin;

NombreDeVariables = Pi->NombreDeVariables;

UmaxMoinsUk = Pi->UmaxMoinsUk;
UkMoinsUmin  =Pi->UkMoinsUmin;
TypeDeVariable = Pi->TypeDeVariable;
DeltaU = Pi->DeltaU;
S1 = Pi->S1;
S2 = Pi->S2;
DeltaS1 = Pi->DeltaS1;
DeltaS2 = Pi->DeltaS2;
Smin    = Pi->Smin;

*TetakP = 1.; *TetakD = 1.; Marge = ZERO; Z = 0.;

for ( i = 0 ; i < NombreDeVariables ; i++ ) {
  UmaxMoinsUkM = UmaxMoinsUk[i] - Marge; 
  if ( UmaxMoinsUkM < Z ) UmaxMoinsUkM = Z;
  UkMoinsUminM = UkMoinsUmin[i] - Marge;
  if ( UkMoinsUminM < Z  ) UkMoinsUminM = Z;

  if ( TypeDeVariable[i] == BORNEE ) {
    if ( DeltaU[i] >= UmaxMoinsUkM || -DeltaU[i] >= UkMoinsUminM ) {
      if ( DeltaU[i] > ZERO ) {
        Xs = UmaxMoinsUkM / DeltaU[i];
        if ( *TetakP > Xs ) *TetakP = Xs;  
      }
      else if ( DeltaU[i] < -ZERO ) {
        Xs = -UkMoinsUminM / DeltaU[i];
        if ( *TetakP > Xs ) *TetakP = Xs;    
      }
      else DeltaU[i] = 0.;
    }
  }
  else if ( TypeDeVariable[i] == BORNEE_INFERIEUREMENT ) {
    if ( -DeltaU[i] >= UkMoinsUminM ) {
      if ( DeltaU[i] < -ZERO ) {
        Xs = -UkMoinsUminM / DeltaU[i];
        if ( *TetakP > Xs ) *TetakP = Xs;   
      }
      else DeltaU[i] = 0.;
    }
  }
  else if ( TypeDeVariable[i] == BORNEE_SUPERIEUREMENT ) {
    if ( DeltaU[i] >= UmaxMoinsUkM ) {
      if ( DeltaU[i] > ZERO ) {
        Xs = UmaxMoinsUkM / DeltaU[i];
        if ( *TetakP > Xs ) *TetakP = Xs; 
      }
      else DeltaU[i] = 0.;
    }  
  }
  /* Pas d'ecretement si non bornee */
  
  /* Variable S1 */
  if (  TypeDeVariable[i] == BORNEE || TypeDeVariable[i] == BORNEE_INFERIEUREMENT ) {
    Sm = S1[i] - Smin;
    if ( Sm < Z ) Sm = Z;
    if ( DeltaS1[i] <= -Sm ) {
      Xs = -S1[i] / DeltaS1[i];
      if ( *TetakD > Xs ) *TetakD = Xs; 
    }
  }

  /* Variable S2 */
  if (  TypeDeVariable[i] == BORNEE || TypeDeVariable[i] == BORNEE_SUPERIEUREMENT ) {
    Sm = S2[i] - Smin;
    if ( Sm < Z ) Sm = Z;
    if ( DeltaS2[i] <= -Sm ) {
      Xs = -S2[i] / DeltaS2[i];
      if ( *TetakD > Xs ) *TetakD = Xs; 
    }
  }
}

# if VERBOSE_PI
  printf("Pas primal TetakP %e - pas dual TetakD %e\n",*TetakP,*TetakD);
# endif

return;
}

/*------------------------------------------------------------------------*/
/*                  Incrementation des variables                          */ 

void PI_Incrementation( PROBLEME_PI * Pi )
{
int i; double TetakP; double TetakD; double CoeffP; double CoeffS; double CoeffL;
double X; char * TypeDeVariable; int NombreDeVariables; double * U; double X1; double X2;
double * U0; double Smin; double * DeltaU; double * S1; double * S2; double * DeltaS1;
double * DeltaS2; double * Umin; double * Umax; double * UkMoinsUmin;
double * UmaxMoinsUk; double * UnSurUkMoinsUmin; double * UnSurUmaxMoinsUk;
double * Lambda; double * Lambda0; double * DeltaLambda; char Type; 
double AlphaK; double UnMoinsAlphaK; int NombreDeContraintes;
double InfDeVzerEtZero; double Div; double UnSurInfDeVzerEtZero; double Vm;
double Amin; double Amax; double Dx; double Dm; double * Alpha; double * Alpha2;

PI_CalculDesTeta( Pi , &TetakP , &TetakD ); /* Calcul du pas primal et dual */

Pi->IterationTetakP[Pi->NumeroDIteration-1] = TetakP;
Pi->IterationTetakD[Pi->NumeroDIteration-1] = TetakD;

CoeffP = Pi->Gamma * TetakP;
CoeffS = Pi->Gamma * TetakD;
CoeffL = CoeffS;

NombreDeVariables   = Pi->NombreDeVariables;
NombreDeContraintes = Pi->NombreDeContraintes;
TypeDeVariable = Pi->TypeDeVariable;
U = Pi->U;
U0 = Pi->U0;
DeltaU = Pi->DeltaU;
S1 = Pi->S1;
S2 = Pi->S2;
DeltaS1 = Pi->DeltaS1;
DeltaS2 = Pi->DeltaS2;
Alpha   = Pi->Alpha;
Alpha2  = Pi->Alpha2;
Smin = Pi->Smin;
Umin = Pi->Umin;
Umax = Pi->Umax;
UkMoinsUmin = Pi->UkMoinsUmin;
UmaxMoinsUk = Pi->UmaxMoinsUk;
UnSurUkMoinsUmin = Pi->UnSurUkMoinsUmin;
UnSurUmaxMoinsUk = Pi->UnSurUmaxMoinsUk;

InfDeVzerEtZero = VZER;
if ( InfDeVzerEtZero < ZERO ) InfDeVzerEtZero = ZERO;
UnSurInfDeVzerEtZero = 1. / InfDeVzerEtZero;

/* Variables U, S1, S2 */
AlphaK        = 0.5;
UnMoinsAlphaK = 1. - AlphaK;
for ( i = 0 ; i < NombreDeVariables ; i++ ) {
	U0[i] = U[i];		
  U [i]+= DeltaU[i] * CoeffP;
  U0[i] = ( AlphaK * U[i] ) + ( UnMoinsAlphaK * U0[i] );
  Type = TypeDeVariable[i];
  if ( Type == BORNEE ) {
    if ( ( U[i] - Umin[i] ) > ZERO ) {
		  UkMoinsUmin[i] = U[i] - Umin[i];
      UnSurUkMoinsUmin[i] = 1. / UkMoinsUmin[i];
		}
    else {
      U[i]           = Umin[i] + VZER;
      UkMoinsUmin[i] = VZER;
      UmaxMoinsUk[i] = Umax[i] - U[i];						
      UnSurUkMoinsUmin[i] = UnSurInfDeVzerEtZero;			
      if ( UmaxMoinsUk[i] > ZERO ) Div = UmaxMoinsUk[i];
      else Div = ZERO;			
      UnSurUmaxMoinsUk[i] = 1. / Div;					
    }
    if ( ( Umax[i] - U[i] ) > ZERO ) {
		  UmaxMoinsUk[i] = Umax[i] - U[i];
      UnSurUmaxMoinsUk[i] = 1. / UmaxMoinsUk[i];
		}
    else {
      U[i] = Umax[i] - VZER;
      UkMoinsUmin[i] = U[i] - Umin[i];    
      UmaxMoinsUk[i] = VZER;			
      UnSurUmaxMoinsUk[i] = UnSurInfDeVzerEtZero;			
      if ( UkMoinsUmin[i] > ZERO ) Div = UkMoinsUmin[i];
			else Div = ZERO;
      UnSurUkMoinsUmin[i] = 1. / Div;
    }
	}
  else if ( Type == BORNEE_INFERIEUREMENT ) {
    if ( ( U[i] - Umin[i] ) > ZERO ) {
		  UkMoinsUmin[i] = U[i] - Umin[i];
      UnSurUkMoinsUmin[i] = 1. / UkMoinsUmin[i];
		}
    else {
      U[i]           = Umin[i] + VZER;
      UkMoinsUmin[i] = VZER;			
      UnSurUkMoinsUmin[i] = UnSurInfDeVzerEtZero;
    }
	}
  else if ( Type == BORNEE_SUPERIEUREMENT ) {
    if ( ( Umax[i] - U[i] ) > ZERO ) {
		  UmaxMoinsUk[i] = Umax[i] - U[i];
      UnSurUmaxMoinsUk[i] = 1. / UmaxMoinsUk[i];
		}
    else {
      U[i] = Umax[i] - VZER;
      UmaxMoinsUk[i] = VZER;
      UnSurUmaxMoinsUk[i] = UnSurInfDeVzerEtZero;
    }	
  }		
}

for ( i = 0 ; i < NombreDeVariables ; i++ ) {
  Type = TypeDeVariable[i];
  if ( Type == BORNEE ) {
    S1[i]+= DeltaS1[i] * CoeffS;
    S2[i]+= DeltaS2[i] * CoeffS;
    if ( S1[i] < Smin ) S1[i] = Smin;
    if ( S2[i] < Smin ) S2[i] = Smin;
		
  }
  else if ( Type == BORNEE_INFERIEUREMENT ) {
    S1[i]+= DeltaS1[i] * CoeffS;
    if ( S1[i] < Smin ) S1[i] = Smin;
  }
  else if ( Type == BORNEE_SUPERIEUREMENT ) {
    S2[i]+= DeltaS2[i] * CoeffS;
    if ( S2[i] < Smin ) S2[i] = Smin;
  }
}	

/* Variables Lambda */
Lambda      = Pi->Lambda;
Lambda0     = Pi->Lambda0;
DeltaLambda = Pi->DeltaLambda;

AlphaK        = 0.5;
UnMoinsAlphaK = 1. - AlphaK;

for ( i = 0 ; i < NombreDeContraintes ; i++ ) {  
  Lambda0[i] = Lambda[i];	
  Lambda[i]+= CoeffL * DeltaLambda[i];	  
  Lambda0[i] = ( AlphaK * Lambda[i] ) + ( UnMoinsAlphaK * Lambda0[i] );	
}

Amin = 1.e-10;
Amax = 1.e+3;
Vm   = 1.;

Dx = -1.0;
Dm = LINFINI_PI;

for ( i = 0 ; i < NombreDeVariables ; i++ ) {
  Type = TypeDeVariable[i];
  X  = 1;
	X1 = S1[i] * UnSurUkMoinsUmin[i];
	X2 = S2[i] * UnSurUmaxMoinsUk[i];
  if ( Type == BORNEE_INFERIEUREMENT ) {
    if ( X1 < Vm ) X = U[i] - Umin[i];
  }
  else if ( Type == BORNEE_SUPERIEUREMENT ) {
    if ( X2 < Vm ) X = Umax[i] - U[i];
  }
	
	if ( X > Amax ) X = Amax;
	if ( X < Amin ) X = Amin;	
		
  Alpha[i]  = X;
  Alpha2[i] = X * X;

  X = X1 + X2;
	X*= Alpha2[i];
	if ( X > Dx ) Dx = X;
	if ( X < Dm ) Dm = X;
	
}

if ( Pi->Resolution == PARFAIT )     Pi->Range*= 2.;
else if ( Pi->Resolution == APPROX ) Pi->Range/= 2.;

if ( Pi->Range < RANGE_MIN )      Pi->Range = RANGE_MIN;
else if ( Pi->Range > RANGE_MAX ) Pi->Range = RANGE_MAX;

#if VERBOSE_PI 
  printf ("      Dm = %e Dx = %e Range = %e\n",Dm,Dx,Pi->Range);
#endif

if ( Dx - Dm > Pi->Range && Pi->NumeroDIteration >= ITERATION_DEBUT_RAFINEMENT_ITERATIF ) {
  X = sqrt( Pi->Range / ( Dx - Dm ) );
  for ( i = 0 ; i < NombreDeVariables ; i++ ) {
	  Alpha[i]*= X;
	  Alpha2[i]*= X * X;
	}
}

return;
}












