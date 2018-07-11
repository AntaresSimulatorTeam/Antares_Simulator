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

   FONCTION: Detection de GUB (General Upper Bound constraints).
	           Type de GUB detectees:
						 - Somme de Xi <= 1 ou Somme de Xi = 1. Dans le cas de l'inegalite
						   il y a au plus une variable egale a 1. Dans le cas de l'egalite
							 il y a une variable egale a 1.
							 Pour ce type de GUB la valeur d'instanciation du groupe de
							 variable choisi est 0.
						 - Somme de Xi = N-1 ou n est le nombre de variables entieres
						   de la contrainte. Dans ce cas toutes les variables sont egales
							 a 1 sauf une qui vaut 0.
							 Pour ce type de GUB la valeur d'instanciation du groupe de
							 variables est 1.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "prs_fonctions.h"

/*----------------------------------------------------------------------------*/

void PNE_DetecterLesGub( PROBLEME_PNE * Pne )   
{
int Cnt; int il; int ilMax; char Gub; int N; int NombreDeGub; int NombreDeContraintes;
int * NbTerm; int * Mdeb; double * B; int * Nuvar; int * TypeDeVariable; int NbNonFix;
double * A; int * NumeroDeContrainteDeLaGub; char * SensContrainte; int Var;
char OnInverse; int Valeur; int * ValeurDInstanciationPourLaGub; int * TypeDeBorne;

# if UTILISER_LES_GUB == NON_PNE
  Pne->NombreDeGub = 0;
  if ( Pne->AffichageDesTraces == OUI_PNE ) {
    printf("Will not branch on GUB constraints\n");
  }	
	return;
# endif

NombreDeContraintes = Pne->NombreDeContraintesTrav;
NbTerm = Pne->NbTermTrav;
Mdeb = Pne->MdebTrav;  
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;
B = Pne->BTrav;
SensContrainte = Pne->SensContrainteTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
NumeroDeContrainteDeLaGub = Pne->NumeroDeContrainteDeLaGub;
ValeurDInstanciationPourLaGub = Pne->ValeurDInstanciationPourLaGub;
TypeDeBorne = Pne->TypeDeBorneTrav;

NombreDeGub = 0;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {

  if ( SensContrainte[Cnt] != '=' ) continue;
  
  N = NbTerm[Cnt];
  if ( N < MIN_TERMES_GUB ) continue;
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
	NbNonFix = 0;
  Gub = OUI_PNE;
  if ( B[Cnt] != 1.0 && B[Cnt] != N - 1 ) continue;
  if ( B[Cnt] == N - 1 && SensContrainte[Cnt] != '=' ) continue;
	if ( B[Cnt] == 1.0 ) Valeur = 0;
	else Valeur = 1;
  while ( il < ilMax ) {
    Var = Nuvar[il];
    if ( TypeDeBorne[Var] != VARIABLE_FIXE ) NbNonFix++;
    if ( TypeDeVariable[Var] != ENTIER || A[il] != 1.0 ) {
      Gub = NON_PNE;
      break;
    }
    il++;
  }  
  if ( Gub == OUI_PNE && NbNonFix >= MIN_TERMES_GUB ) { 
    NumeroDeContrainteDeLaGub[NombreDeGub] = Cnt;
		ValeurDInstanciationPourLaGub[NombreDeGub] = Valeur;
    NombreDeGub++;
  }
}

/* Classement des Gub dans l'ordre decroissant du nombre de termes. Afin de couvrir le plus grand nombre de termes d'une
   knapsack avec le moins grand nombre de GUB */
OnInverse = OUI_PNE;
while ( OnInverse == OUI_PNE ) {
  OnInverse = NON_PNE;
  for ( Cnt = 0 ; Cnt < NombreDeGub - 1 ; Cnt++ ) {
    if ( NbTerm[NumeroDeContrainteDeLaGub[Cnt]] <
         NbTerm[NumeroDeContrainteDeLaGub[Cnt+1]] ) {
      OnInverse = OUI_PNE;
      il = NumeroDeContrainteDeLaGub[Cnt+1];
      NumeroDeContrainteDeLaGub[Cnt+1] = NumeroDeContrainteDeLaGub[Cnt];
      NumeroDeContrainteDeLaGub[Cnt] = il;
    }
  }
}

Pne->NombreDeGub = NombreDeGub;

/* Pour recuperer de la place */
Pne->NumeroDeContrainteDeLaGub     = (int *) realloc( Pne->NumeroDeContrainteDeLaGub    , Pne->NombreDeGub * sizeof( int ) );
Pne->ValeurDInstanciationPourLaGub = (int *) realloc( Pne->ValeurDInstanciationPourLaGub, Pne->NombreDeGub * sizeof( int ) );

if ( Pne->AffichageDesTraces == OUI_PNE ) {
  printf("%d GUB constraints found (native)\n",Pne->NombreDeGub);
}

return;
}
