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

   FONCTION: Application des eta vecteurs pour la forme produit de
	           l'inverse dans le calcul d'une resolution de systeme
						 avec forme produit de l'inverse.

   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"

# define CONTROLE_RAZ_DES_VECTEURS NON_SPX

/*----------------------------------------------------------------------------*/
/* Reste ADRESSAGE_INDIRECT_LU a pogrammer */
void SPX_AppliquerLesEtaVecteursTransposee( PROBLEME_SPX * Spx,
                                            double * U /* Second membre et resultat */,
                                            int * IndexDesTermesNonNuls,
					                                  int * NombreDeTermesNonNuls,
				                                    char   CalculEnHyperCreux,
	                                          char   TypeDeStockage /* COMPACT_LU
                                                                     ADRESSAGE_INDIRECT_LU
				          	                                                 VECTEUR_LU */ 
					                                )
{
int k; int il; int ilMax; double S; int * EtaDeb; int * EtaNbTerm;
int * EtaIndiceLigne; int * EtaColonne;  double * EtaMoins1Valeur;

int j; int NbN; int Col; int * Marqueur; double * W;

EtaDeb    = Spx->EtaDeb;
EtaNbTerm = Spx->EtaNbTerm;
EtaIndiceLigne  = Spx->EtaIndiceLigne;
EtaColonne      = Spx->EtaColonne;
EtaMoins1Valeur = Spx->EtaMoins1Valeur;   

if ( CalculEnHyperCreux == OUI_SPX ) {
  if ( TypeDeStockage == COMPACT_LU ) goto HyperCreux;
  if ( TypeDeStockage == ADRESSAGE_INDIRECT_LU ) {
	  printf("Attention TypeDeStockage = ADRESSAGE_INDIRECT_LU pas teste dans SPX_AppliquerLesEtaVecteursTransposee\n");
	  goto HyperCreux;
	}
	else if ( TypeDeStockage != VECTEUR_LU ) {
	  printf("AppliquerLesEtaVecteursTransposee: type de stockage non gere\n");
	  exit(0);
	}
}

k  = Spx->NombreDeChangementsDeBase - 1;
while ( k >= 0 ) {
  il    = EtaDeb[k];
  ilMax = il + EtaNbTerm[k];
  S     = 0.;
  while ( il < ilMax ) {
    S+= EtaMoins1Valeur[il] * U[EtaIndiceLigne[il]];
    il++;
  }
  /* Colonne egal aussi contrainte pour le terme diagonal */	
  U[EtaColonne[k]] = S;
  k--;
}
return;

HyperCreux:

# if CONTROLE_RAZ_DES_VECTEURS == OUI_SPX
  for ( j = 0 ; j < Spx->NombreDeContraintes ; j++ ) {
    if ( Spx->AReduit[j] != 0 ) {
	    printf("AppliquerLesEtaVecteursTransposee: Spx->AReduit[%d] = %e\n",j,Spx->AReduit[j]);
	    exit(0);
	  }
    if ( Spx->Marqueur[j] != -1 ) {
	    printf("AppliquerLesEtaVecteursTransposee: Spx->Marqueur[%d] = %d\n",j,Spx->Marqueur[j]);
	    exit(0);
	  }	
  }
# endif

Marqueur = Spx->Marqueur;

NbN = *NombreDeTermesNonNuls;

if ( TypeDeStockage == COMPACT_LU ) {
  /* On expand */
  W = Spx->AReduit;  
  for ( j = 0 ; j < NbN ; j++ ) {	
    Col = IndexDesTermesNonNuls[j];		
    W[Col] = U[j];
	  Marqueur[Col] = 1;
  }
}
else {
  W = U;
  for ( j = 0 ; j < NbN ; j++ ) Marqueur[IndexDesTermesNonNuls[j]] = 1;
}

k  = Spx->NombreDeChangementsDeBase - 1;
while ( k >= 0 ) {
  il    = EtaDeb[k];
  ilMax = il + EtaNbTerm[k];
  S     = 0.;
  while ( il < ilMax ) {
    S+= EtaMoins1Valeur[il] * W[EtaIndiceLigne[il]];
    il++;
  }
  /* Colonne egal aussi contrainte pour le terme diagonal */
	Col = EtaColonne[k];
	if ( S != 0.0 ) {
    W[Col] = S;
		if ( Marqueur[Col] == -1 ) {
		  Marqueur[Col] = 1;
	    IndexDesTermesNonNuls[NbN] = Col;				
			NbN++;
		}
	}
	else W[Col] = 0.0;
  k--;
}

if ( TypeDeStockage == COMPACT_LU ) {
  /* On recompacte */
  for ( j = 0 ; j < NbN ; j++ ) {
    Col = IndexDesTermesNonNuls[j];
	  U[j] = W[Col];
    W[Col] = 0;
	  Marqueur[Col] = -1;
  }
}
else {
  for ( j = 0 ; j < NbN ; j++ ) Marqueur[IndexDesTermesNonNuls[j]] = -1;
}

*NombreDeTermesNonNuls = NbN;

return;
}
