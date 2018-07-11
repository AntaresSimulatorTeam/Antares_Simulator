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

   FONCTION: Allocation de la matrice du point interieur et liberation
           
   AUTEUR: R. GONZALEZ

************************************************************************/
# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

# include "lu_fonctions.h"

# ifdef PI_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pi_memoire.h"
# endif
    
#define MARGE_ALLOC 256
      
/*-------------------------------------------------------------------------*/
/*     Allocation de la matrice du point interieur (systeme augmente)      */ 

void PI_AllocMatrice( PROBLEME_PI * Pi )
{
int Cnt; int NombreDeColonnes; int NombreDElementsAlloues;
 
/* Calcul de la taille a allouer (on ne tient pas compte d'une presence eventuelle de couts croises
   pour en tenir compte il faut balayer les couts croises */
NombreDElementsAlloues = 0;
for ( Cnt = 0 ; Cnt < Pi->NombreDeContraintes ; Cnt++ ) NombreDElementsAlloues+= Pi->NbTerm[Cnt];
NombreDElementsAlloues*= 2;
NombreDElementsAlloues+= Pi->NombreDeVariables + Pi->NombreDeContraintes;
NombreDElementsAlloues+= MARGE_ALLOC;
 
NombreDeColonnes = Pi->NombreDeVariables + Pi->NombreDeContraintes;
 
Pi->MatricePi = (MATRICE_PI *) malloc( sizeof( MATRICE_PI ) );
if ( Pi->MatricePi == NULL ) {
  printf(" Point interieur, memoire insuffisante dans le sous programme PI_AllocMatrice \n"); 
  Pi->AnomalieDetectee = OUI_PI;
  longjmp( Pi->Env , Pi->AnomalieDetectee ); 
}

Pi->MatricePi->NombreDeColonnes       = NombreDeColonnes;
Pi->MatricePi->NombreDElementsAlloues = NombreDElementsAlloues;

Pi->MatricePi->Elm    = (double *)  malloc( NombreDElementsAlloues * sizeof( double ) );

Pi->MatricePi->Indl = (int *) malloc( NombreDElementsAlloues * sizeof( int   ) );
Pi->MatricePi->Ideb = (int *) malloc( NombreDeColonnes       * sizeof( int   ) );
Pi->MatricePi->Nonu = (int *) malloc( NombreDeColonnes       * sizeof( int   ) );
Pi->MatricePi->Sec  = (double *) malloc( NombreDeColonnes * sizeof( double ) );
  
Pi->MatricePi->IndexDuTermeDiagonalDansU = (int*) malloc( Pi->MatricePi->NombreDeColonnes * sizeof( int ) );

if ( Pi->MatricePi->Elm  == NULL || Pi->MatricePi->Indl  == NULL || Pi->MatricePi->Ideb  == NULL ||  
     Pi->MatricePi->Nonu == NULL || Pi->MatricePi->Sec   == NULL ||
     Pi->MatricePi->IndexDuTermeDiagonalDansU == NULL ) {
  printf(" Point interieur, memoire insuffisante dans le sous programme PI_AllocMatrice \n"); 
  Pi->AnomalieDetectee = OUI_PI;
  longjmp( Pi->Env , Pi->AnomalieDetectee );
}

/* Et on nettoie tout ca pour etre tranquille */

memset( (char *) Pi->MatricePi->Elm   , 0 , NombreDElementsAlloues * sizeof( double ) ); 

memset( (char *) Pi->MatricePi->Indl , 0 , NombreDElementsAlloues * sizeof( int ) ); 
memset( (char *) Pi->MatricePi->Ideb , 0 , NombreDeColonnes       * sizeof( int ) ); 
memset( (char *) Pi->MatricePi->Nonu , 0 , NombreDeColonnes       * sizeof( int ) );

Pi->MatricePi->IndexDebutDesLignesDeU = NULL;
Pi->MatricePi->NbTermesDesLignesDeU   = NULL;
Pi->MatricePi->ValeurDesTermesDeU     = NULL;
Pi->MatricePi->ValeurDesTermesDeU_SV  = NULL;
Pi->MatricePi->IndicesDeColonneDeU    = NULL;

return;
}

/*--------------------------------------------------------------------------------------------------*/
/*                    Init des triangle L et U de la factorisee                                     */

void PI_InitMatriceLU( PROBLEME_PI * Pi ,
                       int *   IndexDebutDesLignesDeU  , int * NbTermesDesLignesDeU  ,
		                   double * ValeurDesTermesDeU      , int * IndicesDeColonneDeU )
{
int il; int ilMax; int IndiceLigne; int IndiceColonne; int Ordre; int Var; int Nb;
int NbElementsAllouesU; char * T; double * V;
int * Ldeb; int * Lsui; int * Indc; int * Lder; int ilk;
int ilc;int Kp; char Erreur; 

/* Recherche des termes diagonaux et allocations memoire */
for ( IndiceColonne = 0 ; IndiceColonne < Pi->MatricePi->NombreDeColonnes ; IndiceColonne++ ) {
  Pi->MatricePi->IndexDuTermeDiagonalDansU[IndiceColonne] = -1;
}

NbElementsAllouesU = 0;
/* Parcours du triangle U et recherche de l'index auquel en lesquel on trouve le terme diagonal */
for ( Ordre = 0 ; Ordre < Pi->MatricePi->NombreDeColonnes ; Ordre++ ) {
  NbElementsAllouesU+= NbTermesDesLignesDeU[Ordre];
  /* L'indice colonne est donne par l'indice colonne du premier terme
     de U et c'est le terme diagonal */
  il  = IndexDebutDesLignesDeU[Ordre];
  Var = IndicesDeColonneDeU[il];
  Pi->MatricePi->IndexDuTermeDiagonalDansU[Var] = il;  
}

Pi->MatricePi->NbElementsDeU = NbElementsAllouesU;

Nb = Pi->MatricePi->NombreDeColonnes;

T = (char *)   malloc( Nb * sizeof( char   ) );
V = (double *) malloc( Nb * sizeof( double ) );

Pi->MatricePi->IndexDebutDesLignesDeU = (int *)   malloc( Nb                 * sizeof( int   ) );
Pi->MatricePi->NbTermesDesLignesDeU   = (int *)   malloc( Nb                 * sizeof( int   ) );
Pi->MatricePi->ValeurDesTermesDeU     = (double *) malloc( NbElementsAllouesU * sizeof( double ) );
Pi->MatricePi->ValeurDesTermesDeU_SV  = (double *) malloc( NbElementsAllouesU * sizeof( double ) );
Pi->MatricePi->IndicesDeColonneDeU    = (int *)   malloc( NbElementsAllouesU * sizeof( int   ) );

if ( T == NULL || V == NULL ||   
     Pi->MatricePi->IndexDebutDesLignesDeU   == NULL ||  
     Pi->MatricePi->NbTermesDesLignesDeU     == NULL ||  
     Pi->MatricePi->ValeurDesTermesDeU       == NULL ||  
     Pi->MatricePi->ValeurDesTermesDeU_SV    == NULL ||  
     Pi->MatricePi->IndicesDeColonneDeU      == NULL ) {
  printf(" Point interieur, memoire insuffisante dans le sous programme PI_InitMatriceLU \n"); 
  Pi->AnomalieDetectee = OUI_PI;
  longjmp( Pi->Env , Pi->AnomalieDetectee ); 
}

/* Init et raz des vecteurs */
memset( (char *) T , 0 , Nb * sizeof( char ) );
memset( (char *) V , 0 , Nb * sizeof( double ) );

memcpy( (char *) Pi->MatricePi->IndexDebutDesLignesDeU , (char *) IndexDebutDesLignesDeU , Nb * sizeof( int ) );
memcpy( (char *) Pi->MatricePi->NbTermesDesLignesDeU   , (char *) NbTermesDesLignesDeU   , Nb * sizeof( int ) );
memcpy( (char *) Pi->MatricePi->IndicesDeColonneDeU    , (char *) IndicesDeColonneDeU    , NbElementsAllouesU * sizeof( int ) );
memset( (char *) Pi->MatricePi->ValeurDesTermesDeU     , 0 , NbElementsAllouesU * sizeof( double ) );

/* On recopie la matrice d'entree dans les triangles */
/* Triangle U */
/* 1- Chainage de Matrice Pi par lignes */
Ldeb = (int *) malloc( Pi->MatricePi->NombreDeColonnes       * sizeof( int ) );
Lsui = (int *) malloc( Pi->MatricePi->NombreDElementsAlloues * sizeof( int ) );
Indc = (int *) malloc( Pi->MatricePi->NombreDElementsAlloues * sizeof( int ) );
Lder = (int *) malloc( Pi->MatricePi->NombreDeColonnes       * sizeof( int ) ); 
if ( Ldeb  == NULL || Lsui  == NULL || Indc  == NULL || Lder == NULL ) {
  printf(" Point interieur, memoire insuffisante dans le sous programme PI_InitMatriceLU \n"); 
  Pi->AnomalieDetectee = OUI_PI;
  longjmp( Pi->Env , Pi->AnomalieDetectee ); 
}
for ( IndiceLigne = 0 ; IndiceLigne < Pi->MatricePi->NombreDeColonnes ; IndiceLigne++ ) Ldeb[IndiceLigne] = -1;
for ( IndiceColonne = 0 ; IndiceColonne < Pi->MatricePi->NombreDeColonnes ; IndiceColonne++ ) {
  il = Pi->MatricePi->Ideb[IndiceColonne];                  
  ilMax = il + Pi->MatricePi->Nonu[IndiceColonne];   
  while ( il < ilMax ) {
    IndiceLigne = Pi->MatricePi->Indl[il];
    if ( Ldeb[IndiceLigne] < 0 ) {
      Ldeb[IndiceLigne] = il;
      Indc[il]          = IndiceColonne;
      Lsui[il]          = -1;
      Lder[IndiceLigne] = il;
    }
    else {
      ilk = Lder[IndiceLigne];
      Lsui[ilk]         = il;
      Indc[il]          = IndiceColonne ;
      Lsui[il]          = -1;
      Lder[IndiceLigne] = il;
    }      
    il++;
  }
}
/* 2- Tranfert dans le triangle U */
for ( Ordre = 0 ; Ordre < Pi->MatricePi->NombreDeColonnes ; Ordre++ ) {
  il = Pi->MatricePi->IndexDebutDesLignesDeU[Ordre];
  ilMax = il + Pi->MatricePi->NbTermesDesLignesDeU[Ordre];
  while ( il < ilMax ) {
    T[Pi->MatricePi->IndicesDeColonneDeU[il]] = 1;
    il++;
  }
  /* L'indice ligne est aussi l'indice colonne du premier terme
     de U */
  IndiceLigne = Pi->MatricePi->IndicesDeColonneDeU[IndexDebutDesLignesDeU[Ordre]];
  /* Tous les termes a recopier se trouvent dans la ligne "IndiceLigne" de MatricePi */
  il = Ldeb[IndiceLigne];
  while ( il >= 0 ) {
    IndiceColonne = Indc[il];
    if ( T[IndiceColonne] == 1 ) V[IndiceColonne] = Pi->MatricePi->Elm[il];
    il = Lsui[il];
  }
  /* Et on recopie le vecteur */
  il = Pi->MatricePi->IndexDebutDesLignesDeU[Ordre];
  ilMax = il + Pi->MatricePi->NbTermesDesLignesDeU[Ordre];
  while ( il < ilMax ) {
    IndiceColonne = Pi->MatricePi->IndicesDeColonneDeU[il];
    if ( T[IndiceColonne] == 1 ) Pi->MatricePi->ValeurDesTermesDeU[il] = V[IndiceColonne];
    T[IndiceColonne] = 0;
    V[IndiceColonne] = 0.0;
    il++;
  }
}

free( Ldeb );
free( Lsui );
free( Indc );
free( Lder );
Ldeb = NULL;
Lsui = NULL;
Indc = NULL;
Lder = NULL;

free( T );
free( V );
T = NULL;
V = NULL;

# if VERBOSE_PI
  printf("-> Simulation d'une refactorisation\n");
# endif

  LU_RefactorisationSimulation( (MATRICE *) Pi->MatriceFactorisee , &Erreur , Pi->MatricePi->NbElementsDeU );
/* Tester le code Erreur */

# if VERBOSE_PI
  printf("-> Nombre de termes du triangle U de la factorisee %d\n",Pi->MatricePi->NbElementsDeU);
# endif

memcpy( (char *) Pi->MatricePi->ValeurDesTermesDeU_SV ,(char *) Pi->MatricePi->ValeurDesTermesDeU ,
         Pi->MatricePi->NbElementsDeU * sizeof( double ) );

return;
}

/*--------------------------------------------------------------------------------------------------*/
/*                                Liberation de la matrice                                          */

void PI_LibereMatrice( PROBLEME_PI * Pi )
{

if ( Pi->MatriceFactorisee != NULL ) {
  LU_LibererMemoireLU( (MATRICE *) Pi->MatriceFactorisee );
  Pi->MatriceFactorisee = NULL; 
}

if ( Pi->MatricePi == NULL) return;

free( Pi->MatricePi->Elm );
free( Pi->MatricePi->Indl );
free( Pi->MatricePi->Ideb );
free( Pi->MatricePi->Nonu );
free( Pi->MatricePi->Sec );

free( Pi->MatricePi->IndexDebutDesLignesDeU );
free( Pi->MatricePi->NbTermesDesLignesDeU );
free( Pi->MatricePi->ValeurDesTermesDeU );
free( Pi->MatricePi->ValeurDesTermesDeU_SV );
free( Pi->MatricePi->IndicesDeColonneDeU );

free( Pi->MatricePi->IndexDuTermeDiagonalDansU );
 
free( Pi->MatricePi );
  
return;
}





