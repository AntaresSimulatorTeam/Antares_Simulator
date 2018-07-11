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

   FONCTION: Translater les bornes en entree pour mettre le   
             probleme sous la forme standard 
               
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

/*----------------------------------------------------------------------------*/

void SPX_TranslaterLesBornes( PROBLEME_SPX * Spx )
{
int i; int Var; int Cnt; int il; int ilMax; double S; double * XminEntree;
double * XmaxEntree; double * Xmin; double * Xmax; char * TypeDeVariable;
double * XEntree; int * Mdeb; int * NbTerm; double * X; double * A;
double * B; int * Indcol; 

XminEntree     = Spx->XminEntree;
XmaxEntree     = Spx->XmaxEntree;
Xmin           = Spx->Xmin;
Xmax           = Spx->Xmax;
TypeDeVariable = Spx->TypeDeVariable;
XEntree        = Spx->XEntree;
X              = Spx->X;

for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) {  

  XminEntree[i] = Xmin[i];
  XmaxEntree[i] = Xmax[i]; 

  if ( TypeDeVariable[i] == NON_BORNEE ) continue;

  if ( TypeDeVariable[i] == BORNEE ) { 
    Xmax[i] = XmaxEntree[i] - XminEntree[i];
  }
  Xmin[i] = 0.;

  XEntree[i] = X[i];
	
  if ( TypeDeVariable[i] == BORNEE || TypeDeVariable[i] == BORNEE_INFERIEUREMENT ) { 	
    X[i]= XEntree[i] - XminEntree[i];
  }
	
}


Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
A      = Spx->A;
B      = Spx->B;

for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  il    = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  S     = 0.;
  while ( il < ilMax ) {
    Var = Indcol[il];
    if ( TypeDeVariable[Var] != NON_BORNEE ) S+= A[il] * XminEntree[Var];
    il++;
  }
  B[Cnt]-=S;
} 

return;
}
