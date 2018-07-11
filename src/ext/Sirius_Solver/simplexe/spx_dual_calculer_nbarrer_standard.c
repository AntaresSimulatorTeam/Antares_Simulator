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

# define COEFF_VARIABLES   0.75

/*----------------------------------------------------------------------------*/

void SPX_DualCalculerNBarreRStandard( PROBLEME_SPX * Spx )

{
int il; int ilMax; double X; int i; int NombreDeContraintes;  int * NumerosDesVariablesHorsBase;
double * ErBMoinsUn; double * NBarreR; int * Cdeb; int * CNbTerm; double * ACol;
int * NumeroDeContrainte; int Cnt; int * Mdeb; int * NbTerm; int * Indcol; double * A;
int NombreDeVariablesHorsBase; int NombreDeVariables; char Methode; int Var;
char * PositionDeLaVariable; int * NombreDeVariablesHorsBaseDeLaContrainte;
int * IndicesDeLigne; int * LigneDeLaBaseFactorisee;

NombreDeContraintes         = Spx->NombreDeContraintes;   
ErBMoinsUn                  = Spx->ErBMoinsUn; 
NombreDeVariables           = Spx->NombreDeVariables;
NombreDeVariablesHorsBase   = Spx->NombreDeVariablesHorsBase;
NumerosDesVariablesHorsBase = Spx->NumerosDesVariablesHorsBase;
NBarreR                     = Spx->NBarreR;   

Spx->TypeDeStockageDeNBarreR = VECTEUR_SPX;

/* Si ErBMoinsUn est tres creux il vaut mieux la methode 1, sinon la methode 2 permet de faire
   un calcul plus precis */

if ( Spx->InverseProbablementDense == OUI_SPX ) {
  Methode = 2;
}
else {
  if ( NombreDeContraintes < (int) ceil ( COEFF_VARIABLES * NombreDeVariables ) ) {
    Methode = 1;
  }
  else { 
    Methode = 2;
  }
}

/* 2 methodes de calcul des produits scalaires */

if ( Methode == 1 ) {
  /*memset( (char *) NBarreR , 0 , NombreDeVariables * sizeof( double ) );*/
	for ( i = 0 ; i < NombreDeVariables ; i++ ) NBarreR[i] = 0;
	
  Mdeb   = Spx->Mdeb;
  NbTerm = Spx->NbTerm;

	PositionDeLaVariable = Spx->PositionDeLaVariable;
	
	NombreDeVariablesHorsBaseDeLaContrainte = Spx->NombreDeVariablesHorsBaseDeLaContrainte;
	
  Indcol = Spx->Indcol;
  A      = Spx->A;
  for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
    if ( ErBMoinsUn[Cnt] == 0.0 ) continue;
		if ( NombreDeVariablesHorsBaseDeLaContrainte[Cnt] == 0 ) continue;		
    X  = ErBMoinsUn[Cnt];		
    il = Mdeb[Cnt];
    ilMax = il + NbTerm[Cnt];
    while ( il < ilMax ) {
			if ( PositionDeLaVariable[Indcol[il]] != EN_BASE_LIBRE ) {
			  NBarreR[Indcol[il]] += X * A[il];
			}
      il++;
		}		    		
  }	
}
else {
  if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
    Cdeb = Spx->CdebProblemeReduit;
    CNbTerm = Spx->CNbTermProblemeReduit;
    IndicesDeLigne = Spx->IndicesDeLigneDesTermesDuProblemeReduit;
    ACol = Spx->ValeurDesTermesDesColonnesDuProblemeReduit;			
	  LigneDeLaBaseFactorisee = Spx->LigneDeLaBaseFactorisee;
    for ( i = 0 ; i < NombreDeVariablesHorsBase ; i++ ) {
	    Var = NumerosDesVariablesHorsBase[i];
      il    = Cdeb[Var];				
      ilMax = il + CNbTerm[Var];
      X = 0;
      while ( il < ilMax ) {
        X += ErBMoinsUn[LigneDeLaBaseFactorisee[IndicesDeLigne[il]]] * ACol[il];
        il++;
      }			 
      NBarreR[Var] = X;		
    }			
  }
  else {
    Cdeb = Spx->Cdeb;
    CNbTerm = Spx->CNbTerm;
    NumeroDeContrainte = Spx->NumeroDeContrainte;
    ACol = Spx->ACol;

    for ( i = 0 ; i < NombreDeVariablesHorsBase ; i++ ) {
	    Var = NumerosDesVariablesHorsBase[i];
      il    = Cdeb[Var];				
      ilMax = il + CNbTerm[Var];
      X = 0;
      while ( il < ilMax ) {
        X += ErBMoinsUn[NumeroDeContrainte[il]] * ACol[il];
        il++;
      }			 
      NBarreR[Var] = X;		
    }				
  }
}

return;
}



