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

   FONCTION: Armijo
   
                
   AUTEUR: R. GONZALEZ

************************************************************************************/

# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

# ifdef PI_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pi_memoire.h"
# endif

/*------------------------------------------------------------------------*/

double PI_Armijo( PROBLEME_PI * Pi , double CoeffLimite )
{
double t; double tg ; double td; double q0; double qt; double m1;
double * G; double * E; double qPrime0; double Muk;
double * U; double * Lambda; int Var; int Cnt;
double * S1; double * S2; int k; int NbIter;

k = ( 3 * Pi->NombreDeVariables ) + Pi->NombreDeContraintes;
G = (double *) malloc( k * sizeof( double ) );
E = (double *) malloc( k * sizeof( double ) );
S1     = (double *) malloc( Pi->NombreDeVariables   * sizeof( double ) );
S2     = (double *) malloc( Pi->NombreDeVariables   * sizeof( double ) );
U      = (double *) malloc( Pi->NombreDeVariables   * sizeof( double ) );
Lambda = (double *) malloc( Pi->NombreDeContraintes * sizeof( double ) );

Muk = Pi->Muk;
td = CoeffLimite;
m1 = 0.1;
NbIter = 0;

/* Sauvegarde des variables */
t = 0.0;
PI_CalculDesVariables( Pi , t , U , Lambda , S1 , S2 );

q0 = PI_CalculNormeSecondMembre( Pi , Muk , E , U , Lambda , S1 , S2 );

qPrime0 = PI_CalculDeLaDeriveeDuCritereEnZero( Pi , Muk , E , G , U , Lambda , S1 , S2 );

t = td;

printf("t = %e q0 = %e qPrime0 = %e\n",t,q0,qPrime0);
if ( qPrime0 > 0.0 ) goto qPrime0Positif;

CalculDeqt:
NbIter++;

PI_CalculDesVariables( Pi , t , U , Lambda , S1 , S2 );
qt = PI_CalculNormeSecondMembre( Pi , Muk , E , U , Lambda , S1 , S2 );

if ( (qt-q0)/t > m1 * qPrime0 ) {
  /* t trop grand */
  td = t;
  t = 0.75 * td;
  goto CalculDeqt;
}
printf("t = %e qt = %e\n",t,qt);
if ( NbIter > 1 ) printf("Attention nombre d'iterations Armijo superieur a 1: %d\n",NbIter);

qPrime0Positif:
if ( qPrime0 > 0.0 ) {
  printf("La direction ne descend pas, on applique le pas initial\n");
  t = CoeffLimite;
  PI_CalculDesVariables( Pi , t , U , Lambda , S1 , S2 );
  qt = PI_CalculNormeSecondMembre( Pi , Muk , E , U , Lambda , S1 , S2 );
  printf("avec ce pas de %e la prediction est de %e\n",t,qt);
  printf("On prend 0.1 fois le pas\n");
  t = 0.1 * CoeffLimite;
  PI_CalculDesVariables( Pi , t , U , Lambda , S1 , S2 );
  qt = PI_CalculNormeSecondMembre( Pi , Muk , E , U , Lambda , S1 , S2 );
  printf("avec ce pas de %e la prediction est de %e\n",t,qt);
}

free( G );
free( E );
free( U );
free( Lambda );
free( S1 );
free( S2 );

G = NULL;
E = NULL;
U = NULL;
Lambda = NULL;
S1 = NULL;
S2 = NULL;

return( t );
}

/*------------------------------------------------------------------------*/

void PI_CalculDesVariables( PROBLEME_PI * Pi , double t , double * U ,
                            double * Lambda , double * S1 , double * S2 )
{
int Var; int Cnt;

for ( Var = 0 ; Var < Pi->NombreDeVariables ; Var++ ) {  
  U [Var] = Pi->U[Var] + ( t * Pi->DeltaU[Var] );
  S1[Var] = Pi->S1[Var] + ( t * Pi->DeltaS1[Var] );
  S2[Var] = Pi->S2[Var] + ( t * Pi->DeltaS2[Var] );
}
for ( Cnt = 0 ; Cnt < Pi->NombreDeContraintes ; Cnt++ ) {
  Lambda[Cnt] = Pi->Lambda[Cnt] + ( t * Pi->DeltaLambda[Cnt] );
}

return;
}

/*------------------------------------------------------------------------*/

double PI_CalculDeLaDeriveeDuCritereEnZero( PROBLEME_PI * Pi ,
                                            double Muk , double * E , double * G , double * U ,
                                            double * Lambda , double * S1 , double * S2 )
{
int Var; int Cnt; double X; double S; int il; int ilMax; double qPrime0;

/* Calcul du gradient du critere en x */

for ( Var = 0 ; Var < Pi->NombreDeVariables ; Var++ ) {
  /* Derivees en S1 */
  S = 0.0;
  X = 0.0;
  if ( Pi->TypeDeVariable[Var] == BORNEE || Pi->TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) X = -( U[Var] - Pi->Umin[Var] ) / Pi->Alpha[Var];
  S+= X * E[Var];
    
  X = 0.0;
  if ( Pi->TypeDeVariable[Var] == BORNEE || Pi->TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) X = 1.;			 
  S+= X * E[(2 * Pi->NombreDeVariables) + Var];
  
  G[Var] = S;  
  
  /* Derivees en S2 */
  S = 0.0;
  X = 0.0;
  if ( Pi->TypeDeVariable[Var] == BORNEE || Pi->TypeDeVariable[Var] == BORNEE_SUPERIEUREMENT ) X = -( Pi->Umax[Var] - U[Var] ) / Pi->Alpha[Var];
  S+= X * E[Pi->NombreDeVariables + Var];

  X = 0.0;
  if ( Pi->TypeDeVariable[Var] == BORNEE || Pi->TypeDeVariable[Var] == BORNEE_SUPERIEUREMENT ) X = -1.;			 
  S+= X * E[(2 * Pi->NombreDeVariables) + Var];

  G[Pi->NombreDeVariables + Var] = S;  

  /* Derivees en U */
  S = 0.0;
  X = 0.0;
  if ( Pi->TypeDeVariable[Var] == BORNEE || Pi->TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) X = -S1[Var] * Pi->Alpha[Var];
  S+= X * E[Var];

  X = 0.0;
  if ( Pi->TypeDeVariable[Var] == BORNEE || Pi->TypeDeVariable[Var] == BORNEE_SUPERIEUREMENT ) X = S2[Var] * Pi->Alpha[Var];
  S+= X * E[Pi->NombreDeVariables + Var];
 
  X = - Pi->Qpar2[Var] * Pi->Alpha[Var];
  /* Terme de regularisation eventuellement ajoute */
  X-= Pi->RegulVar[Var] * Pi->Alpha[Var];
	
  S+= X * E[(2 * Pi->NombreDeVariables) + Var];

  il    = Pi->Cdeb[Var];
  ilMax = il + Pi->CNbTerm[Var];
  while ( il < ilMax ) {
    Cnt = Pi->NumeroDeContrainte[il];
    X = -Pi->ACol[il] * Pi->Alpha[Var];
    S+= X * E[(3 * Pi->NombreDeVariables) + Cnt];    
    il++;
  }
  G[ (2 * Pi->NombreDeVariables) + Var] = S;
  
}

for ( Cnt = 0 ; Cnt < Pi->NombreDeContraintes ; Cnt++ ) {
  /* Derivees en Lambda */
  /*        */   
  S = 0.;
  il    = Pi->Mdeb[Cnt];
  ilMax = il + Pi->NbTerm[Cnt];
  while ( il < ilMax ) {
    Var = Pi->Indcol[il]; 
    X = -Pi->A[il] * Pi->Alpha[Var];
    S+= X * E[(2 * Pi->NombreDeVariables) + Var];
    il++;
  }
  /* Terme de regularisation eventuellement ajoute */
  X = -Pi->RegulContrainte[Cnt]; 
  S+= X * E[(3 * Pi->NombreDeVariables) + Var];
  
  G[(3 * Pi->NombreDeVariables) + Cnt] = S;  
}

qPrime0 = 0.0;
for ( Var = 0 ; Var < Pi->NombreDeVariables ; Var++ ) {
  qPrime0+= G[Var] * Pi->DeltaS1[Var] * Pi->Alpha[Var];
  qPrime0+= G[Pi->NombreDeVariables + Var] * Pi->DeltaS2[Var] * Pi->Alpha[Var];
  qPrime0+= G[ (2 * Pi->NombreDeVariables) + Var] * Pi->DeltaU[Var] / Pi->Alpha[Var];
}
for ( Cnt = 0 ; Cnt < Pi->NombreDeContraintes ; Cnt++ ) {
  qPrime0+= G[ (3 * Pi->NombreDeVariables) + Cnt] * Pi->DeltaLambda[Cnt];
}

return( qPrime0 );
}

/*------------------------------------------------------------------------*/

double PI_CalculNormeSecondMembre( PROBLEME_PI * Pi , double Muk , double * E , double * U ,
                                   double * Lambda , double * S1 , double * S2 )
{
int Var; double X; int il; int ilMax; double Norme; int Cnt; 

Norme = 0.;

/* Partie variables */
for ( Var = 0 ; Var < Pi->NombreDeVariables ; Var++ ) {
  X = 0.0;
  if ( Pi->TypeDeVariable[Var] == BORNEE || Pi->TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {
    X = S1[Var] * ( U[Var] - Pi->Umin[Var] );			 
  }
  X = Muk - X;
  E[Var] = X;
  Norme+= X * X;
}

for ( Var = 0 ; Var < Pi->NombreDeVariables ; Var++ ) {
  X = 0.0;
  if ( Pi->TypeDeVariable[Var] == BORNEE || Pi->TypeDeVariable[Var] == BORNEE_SUPERIEUREMENT ) {
    X = S2[Var] * ( Pi->Umax[Var] - U[Var] );			 
  }
  X = Muk - X;
  E[Pi->NombreDeVariables + Var] = X;
  Norme+= X * X;
}

for ( Var = 0 ; Var < Pi->NombreDeVariables ; Var++ ) { 
  /* Partie gradient du critere */
  X = ( Pi->Qpar2[Var] * U[Var] ) + Pi->L[Var];
  /* Terme de regularisation eventuellement ajoute */
  X+= Pi->RegulVar[Var] * ( U[Var] - Pi->U0[Var] );
  /* Partie gradient transpose des contraintes */
  il    = Pi->Cdeb[Var];
  ilMax = il + Pi->CNbTerm[Var];
  while ( il < ilMax ) {
    Cnt = Pi->NumeroDeContrainte[il];
    X+= Pi->ACol[il] * Lambda[Cnt];
    il++;
  }
  /* Changement de signe */
  X = -X;
  /* Partie en S1 et S2 */
  if ( Pi->TypeDeVariable[Var] == BORNEE ) {
    X+= S1[Var] - S2[Var];
  }
  else if ( Pi->TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {
    X+= S1[Var];
  }
  else if ( Pi->TypeDeVariable[Var] == BORNEE_SUPERIEUREMENT ) {
    X+= -S2[Var];
  }
  E[(2 * Pi->NombreDeVariables) + Var] = X * Pi->Alpha[Var];
  Norme+= X * X;
} 

/* Partie Contraintes */
for ( Cnt = 0 ; Cnt < Pi->NombreDeContraintes ; Cnt++ ) {
  X = 0.;
  /* Terme de regularisation eventuellement ajoute */
  X+= Pi->RegulContrainte[Cnt] * ( Pi->Lambda[Cnt] - Pi->Lambda0[Cnt] ); 
  /*        */ 
  il    = Pi->Mdeb[Cnt];
  ilMax = il + Pi->NbTerm[Cnt];
  while ( il < ilMax ) {
    Var = Pi->Indcol[il];
    X+= Pi->A[il] * U[Var];
    il++;
  }
  X = Pi->B[Cnt] - X;
    
  E[(3 * Pi->NombreDeVariables) + Cnt] = X;
  Norme+= X * X;
}

Norme = 0.5 * Norme;

return( Norme );
}
        
  

 
