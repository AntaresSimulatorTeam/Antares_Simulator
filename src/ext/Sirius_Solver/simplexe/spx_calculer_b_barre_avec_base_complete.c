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

   FONCTION: Calcul de BBarre = B^{-1} * b 

                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"  

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"

/*----------------------------------------------------------------------------*/

void SPX_CalculerBBarreAvecBaseComplete( PROBLEME_SPX * Spx )
{
int Var; int il; int ilMax; char Save; char SecondMembreCreux;  double * BBarre;
double * B; char * PositionDeLaVariable; int * NumeroDeContrainte; double * ACol;
int * Cdeb; int * CNbTerm; double * Xmax; double XmaxDeVar; 
int NombreDeBornesAuxiliairesUtilisees; char * StatutBorneSupCourante;
char TypeDEntree; char TypeDeSortie; char CalculEnHyperCreux;

BBarre = Spx->BBarre;
B      = Spx->B;
memcpy( (char *) BBarre , ( char *) B , Spx->NombreDeContraintes * sizeof( double ) );

PositionDeLaVariable = Spx->PositionDeLaVariable;
NumeroDeContrainte   = Spx->NumeroDeContrainte;

ACol    = Spx->ACol;
Cdeb    = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;

Xmax = Spx->Xmax;

NombreDeBornesAuxiliairesUtilisees = Spx->NombreDeBornesAuxiliairesUtilisees;
StatutBorneSupCourante             = Spx->StatutBorneSupCourante;

/* On calcule B^{-1} * N * Xmax pour les variables hors base sur borne sup */
/* Ne pas utiliser la liste des variables hors base car elle evolue */
if ( NombreDeBornesAuxiliairesUtilisees == 0 ) {
  /* Seules les variables natives peuvent se trouver sur borne sup. Certes
	   les variables aditionnelles de contraintes d'egalite aussi mais leur
		 borne sup est nulle de toutes facons */
  for ( Var = 0 ; Var < Spx->NombreDeVariablesNatives ; Var++ ) { 
    if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) { 
	    if ( Xmax[Var] == 0.0 ) continue;
	    XmaxDeVar = Xmax[Var];
      il    = Cdeb[Var];
      ilMax = il + CNbTerm[Var];
      while ( il < ilMax ) {
        BBarre[NumeroDeContrainte[il]] -= ACol[il] * XmaxDeVar;
        il++;
      }
		  continue;
	  }
	}  
}
else { 
  for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
    if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) { 
	    if ( Xmax[Var] == 0.0 ) continue;
	    XmaxDeVar = Xmax[Var];
      il    = Cdeb[Var];
      ilMax = il + CNbTerm[Var];
      while ( il < ilMax ) {
        BBarre[NumeroDeContrainte[il]] -= ACol[il] * XmaxDeVar;
        il++;
      }
		  continue;
	  }
	  else if ( StatutBorneSupCourante[Var] == BORNE_AUXILIAIRE_DE_VARIABLE_NON_BORNEE ) {
		  /* Prise en compte des bornes inf auxiliaires sur les variables non bornees */
      if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF ) {
	      /* Car on simule le fait qu'on a cree une borne inf egale a -Xmax */
	      XmaxDeVar = -Xmax[Var];
        il    = Cdeb[Var];
        ilMax = il + CNbTerm[Var];
        while ( il < ilMax ) {
          BBarre[NumeroDeContrainte[il]] -= ACol[il] * XmaxDeVar;
          il++;
        }
			}
		}
  }
}

/* Resoudre BBarre = B^{-1} * b */

TypeDEntree  = VECTEUR_LU;
TypeDeSortie = VECTEUR_LU;
CalculEnHyperCreux = NON_SPX;

Save = NON_LU;
SecondMembreCreux = NON_LU;
SPX_ResoudreBYegalA( Spx, TypeDEntree, BBarre, NULL, NULL, &TypeDeSortie,
										 CalculEnHyperCreux, Save, SecondMembreCreux );
										 
return;
}

