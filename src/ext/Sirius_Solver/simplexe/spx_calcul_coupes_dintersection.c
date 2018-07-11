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

   FONCTION: Calcul des coupes d'intersection.
                
   AUTEUR: R. GONZALEZ

************************************************************************/
# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "pne_define.h"

# ifdef SPX_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "spx_memoire.h"
# endif

# define ZERO_DE_NORME_AU_CARRE      1.e-15 /* 1.e-10  c'est a dire (1.e-5 * 1.e-5) */
# define SEUIL_DE_REDUCTION_AU_CARRE 0.81   /* 0.64 c'est a dire (0.8 * 0.8) */  
# define DELTAOPT_MIN  1.e-6  /*1.e-5 */
# define DELTAOPT_MAX  1.e+2  /*1.e+2*/ 
# define NOMBRE_MAX_DE_CYCLES 10 /*5*/ /*10*/

/*----------------------------------------------------------------------------*/

void SPX_InitCoupesDIntersection( PROBLEME_SPX * Spx , char * LaVariableSpxEstEntiere ,
                                  double * B )
{
int VarSpx; int Cnt; 
DONNEES_POUR_COUPES_DINTERSECTION * DonneesPourCoupesDIntersection;

Spx->DonneesPourCoupesDIntersection = (DONNEES_POUR_COUPES_DINTERSECTION *) malloc( sizeof( DONNEES_POUR_COUPES_DINTERSECTION ) );
if ( Spx->DonneesPourCoupesDIntersection == NULL ) {
  Spx->CoupesDintersectionAllouees = NON_SPX;
  return;      
}

DonneesPourCoupesDIntersection = Spx->DonneesPourCoupesDIntersection;

DonneesPourCoupesDIntersection->NombreDeVariables      = 0;
DonneesPourCoupesDIntersection->LigneDeProduitScalaire = NULL;
DonneesPourCoupesDIntersection->LigneDeMatrice         = NULL;
DonneesPourCoupesDIntersection->NormeAvantReduction    = NULL;

DonneesPourCoupesDIntersection->Vecteur                 = (double *) malloc( Spx->NombreDeVariables   * sizeof( double ) );
DonneesPourCoupesDIntersection->LaVariableSpxEstEntiere = (char   *) malloc( Spx->NombreDeVariables   * sizeof( char   ) );
DonneesPourCoupesDIntersection->B                       = (double *) malloc( Spx->NombreDeContraintes * sizeof( double ) );
DonneesPourCoupesDIntersection->XmaxSv                  = (double *) malloc( Spx->NombreDeVariables   * sizeof( double ) );  
if ( DonneesPourCoupesDIntersection->Vecteur                 == NULL || /*DonneesPourCoupesDIntersection->T == NULL ||*/
     DonneesPourCoupesDIntersection->LaVariableSpxEstEntiere == NULL || DonneesPourCoupesDIntersection->B == NULL ||
     DonneesPourCoupesDIntersection->XmaxSv                  == NULL ) {
  free( DonneesPourCoupesDIntersection->Vecteur );
  free( DonneesPourCoupesDIntersection->LaVariableSpxEstEntiere );
  free( DonneesPourCoupesDIntersection->B ); 
  free( DonneesPourCoupesDIntersection->XmaxSv ); 
  Spx->CoupesDintersectionAllouees = NON_SPX;
  return;
}

for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
  DonneesPourCoupesDIntersection->LaVariableSpxEstEntiere[VarSpx] = LaVariableSpxEstEntiere[VarSpx];
}

for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  DonneesPourCoupesDIntersection->B[Cnt] = B[Cnt];
}

DonneesPourCoupesDIntersection->TSpx     = (char *)   malloc( Spx->NombreDeVariables   * sizeof( char   ) );
DonneesPourCoupesDIntersection->CoeffSpx = (double *) malloc( Spx->NombreDeVariables   * sizeof( double ) ); 
if ( DonneesPourCoupesDIntersection->TSpx == NULL || DonneesPourCoupesDIntersection->CoeffSpx == NULL ) {
  free( DonneesPourCoupesDIntersection->Vecteur );
  free( DonneesPourCoupesDIntersection->LaVariableSpxEstEntiere );
  free( DonneesPourCoupesDIntersection->B ); 
  free( DonneesPourCoupesDIntersection->XmaxSv );
  free( DonneesPourCoupesDIntersection->TSpx );
  free( DonneesPourCoupesDIntersection->CoeffSpx ); 
  Spx->CoupesDintersectionAllouees = NON_SPX;
  return;
}

Spx->CoupesDintersectionAllouees = OUI_SPX;

return;
}

/*----------------------------------------------------------------------------*/

void SPX_TerminerLeCalculDesCoupesDIntersection( PROBLEME_SPX * Spx )
{
int i; int VarSpx; DONNEES_POUR_COUPES_DINTERSECTION * DonneesPourCoupesDIntersection;
LIGNE_DE_MATRICE * LigneDeMatrice;
LIGNE_DE_PRODUITS_SCALAIRES * LigneDeProduitScalaire;

if ( Spx->CoupesDintersectionAllouees == NON_SPX ) return;

DonneesPourCoupesDIntersection = Spx->DonneesPourCoupesDIntersection;
       
if ( DonneesPourCoupesDIntersection->XmaxSv != NULL ) {
  for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
    Spx->Xmax[VarSpx] = DonneesPourCoupesDIntersection->XmaxSv[VarSpx];  
  }
}

for ( i = 0 ; i < DonneesPourCoupesDIntersection->NombreDeVariables ; i++ ) {
  LigneDeMatrice = DonneesPourCoupesDIntersection->LigneDeMatrice[i];
  free( LigneDeMatrice->NumeroDeVariableSpx );
  free( LigneDeMatrice->Coefficient );
  free( LigneDeMatrice );    
  LigneDeProduitScalaire = DonneesPourCoupesDIntersection->LigneDeProduitScalaire[i];
  free( LigneDeProduitScalaire->NumeroDeVariableMatrice );
  free( LigneDeProduitScalaire->ProduitScalaire );
  free( LigneDeProduitScalaire );
}

free( DonneesPourCoupesDIntersection->Vecteur );
free( DonneesPourCoupesDIntersection->LaVariableSpxEstEntiere );
free( DonneesPourCoupesDIntersection->B );
free( DonneesPourCoupesDIntersection->LigneDeProduitScalaire );
free( DonneesPourCoupesDIntersection->LigneDeMatrice );
free( DonneesPourCoupesDIntersection->NormeAvantReduction );
free( DonneesPourCoupesDIntersection->XmaxSv );
free( DonneesPourCoupesDIntersection->TSpx );
free( DonneesPourCoupesDIntersection->CoeffSpx );
free( DonneesPourCoupesDIntersection );

Spx->CoupesDintersectionAllouees = NON_SPX;

return;
}

/*----------------------------------------------------------------------------*/
 
void SPX_AllocLignePourCoupesDIntersection( PROBLEME_SPX * Spx )
{
int i; DONNEES_POUR_COUPES_DINTERSECTION * DonneesPourCoupesDIntersection;

if ( Spx->CoupesDintersectionAllouees == NON_SPX ) return;

DonneesPourCoupesDIntersection = Spx->DonneesPourCoupesDIntersection;

i = DonneesPourCoupesDIntersection->NombreDeVariables;
if ( i <= 0 ) {
  DonneesPourCoupesDIntersection->LigneDeProduitScalaire = (LIGNE_DE_PRODUITS_SCALAIRES **) malloc( (i+1) * sizeof( void * ) );
  DonneesPourCoupesDIntersection->LigneDeMatrice         = (LIGNE_DE_MATRICE **)            malloc( (i+1) * sizeof( void * ) );
  DonneesPourCoupesDIntersection->NormeAvantReduction    = (double *)                       malloc( (i+1) * sizeof( double ) );
}
else {
  DonneesPourCoupesDIntersection->LigneDeProduitScalaire =
    (LIGNE_DE_PRODUITS_SCALAIRES **) realloc( DonneesPourCoupesDIntersection->LigneDeProduitScalaire , (i+1) * sizeof( void * ) );
  DonneesPourCoupesDIntersection->LigneDeMatrice =
    (LIGNE_DE_MATRICE **) realloc( DonneesPourCoupesDIntersection->LigneDeMatrice , (i+1) * sizeof( void * ) );
  DonneesPourCoupesDIntersection->NormeAvantReduction = 
    (double *) realloc( DonneesPourCoupesDIntersection->NormeAvantReduction , (i+1) * sizeof( double ) );
}
if ( DonneesPourCoupesDIntersection->LigneDeProduitScalaire == NULL ||
     DonneesPourCoupesDIntersection->LigneDeMatrice         == NULL ||
     DonneesPourCoupesDIntersection->NormeAvantReduction    == NULL ) {
  free( DonneesPourCoupesDIntersection->LigneDeProduitScalaire );
  free( DonneesPourCoupesDIntersection->LigneDeMatrice );
  free( DonneesPourCoupesDIntersection->NormeAvantReduction );
  Spx->CoupesDintersectionAllouees = NON_SPX;
  return;
}

DonneesPourCoupesDIntersection->LigneDeProduitScalaire[i] = (LIGNE_DE_PRODUITS_SCALAIRES *) malloc( sizeof( LIGNE_DE_PRODUITS_SCALAIRES ) );
DonneesPourCoupesDIntersection->LigneDeMatrice[i]         = (LIGNE_DE_MATRICE *)            malloc( sizeof( LIGNE_DE_MATRICE ) );
if ( DonneesPourCoupesDIntersection->LigneDeProduitScalaire[i] == NULL || DonneesPourCoupesDIntersection->LigneDeMatrice[i] == NULL ) {
  free( DonneesPourCoupesDIntersection->LigneDeProduitScalaire[i] );
  free( DonneesPourCoupesDIntersection->LigneDeMatrice[i] );
  Spx->CoupesDintersectionAllouees = NON_SPX;
  return;
}

DonneesPourCoupesDIntersection->LigneDeProduitScalaire[i]->NombreDeTermes          = 0;
DonneesPourCoupesDIntersection->LigneDeProduitScalaire[i]->NumeroDeVariableMatrice = NULL;
DonneesPourCoupesDIntersection->LigneDeProduitScalaire[i]->ProduitScalaire         = 0;

DonneesPourCoupesDIntersection->LigneDeMatrice[i]->NombreDeTermes      = 0;
DonneesPourCoupesDIntersection->LigneDeMatrice[i]->NumeroDeVariableSpx = NULL;
DonneesPourCoupesDIntersection->LigneDeMatrice[i]->Coefficient         = NULL;

return;
}

/*----------------------------------------------------------------------------*/

void SPX_MatriceCoupesDIntersection( PROBLEME_SPX * Spx )				     
{
char * T; double * Coeff; double SecondMembre; int NbVarCont;
int i; int VarSpx; int NbVar; int NbTermes; double X; char OnStocke;
int * NumeroDeVariableSpx; double * Coefficient; 
DONNEES_POUR_COUPES_DINTERSECTION * DonneesPourCoupesDIntersection;
LIGNE_DE_MATRICE * LigneDeMatrice; int il; double * Vecteur;
LIGNE_DE_PRODUITS_SCALAIRES * LigneDeProduitScalaire; char * LaVariableSpxEstEntiere;

if ( Spx->CoupesDintersectionAllouees == NON_SPX ) return;

DonneesPourCoupesDIntersection = Spx->DonneesPourCoupesDIntersection;

SecondMembre = DonneesPourCoupesDIntersection->AlphaI0;
/* T et Coeff sont mis a jour par le simplexe */
T     = DonneesPourCoupesDIntersection->TSpx;
Coeff = DonneesPourCoupesDIntersection->CoeffSpx;

LaVariableSpxEstEntiere = DonneesPourCoupesDIntersection->LaVariableSpxEstEntiere;

/* Decompte prealable du nombre de termes */
NbVarCont = 0;
NbTermes = 0;
OnStocke = NON_SPX;
for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
  if ( T[VarSpx] != 0 ) {
    if ( LaVariableSpxEstEntiere[VarSpx] == NON_SPX ) {
      OnStocke = OUI_SPX;
			NbVarCont++;
    }
    NbTermes++;              
  }
}
if ( OnStocke == NON_SPX ) return;
if ( NbVarCont < (int) ceil( 0.01 * NbTermes ) ) return;

SPX_AllocLignePourCoupesDIntersection( Spx );
if ( Spx->CoupesDintersectionAllouees == NON_SPX ) return;

NbVar          = DonneesPourCoupesDIntersection->NombreDeVariables;
Vecteur        = DonneesPourCoupesDIntersection->Vecteur;
LigneDeMatrice = DonneesPourCoupesDIntersection->LigneDeMatrice[NbVar];

memset( (char *) Vecteur, 0, Spx->NombreDeVariables * sizeof( double ) );

LigneDeMatrice->SecondMembre = SecondMembre;
LigneDeMatrice->NombreDeTermes = NbTermes;

LigneDeMatrice->Coefficient         = (double *) malloc( LigneDeMatrice->NombreDeTermes * sizeof( double ) );
LigneDeMatrice->NumeroDeVariableSpx = (int *)   malloc( LigneDeMatrice->NombreDeTermes * sizeof( int   ) );
if ( LigneDeMatrice->Coefficient == NULL || LigneDeMatrice->NumeroDeVariableSpx == NULL ) {
  free( LigneDeMatrice->Coefficient );
  free( LigneDeMatrice->NumeroDeVariableSpx );
  Spx->CoupesDintersectionAllouees = NON_SPX;
  return;
}

NbTermes = 0;
NumeroDeVariableSpx = LigneDeMatrice->NumeroDeVariableSpx;
Coefficient         = LigneDeMatrice->Coefficient;
for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
  if ( T[VarSpx] != 0 ) {
    NumeroDeVariableSpx[NbTermes] = VarSpx;      
    Coefficient[NbTermes]         = Coeff[VarSpx];
    if ( LaVariableSpxEstEntiere[VarSpx] == NON_SPX ) {
      Vecteur[VarSpx] = Coeff[VarSpx];
    }
    NbTermes++;              
  }
}

DonneesPourCoupesDIntersection->NombreDeVariables++;
 
/* On fait les produits scalaires (sur les variables continues) avec les lignes qui existent deja */

for ( i = 0 ; i < DonneesPourCoupesDIntersection->NombreDeVariables ; i++ ) {
  LigneDeMatrice = DonneesPourCoupesDIntersection->LigneDeMatrice[i];
  NumeroDeVariableSpx = LigneDeMatrice->NumeroDeVariableSpx;
  Coefficient         = LigneDeMatrice->Coefficient;	
  X = 0.0;
  for ( il = 0 ; il < LigneDeMatrice->NombreDeTermes ; il++ ) {
    /* remarque: Vecteur[variable entiere] = 0 */
    if ( LaVariableSpxEstEntiere[NumeroDeVariableSpx[il]] == OUI_SPX ) continue;	
    X+= Coefficient[il] * Vecteur[NumeroDeVariableSpx[il]];
  }
  
  LigneDeProduitScalaire = DonneesPourCoupesDIntersection->LigneDeProduitScalaire[i];
  
  if ( LigneDeProduitScalaire->NombreDeTermes == 0 ) {
    LigneDeProduitScalaire->ProduitScalaire         = (double *) malloc( (LigneDeProduitScalaire->NombreDeTermes+1) * sizeof( double ) );
    LigneDeProduitScalaire->NumeroDeVariableMatrice = (int *)   malloc( (LigneDeProduitScalaire->NombreDeTermes+1) * sizeof( int   ) );
  }
  else {
    LigneDeProduitScalaire->ProduitScalaire =
      (double *) realloc( LigneDeProduitScalaire->ProduitScalaire , (LigneDeProduitScalaire->NombreDeTermes+1) * sizeof( double ) );      
    LigneDeProduitScalaire->NumeroDeVariableMatrice =
      (int *) realloc( LigneDeProduitScalaire->NumeroDeVariableMatrice , (LigneDeProduitScalaire->NombreDeTermes+1) * sizeof( int ) );      
  }
  if ( LigneDeProduitScalaire->ProduitScalaire     == NULL || LigneDeProduitScalaire->NumeroDeVariableMatrice == NULL ) {
    free( LigneDeProduitScalaire->ProduitScalaire );
    free( LigneDeProduitScalaire->NumeroDeVariableMatrice );
    Spx->CoupesDintersectionAllouees = NON_SPX;
    return;
  }
    
  LigneDeProduitScalaire->ProduitScalaire        [LigneDeProduitScalaire->NombreDeTermes] = X;
  LigneDeProduitScalaire->NumeroDeVariableMatrice[LigneDeProduitScalaire->NombreDeTermes] = NbVar;
  LigneDeProduitScalaire->NombreDeTermes++;
  
}


i = DonneesPourCoupesDIntersection->NombreDeVariables - 1;
LigneDeProduitScalaire = DonneesPourCoupesDIntersection->LigneDeProduitScalaire[i];
DonneesPourCoupesDIntersection->NormeAvantReduction[i] = LigneDeProduitScalaire->ProduitScalaire[0];

return;
}

/*----------------------------------------------------------------------------*/

double SPX_CalculDeLaNouvelleNorme( LIGNE_DE_PRODUITS_SCALAIRES * LigneDeProduitScalaireDeDk,
                                    LIGNE_DE_PRODUITS_SCALAIRES * LigneDeProduitScalaireDeDl,
																		int VariableKDansMatrice, int VariableLDansMatrice ,
																		double DeltaOpt )
{
double NormeAuCarre; double DkT_Dl; int il;

/* C'est dk^t dk + Delta dk^t dl */
/* Recherche du produit dk^t dl */
DkT_Dl = 0.0;	
if ( VariableLDansMatrice >= VariableKDansMatrice ) {
  /* Si VariableLDansMatrice >= VariableKDansMatrice on cherche DkT_Dl en balayant LigneDeProduitScalaireDeDk */
  for ( il = 0 ; il < LigneDeProduitScalaireDeDk->NombreDeTermes ; il++ ) {
    if ( LigneDeProduitScalaireDeDk->NumeroDeVariableMatrice[il] == VariableLDansMatrice ) { 
      DkT_Dl = LigneDeProduitScalaireDeDk->ProduitScalaire[il];			
      break;
    }
  }
}
else {
  /* Si VariableLDansMatrice < VariableKDansMatrice on cherche DkT_Dl en balayant LigneDeProduitScalaireDeDl */
  for ( il = 0 ; il < LigneDeProduitScalaireDeDl->NombreDeTermes ; il++ ) {
    if ( LigneDeProduitScalaireDeDl->NumeroDeVariableMatrice[il] == VariableKDansMatrice ) { 
      DkT_Dl = LigneDeProduitScalaireDeDl->ProduitScalaire[il];			
      break;
    }
  }
}

NormeAuCarre = LigneDeProduitScalaireDeDk->ProduitScalaire[0] + ( 2 * DeltaOpt * DkT_Dl );
NormeAuCarre+= DeltaOpt * DeltaOpt * LigneDeProduitScalaireDeDl->ProduitScalaire[0];
if ( NormeAuCarre < ZERO_DE_NORME_AU_CARRE ) NormeAuCarre = 0.0;

return( NormeAuCarre );
}

/*----------------------------------------------------------------------------*/  
																			
void SPX_MajMatricePourCoupesDIntersection( PROBLEME_SPX * Spx, int VariableKDansMatrice, int VariableLDansMatrice,
                                            double DeltaOpt, double NouvelleNormeDeDkAuCarre )
 
{
int i; int il; int ilDi; double DlT_Di; int VarSpx; int NbFillIn; 
LIGNE_DE_PRODUITS_SCALAIRES * LigneDeProduitScalaireDeDi; LIGNE_DE_PRODUITS_SCALAIRES * LigneDeProduitScalaireDeDk; 
LIGNE_DE_PRODUITS_SCALAIRES * LigneDeProduitScalaireDeDl; int VariableIDansMatrice; 
LIGNE_DE_MATRICE * LigneDeMatriceDk; LIGNE_DE_MATRICE * LigneDeMatriceDl;
DONNEES_POUR_COUPES_DINTERSECTION * DonneesPourCoupesDIntersection;

LIGNE_DE_PRODUITS_SCALAIRES ** LigneDeProduitScalaire; 
char * T; double * Vecteur;

if ( Spx->CoupesDintersectionAllouees == NON_SPX ) return;

DonneesPourCoupesDIntersection = Spx->DonneesPourCoupesDIntersection;

LigneDeProduitScalaire = DonneesPourCoupesDIntersection->LigneDeProduitScalaire;
LigneDeProduitScalaireDeDk = LigneDeProduitScalaire[VariableKDansMatrice];
LigneDeProduitScalaireDeDl = LigneDeProduitScalaire[VariableLDansMatrice];

/* 1- Les produits scalaires */
/* dkT di = dkT di + DeltaOpt dlT di */
for ( i = 0 ; i < VariableKDansMatrice ; i++ ) {
  LigneDeProduitScalaireDeDi = LigneDeProduitScalaire[i];
	VariableIDansMatrice = i;	
  DlT_Di = 0.0;	
  if ( VariableLDansMatrice >= VariableIDansMatrice ) {
	  /* Si VariableLDansMatrice >=  VariableIDansMatrice on cherche DlT_Di en balayant LigneDeProduitScalaireDeDi */
    for ( il = 0 ; il < LigneDeProduitScalaireDeDi->NombreDeTermes ; il++ ) {
      if ( LigneDeProduitScalaireDeDi->NumeroDeVariableMatrice[il] == VariableLDansMatrice ) { 
        DlT_Di = LigneDeProduitScalaireDeDi->ProduitScalaire[il];			
        break;
      }
    }
	}
	else if ( VariableLDansMatrice < VariableIDansMatrice ) {
	  /* Si VariableLDansMatrice <  VariableIDansMatrice on cherche DlT_Di en balayant LigneDeProduitScalaireDeDl */
    for ( il = 0 ; il < LigneDeProduitScalaireDeDl->NombreDeTermes ; il++ ) {
      if ( LigneDeProduitScalaireDeDl->NumeroDeVariableMatrice[il] == VariableIDansMatrice ) { 
        DlT_Di = LigneDeProduitScalaireDeDl->ProduitScalaire[il];			
        break;
      }
    }
	}
  for ( il = 0 ; il < LigneDeProduitScalaireDeDi->NombreDeTermes ; il++ ) {   
    if ( LigneDeProduitScalaireDeDi->NumeroDeVariableMatrice[il] == VariableKDansMatrice ) {          
      LigneDeProduitScalaireDeDi->ProduitScalaire[il] += DeltaOpt * DlT_Di;
      break;
    }
  }
}

/* On complete sur la ligne k */
LigneDeProduitScalaireDeDk->ProduitScalaire[0] = NouvelleNormeDeDkAuCarre;
for ( il = 1 ; il < LigneDeProduitScalaireDeDk->NombreDeTermes ; il++ ) {
	VariableIDansMatrice = LigneDeProduitScalaireDeDk->NumeroDeVariableMatrice[il];
  DlT_Di = 0.0;	
  if ( VariableLDansMatrice >= VariableIDansMatrice ) {
	  /* Si VariableLDansMatrice >  VariableIDansMatrice on cherche DlT_Di en balayant LigneDeProduitScalaireDeDi */
    LigneDeProduitScalaireDeDi = LigneDeProduitScalaire[VariableIDansMatrice];		
    for ( ilDi = 0 ; ilDi < LigneDeProduitScalaireDeDi->NombreDeTermes ; ilDi++ ) {
      if ( LigneDeProduitScalaireDeDi->NumeroDeVariableMatrice[ilDi] == VariableLDansMatrice ) { 
        DlT_Di = LigneDeProduitScalaireDeDi->ProduitScalaire[ilDi];			
        break;
      }
    }	
	}
	else {
	  /* Si VariableLDansMatrice < VariableIDansMatrice on cherche DlT_Di en balayant LigneDeProduitScalaireDeDl */	
    for ( i = 0 ; i < LigneDeProduitScalaireDeDl->NombreDeTermes ; i++ ) {
      if ( LigneDeProduitScalaireDeDl->NumeroDeVariableMatrice[i] == VariableIDansMatrice ) { 
        DlT_Di = LigneDeProduitScalaireDeDl->ProduitScalaire[i];
        break;
      }
    }
	}	
  LigneDeProduitScalaireDeDk->ProduitScalaire[il] += DeltaOpt * DlT_Di;
}

T       = DonneesPourCoupesDIntersection->TSpx;
Vecteur = DonneesPourCoupesDIntersection->Vecteur;
/* On doit faire Dk = Dk + DeltaOpt * Dl */
memset( (char *) T      , 1, Spx->NombreDeVariables * sizeof( char   ) );
memset( (char *) Vecteur, 0, Spx->NombreDeVariables * sizeof( double ) );

NbFillIn = 0;
LigneDeMatriceDk = DonneesPourCoupesDIntersection->LigneDeMatrice[VariableKDansMatrice];
for ( il = 0 ; il < LigneDeMatriceDk->NombreDeTermes ; il++ ) {
  VarSpx = LigneDeMatriceDk->NumeroDeVariableSpx[il];
  Vecteur[VarSpx] = LigneDeMatriceDk->Coefficient[il];
  T      [VarSpx] = 0;
}

LigneDeMatriceDl = DonneesPourCoupesDIntersection->LigneDeMatrice[VariableLDansMatrice];
for ( il = 0 ; il < LigneDeMatriceDl->NombreDeTermes ; il++ ) {
  VarSpx = LigneDeMatriceDl->NumeroDeVariableSpx[il];
  Vecteur[VarSpx] += DeltaOpt * LigneDeMatriceDl->Coefficient[il];
  NbFillIn+= T[VarSpx];
  T[VarSpx] = 0;
}

LigneDeMatriceDk->NombreDeTermes+= NbFillIn;

LigneDeMatriceDk->SecondMembre += DeltaOpt * LigneDeMatriceDl->SecondMembre;

if ( NbFillIn > 0 ) {
  free( LigneDeMatriceDk->Coefficient );
  free( LigneDeMatriceDk->NumeroDeVariableSpx );

  LigneDeMatriceDk->Coefficient         = (double *) malloc( LigneDeMatriceDk->NombreDeTermes * sizeof( double ) );
  LigneDeMatriceDk->NumeroDeVariableSpx = (int *)    malloc( LigneDeMatriceDk->NombreDeTermes * sizeof( int    ) );
  if ( LigneDeMatriceDk->Coefficient == NULL || LigneDeMatriceDk->NumeroDeVariableSpx == NULL ) {
    free( LigneDeMatriceDk->Coefficient );
    free( LigneDeMatriceDk->NumeroDeVariableSpx );
    Spx->CoupesDintersectionAllouees = NON_SPX;
    return;
  }
}

il = 0;
for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
  if ( T[VarSpx] == 0 ) {
    LigneDeMatriceDk->Coefficient        [il] = Vecteur[VarSpx];
    LigneDeMatriceDk->NumeroDeVariableSpx[il] = VarSpx;
    il++;
  }   
}

return;
}

/*----------------------------------------------------------------------------*/  
/* Attention, c'est la routine qui prend le plus de temps et ceci meme avec
   un petit nombre de rebouclages */
   
void SPX_ReductionDesNormesPourCoupesDIntersection( PROBLEME_SPX * Spx )
{
int i; double X; double DeltaOpt; double * Vecteur; int VarSpx; double NormeDeDkAuCarre; int ilNorme; int VariableLDansMatrice;
double NouvelleNormeAuCarre; double NormeDeDlAuCarre; double PlusGrandeReductionDeNorme; double DeltaOptChoisi;
char OnReboucle;int NbFois; int j; int VariableDansMatrice;
int VariableKDansMatriceChoisie;int	VariableLDansMatriceChoisie; double ValeurDuProduitScalaireDeDl;
					
int VariableKDansMatrice; double NouvelleNormeDeDkAuCarre;
LIGNE_DE_PRODUITS_SCALAIRES * LigneDeProduitScalaireDeDl;
LIGNE_DE_PRODUITS_SCALAIRES * LigneDeProduitScalaireDeDk;
char * LaVariableSpxEstEntiere;
DONNEES_POUR_COUPES_DINTERSECTION * DonneesPourCoupesDIntersection;

if ( Spx->CoupesDintersectionAllouees == NON_SPX ) return;

DonneesPourCoupesDIntersection = Spx->DonneesPourCoupesDIntersection;

/* Reinitialisation des Xmax: on se remet dans le contexte des Gomory */
for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
  DonneesPourCoupesDIntersection->XmaxSv[VarSpx] = Spx->Xmax[VarSpx];
  /* Le calcul de BBarre fait intervenir Xmax, il faut dont le corriger pour simuler l'absence d'instanciation */ 
  if ( DonneesPourCoupesDIntersection->LaVariableSpxEstEntiere[VarSpx] == NON_SPX ) continue; 
  /* C'est une variable entiere */
  /* La valeur 1. indique bien que ca ne marche que pour des variables binaires */
  Spx->Xmax[VarSpx] = 1. / Spx->ScaleX[VarSpx];
}

Vecteur = DonneesPourCoupesDIntersection->Vecteur;
memset( (char *) Vecteur, 0, Spx->NombreDeVariables * sizeof( double ) );

LaVariableSpxEstEntiere = DonneesPourCoupesDIntersection->LaVariableSpxEstEntiere;

OnReboucle = OUI_SPX;

NbFois = 0;

/* Pour eviter les warning de compilation */
NouvelleNormeDeDkAuCarre = 0.0; 
DeltaOptChoisi = 0.0;

while ( OnReboucle == OUI_SPX && NbFois < NOMBRE_MAX_DE_CYCLES ) {
OnReboucle = NON_SPX;

for ( i = 0 ; i < DonneesPourCoupesDIntersection->NombreDeVariables ; i++ ) {
  /* On est sur la ligne des produits scalaires Dl */
	VariableLDansMatrice = i;
  LigneDeProduitScalaireDeDl = DonneesPourCoupesDIntersection->LigneDeProduitScalaire[i];
  NormeDeDlAuCarre           = LigneDeProduitScalaireDeDl->ProduitScalaire[0];
  if ( NormeDeDlAuCarre < ZERO_DE_NORME_AU_CARRE ) continue;	

  PlusGrandeReductionDeNorme  = -LINFINI_SPX;
  VariableKDansMatriceChoisie = -1;   
  VariableLDansMatriceChoisie = -1;
  
  for ( ilNorme = 1 ; ilNorme < LigneDeProduitScalaireDeDl->NombreDeTermes ; ilNorme++ ) {

    VariableKDansMatrice       = LigneDeProduitScalaireDeDl->NumeroDeVariableMatrice[ilNorme];
		
    LigneDeProduitScalaireDeDk = DonneesPourCoupesDIntersection->LigneDeProduitScalaire[VariableKDansMatrice];
    NormeDeDkAuCarre           = LigneDeProduitScalaireDeDk->ProduitScalaire[0];
    if ( NormeDeDkAuCarre < ZERO_DE_NORME_AU_CARRE ) continue;
  
    /* Calcul de la norme au carre de Dk + DeltaOpt * Dl */
    DeltaOpt = floor( -LigneDeProduitScalaireDeDl->ProduitScalaire[ilNorme] / NormeDeDlAuCarre );						
		if ( fabs( DeltaOpt ) > DELTAOPT_MIN && fabs( DeltaOpt ) < DELTAOPT_MAX ) {
      NouvelleNormeAuCarre = SPX_CalculDeLaNouvelleNorme( LigneDeProduitScalaireDeDk, LigneDeProduitScalaireDeDl,
																		                      VariableKDansMatrice, VariableLDansMatrice, DeltaOpt );			
      if ( NouvelleNormeAuCarre/NormeDeDkAuCarre < SEUIL_DE_REDUCTION_AU_CARRE ) {
				if ( NouvelleNormeAuCarre == 0.0 ) X = LINFINI_SPX;
				else X = NormeDeDkAuCarre / ( NouvelleNormeAuCarre + 1.e-20 );	/* Pour eviter les divisions par 0 */			  
	      if ( X > PlusGrandeReductionDeNorme ) {
	        PlusGrandeReductionDeNorme  = X;
					VariableKDansMatriceChoisie = VariableKDansMatrice;
					VariableLDansMatriceChoisie = VariableLDansMatrice;
	        DeltaOptChoisi              = DeltaOpt;						
	        NouvelleNormeDeDkAuCarre    = NouvelleNormeAuCarre;
				  if ( PlusGrandeReductionDeNorme == LINFINI_SPX ) break;
				}
      }
    }
    
    /* Calcul de la norme au carre de Dk + DeltaOpt * Dl */
    DeltaOpt = ceil( -LigneDeProduitScalaireDeDl->ProduitScalaire[ilNorme] / NormeDeDlAuCarre );			
    if ( fabs( DeltaOpt ) > DELTAOPT_MIN && fabs( DeltaOpt ) < DELTAOPT_MAX ) {
      NouvelleNormeAuCarre = SPX_CalculDeLaNouvelleNorme( LigneDeProduitScalaireDeDk, LigneDeProduitScalaireDeDl,
																		                      VariableKDansMatrice, VariableLDansMatrice, DeltaOpt );		
      if ( NouvelleNormeAuCarre/NormeDeDkAuCarre < SEUIL_DE_REDUCTION_AU_CARRE ) {
				if ( NouvelleNormeAuCarre == 0.0 ) X = LINFINI_SPX;
				else X = NormeDeDkAuCarre / ( NouvelleNormeAuCarre + 1.e-20 );	/* Pour eviter les divisions par 0 */			  
	      if ( X > PlusGrandeReductionDeNorme ) {
	        PlusGrandeReductionDeNorme  = X;
					VariableKDansMatriceChoisie = VariableKDansMatrice;
					VariableLDansMatriceChoisie = VariableLDansMatrice;
	        DeltaOptChoisi              = DeltaOpt;						
	        NouvelleNormeDeDkAuCarre    = NouvelleNormeAuCarre;
				  if ( PlusGrandeReductionDeNorme == LINFINI_SPX ) break;
				}				       				
      }
    }
    
  }

	/* Examen de la partie non representee dans la matrice et qui correspond a ce qui se trouve a
	   gauche de la diagonale */
	goto FinPartieSuperieure; /* <- inhibition car ca prend du temps et ca n'améliore pas franchement */
  for ( j = 0 ; j < VariableLDansMatrice ; j++ ) {		
    VariableKDansMatrice = j;
    LigneDeProduitScalaireDeDk = DonneesPourCoupesDIntersection->LigneDeProduitScalaire[VariableKDansMatrice];
    NormeDeDkAuCarre           = LigneDeProduitScalaireDeDk->ProduitScalaire[0];
    if ( NormeDeDkAuCarre < ZERO_DE_NORME_AU_CARRE ) continue;		
    for ( ilNorme = 1 ; ilNorme < LigneDeProduitScalaireDeDk->NombreDeTermes ; ilNorme++ ) {
		  VariableDansMatrice = LigneDeProduitScalaireDeDk->NumeroDeVariableMatrice[ilNorme];
		  if ( VariableDansMatrice > VariableLDansMatrice ) break;
		  if ( VariableDansMatrice == VariableLDansMatrice ) {
			  ValeurDuProduitScalaireDeDl = LigneDeProduitScalaireDeDk->ProduitScalaire[ilNorme];
        /* Calcul de la norme au carre de Dk + DeltaOpt * Dl */
        DeltaOpt = floor( -ValeurDuProduitScalaireDeDl / NormeDeDlAuCarre );
        if ( fabs( DeltaOpt ) > DELTAOPT_MIN && fabs( DeltaOpt ) < DELTAOPT_MAX ) {
          NouvelleNormeAuCarre = SPX_CalculDeLaNouvelleNorme( LigneDeProduitScalaireDeDk, LigneDeProduitScalaireDeDl,
																		                          VariableKDansMatrice, VariableLDansMatrice, DeltaOpt );			
          if ( NouvelleNormeAuCarre/NormeDeDkAuCarre < SEUIL_DE_REDUCTION_AU_CARRE ) {
				    if ( NouvelleNormeAuCarre == 0.0 ) X = LINFINI_SPX;
				    else X = NormeDeDkAuCarre / ( NouvelleNormeAuCarre + 1.e-20 );	/* Pour eviter les divisions par 0 */			  
	          if ( X > PlusGrandeReductionDeNorme ) {
	            PlusGrandeReductionDeNorme  = X;
					    VariableKDansMatriceChoisie = VariableKDansMatrice;
					    VariableLDansMatriceChoisie = VariableLDansMatrice;
	            DeltaOptChoisi              = DeltaOpt;						
	            NouvelleNormeDeDkAuCarre    = NouvelleNormeAuCarre;
				      if ( PlusGrandeReductionDeNorme == LINFINI_SPX ) break;
				    }
          }
        }  
        /* Calcul de la norme au carre de Dk + DeltaOpt * Dl */
        DeltaOpt = ceil( -ValeurDuProduitScalaireDeDl / NormeDeDlAuCarre );
        if ( fabs( DeltaOpt ) > DELTAOPT_MIN && fabs( DeltaOpt ) < DELTAOPT_MAX ) {
          NouvelleNormeAuCarre = SPX_CalculDeLaNouvelleNorme( LigneDeProduitScalaireDeDk, LigneDeProduitScalaireDeDl,
																		                          VariableKDansMatrice, VariableLDansMatrice, DeltaOpt );		
          if ( NouvelleNormeAuCarre/NormeDeDkAuCarre < SEUIL_DE_REDUCTION_AU_CARRE ) {
				    if ( NouvelleNormeAuCarre == 0.0 ) X = LINFINI_SPX;
				    else X = NormeDeDkAuCarre / ( NouvelleNormeAuCarre + 1.e-20 );	/* Pour eviter les divisions par 0 */			  
	          if ( X > PlusGrandeReductionDeNorme ) {
	            PlusGrandeReductionDeNorme  = X;
					    VariableKDansMatriceChoisie = VariableKDansMatrice;
					    VariableLDansMatriceChoisie = VariableLDansMatrice;
	            DeltaOptChoisi              = DeltaOpt;						
	            NouvelleNormeDeDkAuCarre    = NouvelleNormeAuCarre;
				      if ( PlusGrandeReductionDeNorme == LINFINI_SPX ) break;
				    }				       				
          }
        }				
        break;
			}
		}		
  }		 	 
  FinPartieSuperieure:

  if ( VariableKDansMatriceChoisie >= 0 ) {
    OnReboucle = OUI_SPX;		
    SPX_MajMatricePourCoupesDIntersection( Spx, VariableKDansMatriceChoisie, VariableLDansMatriceChoisie, DeltaOptChoisi,
		                                       NouvelleNormeDeDkAuCarre );		
  }
   
}

if ( OnReboucle == OUI_SPX ) NbFois++;

}

return;
}

/*----------------------------------------------------------------------------*/

int SPX_NombrePotentielDeCoupesDIntersection( PROBLEME_SPX * Spx )
{
DONNEES_POUR_COUPES_DINTERSECTION * DonneesPourCoupesDIntersection;
if ( Spx->CoupesDintersectionAllouees == NON_SPX ) return( 0 );   
DonneesPourCoupesDIntersection = Spx->DonneesPourCoupesDIntersection;

return( DonneesPourCoupesDIntersection->NombreDeVariables);
}

/*----------------------------------------------------------------------------*/

void SPX_CalculerUneCoupeDIntersection( PROBLEME_SPX * Spx,
                                        int i,
			    
                                        double RapportMaxDesCoeffs,
			                                  double ZeroPourCoeffVariablesDEcart,
			                                  double ZeroPourCoeffVariablesNatives,
			                                  double RelaxRhsAbs,
			                                  double RelaxRhsRel,			  
 																				
                                        int   * NombreDeTermes, 
                                        double * Coefficient, 
                                        int   * IndiceDeLaVariable,
                                        double * SecondMembre,
																				char * OnAEcrete )
{
int il; int VarSpx; double NormeAuCarre; DONNEES_POUR_COUPES_DINTERSECTION * DonneesPourCoupesDIntersection;
LIGNE_DE_PRODUITS_SCALAIRES * LigneDeProduitScalaire; double EpsilonDePiEtPi0; 
LIGNE_DE_MATRICE * LigneDeMatrice; char * LaVariableSpxEstEntiere; double X; double DeltaOpt;
double * Coeff; char * T; double AlphaI0; double * B; char OnRenforce; double ValeurDuZero; 

*OnAEcrete = NON_SPX;
*NombreDeTermes = 0;

ValeurDuZero = ZERO_TERMES_DU_TABLEAU_POUR_GOMORY;

if ( Spx->CoupesDintersectionAllouees == NON_SPX ) return;

DonneesPourCoupesDIntersection = Spx->DonneesPourCoupesDIntersection;
LaVariableSpxEstEntiere = DonneesPourCoupesDIntersection->LaVariableSpxEstEntiere;

if ( DonneesPourCoupesDIntersection->NormeAvantReduction[i] < ZERO_DE_NORME_AU_CARRE /*1.e-9*/ ) {
  return;
}

LigneDeProduitScalaire = DonneesPourCoupesDIntersection->LigneDeProduitScalaire[i];
NormeAuCarre           = LigneDeProduitScalaire->ProduitScalaire[0];	
if ( NormeAuCarre / DonneesPourCoupesDIntersection->NormeAvantReduction[i] > SEUIL_DE_REDUCTION_AU_CARRE /*0.9*/ ) {
  return;
}

LigneDeMatrice = DonneesPourCoupesDIntersection->LigneDeMatrice[i];
/*1- On recupere la split disjuction */
EpsilonDePiEtPi0 = LigneDeMatrice->SecondMembre - floor( LigneDeMatrice->SecondMembre );

/* EpsilonDePiEtPi0 doit etre compris entre 0 et 1 */
if ( EpsilonDePiEtPi0 < 1.e-9 || EpsilonDePiEtPi0 > 0.999999999) return;

/* Les rayons extreme c'est -NBarreR ? */
/*
for ( il = 0 ; il < LigneDeMatrice->NombreDeTermes ; il++ ) {
  LigneDeMatrice->Coefficient[il] *= -1.0;
}
*/
/*2- On renforce la split disjunction */
OnRenforce = OUI_SPX; /* NON_SPX */
if ( OnRenforce == OUI_SPX ) {
  for ( il = 0 ; il < LigneDeMatrice->NombreDeTermes ; il++ ) {
	  VarSpx = LigneDeMatrice->NumeroDeVariableSpx[il];
    /* On choisi une variable entiere k hors base */
	  if ( LaVariableSpxEstEntiere[VarSpx] != OUI_SPX ) continue;
	  /* On dispose directement du coefficient du coefficient pi^t rk car il est mis a jour en cours de reduction des normes */
	  X = LigneDeMatrice->Coefficient[il];
	  if ( ceil( X ) - X > EpsilonDePiEtPi0 ) DeltaOpt = -floor( X );
	  else DeltaOpt = -ceil( X );
	  /* On ajoute la variable dans la split cut */
	  /* Cela se traduit par une modification de Coefficient (et non de Pi) */
    LigneDeMatrice->Coefficient[il] += DeltaOpt;
  }
}
/*
for ( il = 0 ; il < LigneDeMatrice->NombreDeTermes ; il++ ) {
  LigneDeMatrice->Coefficient[il] *= -1.0;
}
*/

AlphaI0 = LigneDeMatrice->SecondMembre;

Coeff = DonneesPourCoupesDIntersection->Vecteur;
T     = DonneesPourCoupesDIntersection->TSpx;
B     = DonneesPourCoupesDIntersection->B;

for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
  T    [VarSpx] = 0;
  Coeff[VarSpx] = 0.;
}

for ( il = 0 ; il < LigneDeMatrice->NombreDeTermes ; il++ ){
  VarSpx = LigneDeMatrice->NumeroDeVariableSpx[il];
  Coeff[VarSpx] = LigneDeMatrice->Coefficient[il];
  T    [VarSpx] = 1;
  /* Nettoyage des tres petites valeurs */
  if ( fabs( Coeff[VarSpx] ) < ValeurDuZero ) {
	  T[VarSpx] = 0;
    Coeff[VarSpx] = 0.;
	}	
}

/* Calcul de la MIR */
SPX_CalculMIRPourCoupeDeGomoryOuIntersection( Spx, RapportMaxDesCoeffs, ZeroPourCoeffVariablesDEcart,
                                              ZeroPourCoeffVariablesNatives, RelaxRhsAbs, RelaxRhsRel,
			                                        AlphaI0, B, T, Coeff, LaVariableSpxEstEntiere,			 
                                              NombreDeTermes, Coefficient, IndiceDeLaVariable, SecondMembre,
                                              OnAEcrete );
																								 
return;
}










