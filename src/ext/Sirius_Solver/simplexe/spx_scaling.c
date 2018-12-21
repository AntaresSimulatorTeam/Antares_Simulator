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

   FONCTION: Scaling
               
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"   
# include "spx_define.h"  

# include "pne_fonctions.h"

# ifdef SPX_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "spx_memoire.h"
# endif

# define SEUIL_ADMISSIBILITE_FONCTION_DE_LA_MATRICE    OUI_SPX /*OUI_SPX*/
# define SEUIL_ADMISSIBILITE_DUALE_FONCTION_DU_SCALING OUI_SPX /*OUI_SPX*/

# define NOMBRE_MAX_DE_PASSES_DE_SCALING          20     /* 20 */
# define MIN_DE_PASSES_DE_SCALING                 5 
# define ARRONDI_EN_PUISSANCES_DE_2               OUI_SPX /* OUI_SPX */

# define RAPPORT_INHIBITION_SCALING            1000.
# define RAPPORT_INHIBITION_SCALING_EN_SERVICE OUI_SPX /* OUI_SPX */

/* Permet de faire un equilibrage apres la moyenne geometrique. Consiste a diviser
   iterativement les lignes et colonnes par la norme L infini (abs du plus grand terme)
   de la ligne ou de la colonne */
# define EQUILIBRAGE_APRES_MOYENNE_GEOMETRIQUE OUI_SPX /*OUI_SPX*/
# define SEUIL_POUR_EQUILIBRAGE_MATRICE        10. /* 10. */ /* Seuil a partir duquel on fait un equilibrage de la matrice */
# define ARRONDI_EN_PUISSANCES_DE_2_POUR_EQUILIBRAGE OUI_SPX /* NON_SPX*/  
                                                             /* l'arrondi n'est peut-etre pas necessaire car on ne fait generalement */
                                                             /* pas plus de 2 passes */
# define SEUIL_ECART_UN 1.e-1 /* Permet de sortir prematurement de la boucle d'equilibrage de la matrice */

# define ZERO   1.e-12 /*1.e-12*/

# define RAPPORT_MAX_COUTS 1.e+4  /*1.e+5*/  
# define RAPPORT_INACCEPTABLE_COUTS  (100*RAPPORT_MAX_COUTS) /*1.e+9*/

# define DEGRADATION_MAX_DU_RAPPORT 10 /*10*/
# define RAPPORT_ACCEPTABLE 1.e+4 /*1.e+4*/

# define PLUS_PETIT_COUT_SOUHAITE (10*VALEUR_PERTURBATION_COUT_A_POSTERIORI) 

# define MIN_SCALE_LIGNE_COUTS 1.e-6 /*1.e-6*/

# define ECART_MIN_XMAX_XMIN 1.e-4 
# define RAPPORT_COUT_LIMITE 1.e+10

# define VERBOSE_SPX_SCALING 0

void SPX_InitMatriceEtCoutsAvantScaling( PROBLEME_SPX *, double * , double * );
void SPX_AppliquerScalingSurMatriceDeTravail( PROBLEME_SPX * , double *, double * );

/*----------------------------------------------------------------------------*/

void SPX_InitMatriceEtCoutsAvantScaling( PROBLEME_SPX * Spx, double * A, double * C )
{
int il; int ilMax; int Cnt; int Var; int * Mdeb; int * NbTerm; double * ScaleX; 
double * Aspx; double * Cspx; int NombreDeVariables; int NombreDeContraintes;
double * ScaleB;

NombreDeVariables = Spx->NombreDeVariables;
NombreDeContraintes = Spx->NombreDeContraintes;
Aspx = Spx->A;
Cspx = Spx->Csv;
Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
ScaleX = Spx->ScaleX;
ScaleB = Spx->ScaleB;
Spx->ScaleLigneDesCouts = 1.;

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  ScaleX[Var] = 1.;
  C[Var] = fabs( Cspx[Var] );
}
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  ScaleB[Cnt ] = 1.;
  il    = Mdeb[Cnt];	
  ilMax = il + NbTerm[Cnt]; 
  while ( il < ilMax ) {
 	  A[il] = fabs( Aspx[il] );		
    il++;
  }
}
return;
}

/*----------------------------------------------------------------------------*/

void SPX_AppliquerScalingSurMatriceDeTravail( PROBLEME_SPX * Spx, double * A, double * C )
{
int il; int ilMax; int Cnt; int Var; int * Mdeb; int * NbTerm; double * ScaleX; 
double * Aspx; double * Cspx; int NombreDeVariables; int NombreDeContraintes;
double * ScaleB; int * Indcol; double ScaleLigneDesCouts;

NombreDeVariables = Spx->NombreDeVariables;
NombreDeContraintes = Spx->NombreDeContraintes;
ScaleLigneDesCouts = Spx->ScaleLigneDesCouts;
Aspx = Spx->A;
Cspx = Spx->Csv;
Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
ScaleX = Spx->ScaleX;
ScaleB = Spx->ScaleB;

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) C[Var] = fabs( Cspx[Var] ) * ScaleX[Var] * ScaleLigneDesCouts;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  il    = Mdeb[Cnt]; 
  ilMax = il + NbTerm[Cnt]; 
  while ( il < ilMax ) {
    A[il] = fabs( Aspx[il] ) * ScaleB[Cnt] * ScaleX[Indcol[il]];		
    il++;
  }
}
return;
}

/*----------------------------------------------------------------------------*/

void SPX_CalculerLeScaling( PROBLEME_SPX * Spx )
{
int il; int ilMax; int k; double X; double UnSurX; int Phase; double PlusGrandTerme; double PlusPetitTerme;
double Rapport; int Flag; double RapportPrecedent; double SeuilRapport; double * A; double * C;
int NombreDeVariables; int NombreDeContraintes; double ScaleLigneDesCoutsOpt; double PlusGrandCout;
double PlusPetitCout; double * ScaleXOpt; double * ScaleBOpt; char FaireScalingDesCouts;
char EquilibrageApresMoyenneGeometrique; char LignesOk; char ColonnesOk; int kMin; double RapportOpt;
double * ScaleX ; double * ScaleB; double ScaleLigneDesCouts; double * XMult; double * PlusGrandTermeColonne; 
double * PlusPetitTermeColonne; int * Mdeb; int * NbTerm; int * Indcol; double RapportIntialDesCouts; 
double RapportOptCouts; int Var; int Cnt; double RapportCouts; char ScalingCoutsInhibe; double X0; double NouveauCout;
char * TypeDeVariable; double * Xmax; double * Xmin; double * Csv; double CoutVar; 

if ( Spx->FaireDuScalingSPX == NON_SPX ) return;

NombreDeVariables   = Spx->NombreDeVariables;
NombreDeContraintes = Spx->NombreDeContraintes;

Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;

A      = (double *) malloc( Spx->NbTermesAlloues   * sizeof( double ) );            
C      = (double *) malloc( NombreDeVariables * sizeof( double ) );      
ScaleX = (double *) malloc( NombreDeVariables * sizeof( double ) );      

PlusGrandTermeColonne = (double *) malloc( NombreDeVariables * sizeof( double ) );
PlusPetitTermeColonne = (double *) malloc( NombreDeVariables * sizeof( double ) );
XMult                 = (double *) malloc( NombreDeVariables * sizeof( double ) );

ScaleB = (double *) malloc( NombreDeContraintes * sizeof( double ) );

ScaleXOpt = (double *) malloc( NombreDeVariables * sizeof( double ) );      
ScaleBOpt = (double *) malloc( NombreDeContraintes * sizeof( double ) );

if ( A == NULL || C == NULL || ScaleX  == NULL || PlusGrandTermeColonne == NULL || PlusPetitTermeColonne == NULL ||
     XMult == NULL || ScaleB == NULL || ScaleXOpt == NULL || ScaleBOpt == NULL ) {    
  printf("Simplexe, sous-programme SPX_FaireScaling : \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); 
}

SPX_InitMatriceEtCoutsAvantScaling( Spx, A, C );

PlusGrandCout = -1.;
PlusPetitCout = LINFINI_SPX;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
	if ( C[Var] == 0.0 ) continue;
	if ( C[Var] < PlusPetitCout ) PlusPetitCout =  C[Var];
	if ( C[Var] > PlusGrandCout ) PlusGrandCout =  C[Var];
}

RapportIntialDesCouts = 1.;
if ( PlusPetitCout < LINFINI_SPX ) RapportIntialDesCouts = PlusGrandCout / PlusPetitCout;

EquilibrageApresMoyenneGeometrique = EQUILIBRAGE_APRES_MOYENNE_GEOMETRIQUE;

RapportPrecedent    = 0.0;
ScaleLigneDesCouts  = 1.0;
SeuilRapport        = 1.e-6;

FaireScalingDesCouts = NON_SPX;
kMin = 1;
Phase = 1;

RapportOpt = LINFINI_PNE; /* Juste pour eviter les warning de compilation */
RapportOptCouts = LINFINI_PNE; /* Juste pour eviter les warning de compilation */
ScaleLigneDesCoutsOpt = 1; /* Juste pour eviter les warning de compilation */

DebutDesPassesDeScaling:
/* Plusieurs passes */

ScalingCoutsInhibe = NON_PNE;

for ( k = 0 ; k < NOMBRE_MAX_DE_PASSES_DE_SCALING ; k++ ) {

  /* Controle du conditionnement avant la passe de scaling */	
  PlusGrandCout = -1.;
  PlusPetitCout = LINFINI_SPX;	  
	if ( FaireScalingDesCouts == OUI_SPX && ScalingCoutsInhibe == NON_PNE ) {
    for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
      X = C[Var];        
      if( X != 0.0 ) {
        if ( X > PlusGrandCout ) PlusGrandCout = X; 
        if ( X < PlusPetitCout ) PlusPetitCout = X; 
      } 
    }
  }
  PlusGrandTerme = PlusGrandCout;
  PlusPetitTerme = PlusPetitCout;		
  for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
    il    = Mdeb[Cnt]; 
    ilMax = il + NbTerm[Cnt]; 
    while ( il < ilMax ) {
      X = A[il];
      if( X != 0.0 ) {
        if ( X > PlusGrandTerme ) PlusGrandTerme = X;
        if ( X < PlusPetitTerme ) PlusPetitTerme = X;
      }
      il++;
    }    
  }
	
  Rapport = PlusGrandTerme / PlusPetitTerme;
	
  #if VERBOSE_SPX_SCALING == 1
    printf(" Avant passe de scaling PlusPetitTerme %e PlusGrandTerme %e rapport %e \n",PlusPetitTerme,PlusGrandTerme,Rapport);
  #endif

  if ( k == 0 ) {
    if ( Rapport < RAPPORT_INHIBITION_SCALING && RAPPORT_INHIBITION_SCALING_EN_SERVICE == OUI_SPX && Phase == 1 ) {
      #if VERBOSE_SPX_SCALING == 1
			  printf("Rapport des termes de la matrice %e RAPPORT_INHIBITION_SCALING %e => on ne fait pas de scaling sur la matrice\n",
				        Rapport, RAPPORT_INHIBITION_SCALING);
      # endif
      break;
		}
	}	

  if ( k >= kMin ) {	  
    if ( fabs ( RapportPrecedent - Rapport ) < SeuilRapport * RapportPrecedent ) {    
      break;
    }
    if ( RapportPrecedent < Rapport ) {
      /* On recupere les coeff de l'etape precedente et on arrete */
      Spx->ScaleLigneDesCouts = ScaleLigneDesCouts;
      memcpy( (char *) Spx->ScaleX, (char *) ScaleX, Spx->NombreDeVariables   * sizeof( double ) );
      memcpy( (char *) Spx->ScaleB, (char *) ScaleB, Spx->NombreDeContraintes * sizeof( double ) );      
      break;
    }		
  }

  RapportPrecedent = Rapport;
	  
  /* Stockage du scaling precedent */
  ScaleLigneDesCouts = Spx->ScaleLigneDesCouts;  
  memcpy( (char *) ScaleX, (char *) Spx->ScaleX, NombreDeVariables   * sizeof( double ) );
  memcpy( (char *) ScaleB, (char *) Spx->ScaleB, NombreDeContraintes * sizeof( double ) );

  ScalingCoutsInhibe = NON_PNE;

  /*-------------------- Scaling des lignes ------------------------*/
	
	/* On regarde les couts pour savoir s'il est necessaire de faire un scaling dessus */
	RapportCouts = 1;
	if ( FaireScalingDesCouts == OUI_SPX ) {
	  Flag = 0;
    PlusGrandCout = -1.;
    PlusPetitCout = LINFINI_SPX;
    for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
      X = C[Var];        
      if( X != 0.0 ) {
        if ( X > PlusGrandCout ) PlusGrandCout = X; 
        if ( X < PlusPetitCout ) { PlusPetitCout = X; Flag = 1; }
      } 
    }
		RapportCouts = PlusGrandCout / PlusPetitCout;
	  if ( Flag == 1 && RapportCouts > RAPPORT_MAX_COUTS ) {		
		  FaireScalingDesCouts = OUI_SPX;
      X = PlusGrandCout * PlusPetitCout;			
      X = sqrt( X );			
      UnSurX = 1. / X;				
      if ( ARRONDI_EN_PUISSANCES_DE_2 == OUI_SPX ) SPX_ArrondiEnPuissanceDe2( &UnSurX );			
      Spx->ScaleLigneDesCouts *= UnSurX;
      for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) C[Var] *= UnSurX; 			
		}
		else ScalingCoutsInhibe = OUI_PNE;
	}

  /* Lignes */
  for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
    Flag           = 0;
    PlusGrandTerme = -1.;
    PlusPetitTerme = LINFINI_SPX;		
    il    = Mdeb[Cnt]; 
    ilMax = il + NbTerm[Cnt]; 
    while ( il < ilMax ) {
      X = A[il];
      if( X != 0.0 ) {
        if ( X > PlusGrandTerme ) { PlusGrandTerme = X; Flag = 1; }
        if ( X < PlusPetitTerme ) { PlusPetitTerme = X; Flag = 1; }
      }
      il++;
    }
    X      = 1.;
    UnSurX = 1.;
    if ( Flag == 1 ) {		
      X = PlusGrandTerme * PlusPetitTerme;      
      X = sqrt( X );
      UnSurX = 1. / X;			
    }

    if ( ARRONDI_EN_PUISSANCES_DE_2 == OUI_SPX ) SPX_ArrondiEnPuissanceDe2( &UnSurX );		
		
    Spx->ScaleB[Cnt] *= UnSurX;
			
   /* Scaling de A */
    il    = Mdeb[Cnt];
    ilMax = il + NbTerm[Cnt]; 
    while ( il < ilMax ) {
     A[il] *= UnSurX;
     il++;
    }
  }	
   
  /*-------------------- Scaling des colonnes ------------------------*/

  /* Recherche PlusPetitTerme PlusGrandTerme */
  for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
    PlusGrandTermeColonne[Var] = -1.;
    PlusPetitTermeColonne[Var] = LINFINI_SPX;  
  }
	
  for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
    il    = Mdeb[Cnt]; 
    ilMax = il + NbTerm[Cnt];				
    while ( il < ilMax ) {
      X = A[il];
      if( X != 0.0 ) {
        if ( X > PlusGrandTermeColonne[Indcol[il]] ) PlusGrandTermeColonne[Indcol[il]] = X; 
        if ( X < PlusPetitTermeColonne[Indcol[il]] ) PlusPetitTermeColonne[Indcol[il]] = X; 
      }
      il++;
    }
  }
  /* Objectif */
  if ( FaireScalingDesCouts == OUI_SPX && ScalingCoutsInhibe == NON_PNE ) { 
    for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) { 
			X = C[Var];
      if( X != 0.0  ) {        
        if ( X > PlusGrandTermeColonne[Var] ) PlusGrandTermeColonne[Var] = X;
        if ( X < PlusPetitTermeColonne[Var] ) PlusPetitTermeColonne[Var] = X;
      }
    }
	}	

  for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
    X = 1.;
    UnSurX = 1.;
    if ( PlusGrandTermeColonne[Var] > 0.0 && PlusPetitTermeColonne[Var] < LINFINI_SPX ) {		
      X = PlusGrandTermeColonne[Var] * PlusPetitTermeColonne[Var];
      X = sqrt( X );
      UnSurX = 1. / X;						
    }
    if ( ARRONDI_EN_PUISSANCES_DE_2 == OUI_SPX ) SPX_ArrondiEnPuissanceDe2( &UnSurX );				
    XMult[Var] = UnSurX;
    Spx->ScaleX[Var] *= UnSurX;
    /* Scaling du cout */
    C[Var] *= UnSurX;   
  }
		
  /* Application de la participation au scaling de A */
  for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
    il    = Mdeb[Cnt]; 
    ilMax = il + NbTerm[Cnt];  
    while ( il < ilMax ) {
      A[il] *= XMult[Indcol[il]];
      il++;
    }
  }  

/* Fin Boucle generale de scaling */
}

/* Il faut appliquer le scaling obtenu juste avant car les coeff de scaling ne correspondent pas toujours a
   l'etat de la matrice */
SPX_AppliquerScalingSurMatriceDeTravail( Spx, A, C );	

if ( EquilibrageApresMoyenneGeometrique == OUI_SPX && Rapport > SEUIL_POUR_EQUILIBRAGE_MATRICE ) {

  # if VERBOSE_SPX_SCALING == 1	
    printf("Phase d'equilibrage ...\n");
  # endif	 
 	 
  for ( k = 0 ; k < NOMBRE_MAX_DE_PASSES_DE_SCALING ; k++ ) {
		
    LignesOk   = OUI_SPX;
		ColonnesOk = OUI_SPX;

    if ( FaireScalingDesCouts == OUI_SPX && k < 1 ) { 
      Flag = 0;
		  PlusGrandCout = -1.;
      for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
        X = C[Var];        
        if( X != 0.0 ) {
          if ( X > PlusGrandCout ) { PlusGrandCout = X; Flag = 1; }
        } 
      }
	    if ( Flag == 1 ) {
        X = PlusGrandCout;			
        UnSurX = 1. / X;				
        if ( ARRONDI_EN_PUISSANCES_DE_2 == OUI_SPX ) SPX_ArrondiEnPuissanceDe2( &UnSurX );			
        Spx->ScaleLigneDesCouts *= UnSurX;
        for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) C[Var] *= UnSurX; 			
	    }
    }
					
    /* Lignes */						
    for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
      PlusGrandTerme = -1.;			  
      il    = Mdeb[Cnt]; 
      ilMax = il + NbTerm[Cnt]; 
      while ( il < ilMax ) {
        if ( A[il] > PlusGrandTerme ) PlusGrandTerme = A[il];
        il++;
      }
			if ( fabs( PlusGrandTerme - 1.0 ) > SEUIL_ECART_UN ) LignesOk = NON_SPX;

      X = 1;      
      UnSurX = 1;			
			if ( PlusGrandTerme > 0 ) {
        X = PlusGrandTerme;      
        UnSurX = 1. / X;
			}									
      if ( ARRONDI_EN_PUISSANCES_DE_2_POUR_EQUILIBRAGE == OUI_SPX ) SPX_ArrondiEnPuissanceDe2( &UnSurX );						
      Spx->ScaleB[Cnt] *= UnSurX;
      /* Scaling de A */
      il    = Mdeb[Cnt];
      ilMax = il + NbTerm[Cnt]; 
      while ( il < ilMax ) {
       A[il] *= UnSurX;
       il++;
      }		
    }
	
    /* Colonnes */
    for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) PlusGrandTermeColonne[Var] = -1.;
				
    for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
      il    = Mdeb[Cnt]; 
      ilMax = il + NbTerm[Cnt];				
      while ( il < ilMax ) {
        if ( A[il] > PlusGrandTermeColonne[Indcol[il]] ) PlusGrandTermeColonne[Indcol[il]] = A[il];				
        il++;
      }
    }  

    if ( FaireScalingDesCouts == OUI_SPX && k < 1 ) { 
      for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) { 
			  X = C[Var];
        if( X != 0.0  ) {        
          if ( X > PlusGrandTermeColonne[Var] ) PlusGrandTermeColonne[Var] = X;
        }
      }
	  }
		
    for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
      X = PlusGrandTermeColonne[Var];
			if ( X <= 0.0 ) continue;
			if ( fabs( X - 1.0 ) > SEUIL_ECART_UN ) ColonnesOk = NON_SPX;
			
      UnSurX = 1.;						
      if ( X > 0  ) {
			  UnSurX = 1. / X;
			}
      if ( ARRONDI_EN_PUISSANCES_DE_2_POUR_EQUILIBRAGE == OUI_SPX ) SPX_ArrondiEnPuissanceDe2( &UnSurX );					
      XMult[Var] = UnSurX;
      Spx->ScaleX[Var] *= UnSurX;
		
      /* Scaling du cout */
      C[Var] *= UnSurX;   
    }
    /* Application de la participation au scaling de A */
    for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
      il    = Mdeb[Cnt]; 
      ilMax = il + NbTerm[Cnt];  
      while ( il < ilMax ) {
        A[il] *= XMult[Indcol[il]];
        il++;
      }
    }

		if ( LignesOk == OUI_SPX && ColonnesOk == OUI_SPX ) break;
		
  }
	
}

/* Recalcul du rapport */	
PlusGrandTerme = -1.;
PlusPetitTerme = LINFINI_SPX;	
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  il    = Mdeb[Cnt]; 
  ilMax = il + NbTerm[Cnt]; 
  while ( il < ilMax ) {
    X = A[il];
    if( X != 0.0 ) {
      if ( X > PlusGrandTerme ) PlusGrandTerme = X;
      if ( X < PlusPetitTerme ) PlusPetitTerme = X;
    }
    il++;
  }    
}

# if VERBOSE_SPX_SCALING == 1	
	printf("Apres equilibrage eventuel:\n");
	printf("  Matrice: PlusPetitTerme %e  PlusGrandTerme %e  Rapport %e\n",PlusPetitTerme,PlusGrandTerme,PlusGrandTerme/PlusPetitTerme);
	printf("  ScaleLigneDesCouts: %e\n",Spx->ScaleLigneDesCouts);
# endif

if ( Phase == 1 ) {	
  /* Stockage des coeff de scaling et calcul du rapport obtenu sur les termes de la matrice */
  RapportOpt = PlusGrandTerme / PlusPetitTerme;
	ScaleLigneDesCoutsOpt = Spx->ScaleLigneDesCouts;
  memcpy( (char *) ScaleXOpt, (char *) Spx->ScaleX, Spx->NombreDeVariables   * sizeof( double ) );
  memcpy( (char *) ScaleBOpt, (char *) Spx->ScaleB, Spx->NombreDeContraintes * sizeof( double ) );      
}
else {

	if ( Spx->ScaleLigneDesCouts < MIN_SCALE_LIGNE_COUTS ) Spx->ScaleLigneDesCouts = MIN_SCALE_LIGNE_COUTS;
	
  Rapport = PlusGrandTerme / PlusPetitTerme;	
	if ( Rapport > DEGRADATION_MAX_DU_RAPPORT * RapportOpt && Rapport > RAPPORT_ACCEPTABLE && RapportOptCouts < RAPPORT_INACCEPTABLE_COUTS ) {
    /* Le rapport s'est trop degrade on conserve les premiers coefficients */
    # if VERBOSE_SPX_SCALING == 1
		  printf("Prise en compte des couts dans le scaling:\n");
	    printf("  Matrice: PlusPetitTerme %e  PlusGrandTerme %e  Rapport %e\n",PlusPetitTerme,PlusGrandTerme,PlusGrandTerme/PlusPetitTerme);
      PlusGrandCout = -1.;
      PlusPetitCout = LINFINI_SPX;
      for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
        X = C[Var];				
        if( X != 0.0 ) {		
          if ( X > PlusGrandCout ) PlusGrandCout = X; 
          if ( X < PlusPetitCout ) PlusPetitCout = X;
        } 
      }
      Rapport = PlusGrandCout / PlusPetitCout;
	    printf("  Couts: PlusPetitCout %e  PlusGrandCout %e  Rapport %e\n",PlusPetitCout,PlusGrandCout,Rapport);			
			printf("  Le rapport sur la matrice se degrade trop  => on prend les coeffs de scaling de la premiers passe (sans le scaling des couts)\n");
    # endif		
	  Spx->ScaleLigneDesCouts = ScaleLigneDesCoutsOpt;
    memcpy( (char *) Spx->ScaleX, (char *) ScaleXOpt, Spx->NombreDeVariables   * sizeof( double ) );
    memcpy( (char *) Spx->ScaleB, (char *) ScaleBOpt, Spx->NombreDeContraintes * sizeof( double ) );
		goto FinScaling;		
	}	
}

Phase++;

/* Controle du scaling resultant des cout: si le rapport est trop grand, on refait un scaling
   en tenant compte des cout */

PlusGrandCout = -1.;
PlusPetitCout = LINFINI_SPX;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  X = C[Var];				
  if( X != 0.0 ) {		
    if ( X > PlusGrandCout ) PlusGrandCout = X; 
    if ( X < PlusPetitCout ) PlusPetitCout = X;
  } 
}
Rapport = PlusGrandCout / PlusPetitCout;
RapportOptCouts = Rapport;

# if VERBOSE_SPX_SCALING == 1	
  printf("  Couts: PlusPetitCout %e  PlusGrandCout %e  Rapport %e  Rapport initial %e\n",PlusPetitCout,PlusGrandCout,Rapport,RapportIntialDesCouts);
# endif
	 
if ( FaireScalingDesCouts == NON_SPX ) {
	if ( RapportOptCouts > RAPPORT_MAX_COUTS && RapportOptCouts > RapportIntialDesCouts ) {
    # if VERBOSE_SPX_SCALING == 1	
		  printf("  Rapport des couts / Rapport initial trop degrade . On refait un scaling avec les couts \n");
		# endif			
		FaireScalingDesCouts = OUI_SPX;
		kMin = 2;

		/* On reinitialise la matrice et les coefficients de scaling */
    SPX_InitMatriceEtCoutsAvantScaling( Spx, A, C ); 
		
    goto DebutDesPassesDeScaling;
	}
	else {
    # if VERBOSE_SPX_SCALING == 1	
		  printf("  Pas de scaling des couts \n");
		# endif			
	}
}

/* On applique le scaling sur les bornes pour corriger les rapprochements excessifs */
TypeDeVariable = Spx->TypeDeVariable;
Xmax = Spx->Xmax;
Xmin = Spx->Xmin;
Csv = Spx->Csv;
ScaleLigneDesCouts = Spx->ScaleLigneDesCouts;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {    
  if ( TypeDeVariable[Var] == BORNEE && Xmax[Var] != Xmin[Var] ) { 
    X0 = Xmax[Var] - Xmin[Var];
    X = X0 / Spx->ScaleX[Var];
    if ( X < ECART_MIN_XMAX_XMIN ) {
      X = X0 / ECART_MIN_XMAX_XMIN;
      /* Il faut verifier qu'on ne deteriore pas trop le rapport plus grand cout / plus petit cout: ne pas depasser 10^10 */
      CoutVar = Csv[Var];
      if ( CoutVar != 0 ) {
        NouveauCout = ScaleLigneDesCouts * fabs( CoutVar ) * X;
        if ( NouveauCout > PlusGrandCout ) {
          if ( NouveauCout / PlusPetitCout > RAPPORT_COUT_LIMITE ) {
            X = ( RAPPORT_COUT_LIMITE * PlusPetitCout ) / ( ScaleLigneDesCouts * fabs( CoutVar ) );           
          }
        }
        else if ( NouveauCout < PlusPetitCout ) {
          if ( PlusGrandCout / NouveauCout > RAPPORT_COUT_LIMITE ) {
            X = PlusGrandCout / ( RAPPORT_COUT_LIMITE * ScaleLigneDesCouts * fabs( CoutVar ) );           
          }
        }
      }
      if ( ARRONDI_EN_PUISSANCES_DE_2_POUR_EQUILIBRAGE == OUI_SPX ) SPX_ArrondiEnPuissanceDe2( &X );					
      Spx->ScaleX[Var] = X;
    }
  }
}
PlusPetitCout = LINFINI_SPX;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( Csv[Var] == 0 ) continue;
  X = fabs( Csv[Var] ) * ScaleLigneDesCouts * Spx->ScaleX[Var];				
  if ( X < PlusPetitCout ) PlusPetitCout = X; 
}

/* Si le cout min est trop petit on le remonte pour ne pas le confondre avec une valeur de bruitage dans le cas degenere */
if ( PlusPetitCout < LINFINI_SPX ) {
  if ( PlusPetitCout < PLUS_PETIT_COUT_SOUHAITE ) {
	  Spx->ScaleLigneDesCouts *= PLUS_PETIT_COUT_SOUHAITE / PlusPetitCout;	
	}
}

FinScaling:

/* Liberation memoire */
free( A );
free( C );
free( PlusGrandTermeColonne );
free( PlusPetitTermeColonne );
free( XMult );
free( ScaleX );
free( ScaleB );
free( ScaleXOpt );
free( ScaleBOpt );

return;
}

/*------------------------------------------------------------------------*/

void SPX_ArrondiEnPuissanceDe2( double * ValeurRetenue )
{
double X; double ValeurPuissanceInferieure; double ValeurPuissanceSuperieure; int  P2;

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
/*                           Scaling                                      */ 
/* Apres avoir calcule les matrices de scaling, on fait le scaling.       */

void SPX_Scaling( PROBLEME_SPX * Spx )
{
int i; int il; int ilMax; double X; double UnSurX; double Seuil; double PlusGrandTerme;
double PlusPetitTerme; int Var; double * InfScaleBSurA; double ScaleLigneDesCouts;
#if VERBOSE_SPX_SCALING == 1
  double Rapport; 
# endif
double UnSurXFoisUnSurSupXmax ; int * Mdeb; int * NbTerm; int * Indcol; double * A;
char * TypeDeVariable; double * ScaleX; double * Csv ; double * XArray; double * Xmin  ;
double * Xmax; double * SeuilDeViolationDeBorne; double * B; double * ScaleB; double ScB;
double MoyenneB; int Nmoy; char SeuilDeViolationdeBorneFonctionDeLaMatrice;
char SeuilDAmissibiliteDualeFonctionDuScaling; double Smin1; double	Smax1; double	Smin2;
double	Smax2; double * SeuilDAmissibiliteDuale1; double * SeuilDAmissibiliteDuale2;

if ( Spx->FaireDuScalingSPX == NON_SPX ) {

  # ifdef UTILISER_BORNES_AUXILIAIRES
    /* On calcule quand-meme la valeur moyenne du second membre */
    B = Spx->B;
    MoyenneB = 0.0;
	  Nmoy     = 0;
    for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) {
	    if ( B[i] != 0.0 ) {
		    MoyenneB+= fabs( B[i] );
		  	Nmoy++;
		  }
	  }
	  Spx->ValeurMoyenneDuSecondMembre = MoyenneB / Nmoy;
	# endif
	
  return;
}

Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
A      = Spx->A;

ScaleX = Spx->ScaleX;
Csv    = Spx->Csv;
XArray = Spx->X;
Xmin   = Spx->Xmin;
Xmax   = Spx->Xmax;
SeuilDeViolationDeBorne = Spx->SeuilDeViolationDeBorne;

TypeDeVariable = Spx->TypeDeVariable;

B      = Spx->B;
ScaleB = Spx->ScaleB;

ScaleLigneDesCouts = Spx->ScaleLigneDesCouts;

MoyenneB = 0.0;
Nmoy     = 0;

/* Maintenant on fait le scaling effectif */
for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) {    
  X      = ScaleX[i];
  UnSurX = 1. / X;
  /* Scaling du cout */
  Csv[i]*= X;        
  /* Scaling des variables */
  UnSurXFoisUnSurSupXmax = UnSurX;
  if ( TypeDeVariable[i] != NON_BORNEE ) Xmin[i]*= UnSurXFoisUnSurSupXmax;
  if ( TypeDeVariable[i] == BORNEE     ) Xmax[i]*= UnSurXFoisUnSurSupXmax;
	/* Ne concerne que les variables natives */
	Seuil = SeuilDeViolationDeBorne[i] * UnSurXFoisUnSurSupXmax;
  if ( Seuil < SEUIL_MIN_DE_VIOLATION_DE_BORNE ) Seuil = SEUIL_MIN_DE_VIOLATION_DE_BORNE;
  else if ( Seuil > SEUIL_MAX_DE_VIOLATION_DE_BORNE ) Seuil = SEUIL_MAX_DE_VIOLATION_DE_BORNE;
	SeuilDeViolationDeBorne[i] = Seuil;  
}

/* Scaling vertical de A */
for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) {		     
  il    = Mdeb[i];
  ilMax = il + NbTerm[i]; 
  while ( il < ilMax ) {
   A[il] *= ScaleX[Indcol[il]];
   il++;
  }
}  

for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) Csv[i] *= ScaleLigneDesCouts;

for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) {
  X = ScaleB[i];
  /* Scaling de B */
  B[i] *= X;
  if ( B[i] != 0.0 ) {		
	  MoyenneB+= fabs( B[i] );
		Nmoy++;
	}	
  /* Scaling de A */
  il    = Mdeb[i];
  ilMax = il + NbTerm[i]; 
  while ( il < ilMax ) {
   A[il] *= X;
   il++;
  }
}

Spx->ValeurMoyenneDuSecondMembre = MoyenneB / (Nmoy+1.e-6); /* Pour eviter les divisions pas 0 */

SeuilDAmissibiliteDualeFonctionDuScaling = SEUIL_ADMISSIBILITE_DUALE_FONCTION_DU_SCALING;
if ( SeuilDAmissibiliteDualeFonctionDuScaling == OUI_SPX ) {
  Smin1 = COEFF_MIN_SEUIL_DUAL  * SEUIL_ADMISSIBILITE_DUALE_1;
	Smax1 = COEFF_MAX_SEUIL_DUAL  * SEUIL_ADMISSIBILITE_DUALE_1;	
	Smin2 = COEFF_MIN_SEUIL_DUAL  * SEUIL_ADMISSIBILITE_DUALE_2;	
	Smax2 = COEFF_MAX_SEUIL_DUAL  * SEUIL_ADMISSIBILITE_DUALE_2;
	
	SeuilDAmissibiliteDuale1 = Spx->SeuilDAmissibiliteDuale1;
	SeuilDAmissibiliteDuale2 = Spx->SeuilDAmissibiliteDuale2;
  for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) {
	  X = SEUIL_ADMISSIBILITE_DUALE_1 * ScaleLigneDesCouts * ScaleX[i];
		if ( X < Smin1 ) {
		  /*printf("Ecretage %e -> %e\n",X,Smin1);*/
			X = Smin1;
		}
		else if ( X > Smax1 ) {
		  /*printf("Ecretage %e -> %e\n",X,Smax1);*/ 
			X = Smax1;
		}
    SeuilDAmissibiliteDuale1[i] = X;
		/* printf("SeuilDAmissibiliteDuale1[%d] = %e\n",i,SeuilDAmissibiliteDuale1[i]); */
	  X = SEUIL_ADMISSIBILITE_DUALE_2 * ScaleLigneDesCouts * ScaleX[i];		
		if ( X < Smin2 ) {
		  /*printf("Ecretage %e -> %e\n",X,Smin2 );*/  
			X = Smin2;
		}
		else if ( X > Smax2 ) {
		  /*printf("Ecretage %e -> %e\n",X,Smax2);*/ 
			X = Smax2;
		}
    SeuilDAmissibiliteDuale2[i] = X;
		/* printf("SeuilDAmissibiliteDuale2[%d] = %e\n",i,SeuilDAmissibiliteDuale2[i]); */
  }
}

SeuilDeViolationdeBorneFonctionDeLaMatrice = SEUIL_ADMISSIBILITE_FONCTION_DE_LA_MATRICE;
InfScaleBSurA = NULL;
if ( SeuilDeViolationdeBorneFonctionDeLaMatrice == OUI_SPX ) {
  InfScaleBSurA = (double *) malloc( Spx->NombreDeVariables * sizeof( double ) );
	if ( InfScaleBSurA == NULL ) SeuilDeViolationdeBorneFonctionDeLaMatrice = NON_SPX;
	else {
	  for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) InfScaleBSurA[Var] = LINFINI_SPX;
	}
}

PlusGrandTerme = -1.;  
PlusPetitTerme = LINFINI_SPX;
if ( SeuilDeViolationdeBorneFonctionDeLaMatrice == OUI_SPX ) {
  for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) {
	  ScB = ScaleB[i];
    il    = Mdeb[i]; 
    ilMax = il + NbTerm[i]; 	
    while ( il < ilMax ) {
      X = fabs( A[il] );
      if( X > ZERO ) {
        if ( X > PlusGrandTerme ) PlusGrandTerme = X;
			  if ( X < PlusPetitTerme ) PlusPetitTerme = X;
			  X = ScB / X;
			  Var = Indcol[il];
        if ( X < InfScaleBSurA[Var] ) InfScaleBSurA[Var] = X;				
      }
      il++;
    }    
  }
	for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
	  Seuil = SEUIL_DE_VIOLATION_DE_BORNE * InfScaleBSurA[Var];
    if ( Seuil < SEUIL_MIN_DE_VIOLATION_DE_BORNE ) Seuil = SEUIL_MIN_DE_VIOLATION_DE_BORNE;
    else if ( Seuil > SEUIL_MAX_DE_VIOLATION_DE_BORNE ) Seuil = SEUIL_MAX_DE_VIOLATION_DE_BORNE;
	  SeuilDeViolationDeBorne[Var] = Seuil;
  }	
  free( InfScaleBSurA );	
}
else {
  for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) {
    il    = Mdeb[i]; 
    ilMax = il + NbTerm[i]; 	
    while ( il < ilMax ) {
      X = fabs( A[il] );
      if( X > ZERO ) {
        if ( X > PlusGrandTerme ) PlusGrandTerme = X;
			  if ( X < PlusPetitTerme ) PlusPetitTerme = X;
      }
      il++;
    }    
  }
}
Spx->PlusGrandTermeDeLaMatrice = PlusGrandTerme;
Spx->PlusPetitTermeDeLaMatrice = PlusPetitTerme;
Spx->RapportDeScaling = PlusGrandTerme/PlusPetitTerme;

#if VERBOSE_SPX_SCALING == 1
 
/* Verification */

printf("Verification apres application effective du scaling:\n");

PlusGrandTerme = -1.;  
PlusPetitTerme = LINFINI_SPX;	
for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) {
  il    = Spx->Mdeb[i]; 
  ilMax = il + Spx->NbTerm[i]; 
  while ( il < ilMax ) {
    X = fabs( Spx->A[il] );
    if( X != 0.0 ) {
      if ( X > PlusGrandTerme ) PlusGrandTerme = X;
      if ( X < PlusPetitTerme ) PlusPetitTerme = X;
    }
    il++;
  }    
}

Rapport = PlusGrandTerme/PlusPetitTerme;
printf("  Matrice: PlusPetitTerme %e  PlusGrandTerme %e  Rapport %e\n",PlusPetitTerme,PlusGrandTerme,Rapport);

PlusGrandTerme = -1.;
PlusPetitTerme = LINFINI_SPX;
for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) {
  X = fabs( Spx->Csv[i] );
  if( X != 0.0 ) {
    if ( X > PlusGrandTerme ) PlusGrandTerme = X;
    if ( X < PlusPetitTerme ) PlusPetitTerme = X; 
  }
}

printf("  ScaleLigneDesCouts: %e\n",Spx->ScaleLigneDesCouts);
Rapport = PlusGrandTerme/PlusPetitTerme;
printf("  Couts: PlusPetitTerme %e  PlusGrandTerme %e  Rapport %e \n",PlusPetitTerme,PlusGrandTerme,Rapport);

#endif

return;
}

/*------------------------------------------------------------------------*/
/*                Cas particulier pour les variables entieres             */	

void SPX_AjusterTolerancesVariablesEntieres( PROBLEME_SPX * Spx )
{
PROBLEME_PNE * Pne; int NombreDeVariablesEntieres; int * NumerosDesVariablesEntieres;
int * CorrespondanceVarEntreeVarSimplexe; double * SeuilDeFractionnalite;
int VarPne; int i; int Var; double * SeuilDeViolationDeBorne; int ic; int icMax;
double InfScaleBSurAmax; int * NumeroDeContrainte;
int * Cdeb; int * CNbTerm; double * ACol; double X; double * ScaleB; double Seuil;
int NombreDeContraintesDuProblemeSansCoupes;

return; /* Pas evident que ce soit une bonne idee */

if ( SEUIL_ADMISSIBILITE_FONCTION_DE_LA_MATRICE == NON_PNE ) return;
if ( Spx->ToleranceSurLesVariablesEntieresAjustees == OUI_SPX ) return;
/* Cas particulier pour les variables entieres */	
Pne = (PROBLEME_PNE *) Spx->ProblemePneDeSpx;
if ( Pne == NULL ) return;
Spx->ToleranceSurLesVariablesEntieresAjustees = OUI_SPX;
NombreDeVariablesEntieres = Pne->NombreDeVariablesEntieresTrav;
if ( NombreDeVariablesEntieres <= 0 ) return;
SeuilDeViolationDeBorne = Spx->SeuilDeViolationDeBorne;
NumerosDesVariablesEntieres = Pne->NumerosDesVariablesEntieresTrav;
CorrespondanceVarEntreeVarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe;
SeuilDeFractionnalite = Pne->SeuilDeFractionnalite;
ScaleB = Spx->ScaleB;
Cdeb = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;
ACol = Spx->ACol;
NumeroDeContrainte = Spx->NumeroDeContrainte;
NombreDeContraintesDuProblemeSansCoupes = Spx->NombreDeContraintesDuProblemeSansCoupes;

for ( i = 0 ; i < NombreDeVariablesEntieres ; i++ ) {
  VarPne = NumerosDesVariablesEntieres[i];
  Var = CorrespondanceVarEntreeVarSimplexe[VarPne];
  if ( Var >= 0 ) {
    InfScaleBSurAmax = LINFINI_SPX;
    ic = Cdeb[Var]; 
    icMax = ic + CNbTerm[Var]; 	
    while ( ic < icMax ) {
      X = fabs( ACol[ic] );
      if( X > ZERO && NumeroDeContrainte[ic] < NombreDeContraintesDuProblemeSansCoupes ) {
			  X = ScaleB[NumeroDeContrainte[ic]] / X;
        if ( X < InfScaleBSurAmax ) InfScaleBSurAmax = X;				
			}
		  ic++;
    }		
    Seuil = SeuilDeFractionnalite[VarPne] * InfScaleBSurAmax;
		/*printf("%d Seuil %e  SeuilDeFractionnalite %e  InfScaleBSurAmax %e\n",Var,Seuil,SeuilDeFractionnalite[VarPne],InfScaleBSurAmax);*/
		/*if ( Seuil > SEUIL_DE_VIOLATION_DE_BORNE ) Seuil = SEUIL_DE_VIOLATION_DE_BORNE;*/		
	  if ( Seuil < 1.e-3 * SEUIL_DE_VIOLATION_DE_BORNE ) Seuil = 1.e-3 * SEUIL_DE_VIOLATION_DE_BORNE;    
		SeuilDeViolationDeBorne[Var] = Seuil;
  }
}		

return;
}

/*------------------------------------------------------------------------*/
/*                         UnScaling                                      */ 

void SPX_UnScaling( PROBLEME_SPX * Spx )
{
int i; double * X; double * ScaleX; 

if ( Spx->FaireDuScalingSPX == NON_SPX ) return;

X       = Spx->X;
ScaleX  = Spx->ScaleX;

/* Quand on fait l'unscaling, on corrige les toutes petites valeurs de X en placant X
   sur la borne lorsqu'elle est hors base, car apres l'unscaling il se peut qu'on viole
	 des contraintes */
for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) X[i] *= ScaleX[i];

return;
}


  
