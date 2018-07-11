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

   FONCTION: Scaling.
             On tient compte du fait que parfois il peut de pas exister
	           de chainage par colonne (cas symetrique).
               
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

# define NOMBRE_MAX_DE_PASSES_DE_SCALING 10
# define ARRONDI_EN_PUISSANCES_DE_2	 OUI_LU  
# define ZERO               1.e-12 	    
# define ZERO_TEST_RAPPORT  1.e-12 
	    
/*----------------------------------------------------------------------------*/

void LU_CalculerLeScaling( MATRICE * Matrice )
{
int il; int ilDeb; int ilMax; int k; double X; double UnSurX;
int Ligne; int Colonne; int LigneColonne    ; 
char Flag; double Rapport       ; double RapportPrecedent   ;
double SeuilRapport     ; double * ScaleX      ; char   ArrondiPuissanceDe2;
double * ScaleB         ; int Rang            ; double Pgt; double Ppt    ;

char * FlagDeColonne            ; double * PlusGrandTermeDeColonne;
double * PlusPetitTermeDeColonne; double * UnSurXDeColonne        ;
char ConserverLaSymetrie        ; double PlusGrandTerme; double PlusPetitTerme;

int * Ldeb; int * LNbTerm; int * LIndiceColonne; double * Elm;

Rang = Matrice->Rang;

if ( Matrice->SolutionSV == NULL ) Matrice->SolutionSV = (double *) malloc( Rang * sizeof( double ) );
if ( Matrice->SolutionSV == NULL ) {
  Matrice->FaireScaling = NON_LU;
  return;
}

Ldeb           = Matrice->Ldeb;
LNbTerm        = Matrice->LNbTerm;
LIndiceColonne = Matrice->LIndiceColonne;
	
Pgt = -1.;
Ppt = LINFINI_LU;
Rapport = 1.;

for ( LigneColonne = 0 ; LigneColonne < Rang ; LigneColonne++ ) {
  Matrice->ScaleX[LigneColonne] = 1.;											  
  Matrice->ScaleB[LigneColonne] = 1.;
}

Matrice->ScalingEnPuissanceDe2 = ARRONDI_EN_PUISSANCES_DE_2;

ConserverLaSymetrie = NON_LU;
if ( Matrice->LaMatriceEstSymetrique == OUI_LU && Matrice->FaireDuPivotageDiagonal == OUI_LU ) {
  ConserverLaSymetrie = OUI_LU;
}
 
FlagDeColonne           = Matrice->Marqueur;
PlusGrandTermeDeColonne = Matrice->W;
PlusPetitTermeDeColonne = Matrice->SolutionIntermediaire;
UnSurXDeColonne         = Matrice->SolutionSV;

ScaleX = (double *) malloc( Rang * sizeof( double ) );      
ScaleB = (double *) malloc( Rang * sizeof( double ) );
if ( ScaleX == NULL || ScaleB == NULL ) {    
  printf("Factorisation LU, sous-programme LU_CalculerLeScaling: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 							  
}

Elm = (double *) malloc( Matrice->TailleAlloueeLignes * sizeof( double) );
if ( Elm == NULL ) {    
  printf("Factorisation LU, sous-programme LU_CalculerLeScaling: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 							  
}

/* Transfert dans une zone de travail */
for ( Ligne = 0 ; Ligne < Rang ; Ligne++ ) {
	il = Ldeb[Ligne];		
  ilMax = il + LNbTerm[Ligne];
	while ( il < ilMax ) {
	  Elm[il] = Matrice->Elm[il];
	  il++;
	}	
}

RapportPrecedent = 0.0;
SeuilRapport     = 1.e-6;

ArrondiPuissanceDe2 = ARRONDI_EN_PUISSANCES_DE_2;

/* Plusieurs passes */

for ( k = 0 ; k < NOMBRE_MAX_DE_PASSES_DE_SCALING ; k++ ) {

  #if VERBOSE_SCALING
    if ( k >= 1 ) {   
      printf(" Avant passe de scaling LU num %d PlusPetitTerme %e PlusGrandTerme %e rapport %e \n",
               k,Ppt,Pgt,Rapport);
    }
  #endif
  
  if ( k >= 1 ) {
    if ( fabs ( RapportPrecedent - Rapport ) < SeuilRapport * RapportPrecedent ) break; 
    if ( RapportPrecedent < Rapport ) {
      /* On recupere les coeff de l'etape precedente et on arrete */      
      for ( LigneColonne = 0 ; LigneColonne < Rang ; LigneColonne++ ) { 
        Matrice->ScaleX[LigneColonne] = ScaleX[LigneColonne];
        Matrice->ScaleB[LigneColonne] = ScaleB[LigneColonne];
      }      
      break;
    }
  }

  if ( k > 0 ) RapportPrecedent = Rapport;
  else RapportPrecedent = LINFINI_LU;

  /* Stockage du scaling precedent */  
  for ( LigneColonne = 0 ; LigneColonne < Rang ; LigneColonne++ )   { 
    ScaleX[LigneColonne] = Matrice->ScaleX[LigneColonne];   
    ScaleB[LigneColonne] = Matrice->ScaleB[LigneColonne];
  }
  
  if ( ConserverLaSymetrie == OUI_LU ) goto ScalingDesLignes;
  
  /*-------------------- Scaling des colonnes ------------------------*/

  for ( Colonne = 0 ; Colonne < Rang ; Colonne++ ) {
    FlagDeColonne[Colonne] = 0;
    PlusGrandTermeDeColonne[Colonne] = -1.;
    PlusPetitTermeDeColonne[Colonne] =  LINFINI_LU;
  }
  /* Recherche PlusPetitTerme PlusGrandTerme */  
  for ( Ligne = 0 ; Ligne < Rang ; Ligne++ ) {
	  il = Ldeb[Ligne];		
    ilMax = il + LNbTerm[Ligne];
	  while ( il < ilMax ) {
      X = Elm[il];
      if( X > ZERO ) {
        Colonne = LIndiceColonne[il];
        if ( X > PlusGrandTermeDeColonne[Colonne] ) { PlusGrandTermeDeColonne[Colonne] = X; FlagDeColonne[Colonne] = 1; }
        if ( X < PlusPetitTermeDeColonne[Colonne] ) { PlusPetitTermeDeColonne[Colonne] = X; FlagDeColonne[Colonne] = 1; }
      }		
	    il++;
	  }					
  }  
  /* Calcul de la moyenne geometrique */    
  for ( Colonne = 0 ; Colonne < Rang ; Colonne++ ) {
    X = 1.;
    UnSurXDeColonne[Colonne] = 1.;
    if ( FlagDeColonne[Colonne] == 1 ) {
      X = PlusGrandTermeDeColonne[Colonne] * PlusPetitTermeDeColonne[Colonne];
      X = sqrt( X );
      UnSurXDeColonne[Colonne] = 1. / X;
    }

    if ( ArrondiPuissanceDe2 == OUI_LU ) LU_ArrondiEnPuissanceDe2( &UnSurXDeColonne[Colonne] );
    Matrice->ScaleX[Colonne]*= UnSurXDeColonne[Colonne]; 
  }
  
  /* Application de la participation au scaling */
  for ( Ligne = 0 ; Ligne < Rang ; Ligne++ ) {		
	  il = Ldeb[Ligne];		
    ilMax = il + LNbTerm[Ligne];
	  while ( il < ilMax ) {
      Elm[il] *= UnSurXDeColonne[LIndiceColonne[il]];  
	    il++;
	  }		
  }

  /*-------------------- Scaling des lignes ------------------------*/
  ScalingDesLignes:
  /* Lignes */

  Pgt = -1.;
  Ppt = LINFINI_LU;
 
  for ( Ligne = 0 ; Ligne < Rang ; Ligne++ ) {
    Flag           = 0;
    PlusGrandTerme = -1.;
    PlusPetitTerme = LINFINI_LU;
    /*   */
	  ilDeb = Ldeb[Ligne];		
    ilMax = ilDeb + LNbTerm[Ligne];
		il = ilDeb;
	  while ( il < ilMax ) {
      X = Elm[il];
      if( X > ZERO ) {
        if ( X > PlusGrandTerme ) { PlusGrandTerme = X; Flag = 1; }
        if ( X < PlusPetitTerme ) { PlusPetitTerme = X; Flag = 1; }
      }		
	    il++;
	  }						
    X      = 1.;
    UnSurX = 1.;
    if ( Flag == 1 ) {      
      X = sqrt( PlusGrandTerme * PlusPetitTerme );
      UnSurX = 1. / X;
    }

    /* Si on doit conserver la symetrie il faut prendre la racine carree afin d'appliquer la meme
       valeur au scaling par colonne */
    if ( ConserverLaSymetrie == OUI_LU ) UnSurX = sqrt( UnSurX );
       
    if ( ArrondiPuissanceDe2 == OUI_LU ) LU_ArrondiEnPuissanceDe2( &UnSurX );
    Matrice->ScaleB[Ligne]*= UnSurX;

    if ( ConserverLaSymetrie == OUI_LU ) UnSurXDeColonne[Ligne] = UnSurX;   

    /* Scaling de A */
    if ( ConserverLaSymetrie != OUI_LU ) {
		  il = ilDeb;			
	    while ( il < ilMax ) {
        Elm[il]*= UnSurX;
        if( Elm[il] > ZERO_TEST_RAPPORT ) {
          if ( Elm[il] > Pgt ) Pgt = Elm[il];
          if ( Elm[il] < Ppt ) Ppt = Elm[il];
        }			
	      il++;
	    }					
    }
    else {
      /* Le test sur Pgt et Ppt est fait apres */
		  il = ilDeb;			
	    while ( il < ilMax ) {
 	      Elm[il]*= UnSurX;
	      il++;
	    }								
    }
    
  }

  if ( ConserverLaSymetrie == OUI_LU ) {
    /* Application de la participation au scaling par colonne */
    for ( Ligne = 0 ; Ligne < Rang ; Ligne++ ) {
	    il = Ldeb[Ligne];		
      ilMax = il + LNbTerm[Ligne];
	    while ( il < ilMax ) {
        Elm[il] *= UnSurXDeColonne[LIndiceColonne[il]];
        if( Elm[il] > ZERO_TEST_RAPPORT ) {
          if ( Elm[il] > Pgt ) Pgt = Elm[il];
          if ( Elm[il] < Ppt ) Ppt = Elm[il];
        }	
	      il++;
	    }			 			
    }
  }
  
  Rapport = Pgt / Ppt;
  	     
  /* Fin Boucle generale de scaling */
}

if ( ConserverLaSymetrie == OUI_LU ) {
  /* Comme dans ce cas on n'a pas mis a jour ScaleX a chaque iteration de scaling, on le fait maintenant */
  memcpy( (char *) Matrice->ScaleX, ( char *) Matrice->ScaleB, Rang * sizeof( double ) );
}

#if VERBOSE_SCALING  
  PlusGrandTerme = -1.;
  PlusPetitTerme = LINFINI_LU;	
  for ( Ligne = 0 ; Ligne < Rang ; Ligne++ ) {    
	  il = Ldeb[Ligne];		
    ilMax = il + LNbTerm[Ligne];
	  while ( il < ilMax ) {
      X = fabs( Elm[il] );
      if( /*X > ZERO*/ X > ZERO_TEST_RAPPORT ) {
        if ( X > PlusGrandTerme ) PlusGrandTerme = X;
        if ( X < PlusPetitTerme ) PlusPetitTerme = X;
      }        
	    il++;
	  }				 
  }
  Rapport = PlusGrandTerme/PlusPetitTerme;
  printf(" A la fin des calculs des coefficients de scaling: \n");
  printf("   PlusPetitTerme %e PlusGrandTerme %e rapport %e \n",PlusPetitTerme,PlusGrandTerme,Rapport);

  PlusGrandTerme = -1.;
  PlusPetitTerme = LINFINI_LU;
  for ( Ligne = 0 ; Ligne < Rang ; Ligne++ ) {
    if ( Matrice->ScaleX[Ligne] < PlusPetitTerme ) PlusPetitTerme = Matrice->ScaleX[Ligne];
    if ( Matrice->ScaleX[Ligne] > PlusGrandTerme ) PlusGrandTerme = Matrice->ScaleX[Ligne] ;
  }
  printf("   Plus petit ScaleX %e Plus grand ScaleX %e\n",PlusPetitTerme,PlusGrandTerme);

  PlusGrandTerme = -1.;
  PlusPetitTerme = LINFINI_LU;
  for ( Ligne = 0 ; Ligne < Rang ; Ligne++ ) {
    if ( Matrice->ScaleB[Ligne] < PlusPetitTerme ) PlusPetitTerme = Matrice->ScaleB[Ligne];
    if ( Matrice->ScaleB[Ligne] > PlusGrandTerme ) PlusGrandTerme = Matrice->ScaleB[Ligne] ;
  }
  printf("   Plus petit ScaleB %e Plus grand ScaleB %e\n",PlusPetitTerme,PlusGrandTerme);
    
#endif

/* Nettoyages */
{ char * Marqueur; double * W; double * SolutionIntermediaire;
  double * SolutionSV;
  Marqueur = Matrice->Marqueur;
  W        = Matrice->W;
  SolutionIntermediaire = Matrice->SolutionIntermediaire;
  SolutionSV            = Matrice->SolutionSV ;
  for ( LigneColonne = 0 ; LigneColonne < Rang ; LigneColonne++ ) {
    /* On remet a jour Marqueur car il etait utilise avec FlagDeColonne */  
    Marqueur[LigneColonne] = 0;
    /* On remet a jour W car il etait utilise avec PlusGrandTermeDeColonne */
    W[LigneColonne] = 0.0;  
    /* On remet a jour SolutionIntermediaire car il etait utilise avec PlusPetitTermeDeColonne */
    SolutionIntermediaire[LigneColonne] = 0.0;
    /* On remet a jour SolutionSV car il etait utilise avec UnSurXDeColonne */  
    SolutionSV[LigneColonne] = 0.0;
  }
}

free( ScaleX );
free( ScaleB );
free( Elm );

return;
}

/*------------------------------------------------------------------------*/

void LU_ArrondiEnPuissanceDe2( double * ValeurRetenue )
{
 double X; int  P2;

X = *ValeurRetenue;
frexp( X , &P2 );
*ValeurRetenue = ldexp( 1. , P2 );

return;
}

/*------------------------------------------------------------------------*/
/*                           Scaling                                      */ 
/* Apres avoir calcule les matrices de scaling, on fait le scaling.       */

void LU_Scaling( MATRICE * Matrice )
{

int Ligne; int il; double X; int ilMax; double * ScaleX; double * ScaleB;
# if VERBOSE_SCALING
  double PlusGrandTerme; double PlusPetitTerme; double Rapport;
# endif

int * Ldeb; int * LNbTerm; int * LIndiceColonne; double * Elm;

Ldeb = Matrice->Ldeb;
LNbTerm = Matrice->LNbTerm;
LIndiceColonne = Matrice->LIndiceColonne;
Elm = Matrice->Elm;
ScaleX = Matrice->ScaleX;
ScaleB = Matrice->ScaleB;
/* Maintenant on fait le scaling effectif */  
/* 1- Scaling des colonnes */
for ( Ligne = 0 ; Ligne < Matrice->Rang ; Ligne++ ) {        
  /* Scaling de la matrice des contraintes */
  il = Ldeb[Ligne];
	ilMax = il + LNbTerm[Ligne];   
	while ( il < ilMax ) {
    Elm[il]*= ScaleX[LIndiceColonne[il]]; 
		il++;
	}
}
/* 2- Scaling des lignes */
for ( Ligne = 0 ; Ligne < Matrice->Rang ; Ligne++ ) {
  X = ScaleB[Ligne];	     		
  il = Ldeb[Ligne];
	ilMax = il + LNbTerm[Ligne];   		
	while ( il < ilMax ) {
    Elm[il]*= X;
	  il++;
	}		
}

#if VERBOSE_SCALING

/* Verification */

printf("Verification apres application effective du scaling:\n");

PlusGrandTerme = -1.;
PlusPetitTerme = LINFINI_LU;
	
for ( Ligne = 0 ; Ligne < Matrice->Rang ; Ligne++ ) {
  il = Ldeb[Ligne];
	ilMax = il + LNbTerm[Ligne];   		
	while ( il < ilMax ) {
    X = fabs( Elm[il] );
    if( X > ZERO_TEST_RAPPORT ) {
      if ( X > PlusGrandTerme ) PlusGrandTerme = X;
      if ( X < PlusPetitTerme ) PlusPetitTerme = X;
    }	
	  il++;
	}		
}

Rapport = PlusGrandTerme/PlusPetitTerme;

printf("         PlusPetitTerme %e PlusGrandTerme %e rapport %e \n",PlusPetitTerme,PlusGrandTerme,Rapport);

#endif

return;
}

/*------------------------------------------------------------------------*/
/*                 Scaling du second membre avant resolution              */ 

void LU_ScalingSecondMembre( MATRICE * Matrice , double * B )
{
int i; double f; int Exposant; int ExposantScale; double * ScaleB;

ScaleB = Matrice->ScaleB;
if ( Matrice->ScalingEnPuissanceDe2 != OUI_LU || 1 /* Car il est inutile de passer par les frexp */ ) {
  for ( i = 0 ; i < Matrice->Rang ; i++ ) B[i] *= ScaleB[i];
}
else {
  for ( i = 0 ; i < Matrice->Rang ; i++ ) { 
    frexp( ScaleB[i] , &ExposantScale );
    ExposantScale--;	
    f = frexp( B[i] , &Exposant );
    B[i] = ldexp( f , Exposant + ExposantScale );
  }
}	  

return;
}

void LU_ScalingSecondMembreTranspose( MATRICE * Matrice , double * B )
{
int i; double f; int Exposant; int ExposantScale; double * ScaleX;

ScaleX = Matrice->ScaleX;
if ( Matrice->ScalingEnPuissanceDe2 != OUI_LU || 1 /* Car il est inutile de passer par les frexp */ ) {
  for ( i = 0 ; i < Matrice->Rang ; i++ ) B[i] *= ScaleX[i];	  
}
else {
  for ( i = 0 ; i < Matrice->Rang ; i++ ) {
    frexp( ScaleX[i] , &ExposantScale );
    ExposantScale--;	
    f = frexp( B[i] , &Exposant );
    B[i] = ldexp( f , Exposant + ExposantScale );	  
  }
}

return;
}

/*------------------------------------------------------------------------*/
/*                              UnScaling                                 */ 

void LU_UnScaling( MATRICE * Matrice , double * Solution )
{
int i; double f; int Exposant; int ExposantScale; double * ScaleX;

ScaleX = Matrice->ScaleX;
if ( Matrice->ScalingEnPuissanceDe2 != OUI_LU || 1 /* Car il est inutile de passer par les frexp */ ) {
  for ( i = 0 ; i < Matrice->Rang ; i++ ) Solution[i] *= ScaleX[i];
}
else {
  for ( i = 0 ; i < Matrice->Rang ; i++ ) { 
    frexp( ScaleX[i] , &ExposantScale );
    ExposantScale--;	
    f = frexp( Solution[i] , &Exposant );
    Solution[i] = ldexp( f , Exposant + ExposantScale );
  }
}

return;
}

void LU_UnScalingTranspose( MATRICE * Matrice , double * Solution )
{
int i; double f; int Exposant; int ExposantScale; double * ScaleB;

ScaleB = Matrice->ScaleB;
if ( Matrice->ScalingEnPuissanceDe2 != OUI_LU || 1 /* Car il est inutile de passer par les frexp */ ) {
  for ( i = 0 ; i < Matrice->Rang ; i++ ) Solution[i] *= ScaleB[i];
}
else {
  for ( i = 0 ; i < Matrice->Rang ; i++ ) { 
    frexp( ScaleB[i] , &ExposantScale );
    ExposantScale--;	
    f = frexp( Solution[i] , &Exposant );
    Solution[i] = ldexp( f , Exposant + ExposantScale );
  }
}

return;
}



