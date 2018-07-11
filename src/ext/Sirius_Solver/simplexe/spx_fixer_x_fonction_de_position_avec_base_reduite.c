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

   FONCTION: On calcule les valeurs de X en fonction de la position 
             hors base ou en base.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# define CYCLE 10

/*----------------------------------------------------------------------------*/

void SPX_FixerXEnFonctionDeSaPositionAvecBaseReduite( PROBLEME_SPX * Spx )
{
int Var; double X; double * ArrayX; char * TypeDeVariable; int Var1; int Cnt;
double SommeDesInfaisabilitesPrimales; double * ArrayXmax; double * BBarre; double * B;
char * ArrayPositionDeLaVariable; int r; char * StatutBorneSupCourante; int Colonne;
int * LigneDeLaBaseFactorisee; int * OrdreColonneDeLaBaseFactorisee; int RangDeLaMatriceFactorisee;
int * ColonneDeLaBaseFactorisee; int * Mdeb; int * NbTerm; int * Indcol; double * A;
int il; int ilMax; double Coeff; int * VariableEnBaseDeLaContrainte; char CalculHorsBaseReduite;
int NbViole;

ArrayX         = Spx->X;
ArrayXmax      = Spx->Xmax;
BBarre         = Spx->BBarre;
TypeDeVariable = Spx->TypeDeVariable;
ArrayPositionDeLaVariable    = Spx->PositionDeLaVariable;
VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;
StatutBorneSupCourante       = Spx->StatutBorneSupCourante;
SommeDesInfaisabilitesPrimales = 0.;

RangDeLaMatriceFactorisee = Spx->RangDeLaMatriceFactorisee;
OrdreColonneDeLaBaseFactorisee = Spx->OrdreColonneDeLaBaseFactorisee;
ColonneDeLaBaseFactorisee = Spx->ColonneDeLaBaseFactorisee;
LigneDeLaBaseFactorisee = Spx->LigneDeLaBaseFactorisee;

B = Spx->B;
Mdeb = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
A = Spx->A;

if ( Spx->CalculValeurDesVariablesBasiquesHorsBaseReduite <= 0 ) {
  CalculHorsBaseReduite = OUI_SPX;
	Spx->CalculValeurDesVariablesBasiquesHorsBaseReduite = CYCLE;
}
else {
  CalculHorsBaseReduite = NON_SPX;
	Spx->CalculValeurDesVariablesBasiquesHorsBaseReduite--;
}

if ( CalculHorsBaseReduite == NON_SPX ) {
  for ( Var = 0 ; Var < Spx->NombreDeVariables ;  Var++ ) ArrayX[Var] = 0;
}

if ( Spx->NombreDeBornesAuxiliairesUtilisees == 0 ) {
  /* Calcul pour les variables hors base */
  for ( Var = 0 ; Var < Spx->NombreDeVariables ;  Var++ ) {
    if ( ArrayPositionDeLaVariable[Var] == EN_BASE_LIBRE ) continue;
    if ( ArrayPositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) ArrayX[Var] = ArrayXmax[Var];
	  else ArrayX[Var] = 0.0;		
	}
}
else {
  /* Calcul pour les variables hors base */
  for ( Var = 0 ; Var < Spx->NombreDeVariables ;  Var++ ) {
    if ( ArrayPositionDeLaVariable[Var] == EN_BASE_LIBRE ) continue;
    if ( ArrayPositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) ArrayX[Var] = ArrayXmax[Var];
	  else {
	    /* La variable est HORS_BASE_A_ZERO ou HORS_BASE_SUR_BORNE_INF */
	    ArrayX[Var] = 0.0;
      if ( ArrayPositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF ) {
		    if ( StatutBorneSupCourante[Var] == BORNE_AUXILIAIRE_DE_VARIABLE_NON_BORNEE ) {
	        /* La variable est hors base sur borne inf mais qu'elle a une borne avec un StatutBorneSupCourante egal a
				     BORNE_AUXILIAIRE_DE_VARIABLE_NON_BORNEE alors elle est a -Xmax[Var] */
	        ArrayX[Var] = -ArrayXmax[Var];				
	      }
		  }
		}
	}
}

/* La base reduite */
for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) { 
	Colonne = ColonneDeLaBaseFactorisee[r];	
	Var = VariableEnBaseDeLaContrainte[ColonneDeLaBaseFactorisee[r]];
  X = BBarre[r];
  /* Attention ici on utilise le fait que par translation on a toujours Xmin = 0. */
  if ( X < 0. && TypeDeVariable[Var] != NON_BORNEE ) SommeDesInfaisabilitesPrimales += -X;	
  else {
    if ( X > ArrayXmax[Var] ) SommeDesInfaisabilitesPrimales += X - ArrayXmax[Var];			
  }		
  ArrayX[Var] = X;			
}

/* La partie hors base reduite */
NbViole = 0;
if ( CalculHorsBaseReduite == OUI_SPX ) {
  for ( r = RangDeLaMatriceFactorisee ; r < Spx->NombreDeContraintes ; r++ ) { 
	  Colonne = ColonneDeLaBaseFactorisee[r];
	  Var = VariableEnBaseDeLaContrainte[ColonneDeLaBaseFactorisee[r]];
	  Cnt = LigneDeLaBaseFactorisee[r];
    X = B[Cnt];
    il    = Mdeb[Cnt];
    ilMax = il + NbTerm[Cnt];
	  Coeff = 1;				
    while ( il < ilMax ) {  
	    Var1 = Indcol[il];
		  if ( Var1 == Var ) Coeff = A[il];				
		  else X -= A[il] * ArrayX[Var1];			
      il++;
	  }
    X /= Coeff;
    /* Attention ici on utilise le fait que par translation on a toujours Xmin = 0. */
    if ( X < 0. && TypeDeVariable[Var] != NON_BORNEE ) {
	    SommeDesInfaisabilitesPrimales += -X;
		  NbViole++;
	  }
    else if ( X > ArrayXmax[Var] ) {
	    SommeDesInfaisabilitesPrimales += X - ArrayXmax[Var];
		  NbViole++;
    }		
	  ArrayX[Var] = X; /* C'est la variable non native */				
  }
  Spx->NombreDeVariablesBasiquesHorsBaseReduiteViolees = NbViole;	
}

Spx->SommeDesInfaisabilitesPrimales = SommeDesInfaisabilitesPrimales;
	
return;
}

