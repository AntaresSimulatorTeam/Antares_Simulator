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

   FONCTION: Resolution du systeme transpose pour obtenir une ligne
             de l'inverse de la base    

   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"

/*----------------------------------------------------------------------------*/
/* Calcul de la ligne de B^{-1} de la variable sortante */
void SPX_CalculerErBMoins1AvecBaseComplete( PROBLEME_SPX * Spx, char CalculEnHyperCreux ) 
{
char TypeDEntree; char TypeDeSortie; int i; double * ErBMoinsUn;

if ( CalculEnHyperCreux != OUI_SPX ) {
  TypeDEntree = VECTEUR_LU;	
  TypeDeSortie = VECTEUR_LU;
  /*memset( (char *) Spx->ErBMoinsUn , 0 , Spx->NombreDeContraintes * sizeof( double ) );*/
	ErBMoinsUn = Spx->ErBMoinsUn;
	for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) ErBMoinsUn[i] = 0.0;
	
  Spx->ErBMoinsUn[Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante]] = 1.;
}
else {
  TypeDEntree  = COMPACT_LU;
	TypeDeSortie = COMPACT_LU;
  Spx->ErBMoinsUn[0] = 1;
  Spx->IndexTermesNonNulsDeErBMoinsUn[0] = Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante];
  Spx->NbTermesNonNulsDeErBMoinsUn = 1;
}

SPX_ResoudreUBEgalC( Spx, TypeDEntree, Spx->ErBMoinsUn, Spx->IndexTermesNonNulsDeErBMoinsUn,
										 &(Spx->NbTermesNonNulsDeErBMoinsUn), &TypeDeSortie, CalculEnHyperCreux );

if ( CalculEnHyperCreux == OUI_SPX ) {
  if ( TypeDeSortie != TypeDEntree ) {
    /* Ca s'est pas bien passe et on s'est forcement retrouve en VECTEUR_LU */
    Spx->TypeDeStockageDeErBMoinsUn = VECTEUR_SPX;		
		Spx->NbEchecsErBMoins++;
		/*
		printf("SPX_CalculerErBMoins1 echec hyper creux ErBMoins1 iteration %d\n",Spx->Iteration);
		*/
		if ( Spx->NbEchecsErBMoins >= SEUIL_ECHEC_CREUX ) {
      # if VERBOSE_SPX
		    printf("Arret de l'hyper creux pour le calcul de la ligne pivot, iteration %d\n",Spx->Iteration);
      # endif			
		  Spx->CalculErBMoinsUnEnHyperCreux = NON_SPX;
      Spx->CountEchecsErBMoins = 0;
		}		
	}
	else Spx->NbEchecsErBMoins = 0;
}
										 
return;
}
