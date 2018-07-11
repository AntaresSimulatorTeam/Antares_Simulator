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

   FONCTION: Refactorisation de la matrice (l'ordre d'elimination
             a deja ete calcule).

	 ATTENTION: Routines speciales pour le point interieur.                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

# include "memstats.h"

# define TAILLE_DU_MEGA_OCTET (1024 * 1024)

/*--------------------------------------------------------------------------------------------------*/

void LU_RefactorisationScanLigne( int il, int ilMax, double ValeurTermeColonnePivot, char * T , double * W,
                                  double * ElmDeU, int * IndexKpDeUouL )
{

while ( il < ilMax ) { 
  if ( T[IndexKpDeUouL[il]] == 1 ) ElmDeU[il]-= W[IndexKpDeUouL[il]] * ValeurTermeColonnePivot;	
  il++;
}

return;
}

/*--------------------------------------------------------------------------------------------------*/    

void LU_RefactorisationScanLignePlein( int il, int ilMax, double ValeurTermeColonnePivot, double * W,
                                       double * ElmDeU , int * IndexKpDeUouL )
{

while ( il < ilMax ) { 
  ElmDeU[il]-= W[IndexKpDeUouL[il]] * ValeurTermeColonnePivot;	
  il++;
}

return;
}

/*--------------------------------------------------------------------------------------------------*/

double LU_RefactorisationControleDuPivot( MATRICE * Matrice,
                                          double   ValeurDeRegularisation,
                                          double * TermeDeRegularisation,
					                                int     ColonnePivot,
                                          int     il,
					                                char *   OnARegularise
																				) 		         
{
double * ElmDeU;

ElmDeU = Matrice->ElmDeU; 

if ( fabs( ElmDeU[il] ) < Matrice->PivotMin ) {
  /*
  if ( TermeDeRegularisation[ColonnePivot] == 0.0 ) {  
	  *OnARegularise = OUI_LU;
	  TermeDeRegularisation[ColonnePivot] = ValeurDeRegularisation;
		
 	}
  ElmDeU[il] = TermeDeRegularisation[ColonnePivot];
  */	
	
  if ( ElmDeU[il] > 0.0 ) {
	  ElmDeU[il] = ValeurDeRegularisation;
    TermeDeRegularisation[ColonnePivot] = ValeurDeRegularisation;
		*OnARegularise = OUI_LU;
 	}
  else {
	  ElmDeU[il] = -ValeurDeRegularisation;
    TermeDeRegularisation[ColonnePivot] = -ValeurDeRegularisation;
		*OnARegularise = OUI_LU;
	}  
	
}

return( ElmDeU[il] );
}

/*--------------------------------------------------------------------------------------------------*/
/*                         Matrice symetrique et pivotage diagonal                                  */
/*                         ATTENTION: special Point Interieur                                       */
 
void LU_Refactorisation( MATRICE * Matrice ,
                         double * ValeurDesTermesDeU    , int   NbElementsU            , 
		                     double * TermeDeRegularisation , double ValeurDeRegularisation ,
			                   char *   OnARegularise         , char * Erreur )
{
int il; int ilMax; int Kp; int Kpp; 
int ic; int ilDebKp; int ilMaxKp ; char CalculPlein; int PremierKpParCalculClassique;  
double ValeurTermeColonnePivot; double ValeurDuPivot; double X; 
double UnSurValeurDuPivot; int IndexAdressesKp; int IndexLignesKp; int j; int k; 
double * ptAdresseDeUHaut; double * ptAdresseDeUModifie; double * ptAdresseUGauche; 
double * W; char * T; int Rang; int * LdebParLigneDeU; int * NbTermesParLigneDeU;
int * IndiceColonneDeU; double * ElmDeU; double * ElmDeL; 
int * DebutInfosAdressesQueKpModifie; int * DebutInfosLignesQueKpModifie;
int * NombreDeLignesQueKpModifie; int * IndexKpDeUouL;
double ** AdresseUGauche; double ** AdresseUHaut; double ** AdresseDeUModifie;
int * NombreDeTermesParLigneQueKpModifie;

/*printf("   LU_Refactorisation      \n"); fflush(stdout);*/

*OnARegularise = NON_LU;
*Erreur        = NON_LU;

/* Recopie de la matrice a factoriser */
/* Triangle U */
memcpy( (char *) Matrice->ElmDeU , (char *) ValeurDesTermesDeU  , NbElementsU * sizeof( double ) );

Rang = Matrice->Rang;
LdebParLigneDeU     = Matrice->LdebParLigneDeU;
NbTermesParLigneDeU = Matrice->NbTermesParLigneDeU;
IndiceColonneDeU    = Matrice->IndiceColonneDeU;
ElmDeU              = Matrice->ElmDeU;
ElmDeL              = Matrice->ElmDeL;
IndexKpDeUouL       = Matrice->IndexKpDeUouL;

DebutInfosAdressesQueKpModifie = Matrice->DebutInfosAdressesQueKpModifie;
DebutInfosLignesQueKpModifie   = Matrice->DebutInfosLignesQueKpModifie;
NombreDeLignesQueKpModifie     = Matrice->NombreDeLignesQueKpModifie;

AdresseUGauche    = Matrice->AdresseUGauche;
AdresseUHaut      = Matrice->AdresseUHaut;
AdresseDeUModifie = Matrice->AdresseDeUModifie;

NombreDeTermesParLigneQueKpModifie = Matrice->NombreDeTermesParLigneQueKpModifie;

PremierKpParCalculClassique = Matrice->PremierKpParCalculClassique;

CalculPlein = NON_LU;

/* Factorisation */
for ( Kp = 0 ; Kp < Rang ; Kp++ ) {
  if ( Kp == PremierKpParCalculClassique ) {
	  /*printf("Refactorisation classique a partir de %d sur %d ecart %d\n",Kp,Rang,Rang-Kp);*/
	  goto CompleterLaFactorisation;
	}
  /* Transfert de la ligne dans un tableau de travail */  
  il    = LdebParLigneDeU[Kp];
  ilMax = il + NbTermesParLigneDeU[Kp];
  ilDebKp = il;
  ilMaxKp = ilMax;
  ValeurDuPivot = LU_RefactorisationControleDuPivot( Matrice , ValeurDeRegularisation, TermeDeRegularisation,
					                                           IndiceColonneDeU[il], il, OnARegularise );		          	
  UnSurValeurDuPivot = 1. / ValeurDuPivot;
  /* Stockage de l'inverse du pivot */
  ElmDeU[il] = UnSurValeurDuPivot;
  /*  */	
  /* Modification de la sous-matrice active */
  IndexAdressesKp = DebutInfosAdressesQueKpModifie[Kp];
  IndexLignesKp   = DebutInfosLignesQueKpModifie[Kp];
  for ( j = 0 ; j < NombreDeLignesQueKpModifie[Kp] ; j++ ) {
    ptAdresseUGauche = AdresseUGauche[IndexLignesKp];
    ValeurTermeColonnePivot = *ptAdresseUGauche * UnSurValeurDuPivot;
    for ( k = 0 ; k < NombreDeTermesParLigneQueKpModifie[IndexLignesKp]; k++ ) {
      ptAdresseDeUHaut    = AdresseUHaut     [IndexAdressesKp];
      ptAdresseDeUModifie = AdresseDeUModifie[IndexAdressesKp];
      X = *ptAdresseDeUHaut * ValeurTermeColonnePivot;
      *ptAdresseDeUModifie-= X;			
      IndexAdressesKp++;
    }
    IndexLignesKp++;
  }     
}
goto RecopieDuTriangleU;

CompleterLaFactorisation:
/* On complete la factorisation par la methode classique */
W = NULL;
T = NULL;
W = (double *) malloc( Rang * sizeof( double ) );
T = (char *)   malloc( Rang * sizeof( char   ) );
if ( W == NULL || T == NULL ) {
  free( W ); W = NULL; free( T ); T = NULL; 
  *Erreur = OUI_LU;
  goto FinRefactorisation;
} 
memset( T , 0 , Rang * sizeof( char ) );

for ( ; Kp < Rang ; Kp++ ) {
  /* Transfert de la ligne dans un tableau de travail */  
  il    = LdebParLigneDeU[Kp];
  ilMax = il + NbTermesParLigneDeU[Kp];
  if ( NbTermesParLigneDeU[Kp] == Rang - Kp ) CalculPlein = OUI_LU;
  ilDebKp = il;
  ilMaxKp = ilMax;
  ValeurDuPivot = LU_RefactorisationControleDuPivot( Matrice , ValeurDeRegularisation, TermeDeRegularisation,
					                                           IndiceColonneDeU[il], il, OnARegularise );	  
  UnSurValeurDuPivot = 1. / ValeurDuPivot;	
  /* Stockage de l'inverse du pivot */
  ElmDeU[il] = UnSurValeurDuPivot;	
  /*  */	
  il++; /* On ne modifie pas la colonne pivot */
	if ( CalculPlein == NON_LU ) {
    while ( il < ilMax ) {
      W[IndexKpDeUouL[il]] = ElmDeU[il];
      T[IndexKpDeUouL[il]] = 1;           
      il++;
    }
	}
	else {
    while ( il < ilMax ) {		
      W[IndexKpDeUouL[il]] = ElmDeU[il];
      il++;
    }	
	}
  /* Maintenant on balaye toutes les lignes de la matrice active qui ont un terme non nul dans la colonne 
     pivot et on les compare a la ligne pivot */     
  ic = ilDebKp;  
  ic++; /* Terme diagonal */
  while ( ic < ilMaxKp ) { 
    ValeurTermeColonnePivot = ElmDeU[ic] * UnSurValeurDuPivot;    
    /* Scanner la ligne */    
    /* Partie U en utilisant le chainage par colonne */        
    Kpp = IndexKpDeUouL[ic];           
    il    = LdebParLigneDeU[Kpp];
    ilMax = il + NbTermesParLigneDeU[Kpp];		
    /* Calcul des termes modifies */          
		if ( CalculPlein == OUI_LU ) {
      LU_RefactorisationScanLignePlein( il, ilMax, ValeurTermeColonnePivot, W, ElmDeU, IndexKpDeUouL );
		}
		else {
      LU_RefactorisationScanLigne( il, ilMax, ValeurTermeColonnePivot, T , W, ElmDeU, IndexKpDeUouL );      
		}		
    /* On passe au terme suivant de la colonne pivot */
    ic++;
  }  

	if ( CalculPlein == NON_LU ) {
    il = ilDebKp;
    while ( il < ilMaxKp ) {
      T[IndexKpDeUouL[il]] = 0;          
      il++;
    }
	}
}
free( W );
free( T );
W = NULL;
T = NULL;

RecopieDuTriangleU:
/* Recopie du triangle U dans le triangle L (car matrice symetrique ) */
for ( Kp = 0 ; Kp < Matrice->Rang ; Kp++ ) {
  il    = LdebParLigneDeU[Kp];
  ilMax = il + NbTermesParLigneDeU[Kp];
  ElmDeL[il] = 1.0;
  X = ElmDeU[il];	
  il++;
  while ( il < ilMax ) { 
    ElmDeL[il] = ElmDeU[il] * X;		
    il++;
  }
}

FinRefactorisation:

return;
}

/*--------------------------------------------------------------------------------------------------*/
/*                         Matrice symetrique et pivotage diagonal                                  */

void LU_RefactorisationSimulation( MATRICE * Matrice , char * Erreur , int NbElementsU )
{
int il; int ilMax; int Kp; int Kpp; int ic ; int ilDebKp; int iKp;
int ilMaxKp       ; int IndexLibrePourLesAdresses; int IndexLibrePourLesNombres;
double ** Adresse; char * T;
int NbElementsAllouesPourLesAdresses; int NbElementsAllouesPourLesNombres;
int NbLignesQueKpModifie; int NbTermesParLigneQueKpModifie;
int Taille; int TailleMaxEnMegaOctets; int Disponible;
int Rang;
int * NbTermesParLigneDeU; int * DebutInfosAdressesQueKpModifie; int * DebutInfosLignesQueKpModifie;
int * LdebParLigneDeU; int * IndexKpDeUouL; int * InverseOrdreLigne;
int * NombreDeTermesParLigneQueKpModifie; int * NombreDeLignesQueKpModifie; 
double * ElmDeU; double ** AdresseDeUModifie; double ** AdresseUHaut; double **  AdresseUGauche;

/*printf("   LU_RefactorisationSimulation  \n"); fflush(stdout);*/

Disponible = (int) (MemoireDisponible() / TAILLE_DU_MEGA_OCTET);

TailleMaxEnMegaOctets = Disponible >> 1;
if ( TailleMaxEnMegaOctets < 10 ) TailleMaxEnMegaOctets = 10;

*Erreur = NON_LU;

Adresse = NULL;
T       = NULL;
Adresse = (double **) malloc( Matrice->Rang * sizeof( void * ) );
T       = (char *)    malloc( Matrice->Rang * sizeof( char   ) );
if ( T == NULL ) {
  *Erreur = OUI_LU;
  goto FinRefactorisationSimulation;
} 
memset( T , 0 , Matrice->Rang * sizeof( char ) );

NbElementsAllouesPourLesAdresses = NbElementsU;
Matrice->DebutInfosAdressesQueKpModifie = (int *)    malloc( Matrice->Rang * sizeof( int ) );
Matrice->AdresseDeUModifie              = (double **) malloc( NbElementsAllouesPourLesAdresses * sizeof( void * ) );
Matrice->AdresseUHaut                   = (double **) malloc( NbElementsAllouesPourLesAdresses * sizeof( void * ) );

NbElementsAllouesPourLesNombres = Matrice->Rang;
Matrice->DebutInfosLignesQueKpModifie       = (int *) malloc( Matrice->Rang * sizeof( int ) );
Matrice->NombreDeLignesQueKpModifie         = (int *) malloc( Matrice->Rang * sizeof( int ) );
Matrice->AdresseUGauche                     = (double **) malloc( NbElementsAllouesPourLesNombres  * sizeof( void * ) );
Matrice->NombreDeTermesParLigneQueKpModifie = (int *)     malloc( NbElementsAllouesPourLesNombres  * sizeof( int    ) );

if (Matrice->DebutInfosAdressesQueKpModifie     == NULL || Matrice->AdresseDeUModifie            == NULL ||
    Matrice->AdresseUHaut                       == NULL || Matrice->DebutInfosLignesQueKpModifie == NULL ||
    Matrice->NombreDeLignesQueKpModifie         == NULL || Matrice->AdresseUGauche               == NULL ||
    Matrice->NombreDeTermesParLigneQueKpModifie == NULL ) {
  *Erreur = OUI_LU;
  goto FinRefactorisationSimulation;
} 

/* Simulation de refactorisation */
IndexLibrePourLesAdresses = 0;
IndexLibrePourLesNombres  = 0;
Matrice->PremierKpParCalculClassique = Matrice->Rang + 1;

/* Controle de taille */
Taille = 3 * ( ( Matrice->Rang * sizeof( int ) / TAILLE_DU_MEGA_OCTET ) );
Taille+= 2 * ( ( NbElementsAllouesPourLesAdresses * sizeof( void *  ) / TAILLE_DU_MEGA_OCTET ) );
Taille+= 2 * ( ( NbElementsAllouesPourLesNombres  * sizeof( int     ) / TAILLE_DU_MEGA_OCTET ) );
if ( Taille > TailleMaxEnMegaOctets ) {
  Matrice->PremierKpParCalculClassique = 0;
  /*
  printf("Fin par controle de taille a %d\n", Matrice->PremierKpParCalculClassique);
  printf("Taille %d TailleMaxEnMegaOctets %d\n",Taille,TailleMaxEnMegaOctets);
	*/
  goto FinRefactorisationSimulation;
}

Rang = Matrice->Rang;

LdebParLigneDeU     = Matrice->LdebParLigneDeU;
NbTermesParLigneDeU = Matrice->NbTermesParLigneDeU;
IndexKpDeUouL       = Matrice->IndexKpDeUouL;
ElmDeU              = Matrice->ElmDeU;
InverseOrdreLigne   = Matrice->InverseOrdreLigne;

DebutInfosAdressesQueKpModifie     = Matrice->DebutInfosAdressesQueKpModifie;
DebutInfosLignesQueKpModifie       = Matrice->DebutInfosLignesQueKpModifie;
AdresseDeUModifie                  = Matrice->AdresseDeUModifie;
AdresseUHaut                       = Matrice->AdresseUHaut;
AdresseUGauche                     = Matrice->AdresseUGauche;
NombreDeLignesQueKpModifie         = Matrice->NombreDeLignesQueKpModifie;
NombreDeTermesParLigneQueKpModifie = Matrice->NombreDeTermesParLigneQueKpModifie;

for ( Kp = 0 ; Kp < Rang ; Kp++ ) {

  /* Si la matrice est pleine on arrete */
  if ( NbTermesParLigneDeU[Kp] >= Rang - Kp ) {
    Matrice->PremierKpParCalculClassique = Kp;
    /*
    printf("Fin par matrice pleine a %d sur %d\n", Matrice->PremierKpParCalculClassique,Matrice->Rang);
		*/
    goto FinRefactorisationSimulation;
  }
  
  DebutInfosAdressesQueKpModifie[Kp] = IndexLibrePourLesAdresses;
  
  DebutInfosLignesQueKpModifie[Kp] = IndexLibrePourLesNombres;
  NbLignesQueKpModifie = 0;
  
  ilDebKp = LdebParLigneDeU[Kp];
  ilMaxKp = ilDebKp + NbTermesParLigneDeU[Kp];
  /*  */
  /* Transfert de la ligne dans un tableau de travail, on ne modifie pas la colonne pivot */  
  il = ilDebKp;
  il++; 
  while ( il < ilMaxKp ) {
    Adresse[IndexKpDeUouL[il]] = &(ElmDeU[il]);
    T      [IndexKpDeUouL[il]] = 1;           
    il++;
  }
  /* Maintenant on balaye toutes les lignes de la matrice active qui ont un terme non nul dans la colonne 
     pivot et on les compare a la ligne pivot */     
  ic = ilDebKp;  
  ic++; /* Terme diagonal */
  while ( ic < ilMaxKp ) { 
    /* Scanner la ligne */    
    /* Partie U en utilisant le chainage par colonne */        
    Kpp = IndexKpDeUouL[ic];		
    il    = LdebParLigneDeU[Kpp];
    ilMax = il + NbTermesParLigneDeU[Kpp];
    /* Calcul des termes modifies */          
    NbTermesParLigneQueKpModifie = 0;
    while ( il < ilMax ) { 
      iKp = IndexKpDeUouL[il];      
      if ( T[iKp] == 1 ) {
        AdresseDeUModifie[IndexLibrePourLesAdresses] = &(ElmDeU[il]);
        AdresseUHaut     [IndexLibrePourLesAdresses] = Adresse[iKp];
        IndexLibrePourLesAdresses++;
        NbTermesParLigneQueKpModifie++;
        if ( IndexLibrePourLesAdresses >= NbElementsAllouesPourLesAdresses ) {				
					/* printf("realloc adresses Kp= %d\n",Kp); */					
					NbElementsAllouesPourLesAdresses+= 2 * NbElementsU;
          /* Controle de taille */
          Taille = 3 * ( ( Matrice->Rang * sizeof( int ) / TAILLE_DU_MEGA_OCTET ) );
          Taille+= 2 * ( ( NbElementsAllouesPourLesAdresses * sizeof( void * ) / TAILLE_DU_MEGA_OCTET ) );
          Taille+= 2 * ( ( NbElementsAllouesPourLesNombres  * sizeof( int    ) / TAILLE_DU_MEGA_OCTET ) );
          if ( Taille > TailleMaxEnMegaOctets ) {
            Matrice->PremierKpParCalculClassique = Kp - 1;
						/*
            printf("Fin par controle de taille a %d sur %d TailleMaxEnMegaOctets %d\n",
						        Matrice->PremierKpParCalculClassique,Matrice->Rang,TailleMaxEnMegaOctets); 
						*/
						goto FinRefactorisationSimulation;
          }	  
          Matrice->AdresseDeUModifie = (double **) realloc( AdresseDeUModifie , NbElementsAllouesPourLesAdresses * sizeof( void * ) );
          Matrice->AdresseUHaut      = (double **) realloc( AdresseUHaut      , NbElementsAllouesPourLesAdresses * sizeof( void * ) );
					AdresseDeUModifie = Matrice->AdresseDeUModifie;
					AdresseUHaut      = Matrice->AdresseUHaut;
        }	
      }            
      il++;
    }
    AdresseUGauche                    [IndexLibrePourLesNombres] = &(ElmDeU[ic]);
    NombreDeTermesParLigneQueKpModifie[IndexLibrePourLesNombres] = NbTermesParLigneQueKpModifie;
    IndexLibrePourLesNombres++;
    NbLignesQueKpModifie++;
    if ( IndexLibrePourLesNombres >= NbElementsAllouesPourLesNombres ) {		
      /* printf("realloc nombres Kp= %d\n",Kp); */			
      NbElementsAllouesPourLesNombres+= 5 * Matrice->Rang;
      /* Controle de taille */
      Taille = 3 * ( ( Matrice->Rang * sizeof( int ) / TAILLE_DU_MEGA_OCTET ) );
      Taille+= 2 * ( ( NbElementsAllouesPourLesAdresses * sizeof( void * ) / TAILLE_DU_MEGA_OCTET ) );
      Taille+= 2 * ( ( NbElementsAllouesPourLesNombres  * sizeof( int    ) / TAILLE_DU_MEGA_OCTET ) );
      if ( Taille > TailleMaxEnMegaOctets ) {
        Matrice->PremierKpParCalculClassique = Kp - 1;
        /*
				printf("Fin par controle de taille a %d sur %d TailleMaxEnMegaOctets %d\n",
				        Matrice->PremierKpParCalculClassique,Matrice->Rang,TailleMaxEnMegaOctets); 
				*/
				goto FinRefactorisationSimulation;
      }	     
      Matrice->AdresseUGauche =
			  (double **) realloc( AdresseUGauche , NbElementsAllouesPourLesNombres * sizeof( void * ) );    
      Matrice->NombreDeTermesParLigneQueKpModifie =
        (int *) realloc( NombreDeTermesParLigneQueKpModifie , NbElementsAllouesPourLesNombres * sizeof( int ) );
      AdresseUGauche                     = Matrice->AdresseUGauche;
			NombreDeTermesParLigneQueKpModifie = Matrice->NombreDeTermesParLigneQueKpModifie;					
    }
    /* On passe au terme suivant de la colonne pivot */
    ic++;
  }
  
  il = ilDebKp;
  while ( il < ilMaxKp ) {
    T[IndexKpDeUouL[il]] = 0;          
    il++;
  }

  NombreDeLignesQueKpModifie[Kp] = NbLignesQueKpModifie;   
  
}

FinRefactorisationSimulation:

free( Adresse );
free( T );
Adresse = NULL;
T       = NULL;

return;
}






























































