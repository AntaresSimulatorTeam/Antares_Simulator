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

   FONCTION: Mise a jour de la forme produit de l'inverse
                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"
# include "lu_fonctions.h"			     

/*----------------------------------------------------------------------------*/

void SPX_MettreAJourLaBase( PROBLEME_SPX * Spx , int * SuccesUpdate )
{
int Cnt; int CntBase; double S; int * EtaDeb; int * EtaColonne; int * EtaNbTerm;
double * ABarreS; double * EtaMoins1Valeur; int * EtaIndiceLigne; int LastEta;
int * CntDeABarreSNonNuls;	int i; int NombreDeChangementsDeBase; int Colonne;

if ( Spx->UtiliserLaLuUpdate == OUI_SPX ) { 
  /* On donne: 
  - le numero de la colonne qui correspond a la variable sortante
  - un pointeur sur la colonne A de la variable entrante 
  - la colonne A
  */
  if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
	  Colonne = Spx->OrdreColonneDeLaBaseFactorisee[Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante]];
	}
	else {
	  Colonne = Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante];
	}
  LU_UpdateLuSpikePret(                 
               Spx->MatriceFactorisee,
               Colonne,
               SuccesUpdate   
                        ); 
  Spx->NombreDeChangementsDeBase++;    
  return;
}

/* Division par 0: c'est pas possible car sinon c'est pas un pivot */
CntBase         = Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante];
ABarreS         = Spx->ABarreS;
EtaDeb          = Spx->EtaDeb;
EtaColonne      = Spx->EtaColonne;
EtaNbTerm       = Spx->EtaNbTerm;
EtaMoins1Valeur = Spx->EtaMoins1Valeur;
EtaIndiceLigne  = Spx->EtaIndiceLigne;
LastEta         = Spx->LastEta;
NombreDeChangementsDeBase = Spx->NombreDeChangementsDeBase;

S = -1. / Spx->ABarreSCntBase;

EtaDeb    [ NombreDeChangementsDeBase ] = LastEta + 1;
EtaColonne[ NombreDeChangementsDeBase ] = CntBase;
EtaNbTerm [ NombreDeChangementsDeBase ] = 0;

if ( Spx->TypeDeStockageDeABarreS == VECTEUR_SPX ) {
  for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
    if ( ABarreS[Cnt] == 0.0 ) {
      /* Ne peut pas arriver si Cnt = CntBase */
		  continue;
		}
    EtaNbTerm[ NombreDeChangementsDeBase ]++;
    LastEta++;
    EtaIndiceLigne[ LastEta ] = Cnt;
    if ( Cnt == CntBase ) EtaMoins1Valeur[LastEta] = -S;
    else                  EtaMoins1Valeur[LastEta] = ABarreS[Cnt] * S;
  }	
}
else {
  CntDeABarreSNonNuls = Spx->CntDeABarreSNonNuls;	
  for ( i = 0 ; i < Spx->NbABarreSNonNuls ; i++ ) {
		if ( ABarreS[i] != 0 ) {
      /* Rq: si Cnt == CntBase ABarreS[i] est different de 0 */
		
      Cnt = CntDeABarreSNonNuls[i];
      EtaNbTerm[ NombreDeChangementsDeBase ]++;
      LastEta++;
      EtaIndiceLigne[ LastEta ] = Cnt;		
      if ( Cnt == CntBase ) EtaMoins1Valeur[LastEta] = -S;    
      else                  EtaMoins1Valeur[LastEta] = ABarreS[i] * S;
			
		}		
  }
}

Spx->LastEta = LastEta;
Spx->NombreDeChangementsDeBase++;

/* Si trop de termes en moyenne on refactorise */
if ( Spx->LastEta > Spx->RemplissageMaxDeLaFPI ) { 
  /*
  printf("Detection d exigence de factorisation dans SPX_MettreAJourLaBase a l iteration %d\n",Spx->Iteration); 
  */  
  Spx->FactoriserLaBase = OUI_SPX;   
}

return;
}
