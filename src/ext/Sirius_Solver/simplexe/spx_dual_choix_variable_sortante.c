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

   FONCTION: Algorithme dual: choix de la variable sortante.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h" 
# include "spx_define.h"

# ifdef SPX_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "spx_memoire.h"
# endif

# include "pne_define.h"
# include "pne_fonctions.h"

# include "lu_define.h"

/*----------------------------------------------------------------------------*/

void SPX_DualChoixDeLaVariableSortante( PROBLEME_SPX * Spx )
{
double PlusGrandeViolation; int ContrainteDeLaVariableSortante; int i; int iSor;
double * ValeurDeViolationDeBorne; double PlusPetiteViolation; int Index;
int * NumerosDesContraintesASurveiller; int Count; int CountMax; 
# if POIDS_DANS_VALEUR_DE_VIOLATION == NON_SPX			
  double * DualPoids; 
# endif

/* RechercherLaVariableSortante */

Spx->VariableSortante = -1; 
Spx->SortSurXmaxOuSurXmin = SORT_PAS;

ContrainteDeLaVariableSortante = -1;

iSor = -1;
PlusGrandeViolation = -1.;
PlusPetiteViolation = LINFINI_SPX;
ValeurDeViolationDeBorne = Spx->ValeurDeViolationDeBorne;
NumerosDesContraintesASurveiller = Spx->NumerosDesContraintesASurveiller;

# if POIDS_DANS_VALEUR_DE_VIOLATION == NON_SPX			
  DualPoids = Spx->DualPoids;
# endif

Count = 0;

Spx->A1 = PNE_Rand( Spx->A1 );
CountMax = (int) ceil(Spx->A1 * Spx->NombreDeContraintesASurveiller);
if ( CountMax < 5 ) CountMax = 5;

for ( i = 0 ; i < Spx->NombreDeContraintesASurveiller ; i++ ) {	
	# if POIDS_DANS_VALEUR_DE_VIOLATION == OUI_SPX			
    if ( ValeurDeViolationDeBorne[i] > PlusGrandeViolation + SEUIL_DE_VIOLATION_DE_BORNE ) { 
	    iSor = i;				
	    PlusGrandeViolation = ValeurDeViolationDeBorne[i];
		  if ( Count > CountMax ) break;
		  Count++;
    }
	# else
	  Index = NumerosDesContraintesASurveiller[i];	
    if ( ValeurDeViolationDeBorne[i] / DualPoids[Index] > PlusGrandeViolation + SEUIL_DE_VIOLATION_DE_BORNE ) { 
	    iSor = i;				
	    PlusGrandeViolation = ValeurDeViolationDeBorne[i] / DualPoids[Index];
		  if ( Count > CountMax ) break;
		  Count++;
    }	
	# endif	
}

if ( iSor >= 0 ) {
	Index = NumerosDesContraintesASurveiller[iSor];
	if ( Spx->BBarre[Index] < 0.0 ) Spx->SortSurXmaxOuSurXmin = SORT_SUR_XMIN;
	else Spx->SortSurXmaxOuSurXmin = SORT_SUR_XMAX;
	
  if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
    Spx->VariableSortante = Spx->VariableEnBaseDeLaContrainte[Spx->ColonneDeLaBaseFactorisee[Index]];
	}
	else {
	  Spx->VariableSortante = Spx->VariableEnBaseDeLaContrainte[Index];
	}
	
}

/* Traces */
#if VERBOSE_SPX 
if ( Spx->LaBaseDeDepartEstFournie == OUI_SPX && Spx->StrongBranchingEnCours != OUI_SPX ) {
  if ( Spx->VariableSortante >= 0 ) {
    printf("  *** Iteration %d\n",Spx->Iteration);
    if ( Spx->OrigineDeLaVariable[Spx->VariableSortante] == NATIVE ) {
      printf("  -> Algorithme dual variable de base sortante %d de type NATIVE ",Spx->VariableSortante);
    }
    else if ( Spx->OrigineDeLaVariable[Spx->VariableSortante] == ECART ) {
      printf("  -> Algorithme dual variable de base sortante %d de type ECART ",Spx->VariableSortante);
    }
    else if ( Spx->OrigineDeLaVariable[Spx->VariableSortante] == BASIQUE_ARTIFICIELLE ) {
      printf("  -> Algorithme dual variable de base sortante %d de type BASIQUE_ARTIFICIELLE ",Spx->VariableSortante);
    }
    else {
      printf("Bug dans l algorithme dual, sous-programme SPX_DualChoixDeLaVariableKiKitLaBase\n");
      exit(0);
    }
    if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) printf("elle SORT_SUR_XMIN "); 
    else if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMAX ) printf("elle SORT_SUR_XMAX ");
    else {
      printf("Bug dans l algorithme dual, sous-programme SPX_DualChoixDeLaVariableKiKitLaBase\n");
      exit(0);
    }  						 
  }
}
#endif
/* Fin traces */

return; 

}

/*----------------------------------------------------------------------------*/

void SPX_DualChoixDeLaVariableSortanteAuHasard( PROBLEME_SPX * Spx )
{
int Cnt; int Var; double Xx; int Nombre; int NombreDeVariablesCandidates; int * VariableCandidate;       
int * VariableEnBaseDeLaContrainte; double * X; double * BBarre; double * Xmax; char * TypeDeVariable;
int i; int UtiliserLaBaseReduite; int RangDeLaMatriceFactorisee; int Index; 
int * OrdreColonneDeLaBaseFactorisee; double * SeuilDeViolationDeBorne; 

UtiliserLaBaseReduite = Spx->UtiliserLaBaseReduite;
OrdreColonneDeLaBaseFactorisee = Spx->OrdreColonneDeLaBaseFactorisee;
RangDeLaMatriceFactorisee = Spx->RangDeLaMatriceFactorisee;

VariableCandidate = (int *) (Spx->Bs); /* On peut aussi utiliser Spx->AReduit */
			      
Spx->VariableSortante        = -1;
NombreDeVariablesCandidates = 0;

VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;
X = Spx->X;
Xmax = Spx->Xmax;    
BBarre = Spx->BBarre;
TypeDeVariable = Spx->TypeDeVariable;

SeuilDeViolationDeBorne = Spx->SeuilDeViolationDeBorne;

for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
		
  Var = VariableEnBaseDeLaContrainte[Cnt];
  
  if ( UtiliserLaBaseReduite == OUI_SPX ) {
	Index = OrdreColonneDeLaBaseFactorisee[Cnt];
    if ( Index >= RangDeLaMatriceFactorisee ) continue;
  }
  else {
    Index = Cnt;
  }  
  
  X[Var] = BBarre[Index];
  if ( TypeDeVariable[Var] == NON_BORNEE ) continue; /* Une variable libre ne sort pas de la base */
  /* La variable est donc bornee ( des 2 cotes ou seulement bornee inferieurement ) */    
  if ( BBarre[Index] < -SeuilDeViolationDeBorne[Var] ) {
    VariableCandidate[NombreDeVariablesCandidates] = Var;
    NombreDeVariablesCandidates++;
    continue;
  }
  else if ( TypeDeVariable[Var] == BORNEE ) { 
    if ( BBarre[Index] > Xmax[Var] + SeuilDeViolationDeBorne[Var] ) {
      VariableCandidate[NombreDeVariablesCandidates] = Var;
      NombreDeVariablesCandidates++;
    }  
  }
}

if ( NombreDeVariablesCandidates <= 0 ) goto FinSelectionAuHasard;

/* On tire un nombre au hasard compris entre 0 et NombreDeVariablesCandidates - 1 */
# if UTILISER_PNE_RAND == OUI_SPX
  Spx->A1 = PNE_Rand( Spx->A1 );
  Xx = Spx->A1 * (NombreDeVariablesCandidates - 1);
# else
  Xx = rand() * Spx->UnSurRAND_MAX * (NombreDeVariablesCandidates - 1);
# endif

Nombre = (int) Xx;
if ( Nombre >= NombreDeVariablesCandidates - 1 ) Nombre = NombreDeVariablesCandidates - 1; 

Spx->VariableSortante = VariableCandidate[Nombre];

if ( UtiliserLaBaseReduite == OUI_SPX ) {
  Index = OrdreColonneDeLaBaseFactorisee[Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante]];
}
else {
  Index = Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante];
}
 
if ( BBarre[Index] < 0.0 ) Spx->SortSurXmaxOuSurXmin = SORT_SUR_XMIN;
else Spx->SortSurXmaxOuSurXmin = SORT_SUR_XMAX;

FinSelectionAuHasard:

for ( i = 0 ; i < NombreDeVariablesCandidates ; i++ ) VariableCandidate[i] = 0;

return; 
}

