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

   FONCTION: Calcul de A_BARRE_S = B-1 * AS
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"   

# include "lu_define.h"

/*----------------------------------------------------------------------------*/
void SPX_TenterRestaurationCalculABarreSEnHyperCreux( PROBLEME_SPX * Spx )
{
double * ABarreS; int Count; int iMx; int i;
	
if ( Spx->CountEchecsABarreS == 0 ) {
  if ( Spx->Iteration % CYCLE_TENTATIVE_HYPER_CREUX == 0 ) {
		Spx->NbEchecsABarreS    = SEUIL_REUSSITE_CREUX;
		Spx->CountEchecsABarreS = SEUIL_REUSSITE_CREUX + 2;
	}
}
if ( Spx->CountEchecsABarreS == 0 ) return;

Spx->CountEchecsABarreS--;		
/* On compte le nombre de termes non nuls du resultat */
ABarreS = Spx->ABarreS;
Count = 0;

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
  iMx = Spx->RangDeLaMatriceFactorisee;
}
else {
  iMx = Spx->NombreDeContraintes;
}

for ( i = 0 ; i < iMx ; i++ ) if ( ABarreS[i] != 0.0 ) Count++;

if ( Count < 0.1 * iMx ) Spx->NbEchecsABarreS--;
if ( Spx->NbEchecsABarreS <= 0 ) {
  # if VERBOSE_SPX
    printf("Remise en service de l'hyper creux pour le calcul de ABarreS, iteration %d\n",Spx->Iteration);
	# endif
  Spx->AvertissementsEchecsABarreS = 0;
  Spx->CountEchecsABarreS = 0;
	Spx->CalculABarreSEnHyperCreux = OUI_SPX;
}
else if ( Spx->CountEchecsABarreS <= 0 ) {
  Spx->CountEchecsABarreS = 0;
  if ( Spx->CalculABarreSEnHyperCreux == NON_SPX ) Spx->AvertissementsEchecsABarreS ++;
  if ( Spx->AvertissementsEchecsABarreS >= SEUIL_ABANDON_HYPER_CREUX ) {
    # if VERBOSE_SPX
      printf("Arret prolonge de l'hyper creux pour le calcul de ABarreS, iteration %d\n",Spx->Iteration);
	  # endif	
	  Spx->CalculABarreSEnHyperCreuxPossible = NON_SPX;		
	}
}

return;
}

/*----------------------------------------------------------------------------*/

void SPX_CalculerABarreS( PROBLEME_SPX * Spx )
{
double * ABarreS; char OK; int IndexBase; int * CntDeABarreSNonNuls; int i;  char TypeDEntree;
char TypeDeSortie; char CalculEnHyperCreux; char HyperCreuxInitial;

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
  SPX_CalculerABarreSAvecBaseReduite( Spx, &HyperCreuxInitial, &CalculEnHyperCreux, &TypeDEntree, &TypeDeSortie );
}
else {
  SPX_CalculerABarreSAvecBaseComplete( Spx, &HyperCreuxInitial, &CalculEnHyperCreux, &TypeDEntree, &TypeDeSortie );
}

ABarreS = Spx->ABarreS;
CntDeABarreSNonNuls = Spx->CntDeABarreSNonNuls;
								 
if ( CalculEnHyperCreux == OUI_SPX ) {
  if ( TypeDeSortie != COMPACT_LU ) {
    CalculEnHyperCreux = NON_SPX;
    /* Ca s'est pas bien passe et on s'est forcement retrouve en VECTEUR_LU */		
		Spx->NbEchecsABarreS++;
		/*printf("Echec hyper creux ABarreS iteration %d\n",Spx->Iteration);*/
		if ( Spx->NbEchecsABarreS >= SEUIL_ECHEC_CREUX ) {
      # if VERBOSE_SPX
        printf("Arret de l'hyper creux pour le calcul de ABarreS, iteration %d\n",Spx->Iteration);
	    # endif			
		  Spx->CalculABarreSEnHyperCreux = NON_SPX;
			Spx->CountEchecsABarreS  = 0;
		}		
  }
  else Spx->NbEchecsABarreS = 0;
}

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
  IndexBase = Spx->OrdreColonneDeLaBaseFactorisee[Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante]];
}
else {
  IndexBase = Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante];
}
										 
if ( CalculEnHyperCreux == OUI_SPX ) {
	Spx->TypeDeStockageDeABarreS = COMPACT_SPX;
  OK = NON_SPX;
  for ( i = 0 ; i < Spx->NbABarreSNonNuls ; i++ ) {
	  if ( CntDeABarreSNonNuls[i] == IndexBase ) {
      Spx->ABarreSCntBase = ABarreS[i];
			OK = OUI_SPX;
      break;
		}
	}
	if ( OK == NON_SPX ) {
	  /* Pb d'epsilon, on prend la aleur de NBarreR */
    Spx->ABarreSCntBase = Spx->NBarreR[Spx->VariableEntrante]; /* Valable que l'on soit en base reduite ou non */
	}	
}
else {
  Spx->TypeDeStockageDeABarreS = VECTEUR_SPX;	
  Spx->ABarreSCntBase = ABarreS[IndexBase];
  /* Si on est pas en hyper creux, on essaie d'y revenir */
	if ( HyperCreuxInitial == NON_SPX ) {
    if ( Spx->CalculABarreSEnHyperCreux == NON_SPX ) {
      if ( Spx->CalculABarreSEnHyperCreuxPossible == OUI_SPX ) {
        SPX_TenterRestaurationCalculABarreSEnHyperCreux( Spx );
	    }
    }   
	}
	
}

return;
}

