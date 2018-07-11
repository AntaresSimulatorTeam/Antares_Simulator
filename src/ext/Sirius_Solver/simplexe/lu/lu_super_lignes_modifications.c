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

   FONCTION: Gestion des super lignes
                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

/*--------------------------------------------------------------------------------------------------*/
/* On peut regrouper la suppression et la recopie d'une ligne de super ligne */

void LU_SupprimerUneLigneDansUneSuperLigne( MATRICE * Matrice , SUPER_LIGNE_DE_LA_MATRICE * SuperLigne , int LigneASupprimer ,
                                            int ColonnePivot )
{
int ic; int i; int NumeroDeLaSuperLigne; int * IndiceColonne; double * ElmColonneDeSuperLigne; 
int * NumerosDesLignesDeLaSuperLigne; int iccDeb; int icc; double X; int CapaciteDesColonnes;
int DernierIndexDeColonne; int * IndexDuTermeDiagonal;

IndiceColonne                  = SuperLigne->IndiceColonne;
ElmColonneDeSuperLigne         = SuperLigne->ElmColonneDeSuperLigne;
NumerosDesLignesDeLaSuperLigne = SuperLigne->NumerosDesLignesDeLaSuperLigne;
IndexDuTermeDiagonal           = SuperLigne->IndexDuTermeDiagonal;
CapaciteDesColonnes            = SuperLigne->CapaciteDesColonnes;

DernierIndexDeColonne = SuperLigne->NombreDeLignesDeLaSuperLigne - 1;

/* Recherche de l'indice ou se trouvent les termes de la ligne */
for ( ic = 0 ; ic < SuperLigne->NombreDeLignesDeLaSuperLigne ; ic++ ) {
  if ( NumerosDesLignesDeLaSuperLigne[ic] == LigneASupprimer ) {
    /* Suppression de toute reference a LigneASupprimer dans la SuperLigne */
    NumerosDesLignesDeLaSuperLigne[ic] = NumerosDesLignesDeLaSuperLigne[DernierIndexDeColonne];
    IndexDuTermeDiagonal          [ic] = IndexDuTermeDiagonal[DernierIndexDeColonne];   
    break;
  }
}

for ( i = 0 ; i < SuperLigne->NombreDeTermes ; i++ ) {
  if ( IndiceColonne[i] == ColonnePivot ) {
    iccDeb = i * CapaciteDesColonnes;
    icc = iccDeb + ic;
    /* Suppression du terme de la super ligne */
    ElmColonneDeSuperLigne[icc] = ElmColonneDeSuperLigne[iccDeb + DernierIndexDeColonne];  
    break;
  }
}
	
/* Suppression des termes de la super ligne */
for ( i = 0 ; i < SuperLigne->NombreDeTermes ; i++ ) {
  if ( IndiceColonne[i] == ColonnePivot ) continue;
  /* Sauvegarde du terme de la ligne pivot */
  iccDeb = i * CapaciteDesColonnes;
  icc = iccDeb + ic;  
  X = ElmColonneDeSuperLigne[icc];
  /* On supprime le terme de la ligne pivot */
  ElmColonneDeSuperLigne[icc] = ElmColonneDeSuperLigne[iccDeb + DernierIndexDeColonne];
}

SuperLigne->NombreDeLignesDeLaSuperLigne--;

if ( SuperLigne->NombreDeLignesDeLaSuperLigne == 0 ) {

  NumeroDeLaSuperLigne = SuperLigne->NumeroDeLaSuperLigne;
  
  free( SuperLigne->NumerosDesLignesDeLaSuperLigne );
  free( SuperLigne->IndexDuTermeDiagonal );  
  free( SuperLigne->IndiceColonne );
  free( SuperLigne->ElmColonneDeSuperLigne );
  free( SuperLigne );

  if ( Matrice->TypeDeClassementHashCodeAFaire[NumeroDeLaSuperLigne+Matrice->Rang] == ELEMENT_HASCODE_A_CLASSER ) {
    Matrice->TypeDeClassementHashCodeAFaire[NumeroDeLaSuperLigne+Matrice->Rang] = ELEMENT_HASCODE_A_NE_PAS_CLASSER;
  }
  else if ( Matrice->TypeDeClassementHashCodeAFaire[NumeroDeLaSuperLigne+Matrice->Rang] == ELEMENT_HASCODE_CLASSE ) {
    Matrice->TypeDeClassementHashCodeAFaire[NumeroDeLaSuperLigne+Matrice->Rang] = ELEMENT_HASCODE_A_DECLASSER;
  }
  else if ( Matrice->TypeDeClassementHashCodeAFaire[NumeroDeLaSuperLigne+Matrice->Rang] == ELEMENT_HASCODE_A_RECLASSER ) {
    Matrice->TypeDeClassementHashCodeAFaire[NumeroDeLaSuperLigne+Matrice->Rang] = ELEMENT_HASCODE_A_DECLASSER;
  }
  
  Matrice->SuperLigne[NumeroDeLaSuperLigne] = NULL;
  
}
else {
  /* S'il reste des lignes mais trop de vide, on retasse */
  if ( SuperLigne->NombreDeLignesDeLaSuperLigne < 10 ) {  
    if ( SuperLigne->CapaciteDesColonnes - SuperLigne->NombreDeLignesDeLaSuperLigne > SuperLigne->NombreDeLignesDeLaSuperLigne ) {
      LU_DiminuerCapaciteDesColonnesDeSuperLigne( Matrice , SuperLigne );
    }
  }
}

return;
}

/*--------------------------------------------------------------------------------------------------*/
/* Supprimer une colonne dans une super ligne, c'est toujours les termes de la colonne pivot */

void LU_SupprimerUnTermeDansUneSuperLigne( MATRICE * Matrice, 
                                           SUPER_LIGNE_DE_LA_MATRICE * SuperLigne,
                                           int icDebColonnePivot,
					                                 int NumeroDeColonneDansSuperLigne )
{
int ic; int ic1; int * IndiceColonne; int i; int * IndexDuTermeDiagonal;

IndiceColonne = SuperLigne->IndiceColonne;

Matrice->HashCodeSuperLigne[SuperLigne->NumeroDeLaSuperLigne]-= Matrice->PoidsDesColonnes[IndiceColonne[NumeroDeColonneDansSuperLigne]];  

/* debut de la colonne pivot */
ic = icDebColonnePivot;
/* Debut de la derniere colonne */
ic1 = SuperLigne->CapaciteDesColonnes * ( SuperLigne->NombreDeTermes - 1 );

/* Rien a faire si on supprime le dernier terme */
if ( ic != ic1 ) {
  memcpy( (char *) &(SuperLigne->ElmColonneDeSuperLigne[ic]) , (char *) &(SuperLigne->ElmColonneDeSuperLigne[ic1]) ,
           SuperLigne->CapaciteDesColonnes * sizeof( double ) );
	   
  IndiceColonne[NumeroDeColonneDansSuperLigne] = IndiceColonne[SuperLigne->NombreDeTermes - 1];
  
  /* Mise a jour des index des termes diagonaux */
  if ( Matrice->FaireDuPivotageDiagonal == OUI_LU ) {
    IndexDuTermeDiagonal = SuperLigne->IndexDuTermeDiagonal;
    for ( i = 0 ; i < SuperLigne->NombreDeLignesDeLaSuperLigne; i++ ) {
      if ( IndexDuTermeDiagonal[i] == SuperLigne->NombreDeTermes - 1 ) {          
        IndexDuTermeDiagonal[i] = NumeroDeColonneDansSuperLigne;      
        break;
      }
    }
  }
  
}

SuperLigne->NombreDeTermes--;	 

return;
}

/*--------------------------------------------------------------------------------------------------*/
/*                  On recopie une ligne d'une super ligne dans une ligne normale                   */

void LU_RecopierUneLigneDeSuperLigneDansLigne( MATRICE * Matrice,
                                               SUPER_LIGNE_DE_LA_MATRICE * SuperLigne,
                                               int LigneACopier )
{
int ic; int i; int il; int NombreDeLignesDeLaSuperLigne; int * NumerosDesLignesDeLaSuperLigne;
int NombreDeTermes; int CapaciteDesColonnes; double * ElmColonneDeSuperLigne; int * IndiceColonne;
int Demande; int ilDeb; int * Ldeb; int * LNbTerm; int * LIndiceColonne; double * Elm;

NombreDeLignesDeLaSuperLigne   = SuperLigne->NombreDeLignesDeLaSuperLigne;   
NumerosDesLignesDeLaSuperLigne = SuperLigne->NumerosDesLignesDeLaSuperLigne;

NombreDeTermes            = SuperLigne->NombreDeTermes;
CapaciteDesColonnes       = SuperLigne->CapaciteDesColonnes;
ElmColonneDeSuperLigne    = SuperLigne->ElmColonneDeSuperLigne;
IndiceColonne             = SuperLigne->IndiceColonne;

Ldeb           = Matrice->Ldeb;
LNbTerm        = Matrice->LNbTerm;
LIndiceColonne = Matrice->LIndiceColonne;
Elm            = Matrice->Elm;

ilDeb = Ldeb[LigneACopier];
if ( ilDeb + NombreDeTermes - 1 > Matrice->LDernierPossible[LigneACopier] ) { 
  Demande = NombreDeTermes;
  LU_AugmenterLaTailleDeLaMatriceActive( Matrice , LigneACopier , Demande );
  ilDeb          = Matrice->Ldeb[LigneACopier];
  LIndiceColonne = Matrice->LIndiceColonne;
  Elm            = Matrice->Elm;
}

for ( ic = 0 ; ic < NombreDeLignesDeLaSuperLigne ; ic++ ) {
  if ( NumerosDesLignesDeLaSuperLigne[ic] == LigneACopier ) break;
}

il = ilDeb;
for ( i = 0 ; i < NombreDeTermes ; i++ ) {
  Elm           [il] = ElmColonneDeSuperLigne[ic];
  LIndiceColonne[il] = IndiceColonne[i];	
  ic+= CapaciteDesColonnes;
  il++;
}
LNbTerm[LigneACopier] = NombreDeTermes;

return;   
}

