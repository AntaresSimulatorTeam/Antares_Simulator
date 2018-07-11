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
             Elimination d'une ligne, cas d'une matrice pleine.
                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

/*--------------------------------------------------------------------------------------------------*/

void LU_EliminationDUneLigneMatricePleine( MATRICE * Matrice , int LignePivot , int ColonnePivot )
{

# ifndef ON_COMPILE
/* Pour eviter les warning de compil */
MATRICE * Mat; int Ligne; int Colonne;
Mat = Matrice;
Ligne = LignePivot;
Colonne = ColonnePivot;
# endif 

# ifdef ON_COMPILE

int il; int ic; int ilU; int ilL ; 
int * IndiceColonne; int * IndiceLigne   ; double UnSurValeurDuPivot; 
double ValTermeColonnePivot; int Ligne; int Colonne; int itC;        
int NbTermesLignePivot; int NbTermesColonnePivot; double * Elm;

int * Ldeb; int * LNbTerm; int * LDernierPossible; int * LIndiceColonne; double * Elm;
int * Cdeb; int * CNbTerm; int * CDernierPossible; int * CIndiceLigne; int * CNbTermMatriceActive;

printf("Attention LU_EliminationDUneLigneMatricePleine ne met pas a jour AbsDuPlusGrandTermeDeLaLigne \n");

Ldeb             = Matrice->Ldeb;
LNbTerm          = Matrice->LNbTerm;
LDernierPossible = Matrice->LDernierPossible;
LIndiceColonne   = Matrice->LIndiceColonne;
Elm              = Matrice->Elm;
 
Cdeb                 = Matrice->Cdeb;
CNbTerm              = Matrice->CNbTerm;
CNbTermMatriceActive = Matrice->CNbTermMatriceActive;
CDernierPossible     = Matrice->CDernierPossible;
CIndiceLigne         = Matrice->CIndiceLigne;

ilDebLignePivot    = Ldeb[LignePivot];
NbTermesLignePivot = LNbTerm[LignePivot];

icDebColonnePivot    = Cdeb[ColonnePivot];
NbTermesColonnePivot = CNbTerm[ColonnePivot]; /* Pour la colonne pivot c'est egal a CNbTermMatriceActive */

/* Transfert de la ligne dans un tableau de travail */
for ( il = ilDebLignePivot ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) Matrice->W[IndiceColonne[il]] = Elm[il];

UnSurValeurDuPivot = 1. / Matrice->W[ColonnePivot];
Matrice->W[ColonnePivot] = 0.0;

/* Maintenant on balaye toutes les lignes de la matrice active qui ont un terme non nul dans la colonne 
   pivot et on les compare a la ligne pivot */

itC   = 0;
for ( ic = icDebColonnePivot ; ic < icDebColonnePivot + NbTermesColonnePivot ; ic++ ) {
  Ligne = CIndiceLigne[ic];
  if ( Ligne != LignePivot ) {
    /* Recherche de la valeur du terme de la ligne IndiceLigne qui se trouve dans la colonne pivot */
    for ( il = Ldeb[Ligne] ; LIndiceColonne[il] != ColonnePivot ; il++ );    				
    ValTermeColonnePivot = Elm[il] * UnSurValeurDuPivot;

    Matrice->ValeurDesTermesDeLaColonnePivot[itC] = ValTermeColonnePivot;
    Matrice->LigneDesTermesDeLaColonnePivot [itC] = Ligne;
    itC++;

    /* Il faut enlever du stockage par ligne, le terme de la ColonnePivot d'indice ligne "IndiceLigne" */
    ptLigne->NombreDeTermes--;    
    Elm          [il] = Elm[ptLigne->NombreDeTermes]; 
    IndiceColonne[il] = IndiceColonne[ptLigne->NombreDeTermes];
    if ( ValTermeColonnePivot != 0.0 ) {
      LU_ScanLigneMatricePleine ( Matrice, Ligne, ColonnePivot, ValTermeColonnePivot );
    }
        
  }
}

/* Transfert de la ligne pivot dans la triangulaire superieure U */
if ( Matrice->IndexLibreDeU + NbTermesLignePivot > Matrice->DernierIndexLibreDeU ) { 
  LU_AugmenterLaTailleDuTriangleU( Matrice , NbTermesLignePivot );
}

ilU = Matrice->IndexLibreDeU;
Matrice->LdebParLigneDeU[Matrice->Kp] = ilU;

/* Le terme diagonal est egal au pivot et est range en premier */
Matrice->ElmDeU[ilU]           = UnSurValeurDuPivot; 	     
Matrice->IndiceColonneDeU[ilU] = ColonnePivot;
ilU++;

Matrice->NbTermesParLigneDeU[Matrice->Kp] = NbTermesLignePivot;

IndiceColonne = ptLignePivot->IndiceColonne;
Elm           = ptLignePivot->Elm;
for ( il = 0 ; il < NbTermesLignePivot ; il++ ) {
  Colonne = IndiceColonne[il];
  /* Inutile de remettre W a 0 car matrice pleine */
  if ( Colonne != ColonnePivot ) {
    Matrice->ElmDeU[ilU]           = Elm[il];
    Matrice->IndiceColonneDeU[ilU] = Colonne;
    ilU++;
    /* Mise a jour de la colonne la matrice active: on dechaine le terme de la
       ligne pivot */
    /* Suppression du terme d'indice ligne "LignePivot" dans la colonne "IndiceColonne" */

		printf("Attention EliminationDUneLigneMatricePleine est obsolete\n");
		printf("a cause du changement des structures decrivant la matrice active\n");
		printf("et a cause de l'introduction de Matrice->CNbTermMatriceActive\n");
		
    ptColonne   = Matrice->ColonneDeLaMatrice[Colonne];
    IndiceLigne = ptColonne->IndiceLigne;    
    for ( ic = 0 ; IndiceLigne[ic] != LignePivot ; ic++ );
    ptColonne->NombreDeTermes--;    
    IndiceLigne[ic] = IndiceLigne[ptColonne->NombreDeTermes];

		
  }
}      
Matrice->IndexLibreDeU = ilU;

/* Transfert de la colonne pivot dans la triangulaire inferieure L */
/* Le terme diagonal vaut 1 et est stocke pour beneficier de l'indice ligne */
if ( Matrice->IndexLibreDeL + NbTermesColonnePivot > Matrice->DernierIndexLibreDeL ) { 
  LU_AugmenterLaTailleDuTriangleL( Matrice , NbTermesColonnePivot );
}

Matrice->NbTermesParColonneDeL[Matrice->Kp] = NbTermesColonnePivot;

ilL = Matrice->IndexLibreDeL;
Matrice->CdebParColonneDeL[Matrice->Kp] = ilL;

/* Initialisaton de ElmDeL */
Matrice->ElmDeL[ilL] = 1.;
Matrice->IndiceLigneDeL[ilL] = LignePivot;
ilL++;

for ( ic = 0 ; ic < itC ; ic++ ){
  Matrice->ElmDeL        [ilL] = Matrice->ValeurDesTermesDeLaColonnePivot[ic];
  Matrice->IndiceLigneDeL[ilL] = Matrice->LigneDesTermesDeLaColonnePivot[ic];
  /* Remarque: a ce stade IndiceLigne ne peut pas etre egal a LignePivot */
  ilL++;
}

Matrice->IndexLibreDeL = ilL;

# endif

return;
}






























































