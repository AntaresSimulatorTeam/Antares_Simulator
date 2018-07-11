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

void SPX_CalculerABarreSAvecBaseComplete( PROBLEME_SPX * Spx, char * HyperCreuxInitial,
                                          char * CalculEnHyperCreux, char * TypeDEntree,
																				  char * TypeDeSortie )
{
int il; int ilMax; char Save; char SecondMembreCreux; double * ABarreS; int * Cdeb; int i;
int * CNbTerm; int * NumeroDeContrainte ; double * ACol; int NbTermesNonNuls; int * CntDeABarreSNonNuls; 

ABarreS            = Spx->ABarreS;
Cdeb               = Spx->Cdeb;
CNbTerm            = Spx->CNbTerm;
NumeroDeContrainte = Spx->NumeroDeContrainte;
ACol               = Spx->ACol;

CntDeABarreSNonNuls = NULL;

*CalculEnHyperCreux = NON_SPX;
*HyperCreuxInitial  = NON_SPX;
if ( Spx->TypeDeCreuxDeLaBase == BASE_HYPER_CREUSE && Spx->CalculABarreSEnHyperCreux == OUI_SPX &&
     Spx->FaireDuRaffinementIteratif <= 0 ) { 
  if ( CNbTerm[Spx->VariableEntrante] < TAUX_DE_REMPLISSAGE_POUR_VECTEUR_HYPER_CREUX * Spx->NombreDeContraintes ) {
    *CalculEnHyperCreux = OUI_SPX;
    *HyperCreuxInitial  = OUI_SPX;		
  }
}

if ( *CalculEnHyperCreux ==  OUI_SPX ) {

	CntDeABarreSNonNuls = Spx->CntDeABarreSNonNuls;	
	NbTermesNonNuls = 0;
  il    = Cdeb[Spx->VariableEntrante];		   
  ilMax = il + CNbTerm[Spx->VariableEntrante];
  while ( il < ilMax ) {
    ABarreS            [NbTermesNonNuls] = ACol[il];
		CntDeABarreSNonNuls[NbTermesNonNuls] = NumeroDeContrainte[il];
		NbTermesNonNuls++;		
    il++;
	}
	*TypeDEntree  = COMPACT_LU;
	*TypeDeSortie = COMPACT_LU;
	
}
else {

	for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) ABarreS[i] = 0;	

  il    = Cdeb[Spx->VariableEntrante];
  ilMax = il + CNbTerm[Spx->VariableEntrante];
  while ( il < ilMax ) {
    ABarreS[NumeroDeContrainte[il]] = ACol[il];	
    il++;
  }
  *TypeDEntree  = VECTEUR_LU;
  *TypeDeSortie = VECTEUR_LU;
	
}
  
/* Resolution du systeme */
Save = OUI_LU; /* Mis a NON_LU si pas de LU update */
SecondMembreCreux = OUI_LU;
SPX_ResoudreBYegalA( Spx, *TypeDEntree, ABarreS, CntDeABarreSNonNuls, &NbTermesNonNuls,
                     TypeDeSortie, *CalculEnHyperCreux, Save, SecondMembreCreux );

Spx->NbABarreSNonNuls = NbTermesNonNuls;

return;
}

