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

void LU_AugmenterCapaciteDesColonnesDeSuperLigne( MATRICE * Matrice ,
                                                  SUPER_LIGNE_DE_LA_MATRICE * SuperLigne ,
                                                  int CapaciteMinDemandee )
{
int Demande; double * ElmColonneDeSuperLigne; int CapaciteDesColonnes; int ic; int ic1; int i;

/* On augmente la capacite de la valeur d'increment */

Demande = CapaciteMinDemandee + INCREMENT_ALLOC_NB_LIGNES_DE_SUPER_LIGNE;
CapaciteDesColonnes = Demande;
/* On veut ajouter des lignes dans la super ligne */
SuperLigne->NumerosDesLignesDeLaSuperLigne = (int *) realloc( SuperLigne->NumerosDesLignesDeLaSuperLigne ,
                                                               Demande * sizeof( int ) );
SuperLigne->IndexDuTermeDiagonal = (int *) realloc( SuperLigne->IndexDuTermeDiagonal ,
                                                     Demande * sizeof( int ) );  
Demande*= SuperLigne->Capacite; 
ElmColonneDeSuperLigne = (double *) malloc( Demande * sizeof( double ) );
if ( ElmColonneDeSuperLigne == NULL ) {
  printf("Factorisation LU, sous-programme LU_AugmenterCapaciteDesColonnesDeSuperLigne: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");  
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env , Matrice->AnomalieDetectee ); 
}

ic  = 0;
ic1 = 0;
for ( i = 0 ; i < SuperLigne->NombreDeTermes ; i++ ) {
  memcpy( (char *) &ElmColonneDeSuperLigne[ic] , (char *) &(SuperLigne->ElmColonneDeSuperLigne[ic1]) ,
           SuperLigne->NombreDeLignesDeLaSuperLigne * sizeof( double ) );          
  ic+=  CapaciteDesColonnes;
  ic1+= SuperLigne->CapaciteDesColonnes;
}

SuperLigne->CapaciteDesColonnes = CapaciteDesColonnes;

free( SuperLigne->ElmColonneDeSuperLigne );

SuperLigne->ElmColonneDeSuperLigne = ElmColonneDeSuperLigne;

return;
}

/*--------------------------------------------------------------------------------------------------*/
/* Si la capacite est trop grande par rapport au nombre de ligne on enleve de la capacite pour
   ameliorer le fonctionnement du cache memoire */
   
void LU_DiminuerCapaciteDesColonnesDeSuperLigne( MATRICE * Matrice ,
                                                 SUPER_LIGNE_DE_LA_MATRICE * SuperLigne )
{
int Demande; double * ElmColonneDeSuperLigne; int CapaciteDesColonnes; int ic; int ic1; int i;

Demande = SuperLigne->NombreDeLignesDeLaSuperLigne + INCREMENT_ALLOC_NB_LIGNES_DE_SUPER_LIGNE;
CapaciteDesColonnes = Demande;

Demande*= SuperLigne->Capacite; 
ElmColonneDeSuperLigne = (double *) malloc( Demande * sizeof( double ) );
if ( ElmColonneDeSuperLigne == NULL ) {
  printf("Factorisation LU, sous-programme LU_DiminuerCapaciteDesColonnesDeSuperLigne: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");  
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env , Matrice->AnomalieDetectee ); 
}

ic  = 0;
ic1 = 0;
for ( i = 0 ; i < SuperLigne->NombreDeTermes ; i++ ) {
  memcpy( (char *) &ElmColonneDeSuperLigne[ic] , (char *) &(SuperLigne->ElmColonneDeSuperLigne[ic1]) ,
           SuperLigne->NombreDeLignesDeLaSuperLigne * sizeof( double ) );          
  ic+=  CapaciteDesColonnes;
  ic1+= SuperLigne->CapaciteDesColonnes;
}

SuperLigne->CapaciteDesColonnes = CapaciteDesColonnes;

free( SuperLigne->ElmColonneDeSuperLigne );

SuperLigne->ElmColonneDeSuperLigne = ElmColonneDeSuperLigne;

return;
}

/*--------------------------------------------------------------------------------------------------*/
/* On veut augmenter le nombre de termes possibles dans les lignes de SuperLigne (pour ajouter des colonnes) */

void LU_AugmenterCapaciteDeSuperLigne( MATRICE * Matrice , SUPER_LIGNE_DE_LA_MATRICE * SuperLigne , int CapaciteMinDemandee )
{
int Demande;  

Demande = CapaciteMinDemandee + INCREMENT_ALLOC_NB_COLONNES_DE_SUPER_LIGNE;
SuperLigne->IndiceColonne = (int *) realloc( SuperLigne->IndiceColonne , Demande * sizeof( int ) );
SuperLigne->Capacite = Demande;

Demande = SuperLigne->Capacite * SuperLigne->CapaciteDesColonnes;
SuperLigne->ElmColonneDeSuperLigne = (double *) realloc( SuperLigne->ElmColonneDeSuperLigne , Demande * sizeof( double ) );
if ( SuperLigne->ElmColonneDeSuperLigne == NULL ) {
  printf("Factorisation LU, sous-programme LU_AugmenterCapaciteDeSuperLigne: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");  
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env , Matrice->AnomalieDetectee ); 
}

return;
}

