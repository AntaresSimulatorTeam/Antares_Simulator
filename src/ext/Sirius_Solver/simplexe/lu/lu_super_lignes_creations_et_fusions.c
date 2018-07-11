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

   FONCTION: Creation et fusion des super lignes
                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

/*--------------------------------------------------------------------------------------------------*/
/* Creation d'une super ligne ( 1ere creation ) */

SUPER_LIGNE_DE_LA_MATRICE * LU_CreerUneSuperLigne( MATRICE * Matrice , int * T , int NombreDeLignes ,
                                                   int * NumeroDesLignes , int NbTermesLigne )
{
SUPER_LIGNE_DE_LA_MATRICE * SuperLigne; int Demande; int i; int Ligne; int ic; int il; int ilMax;

int CapaciteDesColonnes; int * IndexDuTermeDiagonal; int * NumerosDesLignesDeLaSuperLigne;
SUPER_LIGNE_DE_LA_MATRICE ** SuperLigneDeLaLigne; int * IndiceColonneDansSuperLigne; double * ElmColonneDeSuperLigne; 
int * Ldeb; int * LNbTerm; int * LIndiceColonne; double * Elm;

Ldeb           = Matrice->Ldeb;
LNbTerm        = Matrice->LNbTerm;
LIndiceColonne = Matrice->LIndiceColonne;
Elm            = Matrice->Elm;

SuperLigneDeLaLigne = Matrice->SuperLigneDeLaLigne;

SuperLigne = (SUPER_LIGNE_DE_LA_MATRICE *) malloc( sizeof( SUPER_LIGNE_DE_LA_MATRICE ) );
if ( SuperLigne == NULL ) {
  printf("Factorisation LU, sous-programme LU_CreerUneSuperLigne: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 
}

SuperLigne->ScannerLaSuperLigne = NON_LU;

Demande = NombreDeLignes + INCREMENT_ALLOC_NB_LIGNES_DE_SUPER_LIGNE;
SuperLigne->NumerosDesLignesDeLaSuperLigne = (int *) malloc( Demande * sizeof( int ) );
if ( SuperLigne->NumerosDesLignesDeLaSuperLigne == NULL ) {
  printf("Factorisation LU, sous-programme LU_CreerUneSuperLigne: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 
}

/* L'index du terme diagonal n'est utilise que si pivotage diagonal */
SuperLigne->IndexDuTermeDiagonal = (int *) malloc( Demande * sizeof( int ) );
if ( SuperLigne->IndexDuTermeDiagonal == NULL ) {
  printf("Factorisation LU, sous-programme LU_CreerUneSuperLigne: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env , Matrice->AnomalieDetectee ); 
}
IndexDuTermeDiagonal = SuperLigne->IndexDuTermeDiagonal;

SuperLigne->CapaciteDesColonnes = Demande;
CapaciteDesColonnes             = Demande;

NumerosDesLignesDeLaSuperLigne = SuperLigne->NumerosDesLignesDeLaSuperLigne;
for ( i = 0 ; i < NombreDeLignes ; i++ ) {
  Ligne = NumeroDesLignes[i];
  NumerosDesLignesDeLaSuperLigne[i] = Ligne;  
  SuperLigneDeLaLigne[Ligne] = SuperLigne; 
}

SuperLigne->NombreDeLignesDeLaSuperLigne = NombreDeLignes;

/* Modele de la ligne */
Demande = NbTermesLigne + INCREMENT_ALLOC_NB_COLONNES_DE_SUPER_LIGNE;
SuperLigne->IndiceColonne = (int *) malloc( Demande * sizeof( int ) );
if ( SuperLigne->IndiceColonne == NULL ) {
  printf("Factorisation LU, sous-programme LU_CreerUneSuperLigne: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 
}
IndiceColonneDansSuperLigne = SuperLigne->IndiceColonne;
SuperLigne->Capacite        = Demande;

Ligne = NumeroDesLignes[0];

il = Ldeb[Ligne];
ilMax = il + LNbTerm[Ligne];
i = 0;
while ( il < ilMax ) {
  IndiceColonneDansSuperLigne[i] = LIndiceColonne[il];
	i++;
  il++;
}


SuperLigne->NombreDeTermes = NbTermesLigne;
SuperLigne->NumeroDeLaSuperLigne = Matrice->NombreDeSuperLignes;
Matrice->HashCodeSuperLigne[Matrice->NombreDeSuperLignes] = Matrice->HashCodeLigne[Ligne];
Matrice->SuperLigne        [Matrice->NombreDeSuperLignes] = SuperLigne;

Matrice->NombreDeSuperLignes++;

Demande = CapaciteDesColonnes * SuperLigne->Capacite;
SuperLigne->ElmColonneDeSuperLigne = (double *) malloc( Demande * sizeof( double ) );
if ( SuperLigne->ElmColonneDeSuperLigne == NULL ) {
  printf("Factorisation LU, sous-programme LU_CreerUneSuperLigne: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 
}
ElmColonneDeSuperLigne = SuperLigne->ElmColonneDeSuperLigne;

for ( i = 0 ; i < NombreDeLignes ; i++ ) {
  Ligne = NumeroDesLignes[i];		
  il = Ldeb[Ligne];
  ilMax = il + LNbTerm[Ligne];
	while ( il < ilMax ) {
	  ic = CapaciteDesColonnes * ( T[LIndiceColonne[il]] - 1 );
    /* Index du terme diagonal */
    if ( LIndiceColonne[il] == Ligne ) IndexDuTermeDiagonal[i] = T[LIndiceColonne[il]] - 1;
    ic+= i;    
    ElmColonneDeSuperLigne[ic] = Elm[il];
	  il++;
	}
}

return( SuperLigne );
}

/*--------------------------------------------------------------------------------------------------*/

void LU_FusionnerDeuxSuperLignes( MATRICE * Matrice , int * T ,
                                  int PremiereSuperLigne, int DeuxiemeSuperLigne )
{ 
int il1; int il2; int ic1; int ic2; int i; int Ligne; int Demande;

SUPER_LIGNE_DE_LA_MATRICE ** SuperLigneDeLaLigne;
SUPER_LIGNE_DE_LA_MATRICE * SuperLigne1; SUPER_LIGNE_DE_LA_MATRICE * SuperLigne2;

double * ElmColonneDeSuperLigne_1      ; int * IndexDuTermeDiagonal_1; int NombreDeLignesDeLaSuperLigne_1;
int * NumerosDesLignesDeLaSuperLigne_1; int CapaciteDesColonnes_1   ;

double * ElmColonneDeSuperLigne_2      ; int * IndexDuTermeDiagonal_2; int NombreDeLignesDeLaSuperLigne_2;
int * NumerosDesLignesDeLaSuperLigne_2; int CapaciteDesColonnes_2   ; int * IndiceColonne_2             ;  

SuperLigneDeLaLigne = Matrice->SuperLigneDeLaLigne;

SuperLigne1 = Matrice->SuperLigne[PremiereSuperLigne];
SuperLigne2 = Matrice->SuperLigne[DeuxiemeSuperLigne];

Demande = SuperLigne1->NombreDeLignesDeLaSuperLigne + SuperLigne2->NombreDeLignesDeLaSuperLigne;
if ( Demande > SuperLigne1->CapaciteDesColonnes ) {
  LU_AugmenterCapaciteDesColonnesDeSuperLigne( Matrice , SuperLigne1 , Demande ); 
}

ElmColonneDeSuperLigne_1 = SuperLigne1->ElmColonneDeSuperLigne;
ElmColonneDeSuperLigne_2 = SuperLigne2->ElmColonneDeSuperLigne;

NombreDeLignesDeLaSuperLigne_2   = SuperLigne2->NombreDeLignesDeLaSuperLigne;
NumerosDesLignesDeLaSuperLigne_2 = SuperLigne2->NumerosDesLignesDeLaSuperLigne;
IndiceColonne_2                  = SuperLigne2->IndiceColonne;
CapaciteDesColonnes_2            = SuperLigne2->CapaciteDesColonnes;

NombreDeLignesDeLaSuperLigne_1   = SuperLigne1->NombreDeLignesDeLaSuperLigne;
NumerosDesLignesDeLaSuperLigne_1 = SuperLigne1->NumerosDesLignesDeLaSuperLigne;
CapaciteDesColonnes_1            = SuperLigne1->CapaciteDesColonnes;

for ( il2 = 0 ; il2 < SuperLigne2->NombreDeTermes ; il2++ ) {
  
  il1 = T[IndiceColonne_2[il2]];
  il1--;
  ic1 = CapaciteDesColonnes_1 * il1;
  ic1+= NombreDeLignesDeLaSuperLigne_1;

  ic2 = CapaciteDesColonnes_2 * il2;
  
  memcpy( (char *) &(ElmColonneDeSuperLigne_1[ic1]) ,
          (char *) &(ElmColonneDeSuperLigne_2[ic2]) ,
	  NombreDeLignesDeLaSuperLigne_2 * sizeof( double ) );	  
}

IndexDuTermeDiagonal_1 = SuperLigne1->IndexDuTermeDiagonal;
IndexDuTermeDiagonal_2 = SuperLigne2->IndexDuTermeDiagonal;

for ( i = 0 ; i < NombreDeLignesDeLaSuperLigne_2; i++ ) {
  Ligne = NumerosDesLignesDeLaSuperLigne_2[i];
  SuperLigneDeLaLigne[Ligne] = SuperLigne1;  
  NumerosDesLignesDeLaSuperLigne_1[NombreDeLignesDeLaSuperLigne_1] = Ligne;

  if ( Matrice->FaireDuPivotageDiagonal == OUI_LU ) {
    /* Nouvel index du terme diagonal des lignes adjointes */
    il2 = IndexDuTermeDiagonal_2[i];
    /* il2 correspondait a la colonne SuperLigne2->IndiceColonne[il2] => on utilise la transformation T */
    IndexDuTermeDiagonal_1[NombreDeLignesDeLaSuperLigne_1] = T[IndiceColonne_2[il2]] - 1;
  }
  
  NombreDeLignesDeLaSuperLigne_1++;
}
SuperLigne1->NombreDeLignesDeLaSuperLigne = NombreDeLignesDeLaSuperLigne_1;

/* Liberation du struct de la super ligne */

SuperLigne2->NombreDeTermes = 0;
SuperLigne2->NombreDeLignesDeLaSuperLigne = 0;

free( SuperLigne2->NumerosDesLignesDeLaSuperLigne );
free( SuperLigne2->IndexDuTermeDiagonal );
free( SuperLigne2->IndiceColonne );
free( SuperLigne2->ElmColonneDeSuperLigne );
free( SuperLigne2 );

Matrice->SuperLigne[DeuxiemeSuperLigne] = NULL;

return;
}

/*--------------------------------------------------------------------------------------------------*/

void LU_AjouterUneLigneDansUneSuperLigne( MATRICE * Matrice , SUPER_LIGNE_DE_LA_MATRICE * SuperLigne ,
                                          int * T , int LigneAAjouter )
{
int il2; int il2Max; int il1; int ic1; int Demande; int IndexDuTermeDiagonal; double * ElmColonneDeSuperLigne; 
int CapaciteDesColonnes; int NombreDeLignesDeLaSuperLigne;
int * Ldeb; int * LNbTerm; int * LIndiceColonne; double * Elm;

Ldeb           = Matrice->Ldeb;
LNbTerm        = Matrice->LNbTerm;
LIndiceColonne = Matrice->LIndiceColonne;
Elm            = Matrice->Elm;

Demande = SuperLigne->NombreDeLignesDeLaSuperLigne + 1;
if ( Demande > SuperLigne->CapaciteDesColonnes ) {
  LU_AugmenterCapaciteDesColonnesDeSuperLigne( Matrice , SuperLigne , Demande ); 
}

ElmColonneDeSuperLigne       = SuperLigne->ElmColonneDeSuperLigne;
CapaciteDesColonnes          = SuperLigne->CapaciteDesColonnes;
NombreDeLignesDeLaSuperLigne = SuperLigne->NombreDeLignesDeLaSuperLigne;

IndexDuTermeDiagonal = 0;

il2 = Ldeb[LigneAAjouter];
il2Max = il2 + LNbTerm[LigneAAjouter];
while ( il2 < il2Max ) {

  il1 = T[LIndiceColonne[il2]];
  
  if ( LIndiceColonne[il2] == LigneAAjouter ) IndexDuTermeDiagonal = T[LIndiceColonne[il2]] - 1; 
  
  il1--;
  ic1 = CapaciteDesColonnes * il1;
  ic1+= NombreDeLignesDeLaSuperLigne;
  
  ElmColonneDeSuperLigne[ic1] = Elm[il2];

  il2++;
}
  
Matrice->SuperLigneDeLaLigne[LigneAAjouter] = SuperLigne; 

SuperLigne->NumerosDesLignesDeLaSuperLigne[NombreDeLignesDeLaSuperLigne] = LigneAAjouter;
SuperLigne->IndexDuTermeDiagonal          [NombreDeLignesDeLaSuperLigne] = IndexDuTermeDiagonal;
SuperLigne->NombreDeLignesDeLaSuperLigne++;

return;

}

