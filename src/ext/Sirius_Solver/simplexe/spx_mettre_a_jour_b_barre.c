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

   FONCTION: Mise de BBarre = B^{-1} * b 

                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"  

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"
# include "lu_fonctions.h"
  
/*----------------------------------------------------------------------------*/
/*                    Cas de la mise a jour de BBarre                         */

void SPX_MettreAJourBBarre( PROBLEME_SPX * Spx )   
{
int Index; int i; double * Bs; double * BBarre; double * ABarreS; int * CntDeABarreSNonNuls;
double DeltaXSurLaVariableHorsBase; int * T; int * IndexTermesNonNuls; int NbTermesNonNuls;
int IndexCntBase; int NbBBarreModifies; int * IndexDeBBarreModifies; char StockageDeBs; 
int * ColonneDeLaBaseFactorisee; int IndexMax;

/* Bs est toujours a 0 des qu'on a fini de s'en servir */
/*memset( (char *) Bs , 0 , Spx->NombreDeContraintes * sizeof( double ) );*/
/*for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) Bs[i] = 0.0;*/
Bs = Spx->Bs;

IndexTermesNonNuls = (int *) Spx->ErBMoinsUn;

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
  SPX_MettreAJourBBarreAvecBaseReduite( Spx, Bs, IndexTermesNonNuls, &NbTermesNonNuls, &StockageDeBs );
}
else {
  SPX_MettreAJourBBarreAvecBaseComplete( Spx, Bs, IndexTermesNonNuls, &NbTermesNonNuls, &StockageDeBs);
}

T = Spx->T;
ABarreS = Spx->ABarreS;
BBarre  = Spx->BBarre;
DeltaXSurLaVariableHorsBase = Spx->DeltaXSurLaVariableHorsBase;
NbBBarreModifies = 0;
IndexDeBBarreModifies = (int *) Spx->V;

if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
  ColonneDeLaBaseFactorisee = Spx->ColonneDeLaBaseFactorisee;
  IndexMax = Spx->RangDeLaMatriceFactorisee;
	IndexCntBase = Spx->OrdreColonneDeLaBaseFactorisee[Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante]];
}
else {
  IndexMax = Spx->NombreDeContraintes;
	IndexCntBase = Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante];
}

if ( Spx->TypeDeStockageDeABarreS == COMPACT_SPX ) {
  CntDeABarreSNonNuls = Spx->CntDeABarreSNonNuls;	
  for ( i = 0 ; i < Spx->NbABarreSNonNuls ; i++ ) {
    Index = CntDeABarreSNonNuls[i];
	  BBarre[Index]-= DeltaXSurLaVariableHorsBase * ABarreS[i];
	  T[Index] = 1;
	  IndexDeBBarreModifies[NbBBarreModifies] = Index;
	  NbBBarreModifies++;
  }
  if ( StockageDeBs == COMPACT_SPX ) {	
    for ( i = 0 ; i < NbTermesNonNuls ; i++ ) {
	    Index = IndexTermesNonNuls[i];
	    BBarre[Index] -= Bs[i];			
	    Bs[i] = 0;			
	    if ( T[Index] == -1 ) {			
	      T[Index] = 1;
		    IndexDeBBarreModifies[NbBBarreModifies] = Index;
		    NbBBarreModifies++;			
		  }			
	  }
	}  	
  else {	
    for ( Index = 0 ; Index < IndexMax ; Index++ ) {
	    if ( Bs[Index] != 0.0 ) {
	      BBarre[Index] -= Bs[Index];
		    Bs[Index] = 0;				
		    if ( T[Index] == -1 ) {							
		      T[Index] = 1;
		      IndexDeBBarreModifies[NbBBarreModifies] = Index;
		      NbBBarreModifies++;
		    }
		  }
	  }
	}
}
else {
  /* ABarreS est sous forme vecteur */
  if ( StockageDeBs == COMPACT_SPX ) {	
    for ( Index = 0 ; Index < IndexMax ; Index++ ) {
	    if ( ABarreS[Index] != 0.0 ) {
	      BBarre[Index] -= DeltaXSurLaVariableHorsBase * ABarreS[Index];
		    T[Index] = 1;
		    IndexDeBBarreModifies[NbBBarreModifies] = Index;
		    NbBBarreModifies++;				
	    }
	  }		
    for ( i = 0 ; i < NbTermesNonNuls ; i++ ) {
	    Index = IndexTermesNonNuls[i];
	    BBarre[Index]-= Bs[i];
	    Bs[i] = 0;			
	    if ( T[Index] == -1 ) {							
	      T[Index] = 1;
		    IndexDeBBarreModifies[NbBBarreModifies] = Index;
		    NbBBarreModifies++;
		  }		
	  }
	}							
	else {	
    for ( Index = 0 ; Index < IndexMax ; Index++ ) {
	    if ( Bs[Index] != 0.0 || ABarreS[Index] != 0.0 ) {
        BBarre[Index]-= Bs[Index] + ( DeltaXSurLaVariableHorsBase * ABarreS[Index] );
	 	    Bs[Index] = 0;				
		    T[Index] = 1;
		    IndexDeBBarreModifies[NbBBarreModifies] = Index;
		    NbBBarreModifies++;				
		  }
	  }
	}  
}

/* Cas particulier des variables echangees */
if ( Spx->ChangementDeBase == OUI_SPX ) {
  if ( Spx->PositionDeLaVariable[Spx->VariableEntrante] == HORS_BASE_SUR_BORNE_INF || 
       Spx->PositionDeLaVariable[Spx->VariableEntrante] == HORS_BASE_A_ZERO ) {			 
    BBarre[IndexCntBase] = DeltaXSurLaVariableHorsBase;
	  if ( T[IndexCntBase] == -1 ) {							
	    T[IndexCntBase] = 1;
	    IndexDeBBarreModifies[NbBBarreModifies] = IndexCntBase;
	    NbBBarreModifies++;
	  }				
  }
  else {	
    BBarre[IndexCntBase] = DeltaXSurLaVariableHorsBase + Spx->Xmax[Spx->VariableEntrante];		
	  if ( T[IndexCntBase] == -1 ) {							
	    T[IndexCntBase] = 1;
	    IndexDeBBarreModifies[NbBBarreModifies] = IndexCntBase;
	    NbBBarreModifies++;
	  }				
  }
}

Spx->BBarreAEteCalculeParMiseAJour = OUI_SPX;

SPX_MajDesVariablesEnBaseAControler( Spx, NbBBarreModifies, IndexDeBBarreModifies, T );

return;
}   

