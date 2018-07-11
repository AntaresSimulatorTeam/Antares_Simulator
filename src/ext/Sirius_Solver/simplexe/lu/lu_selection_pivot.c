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

   FONCTION: Decomposition LU de la base.
             Choix du pivot.
	     
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# if OUTILS_DE_GESTION_MEMOIRE_DE_PNE_SOLVEUR	
  # include "mem_fonctions.h"
#endif

/*--------------------------------------------------------------------------------------------------*/
/*                   Selection du pivot en utilisant le nombre de Markowitz                         */

void LU_SelectionDuPivot( MATRICE * Matrice,
                          int * LignePivotChoisie, 
                          int * ColonnePivotChoisie  
                        )
{
int Ligne    ; int Colonne; int k ; double Mu         ; double MuTest     ; int NuScan;
double SeuilK ; double X    ; int il; int IndiceLigne  ; int kDeb; int iChoix;
char UtiliserLesSuperLignes; int * CNbTermMatriceActive;
int Rang; int MaxScan; int Kp; int NbTopt; int NbT;
int * PremLigne; int * SuivLigne; int * PremColonne; int * SuivColonne;

char * LigneRejeteeTemporairementPourPivotage; char * ColonneRejeteeTemporairementPourPivotage;
double ValeurDuPivotMinExtreme; char ContexteDeLaFactorisation; char ExclureLesEchecsMarkowitz;

int * Ldeb; int * LNbTerm; int * LIndiceColonne; double * Elm;
int * Cdeb; int * CNbTerm; int * CIndiceLigne;

UtiliserLesSuperLignes    = Matrice->UtiliserLesSuperLignes;
ValeurDuPivotMinExtreme   = Matrice->ValeurDuPivotMinExtreme;
ContexteDeLaFactorisation = Matrice->ContexteDeLaFactorisation;
ExclureLesEchecsMarkowitz = Matrice->ExclureLesEchecsMarkowitz;

Rang = Matrice->Rang;
PremLigne = Matrice->PremLigne;
SuivLigne = Matrice->SuivLigne;
PremColonne = Matrice->PremColonne;
SuivColonne = Matrice->SuivColonne;

LigneRejeteeTemporairementPourPivotage   = Matrice->LigneRejeteeTemporairementPourPivotage; 
ColonneRejeteeTemporairementPourPivotage = Matrice->ColonneRejeteeTemporairementPourPivotage;
    
*LignePivotChoisie   = -1;
*ColonnePivotChoisie = -1;

MaxScan = Matrice->MaxScan;
Kp      = Matrice->Kp;

Ldeb = Matrice->Ldeb;
LNbTerm = Matrice->LNbTerm;
LIndiceColonne = Matrice->LIndiceColonne;
Elm = Matrice->Elm;

Cdeb = Matrice->Cdeb;
CNbTerm = Matrice->CNbTerm;
CIndiceLigne = Matrice->CIndiceLigne;
CNbTermMatriceActive = Matrice->CNbTermMatriceActive;

/* Balayage des lignes qui on 1 terme */
Ligne = PremLigne[1];
while ( Ligne >= 0 ) {   
  if ( LigneRejeteeTemporairementPourPivotage[Ligne] == NON_LU ) { 
    /* Recherche de la valeur du terme pour controler si pivot nul */
    /* De toutes facons si la ligne a un seul terme, il ne sera jamais modifie */
    if ( UtiliserLesSuperLignes == OUI_LU ) {
      if ( Matrice->SuperLigneDeLaLigne[Ligne] != NULL ) {
			  /* Eviter de faire ca c'est a dire une recopie juste pour avoir la valeur */
        LU_RecopierUneLigneDeSuperLigneDansLigne( Matrice, Matrice->SuperLigneDeLaLigne[Ligne], Ligne );
	      X = fabs( Elm[Ldeb[Ligne]] ); /* Car 1 seul terme dans la ligne */
        iChoix = Matrice->SuperLigneDeLaLigne[Ligne]->IndiceColonne[0];	
      }
      else { 
        X = fabs( Elm[Ldeb[Ligne]] );
	      iChoix = LIndiceColonne[Ldeb[Ligne]];
      }
    }
    else {
      X = fabs( Elm[Ldeb[Ligne]] );
	    iChoix = LIndiceColonne[Ldeb[Ligne]];
    }    
    if ( X >= ValeurDuPivotMinExtreme ) {
      /* Pivot acceptable */
      *LignePivotChoisie   = Ligne;     
      *ColonnePivotChoisie = iChoix;
			goto FinSelectionPivot;	 
    }
    else if ( ExclureLesEchecsMarkowitz == OUI_LU && X < ValeurDuPivotMinExtreme ) {
      LigneRejeteeTemporairementPourPivotage[Ligne] = OUI_LU;      
      LU_DeClasserUneLigne( Matrice , Ligne , 1 );     
    }
  }    
  Ligne = SuivLigne[Ligne];
}

/* Balayage des colonnes qui on 1 terme */
Colonne = PremColonne[1];
while ( Colonne >= 0 ) {
  /* Compacter la colonne */
  if ( CNbTerm[Colonne] != 1 ) {
    LU_SupprimerTermesInutilesDansColonne( Matrice, Colonne, Cdeb[Colonne] ); 
  }
  /* Recherche de la valeur du terme pour controler si pivot acceptable */
  IndiceLigne = CIndiceLigne[Cdeb[Colonne]];
  if ( LigneRejeteeTemporairementPourPivotage[IndiceLigne] == NON_LU ) { 
    /* Recherche de l'index auquel on trouve la valeur du terme pivot potentiel */

    if ( UtiliserLesSuperLignes == OUI_LU ) {
      if ( Matrice->SuperLigneDeLaLigne[IndiceLigne] != NULL ) {					  
        for ( il = 0 ; Matrice->SuperLigneDeLaLigne[IndiceLigne]->IndiceColonne[il] != Colonne ; il++ );
	      /* Dans ce cas la SuperLigne ne peut contenir qu'une seule ligne */
	      il = Matrice->SuperLigneDeLaLigne[IndiceLigne]->CapaciteDesColonnes * il;
	      X  = fabs( Matrice->SuperLigneDeLaLigne[IndiceLigne]->ElmColonneDeSuperLigne[il] );	
      }
      else {		 				
        for ( il = Ldeb[IndiceLigne] ; LIndiceColonne[il] != Colonne ; il++ );				
        X = fabs( Elm[il] );
      }
    }
    else {
      for ( il = Ldeb[IndiceLigne] ; LIndiceColonne[il] != Colonne ; il++ );				
      X = fabs( Elm[il] );	
    }
        
    /* De toutes facons si la colonne a une seul terme il ne sera jamais modifie donc on le prend */
    if ( X >= ValeurDuPivotMinExtreme) { 
      /* Pivot acceptable */
      *ColonnePivotChoisie = Colonne;
      *LignePivotChoisie   = IndiceLigne;
			goto FinSelectionPivot;	 		
    }   
    else if ( ExclureLesEchecsMarkowitz == OUI_LU && X < ValeurDuPivotMinExtreme ) {    
      ColonneRejeteeTemporairementPourPivotage[Colonne] = OUI_LU;
      LU_DeClasserUneColonne( Matrice , Colonne , 1 );     
    }
  }		
  Colonne = SuivColonne[Colonne];
}

/* Suite */

kDeb = 2;
Mu     = Matrice->RangAuCarrePlus1; 
MuTest = Mu;
NuScan = 0;

if ( ContexteDeLaFactorisation == LU_SIMPLEXE ) goto ChoixPourSimplexe;

/* Strategie colonne d'abord */
for ( k = kDeb ; k <= Rang - Kp ; k++ ) {
  /* Balayage des colonnes qui on k termes */
  Colonne = PremColonne[k];
  while ( Colonne >= 0 ) {
    if ( ColonneRejeteeTemporairementPourPivotage[Colonne] == NON_LU ) {		
      LU_InitMinMarkowitzDeColonne( Matrice , Colonne , &Ligne , &X );      
      if ( X < Mu ) {      	
        Mu = X;
        *LignePivotChoisie   = Ligne;	  
        *ColonnePivotChoisie = Colonne;	
      }
      if ( X < MuTest ) NuScan++; 
      if ( NuScan == MaxScan ) goto FinSelectionPivot; 
    }
    Colonne = SuivColonne[Colonne];
  }    /* */    
  /* Balayage des lignes qui on k termes */
  Ligne = PremLigne[k];
  while ( Ligne >= 0 ) {
    if ( LigneRejeteeTemporairementPourPivotage[Ligne] == NON_LU ) {		
      LU_InitMinMarkowitzDeLigne( Matrice , Ligne , &Colonne , &X );      
      if ( X < Mu ) {            
        Mu  = X;
        *LignePivotChoisie   = Ligne;
        *ColonnePivotChoisie = Colonne;	  	  
      }
      if ( X < MuTest ) NuScan++; 
      if ( NuScan == MaxScan ) goto FinSelectionPivot;
    }
    Ligne = SuivLigne[Ligne];
  }    
  /* */
}

goto FinSelectionPivot;

ChoixPourSimplexe:

/* Strategie ligne d'abord */
NbTopt = Rang << 1;

for ( k = kDeb ; k <= Rang ; k++ ) {
  SeuilK = ( k - 1 ) * ( k - 1 );
  /* */
  /* Balayage des lignes qui on k termes */
  Ligne = PremLigne[k];
  while ( Ligne >= 0 ) {
    if ( LigneRejeteeTemporairementPourPivotage[Ligne] == NON_LU ) {		
      LU_InitMinMarkowitzDeLigne( Matrice , Ligne , &Colonne , &X );			
      if ( X < Mu ) {            
        Mu  = X;				
				NbTopt = CNbTermMatriceActive[Colonne] - 1;				
        *LignePivotChoisie   = Ligne;
        *ColonnePivotChoisie = Colonne;	  
        if ( Mu <= SeuilK ) goto FinSelectionPivot;
      }
			else if ( X == Mu ) {
			  NbT = CNbTermMatriceActive[Colonne] - 1;
        if ( NbT < NbTopt ) {
          Mu  = X;				
				  NbTopt = NbT;				
          *LignePivotChoisie   = Ligne;
          *ColonnePivotChoisie = Colonne;	  
          if ( Mu <= SeuilK ) goto FinSelectionPivot;
				}
			}			
      if ( X < MuTest ) NuScan++; 
      if ( NuScan == MaxScan ) goto FinSelectionPivot; 
    }
    Ligne = SuivLigne[Ligne];
  }    
  /* Balayage des colonnes qui on k termes */
  Colonne = PremColonne[k];
  while ( Colonne >= 0 ) {
    if ( ColonneRejeteeTemporairementPourPivotage[Colonne] == NON_LU ) {		
      LU_InitMinMarkowitzDeColonne( Matrice , Colonne , &Ligne , &X );			
      if ( X < Mu ) {           
        Mu  = X;
				NbTopt = k - 1;
        *ColonnePivotChoisie = Colonne;
        *LignePivotChoisie   = Ligne;		
        if ( Mu <= SeuilK ) goto FinSelectionPivot; 
      }
			else if ( X == Mu ) {
        if ( k - 1 < NbTopt ) {
          Mu  = X;
				  NbTopt = k - 1;
          *ColonnePivotChoisie = Colonne;
          *LignePivotChoisie   = Ligne;		
          if ( Mu <= SeuilK ) goto FinSelectionPivot; 				 
			  }
			}
      if ( X < MuTest ) NuScan++; 
      if ( NuScan == MaxScan ) goto FinSelectionPivot; 
    }
    Colonne = SuivColonne[Colonne];
  }
  /* */
}

FinSelectionPivot:
if ( *ColonnePivotChoisie >= 0 ) {
  if ( CNbTerm[*ColonnePivotChoisie] != CNbTermMatriceActive[*ColonnePivotChoisie] ) {
    LU_SupprimerTermesInutilesDansColonne( Matrice, *ColonnePivotChoisie, Cdeb[*ColonnePivotChoisie] );
  }
}

return;
}




