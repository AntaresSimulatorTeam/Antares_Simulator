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

   FONCTION:   Calcul de Pi = c_B * B^{-1} c'est a dire              
               resolution de u B = c

   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"

/*----------------------------------------------------------------------------*/

void SPX_CalculerPiAvecBaseReduite( PROBLEME_SPX * Spx )
{
int Cnt; double * Pi; double * C; int * VariableEnBaseDeLaContrainte; char TypeDEntree;
char TypeDeSortie; char CalculEnHyperCreux; char SecondMembreCreux; double * AReduit;
int * IndexAReduit; int RangDeLaMatriceFactorisee; int NombreDeTermesNonNulsDuVecteurReduit;
int * OrdreColonneDeLaBaseFactorisee; int * LigneDeLaBaseFactorisee; int r; 

Pi = Spx->Pi;
C  = Spx->C;
VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;

RangDeLaMatriceFactorisee = Spx->RangDeLaMatriceFactorisee;
AReduit = Spx->AReduit;
IndexAReduit = Spx->IndexAReduit;
OrdreColonneDeLaBaseFactorisee = Spx->OrdreColonneDeLaBaseFactorisee;
LigneDeLaBaseFactorisee = Spx->LigneDeLaBaseFactorisee;

for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  r = OrdreColonneDeLaBaseFactorisee[Cnt];
	if ( r < RangDeLaMatriceFactorisee ) {
	  AReduit[r] += C[VariableEnBaseDeLaContrainte[Cnt]]; /* Car AReduit peut avoir ete initialise grace
		                                                       a la partie hors base reduite */
	}
	else {
	  /* Si on neglige les variables d'ecart basique il faut considerer que Pi = 0 */
	  Pi[LigneDeLaBaseFactorisee[r]] = 0;
	}
}

TypeDEntree  = VECTEUR_LU;
TypeDeSortie = VECTEUR_LU;
CalculEnHyperCreux = NON_SPX;
SecondMembreCreux = NON_SPX;

/* Eventuellement les Eta vecteurs */
/* Attention il faut âs faire comme ca mais resoudre tout le systeme avec les eta */
/*
if ( Spx->UtiliserLaLuUpdate == NON_SPX ) {
  SPX_AppliquerLesEtaVecteursTransposee( Spx, Pi, IndexDesTermesNonNuls, NombreDeTermesNonNuls,
	                                       CalculEnHyperCreux, TypeDEntree );	
}
*/

SPX_ResolutionDeSystemeTransposee( Spx, TypeDEntree, AReduit, IndexAReduit, &NombreDeTermesNonNulsDuVecteurReduit,																		
                                   &TypeDeSortie, CalculEnHyperCreux, SecondMembreCreux );

for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) {
  Pi[LigneDeLaBaseFactorisee[r]] = AReduit[r];
  AReduit[r] = 0;  
}

# if VERIFICATION_PI == OUI_SPX
printf("------------- CalculerPi  Spx->NombreDeChangementsDeBase %d -------------\n",Spx->NombreDeChangementsDeBase);
{
double * Buff; int i; int Var; int ic; int icMx; double S; double * Sortie;
Buff = (double *) malloc( Spx->NombreDeContraintes * sizeof( double ) );
Sortie = (double *) malloc( Spx->NombreDeContraintes * sizeof( double ) );
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) Buff[Cnt] = C[VariableEnBaseDeLaContrainte[Cnt]];
for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) Sortie[i] = Pi[i];
for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) {
  Var = Spx->VariableEnBaseDeLaContrainte[i];
	ic = Spx->Cdeb[Var];
	icMx = ic + Spx->CNbTerm[Var];
	S = 0;
	while ( ic < icMx ) {
	  S += Spx->ACol[ic] * Sortie[Spx->NumeroDeContrainte[ic]];
	  ic++;
	}
	if ( fabs( S - Buff[i] ) > 1.e-7 ) {
	  printf("i = %d  S %e Buff %e  ecart %e\n",i,S,Buff[i],fabs( S - Buff[i] ));
		exit(0);
	}
}
printf("Fin verif  CalculerPi OK\n");
free( Buff );
free( Sortie );

SPX_VerifierLesVecteursDeTravail( Spx );

}
# endif

return;  
}

