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

   FONCTION: 
   
                
   AUTEUR: R. GONZALEZ

************************************************************************************/

# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

/*------------------------------------------------------------------------*/
/*                  Calcul du coefficient barriere                        */ 

void PI_Calmuk( PROBLEME_PI * Pi )
{
int Var; double Ga   ; double TetakP         ; double TetakD   ; double Produit;
double * UkMoinsUmin  ; double * UmaxMoinsUk  ; double * DeltaU ; double * S1   ;
double *S2            ; double * DeltaS1      ; double * DeltaS2                ; 
char * TypeDeVariable ; double * DeltaUDeltaS1; double * DeltaUDeltaS2          ;
int NombreDeVariables; char Type;

PI_CalculDesTeta( Pi , &TetakP , &TetakD );

UkMoinsUmin = Pi->UkMoinsUmin;
UmaxMoinsUk = Pi->UmaxMoinsUk;
DeltaU      = Pi->DeltaU;
S1 = Pi->S1;
S2 = Pi->S2;
DeltaS1 = Pi->DeltaS1;
DeltaS2 = Pi->DeltaS2;

NombreDeVariables = Pi->NombreDeVariables;

TypeDeVariable = Pi->TypeDeVariable;

DeltaUDeltaS1 = Pi->DeltaUDeltaS1;
DeltaUDeltaS2 = Pi->DeltaUDeltaS2;

if ( Pi->NumeroDIteration <= 1 ) {
  for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
    DeltaUDeltaS1[Var] = 0.; 
    DeltaUDeltaS2[Var] = 0.; 	
	}
}

Produit = TetakP * TetakD;
Ga      = 0.0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  Type = TypeDeVariable[Var];
  if ( Type == BORNEE ) {
    Ga+= ( UkMoinsUmin[Var] + ( DeltaU[Var] * TetakP ) ) * ( S1[Var] + ( DeltaS1[Var] * TetakD ));
    Ga+= ( UmaxMoinsUk[Var] - ( DeltaU[Var] * TetakP ) ) * ( S2[Var] + ( DeltaS2[Var] * TetakD ));
 		DeltaUDeltaS1[Var] = DeltaU[Var] * DeltaS1[Var] * Produit;
    DeltaUDeltaS2[Var] = DeltaU[Var] * DeltaS2[Var] * Produit;
  }
  else if ( Type == BORNEE_INFERIEUREMENT ) {
    Ga+= ( UkMoinsUmin[Var] + ( DeltaU[Var] * TetakP ) ) * ( S1[Var] + ( DeltaS1[Var] * TetakD ));
		DeltaUDeltaS1[Var] = DeltaU[Var] * DeltaS1[Var] * Produit;		
  }
  else if ( Type == BORNEE_SUPERIEUREMENT ) {
    Ga+= ( UmaxMoinsUk[Var] - ( DeltaU[Var] * TetakP ) ) * ( S2[Var] + ( DeltaS2[Var] * TetakD ));
		DeltaUDeltaS2[Var] = DeltaU[Var] * DeltaS2[Var] * Produit;
  }
}
Ga = fabs( Ga ); /* Pour les cas extremes ou il peut y avoir des imprecisions qui donnent
                    un nombre negatif. Si on met 0 cela revient a rester sur une iteration
		    affine donc on prend fabs */
Pi->Muk = Ga / Pi->G; 

Pi->Muk = pow( Pi->Muk , /*2.0*/ 0.5 );

Pi->Muk = Pi->Muk * ( Ga * Pi->Ro );

if ( Pi->Muk < MUK_MIN ) Pi->Muk = MUK_MIN;
if ( Pi->Muk > MUK_MAX ) Pi->Muk = MUK_MAX;

return;
}

/*------------------------------------------------------------------------*/
/*                  Correction pour la centralite                         */ 

void PI_Calcent( PROBLEME_PI * Pi )
{
int Var; char * TypeDeVariable   ; double * SecondMembre   ; double * UnSurUkMoinsUmin  ;
double * UnSurUmaxMoinsUk         ; double * DeltaUDeltaS1  ; double *DeltaUDeltaS2      ;
double Muk; int NombreDeVariables; int NombreDeContraintes; int Cnt; double * RegulVar;
double * U; double * U0           ; double * RegulContrainte; double * Lambda            ;
double * Lambda0                  ; double X; double * Alpha; char Type;

NombreDeVariables   = Pi->NombreDeVariables;
NombreDeContraintes = Pi->NombreDeContraintes;

memcpy( (char *) Pi->SecondMembre, (char *) Pi->SecondMembreAffine, 
        ( NombreDeVariables + NombreDeContraintes ) * sizeof( double ) );

TypeDeVariable   = Pi->TypeDeVariable;
SecondMembre     = Pi->SecondMembre;
UnSurUkMoinsUmin = Pi->UnSurUkMoinsUmin;
UnSurUmaxMoinsUk = Pi->UnSurUmaxMoinsUk;
DeltaUDeltaS1    = Pi->DeltaUDeltaS1;
DeltaUDeltaS2    = Pi->DeltaUDeltaS2;

RegulVar = Pi->RegulVar;
U        = Pi->U;
U0       = Pi->U0;
RegulContrainte = Pi->RegulContrainte;
Lambda          = Pi->Lambda;
Lambda0         = Pi->Lambda0;

Alpha = Pi->Alpha;

Muk = Pi->Muk;

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  X = 0.0;
	Type = TypeDeVariable[Var];
  if ( Type == BORNEE ) {
    X = ( Muk * ( UnSurUkMoinsUmin[Var] - UnSurUmaxMoinsUk[Var] ) )    
              - ( UnSurUkMoinsUmin[Var] * DeltaUDeltaS1[Var] ) 
              - ( UnSurUmaxMoinsUk[Var] * DeltaUDeltaS2[Var] );			 
  }
  else if ( Type == BORNEE_INFERIEUREMENT ) {
    X = ( Muk * UnSurUkMoinsUmin[Var] )    
          - ( UnSurUkMoinsUmin[Var] * DeltaUDeltaS1[Var] );
  }
  else if ( Type == BORNEE_SUPERIEUREMENT ) {
    X = ( Muk * ( - UnSurUmaxMoinsUk[Var] ) )    
          - ( UnSurUmaxMoinsUk[Var] * DeltaUDeltaS2[Var] );  
	
  }
	SecondMembre[Var]+= X * Alpha[Var];
	
  /* Ajout des termes correctifs pour tenir compte de la regularisation issus de la derniere
     factorisation */	
  SecondMembre[Var]-= ( RegulVar[Var] * ( U[Var] - U0[Var] ) ) * Alpha[Var];
	
}

/* Ajout des termes correctifs pour tenir compte de la regularisation issus de la derniere
   factorisation */

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  /*SecondMembre[NombreDeVariables + Cnt]-= RegulContrainte[Cnt] * ( Lambda[Cnt] - Lambda0[Cnt] );*/
  SecondMembre[NombreDeVariables + Cnt]+= RegulContrainte[Cnt] * ( Lambda[Cnt] - Lambda0[Cnt] );
}

return;
}

/*------------------------------------------------------------------------*/
/*                Calcul de l'ecart de complementarite                    */  

void PI_Caldel( PROBLEME_PI * Pi )
{ 
int Var; int Icopt   ; double Ecopt; double x; double X; char * TypeDeVariable;
double * UkMoinsUmin   ; double * UmaxMoinsUk  ; double * S1; double *S2        ;
double SeuilDOptimalite; double G; char Type;

TypeDeVariable   = Pi->TypeDeVariable;

UkMoinsUmin = Pi->UkMoinsUmin;
UmaxMoinsUk = Pi->UmaxMoinsUk;
S1 = Pi->S1;
S2 = Pi->S2;
SeuilDOptimalite = Pi->SeuilDOptimalite;

Pi->G = 0.;
Ecopt = 0.;
Icopt = -1;

G = Pi->G;

for ( Var = 0 ; Var < Pi->NombreDeVariables ; Var++ ) {
  Type = TypeDeVariable[Var];
  if ( Type == BORNEE ) {
    x = UkMoinsUmin[Var] * S1[Var];
    X = fabs( x );
    if ( X > SeuilDOptimalite ) {
      if ( X > Ecopt ) {
        Ecopt = X;
				Icopt = Var;
      }
    }
    G+= x;
    x = UmaxMoinsUk[Var] * S2[Var];
    X = fabs( x );
    if ( X > SeuilDOptimalite ) {
      if ( X > Ecopt ) {
        Ecopt = X;
				Icopt = Var;
      }
    }
    G+= x;
  }
  else if ( Type == BORNEE_INFERIEUREMENT ) {
    x = UkMoinsUmin[Var] * S1[Var];
    X = fabs( x );
    if ( X > SeuilDOptimalite ) {    
      if ( X > Ecopt ) {
        Ecopt = X;
				Icopt = Var;
      }
    }
    G+= x;  
  }
  else if ( Type == BORNEE_SUPERIEUREMENT ) {  
    x = UmaxMoinsUk[Var] * S2[Var];
    X = fabs( x );
    if ( X > SeuilDOptimalite ) {    
      if ( X > Ecopt ) {
        Ecopt = X;
				Icopt = Var;
      }
    }
    G+= x;
  }
  /* Si non bornee rien a faire */
}

if ( G == 0.0 ) G = 1.0;
Pi->G = G;

/* Test d'optimalite */
Pi->MaxOptimalite[Pi->NumeroDIteration-1] = Ecopt;
if ( Ecopt > Pi->SeuilDOptimalite ) Pi->ArretOpt = NON_PI;

#if VERBOSE_PI                                 
  printf("Optimalite   : seuil %e ecart max %e ", Pi->SeuilDOptimalite,Ecopt);
  if ( Icopt >= 0 ) {
    printf("variable   %d   min %e valeur %e max %e S1 %e S2 %e ",
            Icopt,Pi->Umin[Icopt],Pi->U[Icopt],Pi->Umax[Icopt],Pi->S1[Icopt],Pi->S2[Icopt]);								
    if ( Pi->TypeDeVariable[Icopt] == BORNEE ) printf(" BORNEE\n");
    else if ( Pi->TypeDeVariable[Icopt] == BORNEE_INFERIEUREMENT ) printf(" BORNEE_INFERIEUREMENT\n");
    else if ( Pi->TypeDeVariable[Icopt] == BORNEE_SUPERIEUREMENT ) printf(" BORNEE_SUPERIEUREMENT\n");
    else if ( Pi->TypeDeVariable[Icopt] == NON_BORNEE ) printf(" NON_BORNEE\n");				
  }
  else {
    printf("\n");
  }
  
#endif

return;
}

