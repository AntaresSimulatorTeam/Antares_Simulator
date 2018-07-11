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

   FONCTION: En exploration rapide (pilotee par le branch and bound),
             choix de la variable a instancier.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

#include "pne_define.h"

#include "bb_define.h"
#include "bb_fonctions.h"

# ifdef SPX_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "spx_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void SPX_ChoisirLaVariableAInstancier( PROBLEME_SPX * Spx,
                                       void * BbProb,
                                       int * VariablePneAInstancier,
				                               int * SortSurXmaxOuSurXmin )

{
int Cnt; int i  ; double X   ; int TypeDeSortie          ; double Seuil_0; 
double S; double Y; int VarSpx; int VariableSpxAInstancier; double Seuil_1;
int NbVarFractionnaires       ; double NormeInfaisabilite  ;
double PlusGrandeVariation     ;
double * Fractionalite; int * NumVarFrac; /*char OnInverse; int VSpx;*/

double * BBarre; int * VariableEnBaseDeLaContrainte; char * PositionDeLaVariable; int VarPne;
double * ScaleX; int * ContrainteDeLaVariableEnBase;
double * Xmax  ; double * DualPoids ; int * CorrespondanceVarEntreeVarSimplexe  ; 
int NombreDeVariablesBinairesPne   ; int * NumeroDesVariablesBinairesPne       ;
double PlusPetiteVariation; double Poids; double S1; double S2;
BB * Bb;

Bb  = (BB *) BbProb; 
  
/************************************************************************/

*VariablePneAInstancier = -1;
*SortSurXmaxOuSurXmin = SORT_PAS;

if ( Spx->YaUneSolution == NON_SPX ) return;

SPX_CalculDuCout( Spx ); 
if ( Spx->Cout > Spx->CoutMax ) {
  printf("Fin par coutmax dans choix var a instancier\n");
  Spx->YaUneSolution = NON_SPX;
  return; 
}

/*
printf("Cout de la solution relaxee %e\n",Spx->Cout);
*/

VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;
BBarre                       = Spx->BBarre;
PositionDeLaVariable         = Spx->PositionDeLaVariable;
ContrainteDeLaVariableEnBase = Spx->ContrainteDeLaVariableEnBase;
ScaleX                       = Spx->ScaleX;
Xmax                         = Spx->Xmax;
DualPoids                    = Spx->DualPoids;


CorrespondanceVarEntreeVarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe;

NombreDeVariablesBinairesPne  = Bb->NombreDeVariablesEntieresDuProbleme;
NumeroDesVariablesBinairesPne = Bb->NumerosDesVariablesEntieresDuProbleme;

VariableSpxAInstancier = -1;
TypeDeSortie           = SORT_PAS;
   
PlusGrandeVariation = -LINFINI_SPX;
PlusPetiteVariation = LINFINI_SPX;

Seuil_0 = 5.e-7;
Seuil_1 = 1. - Seuil_0;

NbVarFractionnaires = 0;
NormeInfaisabilite  = 0.0;

Fractionalite = (double *) malloc(  Spx->NombreDeVariables  * sizeof( double ) );
NumVarFrac    = (int *)   malloc(  Spx->NombreDeVariables  * sizeof( int   ) );
 
/* Decompte du nombre de variables fractionnaires */
for ( i = 0 ; i < NombreDeVariablesBinairesPne ; i++ ) {
  VarPne = NumeroDesVariablesBinairesPne[i];
  VarSpx = CorrespondanceVarEntreeVarSimplexe[VarPne];
  if ( VarSpx < 0 || VarSpx >= Spx->NombreDeVariables ) continue;
  if ( Spx->Xmin[VarSpx] == Spx->Xmax[VarSpx] ) continue;
  if ( PositionDeLaVariable[VarSpx] != EN_BASE_LIBRE ) continue;
  Cnt = ContrainteDeLaVariableEnBase[VarSpx];
  X = BBarre[Cnt];
  Y = X * ScaleX[VarSpx];
  if ( Y < Seuil_0 || Y > Seuil_1 ) continue;
  Fractionalite[NbVarFractionnaires] = fabs( Y - 0.5 );
  NumVarFrac[NbVarFractionnaires] = VarSpx;
  NormeInfaisabilite+= fabs( Y - floor( Y + 0.5 ) );  
  NbVarFractionnaires++;
}

/*
printf("NbVarFractionnaires %d  NormeInfaisabilite %e\n",NbVarFractionnaires,NormeInfaisabilite);
*/

if ( NbVarFractionnaires <= 0 ) return;

/* Classement par fractionalite croissante */
/*
OnInverse = OUI_SPX;
while ( OnInverse == OUI_SPX ) {
  OnInverse = NON_SPX;
  for ( i = 0 ; i < NbVarFractionnaires - 1 ; i++ ) {
    if ( Bb->NombreDeSolutionsEntieresTrouvees == 0 ) {
      if ( Fractionalite[i] < Fractionalite[i+1] ) { 
        OnInverse = OUI_SPX;
        VSpx = NumVarFrac[i];
        NumVarFrac[i] = NumVarFrac[i+1];
        NumVarFrac[i+1] = VSpx;
        X = Fractionalite[i];
        Fractionalite[i] = Fractionalite[i+1];
        Fractionalite[i+1] = X;
      }
    }
    else {
      if ( Fractionalite[i] > Fractionalite[i+1] ) { 
        OnInverse = OUI_SPX;
        VSpx = NumVarFrac[i];
        NumVarFrac[i] = NumVarFrac[i+1];
        NumVarFrac[i+1] = VSpx;
        X = Fractionalite[i];
        Fractionalite[i] = Fractionalite[i+1];
        Fractionalite[i+1] = X;
      }
    }
  }
}
*/

for ( i = 0 ; i < NbVarFractionnaires ; i++ ) { 
  VarSpx  = NumVarFrac[i];
  if ( PositionDeLaVariable[VarSpx] != EN_BASE_LIBRE ) continue;
  Cnt = ContrainteDeLaVariableEnBase[VarSpx];
  X = BBarre[Cnt];
  Y = X * ScaleX[VarSpx];
  if ( Y < Seuil_0 || Y > Seuil_1 ) continue;

  Poids = DualPoids[Cnt];

  S1 = ( X * X ) / Poids; 
  S2 = ( (Xmax[VarSpx] - X) * (Xmax[VarSpx] - X) ) / Poids;
	S = S1 + S2;
  if ( Bb->NombreDeSolutionsEntieresTrouvees != 0 ) {
    if ( S > PlusGrandeVariation ) {
      VariableSpxAInstancier = VarSpx;
      if ( S2 > S1 )  TypeDeSortie = SORT_SUR_XMAX;
      else  TypeDeSortie = SORT_SUR_XMIN;			
      PlusGrandeVariation = S;
    }
  }
  else {
    if ( S < PlusPetiteVariation ) {
      VariableSpxAInstancier = VarSpx;
      if ( S2 > S1 )  TypeDeSortie = SORT_SUR_XMIN;
      else  TypeDeSortie = SORT_SUR_XMAX;			
      PlusPetiteVariation = S;
    }
  }
		
}

free( Fractionalite );
free( NumVarFrac );

if ( VariableSpxAInstancier >= 0 ) {
  /*
  VarSpx = VariableSpxAInstancier;
  Cnt = ContrainteDeLaVariableEnBase[VarSpx];
  X = BBarre[Cnt];
  Y = X * ScaleX[VarSpx];
  printf("Var a instancier valeur %e poids %e cout sol %e delta cout si Xmin %e si Xmax %e coutmax %e\n",
          Y, DualPoids[Cnt] ,
	        Spx->Cout,
	        sqrt( ( X * X ) / DualPoids[Cnt] ),
	        sqrt( ( (Xmax[VarSpx] - X) * (Xmax[VarSpx] - X) ) / DualPoids[Cnt] ),
	        Spx->CoutMax );
	*/  
  *VariablePneAInstancier = Spx->CorrespondanceVarSimplexeVarEntree[VariableSpxAInstancier];
  *SortSurXmaxOuSurXmin   = TypeDeSortie;
}

return; 

}



