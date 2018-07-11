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
	           l'inverse
	 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"
   
# include "lu_define.h"

# define CONTROLE_RAZ_DES_VECTEURS NON_SPX

/*----------------------------------------------------------------------------*/
/* Reste ADRESSAGE_INDIRECT_LU a pogrammer */
void SPX_AppliquerLesEtaVecteurs( PROBLEME_SPX * Spx,
                                  double * A,
                                  int * IndexDesTermesNonNuls,
			                            int * NombreDeTermesNonNuls,
				                          char   CalculEnHyperCreux,
				                          char   TypeDeStockage /* COMPACT_LU
							                    ADRESSAGE_INDIRECT_LU
							                    VECTEUR_LU */ 
      		                      )
{
int   k; int Colonne; int il; int ilMax; double S; int * EtaIndiceLigne; int Cnt;
double * EtaMoins1Valeur; int * EtaNbTerm; int * EtaColonne; int j; int NbN; 
int * Marqueur; double * W; 

EtaIndiceLigne  = Spx->EtaIndiceLigne;
EtaMoins1Valeur = Spx->EtaMoins1Valeur;
EtaNbTerm       = Spx->EtaNbTerm;
EtaColonne      = Spx->EtaColonne;
  
if ( CalculEnHyperCreux == OUI_SPX ) {
  if ( TypeDeStockage == COMPACT_LU ) goto HyperCreux;
  else if ( TypeDeStockage == ADRESSAGE_INDIRECT_LU ) goto HyperCreux;
	else if ( TypeDeStockage != VECTEUR_LU ){
	  printf("AppliquerLesEtaVecteurs: type de stockage non gere\n");
	  exit(0);
  }
}

k  = 0;
il = 0;
while ( k < Spx->NombreDeChangementsDeBase ) {	
  ilMax   = il + EtaNbTerm[k];
  Colonne = EtaColonne[k];
  S          = A[Colonne];
  A[Colonne] = 0.;
  if ( S != 0.0 ) {
    while ( il != ilMax ) {			    				
      A[EtaIndiceLigne[il]]+= EtaMoins1Valeur[il] * S;
      il++;
    }
  }	
  else il = ilMax; 
  k++;
}

return;

HyperCreux:

# if CONTROLE_RAZ_DES_VECTEURS == OUI_SPX
  for ( j = 0 ; j < Spx->NombreDeContraintes ;  j++ ) {
    if ( Spx->AReduit[j] != 0 ) {
	    printf("Spx->AReduit[%d] = %e\n",j,Spx->AReduit[j]);
	    exit(0);
	  }
    if ( Spx->Marqueur[j] != -1 ) {
	    printf("Spx->Marqueur[%d] = %d\n",j,Spx->Marqueur[j]);
	    exit(0);
	  }	
  }
# endif

Marqueur = Spx->Marqueur;

NbN = *NombreDeTermesNonNuls;

if ( TypeDeStockage == COMPACT_LU ) {
  W = Spx->AReduit;	
  for ( j = 0 ; j < NbN ; j++ ) {
    Cnt = IndexDesTermesNonNuls[j];		
    W[Cnt] = A[j];
	  Marqueur[Cnt] = 1;
  }
}
else {
  W = A;
  for ( j = 0 ; j < NbN ; j++ ) Marqueur[IndexDesTermesNonNuls[j]] = 1;
}

k  = 0;
il = 0;
while ( k < Spx->NombreDeChangementsDeBase ) {	
  ilMax   = il + EtaNbTerm[k];
  Colonne = EtaColonne[k];
  S          = W[Colonne];
  W[Colonne] = 0.;
  if ( S != 0.0 ) {		  			
    while ( il != ilMax ) {
		  Cnt = EtaIndiceLigne[il];			
      W[Cnt] += EtaMoins1Valeur[il] * S;			
			if ( Marqueur[Cnt] == -1 ) {
			  Marqueur[Cnt] = 1;
	      IndexDesTermesNonNuls[NbN] = Cnt;				
				NbN++;
			}
      il++;
    }
  }
  else il = ilMax;
  k++;
}

if ( TypeDeStockage == COMPACT_LU ) {
  for ( j = 0 ; j < NbN ; j++ ) {
    Cnt = IndexDesTermesNonNuls[j];
		/* Ca consomme du temps pour pas grand chose 
		if ( W[Cnt] == 0 ) {
		  NbN--;
			IndexDesTermesNonNuls[j] = IndexDesTermesNonNuls[NbN];			
	    Marqueur[Cnt] = -1;			
			j--;
			continue;
		}
		*/
	  A[j] = W[Cnt];
    W[Cnt] = 0;		
	  Marqueur[Cnt] = -1;
  }
}
else {
  for ( j = 0 ; j < NbN ; j++ ) Marqueur[IndexDesTermesNonNuls[j]] = -1;
}

*NombreDeTermesNonNuls = NbN;

return;
}


