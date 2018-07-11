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

   FONCTION:  Calcul du chainage de la transposee de la matrice des
              contraintes
               
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# ifdef SPX_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "spx_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void SPX_ModifierLeChainageDeLaTransposee( PROBLEME_SPX * Spx )
{
int il; int ilMax; int Var; int j;

int * Cdeb; int * CNbTermSansCoupes;
double * ACol; int * NumeroDeContrainte; int * CNbTermesDeCoupes;

int * Mdeb; double * A; int * NbTerm; int * Indcol; int ic1; int ic2;
int ic1Max; int NbT; int Cnt; 

int NombreDeVariablesDuProblemeSansCoupes;
int * CdebNew; int * CNbTermNew; double * AColNew; int * NumeroDeContrainteNew;

CdebNew               = (int *)   malloc( Spx->NombreDeVariablesAllouees * sizeof( int ) ); 
CNbTermNew            = (int *)   malloc( Spx->NombreDeVariablesAllouees * sizeof( int ) ); 
AColNew               = (double *) malloc( Spx->NbTermesAlloues   * sizeof( double ) ); 
NumeroDeContrainteNew = (int *)   malloc( Spx->NbTermesAlloues   * sizeof( int   ) ); 
if ( CdebNew == NULL || CNbTermNew == NULL || AColNew == NULL || NumeroDeContrainteNew == NULL ) {
  printf(" Simplexe: memoire insuffisante dans SPX_ModifierLeChainageDeLaTransposee\n");
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); 
}

NombreDeVariablesDuProblemeSansCoupes = Spx->NombreDeVariablesDuProblemeSansCoupes;

CNbTermSansCoupes = Spx->CNbTermSansCoupes;
CNbTermesDeCoupes = Spx->CNbTermesDeCoupes;
Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
A      = Spx->A;
Cdeb   = Spx->Cdeb;
ACol   = Spx->ACol;
NumeroDeContrainte = Spx->NumeroDeContrainte;

ic2 = 0;
for ( Var = 0 ; Var < NombreDeVariablesDuProblemeSansCoupes; Var++ ) {
  /* On Copie la colonne */
  ic1 = Cdeb[Var];
  NbT = CNbTermSansCoupes[Var];
  ic1Max = ic1 + NbT;
  CdebNew   [Var] = ic2;
  CNbTermNew[Var] = NbT;
	/*
  while ( ic1 < ic1Max ) {
    AColNew              [ic2] = ACol[ic1];
    NumeroDeContrainteNew[ic2] = NumeroDeContrainte[ic1];
    ic1++;
    ic2++;
  }
	*/	
	j = ic1Max-ic1;
	memcpy( (char *) &AColNew[ic2], (char *) &ACol[ic1], j * sizeof( double ) );
	memcpy( (char *) &NumeroDeContrainteNew[ic2], (char *) &NumeroDeContrainte[ic1], j * sizeof( int ) );
  ic2 += j;	
	/* On laisse de la place pour les termes des coupes */
  ic2 += CNbTermesDeCoupes[Var];
}

/* Les autres variables n'ont que des 1 car ce sont des variables d'ecart de coupes */
for ( ; Var < Spx->NombreDeVariables; Var++ ) {
  CdebNew   [Var] = ic2;
  CNbTermNew[Var] = 1;
  ic2++;
}

/* On balaie les coupes */
for ( Cnt = Spx->NombreDeContraintesDuProblemeSansCoupes ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  il = Mdeb[Cnt];	 
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
    Var = Indcol[il];
    if ( Var < NombreDeVariablesDuProblemeSansCoupes ) {
      ic2 = CdebNew[Var] + CNbTermNew[Var];
      AColNew[ic2] = A[il];		
      CNbTermNew[Var]++;
    }
    else {
      /* Variable d'ecart de coupe */
      ic2 = CdebNew[Var];
      AColNew[ic2] = 1.;
    }
    /*AColNew[ic2] = A[il];*/
    NumeroDeContrainteNew[ic2] = Cnt;
    il++;
  }  
}

free( Spx->Cdeb );
free( Spx->CNbTerm );
free( Spx->ACol );
free( Spx->NumeroDeContrainte );

Spx->Cdeb = CdebNew; 
Spx->CNbTerm = CNbTermNew; 
Spx->ACol = AColNew; 
Spx->NumeroDeContrainte = NumeroDeContrainteNew;

return;
} 

