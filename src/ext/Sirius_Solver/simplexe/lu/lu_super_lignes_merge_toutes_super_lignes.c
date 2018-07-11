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

   FONCTION: Merge de toutes les super lignes
                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

/*--------------------------------------------------------------------------------------------------*/
/* On remplit toute la matrice avec des 0 au besoin et on met tout ca dans une super ligne */
/* Attention là dessous c'est codé que pour les matrices symetriques */

void LU_MettreTouteLaMatriceDansUneSuperLigne( MATRICE * Matrice )  
{
SUPER_LIGNE_DE_LA_MATRICE * SuperLigne; SUPER_LIGNE_DE_LA_MATRICE * SuperLigne2;
int Demande; int i; int NombreDeTermes; int Ligne; int Colonne; int NombreDeLignes;
int ic; int il; int Kp; int NbTermesLigne; int ic1; int ic2; int il1; int il2; int * T;
int NbTermesSuperLigne; int * OrdreColonne; int * OrdreLigne;
int * NumerosDesLignesDeLaSuperLigne; int * IndexDuTermeDiagonal; int * IndiceColonne;
double * ElmColonneDeSuperLigne; char FaireDuPivotageDiagonal;

SUPER_LIGNE_DE_LA_MATRICE ** SuperLigneDeLaLigne; double * ElmColonneDeSuperLigne_2;
int CapaciteDesColonnes; int CapaciteDesColonnes_2; int NombreDeLignesDeLaSuperLigne_2;
int * NumerosDesLignesDeLaSuperLigne_2; int * IndexDuTermeDiagonal_2;

int * Ldeb; int * LNbTerm; int * LIndiceColonne; int ilDeb; double * Elm;

/*printf("Matrice->Kp %d on met toutes les lignes dans une seule super ligne\n",Matrice->Kp);*/

SuperLigneDeLaLigne     = Matrice->SuperLigneDeLaLigne;

Ldeb           = Matrice->Ldeb;
LNbTerm        = Matrice->LNbTerm;
LIndiceColonne = Matrice->LIndiceColonne;
Elm            = Matrice->Elm;

OrdreLigne              = Matrice->OrdreLigne;
OrdreColonne            = Matrice->OrdreColonne;
FaireDuPivotageDiagonal = Matrice->FaireDuPivotageDiagonal;

T = (int *) Matrice->W;
memset( (char *) T , 0 , Matrice->Rang * sizeof( int ) );

/* Construction d'un structure de superligne dans laquelle on va mettre toutes les lignes
   et les super lignes qu'il reste */
   
NbTermesLigne  = Matrice->Rang - Matrice->Kp;
NombreDeLignes = NbTermesLigne;

SuperLigne = (SUPER_LIGNE_DE_LA_MATRICE *) malloc( sizeof( SUPER_LIGNE_DE_LA_MATRICE ) );
if ( SuperLigne == NULL ) {
  printf("Factorisation LU, sous-programme LU_CreerUneSuperLigne: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 
}

Demande = NombreDeLignes;
SuperLigne->NumerosDesLignesDeLaSuperLigne = (int *) malloc( Demande * sizeof( int ) );
if ( SuperLigne->NumerosDesLignesDeLaSuperLigne == NULL ) {
  printf("Factorisation LU, sous-programme LU_CreerUneSuperLigne: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env , Matrice->AnomalieDetectee );
}

SuperLigne->IndexDuTermeDiagonal = (int *) malloc( Demande * sizeof( int ) );
if ( SuperLigne->IndexDuTermeDiagonal == NULL ) {
  printf("Factorisation LU, sous-programme LU_CreerUneSuperLigne: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env , Matrice->AnomalieDetectee ); 
}

Demande = NbTermesLigne;
SuperLigne->IndiceColonne = (int *) malloc( Demande * sizeof( int ) );
if ( SuperLigne->IndiceColonne == NULL ) {
  printf("Factorisation LU, sous-programme LU_CreerUneSuperLigne: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env , Matrice->AnomalieDetectee ); 
}

SuperLigne->ScannerLaSuperLigne          = NON_LU;
SuperLigne->CapaciteDesColonnes          = NombreDeLignes;
SuperLigne->NombreDeLignesDeLaSuperLigne = NombreDeLignes;
SuperLigne->Capacite                     = NbTermesLigne;
SuperLigne->NombreDeTermes               = NbTermesLigne;
SuperLigne->NumeroDeLaSuperLigne         = Matrice->NombreDeSuperLignes;

NbTermesSuperLigne = NbTermesLigne;

NumerosDesLignesDeLaSuperLigne = SuperLigne->NumerosDesLignesDeLaSuperLigne;
IndexDuTermeDiagonal           = SuperLigne->IndexDuTermeDiagonal;
IndiceColonne                  = SuperLigne->IndiceColonne;
CapaciteDesColonnes            = SuperLigne->CapaciteDesColonnes;

il = 0;
for ( Kp = Matrice->Kp ; Kp < Matrice->Rang ; Kp++ ) {
  Colonne = OrdreColonne[Kp];    
  IndiceColonne[il] = Colonne;
  T[Colonne] = il + 1;
  il++;
}

/* Le hash code devient inutile */
Matrice->HashCodeSuperLigne[Matrice->NombreDeSuperLignes] = 0;

Matrice->SuperLigne[Matrice->NombreDeSuperLignes] = SuperLigne;
SuperLigne->NumeroDeLaSuperLigne = Matrice->NombreDeSuperLignes;
Matrice->NombreDeSuperLignes++;

Demande = CapaciteDesColonnes * SuperLigne->Capacite;

/*
printf("CapaciteDesColonnes %d\n",SuperLigne->CapaciteDesColonnes);
printf("Capacite %d\n",SuperLigne->Capacite);
printf("Demande %d\n",Demande);
*/

SuperLigne->ElmColonneDeSuperLigne = (double *) malloc( Demande * sizeof( double ) );
if ( SuperLigne->ElmColonneDeSuperLigne == NULL ) {
  printf("Factorisation LU, sous-programme LU_CreerUneSuperLigne: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env , Matrice->AnomalieDetectee ); 
}

ElmColonneDeSuperLigne = SuperLigne->ElmColonneDeSuperLigne;
/* RAZ obligatoire */
memset( (char *) ElmColonneDeSuperLigne , 0 , Demande * sizeof( double ) );

NombreDeLignes = 0;
/* On ajoute les lignes */
for ( Kp = Matrice->Kp ; Kp < Matrice->Rang ; Kp++ ) {
  Ligne = OrdreLigne[Kp]; 
  if ( SuperLigneDeLaLigne[Ligne] != NULL ) continue; /* La ligne est dans une super ligne */
  /*
	printf("On ajoute la ligne %d\n",Ligne);
	*/
  /* On ajoute la ligne a la super ligne */
  ilDeb = Ldeb[Ligne];
  NbTermesLigne = LNbTerm[Ligne];

	
  for ( il = ilDeb ; il < ilDeb + NbTermesLigne ; il++ ) {
    ic = CapaciteDesColonnes * ( T[LIndiceColonne[il]] - 1 );

    if ( LIndiceColonne[il] == Ligne ) {
      IndexDuTermeDiagonal[NombreDeLignes] = T[LIndiceColonne[il]] - 1;
    }  
    
    if ( ic >= 0 ) {
      ic+= NombreDeLignes;    
      ElmColonneDeSuperLigne[ic] = Elm[il];
    }
    else {
      printf("Bug, les termes de la ligne %d ont un indice colonne faux %d\n",Ligne,IndiceColonne[il]);     
    }
  }
  /*
	printf("OK\n");
	*/
  SuperLigneDeLaLigne[Ligne] = SuperLigne;
  NumerosDesLignesDeLaSuperLigne[NombreDeLignes] = Ligne;  
  NombreDeLignes++;

  /* On ne libere pas la ligne pour la reutiliser plus tard */
	
  if ( NbTermesLigne != NbTermesSuperLigne ) {
    LU_DeClasserUneLigne( Matrice , Ligne , NbTermesLigne );
    LU_ClasserUneLigne( Matrice , Ligne , NbTermesSuperLigne );
  }
  
}

/* On ajoute les super lignes */
for ( Kp = 0 ; Kp < Matrice->NombreDeSuperLignes - 1 ; Kp++ ) {
  SuperLigne2 = Matrice->SuperLigne[Kp];
  if ( SuperLigne2 == NULL ) continue;
  /* Ajoute la super ligne a la super ligne */
  /*
	printf("On ajoute la super ligne %X  NombreDeTermes %d  Nombre de lignes %d\n",
          SuperLigne2,SuperLigne2->NombreDeTermes,SuperLigne2->NombreDeLignesDeLaSuperLigne);
	*/
	
  NombreDeTermes                 = SuperLigne2->NombreDeTermes;
  IndiceColonne	                 = SuperLigne2->IndiceColonne;
  ElmColonneDeSuperLigne_2       = SuperLigne2->ElmColonneDeSuperLigne;
  CapaciteDesColonnes_2          = SuperLigne2->CapaciteDesColonnes;
  NombreDeLignesDeLaSuperLigne_2 = SuperLigne2->NombreDeLignesDeLaSuperLigne;
  IndexDuTermeDiagonal_2         = SuperLigne2->IndexDuTermeDiagonal;
  
  for ( il2 = 0 ; il2 < NombreDeTermes ; il2++ ) {    
    il1 = T[IndiceColonne[il2]] - 1;
    if ( il1 >= 0 ) {
      ic1 = CapaciteDesColonnes * il1;
      ic1+= NombreDeLignes;
      ic2 = CapaciteDesColonnes_2 * il2;  
      memcpy( (char *) &(ElmColonneDeSuperLigne[ic1]) ,
              (char *) &(ElmColonneDeSuperLigne_2[ic2]) ,
	      NombreDeLignesDeLaSuperLigne_2 * sizeof( double ) );
    }
    else {
      printf("Bug, les termes de la super ligne %lX ont un indice colonne faux %d\n",
              (unsigned long) SuperLigne2,SuperLigne2->IndiceColonne[il2]);     
    }
  }
  /*printf("OK\n");*/
 
  NumerosDesLignesDeLaSuperLigne_2 = SuperLigne2->NumerosDesLignesDeLaSuperLigne;
  for ( i = 0 ; i < NombreDeLignesDeLaSuperLigne_2; i++ ) {
    Ligne = NumerosDesLignesDeLaSuperLigne_2[i];    
    SuperLigneDeLaLigne[Ligne] = SuperLigne;

    if ( NombreDeTermes != NbTermesSuperLigne ) {
      LU_DeClasserUneLigne( Matrice , Ligne , NombreDeTermes );
      LU_ClasserUneLigne( Matrice , Ligne , NbTermesSuperLigne );
    }  
    
    NumerosDesLignesDeLaSuperLigne[NombreDeLignes] = Ligne;

    if ( FaireDuPivotageDiagonal == OUI_LU ) {
      /* Nouvel index du terme diagonal des lignes adjointes */
      il2 = IndexDuTermeDiagonal_2[i];
      /* il2 correspondait a la colonne SuperLigne2->IndiceColonne[il2] => on utilise la transformation T */
      IndexDuTermeDiagonal[NombreDeLignes] = T[IndiceColonne[il2]] - 1;
    }
    
    NombreDeLignes++;      
  }

  /* Liberation du struct de la super ligne */
  SuperLigne2->NombreDeTermes = 0;
  SuperLigne2->NombreDeLignesDeLaSuperLigne = 0;

  free( NumerosDesLignesDeLaSuperLigne_2 );
  free( IndexDuTermeDiagonal_2 );
  free( IndiceColonne );
  free( ElmColonneDeSuperLigne_2 );
  free( SuperLigne2 );

  Matrice->SuperLigne[Kp] = NULL; 
  
}

memset( (char *) T , 0 , Matrice->Rang * sizeof( int ) );

return;
}

