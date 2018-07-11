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
/***********************************************************************************

   FONCTION: 

                   
   AUTEUR: R. GONZALEZ

************************************************************************************/

# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

# ifdef PI_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pi_memoire.h"
# endif

# define NOMBRE_MAX_DE_PASSES_DE_SCALING  20      /* 4 */
# define FAIRE_SCALING_DES_COUTS          OUI_PI  /* Attention, ce n'est peut-etre pas la bonne option. En effet faire un
                                                     scaling des couts c'est faire un scaling du second membre dans le cas
																										 lneaire */
# define RAPPORT_DE_NEUTRALISATION        10.
# define ARRONDI_EN_PUISSANCES_DE_2       OUI_PI
# define SCALING_DES_BORNES               OUI_PI

# define ZERO_SC 1.e-12

/*------------------------------------------------------------------------*/
/*                         Scaling                                        */ 

void PI_CalculerLeScaling( PROBLEME_PI * Pi )
{
int i; int il; int ilMax; int k; double X; double UnSurX; 
double PlusGrandTerme; double PlusPetitTerme; int NombreDeVariables;
char Flag; double Rapport; double RapportPrecedent; double SeuilRapport;
double * A; double * L; double * ScaleU; double * ScaleB;
double ScaleLigneDesCouts; double Zero; double * UnSurUmaxMoinsUmin;
char * TypeDeVariable; double * Umin; double * Umax; int NombreDeContraintes;
int * Mdeb; int * NbTerm; int * Cdeb; int * Csui; double ScaleLigneDesU;

/*
printf("Attention pas de scaling dans le PI\n");
return;
*/

Mdeb   = Pi->Mdeb; 
NbTerm = Pi->NbTerm; 
Cdeb   = Pi->Cdeb;
Csui   = Pi->Csui;

NombreDeVariables   = Pi->NombreDeVariables;
NombreDeContraintes = Pi->NombreDeContraintes;

Zero = ZERO_SC;

A      = (double *) malloc( Pi->NbTermesAllouesPourA * sizeof( double ) );            
L      = (double *) malloc( NombreDeVariables        * sizeof( double ) );      
ScaleU = (double *) malloc( NombreDeVariables        * sizeof( double ) );      
ScaleB = (double *) malloc( NombreDeContraintes      * sizeof( double ) );

UnSurUmaxMoinsUmin = (double *) malloc( NombreDeVariables    * sizeof( double ) );      

if ( A == NULL || L == NULL || ScaleU  == NULL || ScaleB == NULL || UnSurUmaxMoinsUmin == NULL ) {    
  printf("Simplexe, sous-programme SPX_FaireScaling : \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  Pi->AnomalieDetectee = OUI_PI;
  longjmp( Pi->Env , Pi->AnomalieDetectee ); 
}

for ( i = 0 ; i < NombreDeContraintes ; i++ ) {
  il    = Mdeb[i]; 
  ilMax = il + NbTerm[i]; 
  while ( il < ilMax ) {
    A[il] = fabs( Pi->A[il] );
    il++;
  }    
}

TypeDeVariable = Pi->TypeDeVariable;
Umin = Pi->Umin;
Umax = Pi->Umax;

X = 0.0;
for ( i = 0 ; i < NombreDeVariables ; i++ ) {
  L[i] = fabs( Pi->L[i] );
  X+= fabs( Pi->Q[i] );
	if ( SCALING_DES_BORNES == OUI_PI ) {
    UnSurUmaxMoinsUmin[i] = 0.0;
		if ( TypeDeVariable[i] == BORNEE ) UnSurUmaxMoinsUmin[i] = 1.0 /(Umax[i] - Umin[i]);
	}
}

if ( X != 0.0 ) Pi->PrendreEnCompteLesCoutsLineairesDansLeScaling = NON_PI;
else Pi->PrendreEnCompteLesCoutsLineairesDansLeScaling = FAIRE_SCALING_DES_COUTS;

RapportPrecedent = 0.0;
SeuilRapport     = 1.e-6;

/* Analyse des donnees */
PlusGrandTerme = -1.;
PlusPetitTerme =  LINFINI_PI;

if ( Pi->PrendreEnCompteLesCoutsLineairesDansLeScaling == OUI_PI ) {
  for ( i = 0 ; i < NombreDeVariables ; i++ ) {
    X = L[i];
    if( X > Zero ) {
      if ( X > PlusGrandTerme ) PlusGrandTerme = X;
      if ( X < PlusPetitTerme ) PlusPetitTerme = X; 
    }		
  }
}
if ( SCALING_DES_BORNES == OUI_PI ) {
  for ( i = 0 ; i < NombreDeVariables ; i++ ) {
    X = UnSurUmaxMoinsUmin[i];
    if( X > Zero ) {
      if ( X > PlusGrandTerme ) PlusGrandTerme = X;
      if ( X < PlusPetitTerme ) PlusPetitTerme = X; 
    }
  }
}
for ( i = 0 ; i < NombreDeContraintes ; i++ ) {
  il = Mdeb[i];
  ilMax = il + NbTerm[i];
  while ( il < ilMax ) {
    X = A[il];
    if( X > Zero ) {
      if ( X > PlusGrandTerme ) PlusGrandTerme = X;
      if ( X < PlusPetitTerme ) PlusPetitTerme = X;
    }
    il++;
  }
}

Rapport = PlusGrandTerme/PlusPetitTerme;

#if VERBOSE_PI
  printf(" Avant scaling PlusPetitTerme %20.10lf PlusGrandTerme %20.10lf rapport %20.10lf \n",
           PlusPetitTerme,PlusGrandTerme,Rapport);
#endif

if ( Rapport < RAPPORT_DE_NEUTRALISATION ) goto FinCalculScaling;

for ( k = 0 ; k < NOMBRE_MAX_DE_PASSES_DE_SCALING ; k++ ) {
  /* Controle du conditionnement avant la passe de scaling */
  PlusGrandTerme = -1.;
  PlusPetitTerme = LINFINI_PI;
  
  if ( Pi->PrendreEnCompteLesCoutsLineairesDansLeScaling == OUI_PI ) {  
    for ( i = 0 ; i < NombreDeVariables ; i++ ) {
      X = L[i];
      if( X > Zero ) {
        if ( X > PlusGrandTerme ) PlusGrandTerme = X;
        if ( X < PlusPetitTerme ) PlusPetitTerme = X; 
      }
    }
  }
  if ( SCALING_DES_BORNES == OUI_PI ) {
    for ( i = 0 ; i < NombreDeVariables ; i++ ) {
      X = UnSurUmaxMoinsUmin[i];
      if( X > Zero ) {
        if ( X > PlusGrandTerme ) PlusGrandTerme = X;
        if ( X < PlusPetitTerme ) PlusPetitTerme = X; 
      }
    }
  }	
  for ( i = 0 ; i < NombreDeContraintes ; i++ ) {
    il    = Mdeb[i]; 
    ilMax = il + NbTerm[i]; 
    while ( il < ilMax ) {
      X = A[il];
      if( X > Zero ) {
        if ( X > PlusGrandTerme ) PlusGrandTerme = X;
        if ( X < PlusPetitTerme ) PlusPetitTerme = X;
      }
      il++;
    }    
  }

  Rapport = PlusGrandTerme/PlusPetitTerme;

  #if VERBOSE_PI
    printf(" Avant passe de scaling PlusPetitTerme %e PlusGrandTerme %e rapport %e \n",PlusPetitTerme,PlusGrandTerme,Rapport);
  #endif

  if ( k >= 1 ) {
    if ( fabs ( RapportPrecedent - Rapport ) < SeuilRapport * RapportPrecedent ) {    
      break;
    }
    if ( RapportPrecedent < Rapport ) {
      /* On recupere les coeff de l'etape precedente et on arrete */
      Pi->ScaleLigneDesCouts = ScaleLigneDesCouts;
      Pi->ScaleLigneDesU     = ScaleLigneDesU;			
      for ( i = 0 ; i < NombreDeVariables ; i++ )   Pi->ScaleU[i] = ScaleU[i];
      for ( i = 0 ; i < NombreDeContraintes ; i++ ) Pi->ScaleB[i] = ScaleB[i];
      break;
    }
  }

  RapportPrecedent = Rapport;

  /* Stockage du scaling precedent */
  ScaleLigneDesCouts = Pi->ScaleLigneDesCouts;
  ScaleLigneDesU     = Pi->ScaleLigneDesU;
  for ( i = 0 ; i < NombreDeVariables ; i++ )   ScaleU[i] = Pi->ScaleU[i];
  for ( i = 0 ; i < NombreDeContraintes ; i++ ) ScaleB[i] = Pi->ScaleB[i];

  /* Scaling des colonnes: attention dans le scaling des colonnes on ne tient pas comte 
     des couts quadratiques */
  for ( i = 0 ; i < NombreDeVariables ; i++ ) {
    Flag = 0;
    PlusGrandTerme = -1.;
    PlusPetitTerme = LINFINI_PI;
    if ( Pi->PrendreEnCompteLesCoutsLineairesDansLeScaling == OUI_PI ) {  
      X = L[i];        
      if( X > Zero ) {
        if ( X > PlusGrandTerme ) { PlusGrandTerme = X; Flag = 1; }
        if ( X < PlusPetitTerme ) { PlusPetitTerme = X; Flag = 1; }
      }
    }
    /*   */
    il = Cdeb[i];
    while ( il >= 0 ) {
      X = A[il];
      if( X > Zero ) {
        if ( X > PlusGrandTerme ) { PlusGrandTerme = X; Flag = 1; }
        if ( X < PlusPetitTerme ) { PlusPetitTerme = X; Flag = 1; }
      }
      il = Csui[il];
    }
    /*   */
	  if ( SCALING_DES_BORNES == OUI_PI ) {
      X = UnSurUmaxMoinsUmin[i];
      if( X > Zero ) {
        if ( X > PlusGrandTerme ) { PlusGrandTerme = X; Flag = 1; }
        if ( X < PlusPetitTerme ) { PlusPetitTerme = X; Flag = 1; }
			}
		}
    /*   */		
    X = 1.;
    UnSurX = 1.;
    if ( Flag == 1 ) {
      X = PlusGrandTerme * PlusPetitTerme;
      X = sqrt( X );
      UnSurX = 1. / X;
    }
		/* Arrondi en puissance de 2 */
    if ( ARRONDI_EN_PUISSANCES_DE_2 == OUI_PI ) PI_ArrondiEnPuissanceDe2( &UnSurX );
    /*  */		
    Pi->ScaleU[i]*= UnSurX; 
    /* Scaling du cout */
    if ( Pi->PrendreEnCompteLesCoutsLineairesDansLeScaling == OUI_PI ) L[i]*= UnSurX;
    if ( SCALING_DES_BORNES == OUI_PI ) UnSurUmaxMoinsUmin[i]*= UnSurX;		
    /* Scaling de A */
    il = Cdeb[i];
    while ( il >= 0 ) {
     A[il]*= UnSurX;
     il = Csui[il];
    }
  }

  /*-------------------- Scaling des lignes ------------------------*/

  /* Objectif */
  if ( Pi->PrendreEnCompteLesCoutsLineairesDansLeScaling == OUI_PI ) {  
    Flag = 0;
    PlusGrandTerme = -1.;
    PlusPetitTerme = LINFINI_PI;
    for ( i = 0 ; i < NombreDeVariables ; i++ ) {
      X = L[i];        
      if( X > Zero ) {
        if ( X > PlusGrandTerme ) { PlusGrandTerme = X; Flag = 1; }
        if ( X < PlusPetitTerme ) { PlusPetitTerme = X; Flag = 1; }
      } 
    }  
    X = 1.;
    UnSurX = 1.;
    if ( Flag == 1 ) {
      X = PlusGrandTerme * PlusPetitTerme;
      X = sqrt( X );
      UnSurX = 1. / X;
    }
		/* Arrondi en puissance de 2 */
    if ( ARRONDI_EN_PUISSANCES_DE_2 == OUI_PI ) PI_ArrondiEnPuissanceDe2( &UnSurX );
    /*  */				
    Pi->ScaleLigneDesCouts*= UnSurX;
    for ( i = 0 ; i < NombreDeVariables ; i++ ) L[i]*= UnSurX;
  }
  /* Lignes */
  for ( i = 0 ; i < NombreDeContraintes ; i++ ) {
    Flag = 0;
    PlusGrandTerme = -1.;
    PlusPetitTerme = LINFINI_PI;
    il = Mdeb[i];
    ilMax = il + NbTerm[i];
    while ( il < ilMax ) {
      X = A[il];
      if( X > Zero ) {
        if ( X > PlusGrandTerme ) { PlusGrandTerme = X; Flag = 1; }
        if ( X < PlusPetitTerme ) { PlusPetitTerme = X; Flag = 1; }
      }
      il++;
    }
    X = 1.;
    UnSurX = 1.;
    if ( Flag == 1 ) {
      X = PlusGrandTerme * PlusPetitTerme;
      X = sqrt( X );
      UnSurX = 1. / X;
    }
		/* Arrondi en puissance de 2 */
    if ( ARRONDI_EN_PUISSANCES_DE_2 == OUI_PI ) PI_ArrondiEnPuissanceDe2( &UnSurX );
    /*  */					
    Pi->ScaleB[i]*= UnSurX;
    /* Scaling de A */
    il = Mdeb[i];
    ilMax = il + NbTerm[i];
    while ( il < ilMax ) {
     A[il]*= UnSurX;
     il++;
    }
  }
  /* Bornes */
	if ( SCALING_DES_BORNES == OUI_PI ) {
    for ( i = 0 ; i < NombreDeVariables ; i++ ) {
	    Flag = 0;
      PlusGrandTerme = -1.;
      PlusPetitTerme = LINFINI_PI;
			X = UnSurUmaxMoinsUmin[i];
      if( X > Zero ) {
        if ( X > PlusGrandTerme ) { PlusGrandTerme = X; Flag = 1; }
        if ( X < PlusPetitTerme ) { PlusPetitTerme = X; Flag = 1; }
			}
		}			
    X = 1.;
    UnSurX = 1.;
    if ( Flag == 1 ) {
      X = PlusGrandTerme * PlusPetitTerme;
      X = sqrt( X );
      UnSurX = 1. / X;
    }
	  /* Arrondi en puissance de 2 */
    if ( ARRONDI_EN_PUISSANCES_DE_2 == OUI_PI ) PI_ArrondiEnPuissanceDe2( &UnSurX );
    /*  */				
    /*Pi->ScaleLigneDesU*= 1. / UnSurX;*/
		/* Il vaut mieux ne pas retriturer les bornes avec ScaleLigneDesU */
		Pi->ScaleLigneDesU = 1.;
    for ( i = 0 ; i < NombreDeVariables ; i++ ) {
      UnSurUmaxMoinsUmin[i]*= UnSurX;
	  }
	}	
}

FinCalculScaling:

free( A );            
free( L );      
free( ScaleU );      
free( ScaleB );  
free( UnSurUmaxMoinsUmin );

return;
}


/*------------------------------------------------------------------------*/
/*                           Scaling                                      */ 
/* Apres avoir calcule les matrices de scaling, on fait le scaling.       */

void PI_Scaling( PROBLEME_PI * Pi )
{
int   i; int il    ; int   ilMax; double X; double UnSurX ; double Rapport;
double PlusGrandTerme; double PlusPetitTerme ; double Zero   ;
int NombreDeVariables; int NombreDeContraintes; int * Cdeb; int * Csui;
int * Mdeb; int * NbTerm; 
double * ScaleU; double * L; double * Q; double * A; double * U; double * Umin;
double * Umax; double * ScaleB; double * B; 
char * TypeDeVariable;

Zero = ZERO_SC;

NombreDeVariables   = Pi->NombreDeVariables;
NombreDeContraintes = Pi->NombreDeContraintes;

ScaleU = Pi->ScaleU;
ScaleB = Pi->ScaleB;
L = Pi->L;
Q = Pi->Q;
A = Pi->A;
U = Pi->U;
B = Pi->B;
Umin   = Pi->Umin;
Umax   = Pi->Umax;
Cdeb   = Pi->Cdeb;
Csui   = Pi->Csui;
Mdeb   = Pi->Mdeb;
NbTerm = Pi->NbTerm;
TypeDeVariable = Pi->TypeDeVariable;

/* Maintenant on fait le scaling effectif */
for ( i = 0 ; i < NombreDeVariables ; i++ ) {    
  X      = ScaleU[i];
  UnSurX = 1. / X;
  /* Scaling du cout */
  L[i]*= X;
  Q[i]*= X * X;
  /* Scaling de la matrice des contraintes A */
  il = Cdeb[i];
  while ( il >= 0 ) {
    A[il]*= X; 
    il = Csui[il];
  }
  /* Scaling des variables */
  U[i]*= UnSurX;
  if ( TypeDeVariable[i] != NON_BORNEE ) Umin[i]*= UnSurX;
  if ( TypeDeVariable[i] == BORNEE     ) Umax[i]*= UnSurX;
}

X = Pi->ScaleLigneDesU;
for ( i = 0 ; i < Pi->NombreDeVariables ; i++ ) {
  if ( TypeDeVariable[i] == BORNEE ) {
	  U [i]*= X;
	  Umin[i]*= X;
	  Umax[i]*= X;
	}
}

X = Pi->ScaleLigneDesCouts;
for ( i = 0 ; i < NombreDeVariables ; i++ ) {
  L[i]*= X;
}
	
for ( i = 0 ; i < NombreDeContraintes ; i++ ) {
  X = ScaleB[i];
  /* Scaling de B */
  B[i]*= X;			     
  /* Scaling de A */
  il    = Mdeb[i];
  ilMax = il + NbTerm[i]; 
  while ( il < ilMax ) {
   A[il]*= X;
   il++;
  }
}

#if VERBOSE_PI

/* Verification */

printf("Verification apres application effective du scaling:\n");

PlusGrandTerme = -1.;
PlusPetitTerme = LINFINI_PI;

if ( Pi->PrendreEnCompteLesCoutsLineairesDansLeScaling == OUI_PI ) {  
  for ( i = 0 ; i < NombreDeVariables ; i++ ) {
    X = fabs( L[i] );
    if( X > Zero ) {
      if ( X > PlusGrandTerme ) PlusGrandTerme = X;
      if ( X < PlusPetitTerme ) PlusPetitTerme = X; 
    }
  }
}
	
for ( i = 0 ; i < NombreDeContraintes ; i++ ) {
  il    = Mdeb[i]; 
  ilMax = il + NbTerm[i]; 
  while ( il < ilMax ) {
    X = fabs( A[il] );
    if( X > Zero ) {
      if ( X > PlusGrandTerme ) PlusGrandTerme = X;
      if ( X < PlusPetitTerme ) PlusPetitTerme = X;
    }
    il++;
  }    
}

Rapport = PlusGrandTerme/PlusPetitTerme;

printf("         PlusPetitTerme %20.10lf PlusGrandTerme %20.10lf rapport %20.10lf \n",PlusPetitTerme,PlusGrandTerme,Rapport);

#endif

return;
}

/*------------------------------------------------------------------------*/

void PI_ArrondiEnPuissanceDe2( double * ValeurRetenue )
{
double PuissanceDe2             ; double X ; double ValeurPuissanceInferieure; 	  
double ValeurPuissanceCourante  ; int  P2  ; double ValeurPuissanceSuperieure;
int i; double Y; double Y1     ; double Y2; double Y3;

/*
X = *ValeurRetenue;
frexp( X , &P2 );

ValeurPuissanceInferieure = ldexp( 1. , P2-1 );
ValeurPuissanceCourante   = ldexp( 1. , P2   );
ValeurPuissanceSuperieure = ldexp( 1. , P2+1 );

i  = 0;
Y  = LINFINI_PI;;
Y1 = fabs( X - ValeurPuissanceInferieure );
Y2 = fabs( X - ValeurPuissanceCourante );
Y3 = fabs( X - ValeurPuissanceSuperieure );

if ( Y1 < Y ) { Y = Y1; i = 1; }
if ( Y2 < Y ) { Y = Y2; i = 2; }
if ( Y3 < Y ) { Y = Y3; i = 3; }

if ( i == 1 )      *ValeurRetenue = ValeurPuissanceInferieure;
else if ( i == 2 ) *ValeurRetenue = ValeurPuissanceCourante;
else if ( i == 3 ) *ValeurRetenue = ValeurPuissanceSuperieure;
else               *ValeurRetenue = ValeurPuissanceCourante;
*/

X = *ValeurRetenue;
frexp( X , &P2 );

ValeurPuissanceInferieure = ldexp( 1. , P2-1 );
ValeurPuissanceSuperieure = ldexp( 1. , P2   );

if ( fabs( X - ValeurPuissanceInferieure ) > fabs ( X - ValeurPuissanceSuperieure ) ) {
  *ValeurRetenue = ValeurPuissanceSuperieure;
}
else {
  *ValeurRetenue = ValeurPuissanceInferieure;
}

return;
}

/*------------------------------------------------------------------------*/
/*                         UnScaling                                      */ 

void PI_UnScaling( PROBLEME_PI * Pi )
{
int i; double * ScaleU; double * U; double * S1; double * S2;
double UnSurScaleLigneDesU ; char * TypeDeVariable;

ScaleU = Pi->ScaleU;
U  = Pi->U;
S1 = Pi->S1;
S2 = Pi->S2;

UnSurScaleLigneDesU = 1. / Pi->ScaleLigneDesU;

TypeDeVariable = Pi->TypeDeVariable;

for ( i = 0 ; i < Pi->NombreDeVariables ; i++ ) {
  U[i]*= ScaleU[i];
	
	if ( TypeDeVariable[i] == BORNEE ) U[i]*= UnSurScaleLigneDesU;
	
  S1[i]/= ScaleU[i]; 
  S2[i]/= ScaleU[i]; 
}

return;
}


