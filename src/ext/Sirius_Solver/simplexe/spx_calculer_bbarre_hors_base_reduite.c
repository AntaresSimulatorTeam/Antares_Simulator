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

   FONCTION: Calcul de BBarre = B^{-1} * b pour les contraintes/variables
             hors base reduite.

                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"  

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"

# define DEBUG NON_SPX  

/*----------------------------------------------------------------------------*/

void SPX_CalculerBBarreAHorsReduite( PROBLEME_SPX * Spx )
{
int il; int ilMax; double * BBarre; double * B; char * PositionDeLaVariable; 
double * Xmax; int NombreDeBornesAuxiliairesUtilisees; char * StatutBorneSupCourante; 
int Cnt; int Var1; int r; double * A; int * Mdeb; int * NbTerm; double ValBBarre;
int * ContrainteDeLaVariableEnBase; int * Indcol; int RangDeLaMatriceFactorisee; 
int * LigneDeLaBaseFactorisee; char Position; double * Bs; int * ColonneDeLaBaseFactorisee;
int Var; double Coeff;

if ( Spx->UtiliserLaBaseReduite == NON_SPX ) return; 

BBarre = Spx->BBarre;
B = Spx->B;
Bs = Spx->Bs;

RangDeLaMatriceFactorisee = Spx->RangDeLaMatriceFactorisee;
LigneDeLaBaseFactorisee = Spx->LigneDeLaBaseFactorisee;
ColonneDeLaBaseFactorisee	= Spx->ColonneDeLaBaseFactorisee;

Xmax = Spx->Xmax;

Mdeb = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
A = Spx->A;
PositionDeLaVariable = Spx->PositionDeLaVariable;
ContrainteDeLaVariableEnBase = Spx->ContrainteDeLaVariableEnBase;

NombreDeBornesAuxiliairesUtilisees = Spx->NombreDeBornesAuxiliairesUtilisees;
StatutBorneSupCourante = Spx->StatutBorneSupCourante;  

/* A ce stade les valeurs de BBarre dans la base reduite sont classes de 0 a RangDeLaMatriceFactorisee.
   On les reclasse par mesure de precaution */
for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) { 
	Bs[ColonneDeLaBaseFactorisee[r]] = BBarre[r]; /* Il faut les sauvegarder dans Bs */
}
/* Puis on les remet dans BBarre */
for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) { 
  Cnt = ColonneDeLaBaseFactorisee[r];
  BBarre[Cnt] =	Bs[Cnt];
	Bs[Cnt] = 0;
}

if ( NombreDeBornesAuxiliairesUtilisees == 0 ) {	
  for ( r = RangDeLaMatriceFactorisee ; r < Spx->NombreDeContraintes ; r++ ) { 
    Cnt = LigneDeLaBaseFactorisee[r];
		Var = Spx->VariableEnBaseDeLaContrainte[ColonneDeLaBaseFactorisee[r]];
    ValBBarre = B[Cnt];
    il    = Mdeb[Cnt];
    ilMax = il + NbTerm[Cnt];
		Coeff = 1;				
    while ( il < ilMax ) {  
	    Var1 = Indcol[il];
			if ( Var1 == Var ) {
			  Coeff = A[il];				
				goto NextIl_0;
			}
	    Position = PositionDeLaVariable[Var1];
	    if ( Position == EN_BASE_LIBRE ) {
	      ValBBarre -= A[il] * BBarre[ContrainteDeLaVariableEnBase[Var1]]; 
	    }
	    else if ( Position == HORS_BASE_SUR_BORNE_SUP ) {
        ValBBarre -= A[il] * Xmax[Var1];
	    }
			NextIl_0:
      il++;
	  }		
		BBarre[ContrainteDeLaVariableEnBase[Var]] = ValBBarre/Coeff; /* C'est la variable non native */
  }
}
else {	
  for ( r = RangDeLaMatriceFactorisee ; r < Spx->NombreDeContraintes ; r++ ) { 
    Cnt = LigneDeLaBaseFactorisee[r];
		Var = Spx->VariableEnBaseDeLaContrainte[ColonneDeLaBaseFactorisee[r]];
    ValBBarre = B[Cnt];
    il    = Mdeb[Cnt];
    ilMax = il + NbTerm[Cnt];
		Coeff = 1;
    while ( il < ilMax ) {  
	    Var1 = Indcol[il];
			if ( Var1 == Var ) {
			  Coeff = A[il];
				goto NextIl_1;
			}			
	    Position = PositionDeLaVariable[Var1];
	    if ( Position == EN_BASE_LIBRE ) {
	      ValBBarre -= A[il] * BBarre[ContrainteDeLaVariableEnBase[Var1]];
	    }
	    else if ( Position == HORS_BASE_SUR_BORNE_SUP ) {
        ValBBarre -= A[il] * Xmax[Var1];
			}
	    else if ( Position == HORS_BASE_SUR_BORNE_INF ) {			
		    if ( StatutBorneSupCourante[Var1] == BORNE_AUXILIAIRE_DE_VARIABLE_NON_BORNEE ) {
				  /*printf("CalculerBBarreAHorsReduite variable non bornee sur borne inf fictive %d\n",Var1);*/
          ValBBarre += A[il] * Xmax[Var1];
		    }
	    }
			NextIl_1:
      il++;
	  }	 		
		BBarre[ContrainteDeLaVariableEnBase[Var]] = ValBBarre/Coeff; /* C'est la variable non native */
  }
}

return;
}
