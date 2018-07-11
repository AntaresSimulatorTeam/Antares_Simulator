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
   FONCTION: Chainage de la metrice des contrainte avec uniquement les
	           variables hors base.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

/*----------------------------------------------------------------------------*/

void SPX_MettreAJourLaMatriceHorsBase( PROBLEME_SPX * Spx ) 
{
int Var; int ic; int icMx; int Cnt; int ilHorsBase; int ilHorsBaseMax; int ic1;
int * Cdeb; int * CNbTerm; int * NumeroDeContrainte; int * IndexDansLaMatriceHorsBase;
int * MdebHorsBase; int * NbTermHorsBase; int * IndcolHorsBase; int * InverseIndexDansLaMatriceHorsBase;
double * AHorsBase; double * ACol;

Cdeb = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;
NumeroDeContrainte = Spx->NumeroDeContrainte;
ACol = Spx->ACol;
IndexDansLaMatriceHorsBase = Spx->IndexDansLaMatriceHorsBase;
MdebHorsBase = Spx->MdebHorsBase;
NbTermHorsBase = Spx->NbTermHorsBase;
AHorsBase = Spx->AHorsBase;
IndcolHorsBase = Spx->IndcolHorsBase;
InverseIndexDansLaMatriceHorsBase = Spx->InverseIndexDansLaMatriceHorsBase;
 
/* On enleve la variable entrante */
Var = Spx->VariableEntrante;
ic = Cdeb[Var];   
icMx = ic + CNbTerm[Var];
while ( ic < icMx ) {
  Cnt = NumeroDeContrainte[ic];
	ilHorsBase = IndexDansLaMatriceHorsBase[ic];
  /* On doit enlever le terme qui se trouve a cet emplacement */
	ilHorsBaseMax = MdebHorsBase[Cnt] + NbTermHorsBase[Cnt] - 1;

	AHorsBase[ilHorsBase] = AHorsBase[ilHorsBaseMax];
	IndcolHorsBase[ilHorsBase] = IndcolHorsBase[ilHorsBaseMax];

	ic1 = InverseIndexDansLaMatriceHorsBase[ilHorsBaseMax];
  IndexDansLaMatriceHorsBase[ic1] = ilHorsBase;
	InverseIndexDansLaMatriceHorsBase[ilHorsBase] = ic1;

	NbTermHorsBase[Cnt]--; /* MdebHorsBase ne change jamais */
		
  ic++;
}

/* On ajoute la variable sortante */
Var = Spx->VariableSortante;  
ic = Cdeb[Var];
icMx = ic + CNbTerm[Var];
while ( ic < icMx ) {
  Cnt = NumeroDeContrainte[ic];

  /* On ajoute a la fin */
	ilHorsBase = MdebHorsBase[Cnt] + NbTermHorsBase[Cnt] - 1;

	AHorsBase[ilHorsBase] = ACol[ic];
	IndcolHorsBase[ilHorsBase] = Spx->VariableSortante;

  IndexDansLaMatriceHorsBase[ic] = ilHorsBase;
	InverseIndexDansLaMatriceHorsBase[ilHorsBase] = ic;
	
	NbTermHorsBase[Cnt]++; /* MdebHorsBase ne change jamais */	
		
  ic++;
}

return;
}

/*----------------------------------------------------------------------------*/

void SPX_InitMatriceHorsBase( PROBLEME_SPX * Spx )
{
int NombreDeContraintes; int NombreDeVariables; char * PositionDeLaVariable; int Var;
int ic; int icMx; int Cnt; int ilHorsBase; int * Cdeb; int * CNbTerm; int * NumeroDeContrainte;
int * IndexDansLaMatriceHorsBase; int * MdebHorsBase; int * NbTermHorsBase;
int * IndcolHorsBase; int * InverseIndexDansLaMatriceHorsBase; int * Mdeb;
double * AHorsBase; double * ACol;

if ( Spx->MdebHorsBase == NULL ) {
  Spx->IndexDansLaMatriceHorsBase = (int *) malloc( Spx->NbTermesAlloues * sizeof( int ) );
  Spx->MdebHorsBase = (int *) malloc( Spx->NombreDeContraintesAllouees * sizeof( int ) );
  Spx->NbTermHorsBase = (int *) malloc( Spx->NombreDeContraintesAllouees * sizeof( int ) );
  Spx->AHorsBase = (double *) malloc( Spx->NbTermesAlloues * sizeof( double ) );
  Spx->IndcolHorsBase = (int *) malloc( Spx->NbTermesAlloues * sizeof( int ) );
  Spx->InverseIndexDansLaMatriceHorsBase = (int *) malloc( Spx->NbTermesAlloues * sizeof( int ) );
}

NombreDeContraintes = Spx->NombreDeContraintes;
NombreDeVariables = Spx->NombreDeVariables;
PositionDeLaVariable = Spx->PositionDeLaVariable;

Mdeb = Spx->Mdeb;

Cdeb = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;
NumeroDeContrainte = Spx->NumeroDeContrainte;
ACol = Spx->ACol;

IndexDansLaMatriceHorsBase = Spx->IndexDansLaMatriceHorsBase;
MdebHorsBase = Spx->MdebHorsBase;
NbTermHorsBase = Spx->NbTermHorsBase;
AHorsBase = Spx->AHorsBase;
IndcolHorsBase = Spx->IndcolHorsBase;
InverseIndexDansLaMatriceHorsBase = Spx->InverseIndexDansLaMatriceHorsBase;

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  MdebHorsBase[Cnt] = Mdeb[Cnt];
	NbTermHorsBase[Cnt] = 0;
}

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( PositionDeLaVariable[Var] == EN_BASE_LIBRE ) continue;
  ic = Cdeb[Var];
  icMx = ic + CNbTerm[Var];
  while ( ic < icMx ) {
    Cnt = NumeroDeContrainte[ic];
		ilHorsBase = MdebHorsBase[Cnt] + NbTermHorsBase[Cnt];
	  AHorsBase[ilHorsBase] = ACol[ic];
	  IndcolHorsBase[ilHorsBase] = Var;    

		IndexDansLaMatriceHorsBase[ic] = ilHorsBase;
	  InverseIndexDansLaMatriceHorsBase[ilHorsBase] = ic;

   	NbTermHorsBase[Cnt]++;
	
    ic++;
  }		
}

return;
}
