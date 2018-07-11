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

   FONCTION: Allouer / desallouer les tableaux pour la factoristion LU
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void LU_ConstruireProbleme(
                    MATRICE * Matrice,
                    double * ValeurDesTermesDeLaMatrice, 
                    int   * IndicesDeLigne, 
                    int   * Ideb,  
                    int   * NbTermesDesColonnes, 
                    int     NbCol,
                    int     NbTrm_L,
                    int     NbTrm_U
                          )
{
int icCourant; int il; long ilDeb; int i; int Ligne; int Colonne; int ic; 
int icMax; int Rang; char Trouve; int MargePourCreationDeTermesLignes;
int MargePourCreationDeTermesColonnes; int * IndexCourant;
int * Ldeb; int * LNbTerm; int * LDernierPossible; int * LIndiceColonne; double * Elm;
int * LignePrecedente; int * LigneSuivante;  
int * Cdeb; int * CNbTerm; int * CDernierPossible; int * CIndiceLigne;
int * ColonnePrecedente;int * ColonneSuivante;

Matrice->Rang = NbCol; /* C'est le rang en etant optimiste */
Rang          = Matrice->Rang ;

Matrice->RangAuCarre      = (double) Rang * (double) Rang;
Matrice->RangAuCarrePlus1 = Matrice->RangAuCarre + 1;

Matrice->MaxScan = 3; 
if ( Matrice->ContexteDeLaFactorisation != LU_SIMPLEXE ) Matrice->MaxScan = 10;

Matrice->PivotMin = Matrice->ValeurDuPivotMin;

Matrice->EtapeSinguliere = Rang;
					     
Matrice->LuUpdateEnCours = NON_LU;

Matrice->SecondMembreCreux = NON_LU;

Matrice->FactoriserEnMatricePleine = NON_LU;
  
i = Rang + 1;
Matrice->PlusPetitNombreDeTermesDesLignes   = i;
Matrice->PlusPetitNombreDeTermesDesColonnes = i;

/* Transferts dans les tableaux de travail */
/* Initialisations */

for ( i = 0 ; i < Rang; i++ ) Matrice->AbsDuPlusGrandTermeDeLaLigne[i] = -1.;
memset( (char *) Matrice->ColonneRejeteeTemporairementPourPivotage, NON_LU , Rang * sizeof( char ) );
memset( (char *) Matrice->LigneRejeteeTemporairementPourPivotage  , NON_LU , Rang * sizeof( char ) ); 
for ( i = 0 ; i <= Rang; i++ ) Matrice->PremLigne[i] = -1;
for ( i = 0 ; i < Rang; i++ ) Matrice->OrdreLigne[i] = i;
for ( i = 0 ; i < Rang; i++ ) Matrice->InverseOrdreLigne[i] = i;
for ( i = 0 ; i < Rang; i++ ) Matrice->OrdreColonne[i] = i;
for ( i = 0 ; i < Rang; i++ ) Matrice->InverseOrdreColonne[i] = i;

MargePourCreationDeTermesLignes = Matrice->MargePourCreationDeTermesLignes;

Ldeb    = Matrice->Ldeb;
LNbTerm = Matrice->LNbTerm;
LDernierPossible = Matrice->LDernierPossible;
LIndiceColonne   = Matrice->LIndiceColonne;
Elm              = Matrice->Elm;
LignePrecedente  = Matrice->LignePrecedente;
LigneSuivante    = Matrice->LigneSuivante;

Cdeb    = Matrice->Cdeb;
CNbTerm = Matrice->CNbTerm;
CDernierPossible  = Matrice->CDernierPossible;
CIndiceLigne      = Matrice->CIndiceLigne;
ColonnePrecedente = Matrice->ColonnePrecedente;
ColonneSuivante   = Matrice->ColonneSuivante;

for ( i = 0 ; i < Rang ; i++ ) {
	LignePrecedente  [i] = i-1;
	ColonnePrecedente[i] = i-1;
	LigneSuivante    [i] = i+1;
	ColonneSuivante  [i] = i+1;	
}
LigneSuivante  [Rang - 1] = -1;
ColonneSuivante[Rang - 1] = -1;

memset( (char *) LNbTerm, 0, Rang * sizeof( int ) );
for ( Colonne = 0 ; Colonne < Rang ; Colonne++ ) {  
  ic    = Ideb[Colonne];
  icMax = ic + NbTermesDesColonnes[Colonne];  
  while ( ic < icMax ) {  
    LNbTerm[IndicesDeLigne[ic]]++; 
    ic++;			 
  }
}

IndexCourant = (int *) Matrice->W;
il = 0;
for ( Ligne = 0 ; Ligne < Rang ; Ligne++ ) {
  Ldeb[Ligne] = il;
	IndexCourant[Ligne] = il;
	il += LNbTerm[Ligne] + MargePourCreationDeTermesLignes;
  LDernierPossible[Ligne] = il - 1;
}
Matrice->LIndexLibre   = il;
Matrice->DerniereLigne = Rang - 1;

/* Stockage des lignes */  
if ( Matrice->FaireDuPivotageDiagonal == NON_LU ) {
  for ( Colonne = 0 ; Colonne < Rang ; Colonne++ ) {
    ic    = Ideb[Colonne];
    icMax = ic + NbTermesDesColonnes[Colonne];  
    while ( ic < icMax ) {
		  il = IndexCourant[IndicesDeLigne[ic]];
			IndexCourant[IndicesDeLigne[ic]]++;			
		  Elm[il] = ValeurDesTermesDeLaMatrice[ic];
      LIndiceColonne[il] = Colonne;   
      ic++;			 
    }
  }
}
else {
  /* Si on fait du pivotage diagonal on met le terme diagonal en premier */
  /* Attention, il faut absolument qu'il y ait un terme diagonal meme nul */
  for ( Colonne = 0 ; Colonne < Rang ; Colonne++ ) {
    ic    = Ideb[Colonne];
    icMax = ic + NbTermesDesColonnes[Colonne];  
    while ( ic < icMax ) {
		  Ligne = IndicesDeLigne[ic];
		  il = IndexCourant[Ligne];
			IndexCourant[Ligne]++;			
      if ( Ligne == Colonne ) {
			  ilDeb = Ldeb[Ligne];
	      /* On inverse */
        Elm           [il]    = Elm[ilDeb];
        LIndiceColonne[il]    = LIndiceColonne[ilDeb];				
	      Elm           [ilDeb] = ValeurDesTermesDeLaMatrice[ic];
	      LIndiceColonne[ilDeb] = Colonne;
        ic++;			 
	      goto SansTest;
      }
      else {
        Elm           [il] = ValeurDesTermesDeLaMatrice[ic];
        LIndiceColonne[il] = Colonne;
      }
      ic++;			 
    }

    /* Si on ne trouve pas de terme diagonal, on en met un qui vaut 0. On tape un peu dans la marge allouee
       mais comme elle n'est pas nulle, ca passe */
    Ligne = Colonne;		
		ilDeb = Ldeb[Ligne];
		il    = ilDeb + LNbTerm[Ligne];				
    Elm           [il]    = Elm[ilDeb];
    LIndiceColonne[il]    = LIndiceColonne[ilDeb];		
    Elm           [ilDeb] = 0.0;
    LIndiceColonne[ilDeb] = Colonne;
    LNbTerm[Ligne]++;
    /* Comme on a cree un terme qui ne figurait pas dans la matrice d'entree, il faut en tenir compte
       dans le nombre de termes de la matrice */
    Matrice->NombreDeTermes++;
    Matrice->LIndexLibre++;
       
    SansTest:
    while ( ic < icMax ) {  
			il = IndexCourant[IndicesDeLigne[ic]];
			IndexCourant[IndicesDeLigne[ic]]++;			
      Elm           [il] = ValeurDesTermesDeLaMatrice[ic];
      LIndiceColonne[il] = Colonne;
      ic++;			 
    }    
  }  
}

/* Stockage des colonnes si necessaire */
if ( Matrice->LaMatriceEstSymetriqueEnStructure == OUI_LU ) {
  /* Pas besoin du stockage par colonne */
  goto FinDesOperationsDeStockage;
}

if ( Matrice->LaMatriceEstSymetrique == OUI_LU ) {
  /* Pas besoin du stockage par colonne */
  goto FinDesOperationsDeStockage;
}

/* Stockage par colonnes */
for ( i = 0 ; i <= Rang; i++ ) Matrice->PremColonne[i] = -1;

MargePourCreationDeTermesColonnes = Matrice->MargePourCreationDeTermesColonnes;

/* Colonne de la matrice */
icCourant = 0;
if ( Matrice->FaireDuPivotageDiagonal == NON_LU ) {  
  for ( Colonne = 0 ; Colonne < Rang ; Colonne++ ) {
    ic    = Ideb[Colonne];
    icMax = ic + NbTermesDesColonnes[Colonne];
    Cdeb   [Colonne] = icCourant;
		CNbTerm[Colonne] = NbTermesDesColonnes[Colonne];
    while ( ic != icMax ) {  
      CIndiceLigne[icCourant] = IndicesDeLigne[ic];
      icCourant++;
      ic++;			 
    }
	  icCourant += MargePourCreationDeTermesColonnes;
		CDernierPossible[Colonne] = icCourant - 1;
  }
}
else {
    /* Pivotage diagonal demande */
  Trouve = NON_LU;
  for ( Colonne = 0 ; Colonne < Rang ; Colonne++ ) {
    ic    = Ideb[Colonne];
    icMax = ic + NbTermesDesColonnes[Colonne];
    Cdeb   [Colonne] = icCourant;
		CNbTerm[Colonne] = NbTermesDesColonnes[Colonne];		
    while ( ic != icMax ) {  
      Ligne = IndicesDeLigne[ic];
      if ( Ligne == Colonne ) Trouve = OUI_LU;
      /* */   
      CIndiceLigne[icCourant] = Ligne;
      icCourant++;			
      /* */
      ic++;			 
    }      
    /* Si on a demande le pivotage diagonal et qu'il n'y a pas de terme diagonal on en cree un */
    /* On tape dans la marge d'allocation */
    if ( Trouve == NON_LU ) {
      CIndiceLigne[icCourant] = Colonne;
      CNbTerm[Colonne]++;
    }
	  icCourant += MargePourCreationDeTermesColonnes;
		CDernierPossible[Colonne] = icCourant - 1;
	}
}
Matrice->CIndexLibre     = CDernierPossible[Rang - 1] + 1;
Matrice->DerniereColonne = Rang - 1;

FinDesOperationsDeStockage:

Matrice->Kp = 0;

/* Informations pour les triangles */
Matrice->IndexLibreDeL = 0;
Matrice->DernierIndexLibreDeL = NbTrm_L - 1;
								     
Matrice->IndexLibreDeU = 0;
Matrice->DernierIndexLibreDeU = NbTrm_U - 1;

memset( Matrice->W        , 0 , Rang * sizeof( double ) );
memset( Matrice->Marqueur , 0 , Rang * sizeof( char   ) );

/* CNbTerm et CNbTermMatriceActive inutile dans le cas des matrices symetriques ou symetriques en topologie */
/* On pourrait donc dans ce cas ne pas les allouer non plus */
memcpy( (char *) Matrice->CNbTermMatriceActive, (char *) Matrice->CNbTerm, Rang * sizeof( int ) );

/* Scaling eventuel */
if ( Matrice->FaireScaling == OUI_LU ) LU_CalculerLeScaling( Matrice );
/* Attention: Matrice->FaireScaling peut etre mis a NON_LU dans LU_CalculerLeScaling */
if ( Matrice->FaireScaling == OUI_LU ) LU_Scaling( Matrice );

return;
}











