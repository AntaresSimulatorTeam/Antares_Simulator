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

   FONCTION: Calcul d'une borne auxiliaire pour rendre une variable
	           duale admissible.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# define INF_XMAX 100.
 
/*----------------------------------------------------------------------------*/

# ifdef UTILISER_BORNES_AUXILIAIRES

/*----------------------------------------------------------------------------*/
/* Utilisation de bornes auxilaires pour forcer l'admissibilite duale */
double SPX_CalculerUneBorneAuxiliaire( PROBLEME_SPX * Spx , int Var )	
{
double Amoy; int il; int ilMax; int * Cdeb; int * CNbTerm; int Cnt; double Smin;
double Xmax; double * ACol; int * NumeroDeContrainte; double NormeL1deB; int Var1;
double NormeL1deA; double * B; double Xmx1; double Xmx2; char * TypeDeVariable;
int * Indcol; double * A; double * Xi; double * Xs;

B = Spx->B;

Cdeb    = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;
ACol    = Spx->ACol;
NumeroDeContrainte = Spx->NumeroDeContrainte;

/* On regarde si on dispose d'une borne auxiliaire */			
if ( Spx->TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {		
  if ( Spx->StatutBorneSupAuxiliaire[Var] == BORNE_AUXILIAIRE_PRESOLVE ) {
  	Xmax = Spx->BorneSupAuxiliaire[Var];
		/* Maintenant que la borne auxiliaire a ete utilisee on l'invalide pour ne plus l'utiliser */
    Spx->StatutBorneSupAuxiliaire[Var] = BORNE_AUXILIAIRE_INVALIDE;				
		return( Xmax );
  }
}

B = Spx->B;

Cdeb    = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;
ACol    = Spx->ACol;
NumeroDeContrainte = Spx->NumeroDeContrainte;

TypeDeVariable = Spx->TypeDeVariable;

if ( Spx->OrigineDeLaVariable[Var] == ECART ) goto VariableDEcart;

il    = Cdeb[Var];
ilMax = il + CNbTerm[Var];
Amoy  = 0.0;
NormeL1deB = 0.0;
NormeL1deA = 0.0;						
while ( il < ilMax ) {							
  NormeL1deB+= fabs( B[NumeroDeContrainte[il]] );
  NormeL1deA+= fabs( ACol[il] );
  Amoy+= fabs( ACol[il] );								   
  il++;
}
Amoy/= CNbTerm[Var];
Xmx1 = (1. + Spx->ValeurMoyenneDuSecondMembre ) / ( 1. + Amoy );            
Xmx2 = (1. + NormeL1deB ) / ( 1. + NormeL1deA );						
if ( Xmx1 > Xmx2 ) Xmax = Xmx1;
else Xmax = Xmx2;

goto Fin;

VariableDEcart:
/* Variable d'ecart */
Indcol = Spx->Indcol;
A = Spx->A;
Xi = Spx->Xmin;
Xs = Spx->Xmax;
Cnt = NumeroDeContrainte[Cdeb[Var]];
il = Spx->Mdeb[Cnt];
ilMax = il + Spx->NbTerm[Cnt] - 1;
Smin = 0;
while ( il < ilMax ) {
  Var1 = Indcol[il];
  if ( A[il] > 0 ) {
		/* On place la variable au min */
		if ( TypeDeVariable[Var1] != VARIABLE_NON_BORNEE ) {
			Smin += A[il] * Xi[Var1];
		}
		else {
			Smin += A[il] * (-1.e+6);
		}
	}
	else if ( A[il] < 0 ) {
	  /* On place la variable au max */
		if ( TypeDeVariable[Var1] != VARIABLE_NON_BORNEE && TypeDeVariable[Var1] != VARIABLE_BORNEE_INFERIEUREMENT ) {
			Smin += A[il] * Xs[Var1];
		}
		else Smin += A[il] * 1.e+6;	
	}
	il++;	
}
Xmax = fabs( B[Cnt] - Smin );	
Xmax += 10.; /* Pour qu'elle soit plutot en base plutot que sur borne sup */
if ( Xmax > 1.e+7 ) Xmax = 1.e+7;

Fin:

if ( TypeDeVariable[Var] == NON_BORNEE ) Xmax *= 2.0;
Xmax *= (double) Spx->CoeffMultiplicateurDesBornesAuxiliaires;

if ( Xmax < INF_XMAX ) Xmax = INF_XMAX;

return( Xmax );     

}

# endif





