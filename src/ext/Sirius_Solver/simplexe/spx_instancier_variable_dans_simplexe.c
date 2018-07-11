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

   FONCTION: Selon le cas:
             Modification des donnees du simplexe pour simuler une
             instanciation.
	     Remise des donnees dans l'etat initial.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

/*----------------------------------------------------------------------------*/
/* On modifie les donnees pour simuler une instanciation */

void Spx_ModifierLesDonneesSurInstanciation( PROBLEME_SPX * Spx,
                                             int VariablePneAInstancier,
					     int SortSurXmaxOuSurXmin )
{
double XmaxSV; int il; int ilMax; int VariableSpxAInstancier;

VariableSpxAInstancier = Spx->CorrespondanceVarEntreeVarSimplexe[VariablePneAInstancier];

if ( Spx->Xmax[VariableSpxAInstancier] == Spx->Xmin[VariableSpxAInstancier] || Spx->Xmin[VariableSpxAInstancier] != 0.0 ) {
  printf("erreur instanciation de la variable spx %d alors que Xmin %e et Xmax %e\n",
  VariableSpxAInstancier,Spx->Xmin[VariableSpxAInstancier],Spx->Xmax[VariableSpxAInstancier]);
  exit(0);
}

XmaxSV = Spx->Xmax[VariableSpxAInstancier];
Spx->Xmax[VariableSpxAInstancier] = 0.0;

if ( SortSurXmaxOuSurXmin == SORT_SUR_XMAX ) {

  Spx->PartieFixeDuCout+= (Spx->C[VariableSpxAInstancier] * XmaxSV ) / Spx->ScaleLigneDesCouts;

  il    = Spx->Cdeb[VariableSpxAInstancier];
  ilMax = il + Spx->CNbTerm[VariableSpxAInstancier];
  while ( il < ilMax ) {
    Spx->B[Spx->NumeroDeContrainte[il]]-= Spx->ACol[il] * XmaxSV;
    il++;
  }
}

return;
}

/*----------------------------------------------------------------------------*/
/* On remet les donnees dans l'etat avant instanciation */

void Spx_RemettreLesDonneesAvantInstanciation( PROBLEME_SPX * Spx , int VariablePneARemettre , int SortaitSurXmaxOuSurXmin )
{
double Xmax; int il; int ilMax; int VariablesSpxARemettre;

if ( SortaitSurXmaxOuSurXmin != SORT_SUR_XMIN && SortaitSurXmaxOuSurXmin != SORT_SUR_XMAX ) return;

VariablesSpxARemettre = Spx->CorrespondanceVarEntreeVarSimplexe[VariablePneARemettre]; 

Spx->Xmax[VariablesSpxARemettre] = 1.0 / Spx->ScaleX[VariablesSpxARemettre];
Xmax = Spx->Xmax[VariablesSpxARemettre];

if ( SortaitSurXmaxOuSurXmin == SORT_SUR_XMAX ) {

  Spx->PartieFixeDuCout-= (Spx->C[VariablesSpxARemettre] * Xmax) / Spx->ScaleLigneDesCouts;

  il    = Spx->Cdeb[VariablesSpxARemettre];
  ilMax = il + Spx->CNbTerm[VariablesSpxARemettre];
  while ( il < ilMax ) {
    Spx->B[Spx->NumeroDeContrainte[il]]+= Spx->ACol[il] * Xmax;
    il++;
  }
}

return;
}

