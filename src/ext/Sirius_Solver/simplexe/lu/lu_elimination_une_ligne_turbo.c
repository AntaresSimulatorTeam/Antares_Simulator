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

   FONCTION: Elimination de la ligne pivot version turbo.
             Attention il y a des conditions:
	     1- La matrice doit etre symetrique en topologie
	     2- Il faut faire du pivotage diagonal
	     Ce sous programme ne prend pas en charge les super lignes.

   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

/*--------------------------------------------------------------------------------------------------*/

void LU_EliminationDUneLigneVersionTurbo( MATRICE * Matrice , int LignePivot , int ColonnePivot )
{
int il; int ill; int illDeb; double X; int NbNonuLignePivot; int Demande; double UnSurValeurDuPivot;
int NbTermesLignePivot; int NbTermesColonnePivot; int NbTermesLigne; int Ligne; int Colonne;
int ilDebLignePivot; char * T; double * W; double ValTermeColonnePivot; int f; int i; double PivotMin; 
double * AbsValeurDuTermeDiagonal; double * AbsDuPlusGrandTermeDeLaLigne; int illLigne;
int * Ldeb; int * LNbTerm; int * LDernierPossible; int * LIndiceColonne; double * Elm;
int * IndiceLigneDeL; double * ElmDeL; int ilL;

/*printf("Kp %d Rang %d LignePivot %d\n",Matrice->Kp,Matrice->Rang,LignePivot);*/ 

PivotMin                     = Matrice->PivotMin;
AbsValeurDuTermeDiagonal     = Matrice->AbsValeurDuTermeDiagonal;
AbsDuPlusGrandTermeDeLaLigne = Matrice->AbsDuPlusGrandTermeDeLaLigne;

Ldeb = Matrice->Ldeb;
LNbTerm = Matrice->LNbTerm;
LDernierPossible = Matrice->LDernierPossible;
LIndiceColonne = Matrice->LIndiceColonne;
Elm = Matrice->Elm;

ilDebLignePivot    = Ldeb[LignePivot];
NbTermesLignePivot = LNbTerm[LignePivot];   

NbTermesColonnePivot = NbTermesLignePivot;

Matrice->IndexLibreDeU += NbTermesLignePivot; /* Servira a allouer le triangle U */

Matrice->NombreDeTermes -= NbTermesLignePivot + (NbTermesLignePivot-1);

/* printf("Matrice->Kp = %d LignePivot %d  NbTermesLignePivot %d\n",Matrice->Kp,LignePivot,NbTermesLignePivot); */

W = Matrice->W;
T = Matrice->Marqueur;

/* Transfert de la ligne dans un tableau de travail */
/* Le terme diagonal est range en premier */
UnSurValeurDuPivot = 1. / Elm[ilDebLignePivot];

/* On inverse tout de suite le terme pivot */
Elm[ilDebLignePivot] = UnSurValeurDuPivot;

for ( il = ilDebLignePivot + 1 ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
  W[LIndiceColonne[il]] = Elm[il];
  T[LIndiceColonne[il]] = 1;	
}

/* Il faut dechainer la ligne pivot du chainage fonction du nombre de termes (la colonne pivot a ete dechainee ci-dessus 
   dans la foulee) */
LU_DeClasserUneLigne( Matrice , LignePivot , NbTermesLignePivot );

NbNonuLignePivot = NbTermesLignePivot - 1;

ElmDeL         = Matrice->ElmDeL; 
IndiceLigneDeL = Matrice->IndiceLigneDeL;
/* Transfert de la colonne pivot dans la triangulaire inferieure L et modification de la matrice active */
/* Le terme diagonal vaut 1 et est stocke pour beneficier de l'indice ligne */
if ( Matrice->IndexLibreDeL + NbTermesColonnePivot > Matrice->DernierIndexLibreDeL ) { 
  LU_AugmenterLaTailleDuTriangleL( Matrice , NbTermesColonnePivot );
  ElmDeL         = Matrice->ElmDeL; 
  IndiceLigneDeL = Matrice->IndiceLigneDeL;  
}

Matrice->NbTermesParColonneDeL[Matrice->Kp] = NbTermesColonnePivot;

ilL = Matrice->IndexLibreDeL;
Matrice->CdebParColonneDeL[Matrice->Kp] = ilL;

/* Initialisaton de ElmDeL */
ElmDeL        [ilL] = 1.;
IndiceLigneDeL[ilL] = LignePivot;
ilL++;

il = ilDebLignePivot + 1;
for ( i = 1 ; i < NbTermesLignePivot ; i++ , il++ ) {  
  /* Le terme diagonal est toujours range en premier */
  /* En fait, il y a aussi un terme d'indice ligne egal a cette "Colonne" dans la colonne pivot */
  Ligne = LIndiceColonne[il];  
  /* Recherche de la valeur du terme de la ligne Ligne qui se trouve dans la colonne pivot */
	illDeb = Ldeb[Ligne];
  for ( ill = illDeb ; LIndiceColonne[ill] != ColonnePivot ; ill++ );	
  ValTermeColonnePivot = Elm[ill] * UnSurValeurDuPivot;
  /* Triangle L */
  ElmDeL        [ilL] = ValTermeColonnePivot;  
  IndiceLigneDeL[ilL] = Ligne;  
  ilL++;
  /*   */	
  /* Il faut dechainer la ligne du chainage fonction du nombre de termes car son nombre de termes peut changer */
  NbTermesLigne = LNbTerm[Ligne];	
  LU_DeClasserUneLigne( Matrice , Ligne , NbTermesLigne );        
  /* Suppression du terme dans le chainage par ligne */		
	NbTermesLigne--;
	LNbTerm[Ligne] = NbTermesLigne;
  Elm           [ill] = Elm[illDeb + NbTermesLigne];
  LIndiceColonne[ill] = LIndiceColonne[illDeb + NbTermesLigne];  
  /* Scan de la ligne */
  /* Attention il faut faire le scan ligne meme si ValTermeColonnePivot est egal a 0, car en cas de pivotage
     diagonal sur matrice symetrique en topologie on perd la symetrie si on ne le fait pas */
	/* Et egalement pour ne pas perdre de termes dans les refactorisations (hades) */
  if ( NbNonuLignePivot <= 0 ) goto FinDuScan;
  
  /* f sert a mesurer le "fill-in" */
  f = NbNonuLignePivot; 
  /* Transformation des termes de la ligne scannee */
  for ( ill = illDeb ; ill < illDeb + NbTermesLigne ; ill++ ) {
    Colonne = LIndiceColonne[ill];
    if ( T[Colonne] == 1 ) {
      T[Colonne] = 0; 
      Elm[ill]-= W[Colonne] * ValTermeColonnePivot; 
      f--;
      if ( f == 0 ) break;
    }
  }	
  if ( f > 0 ) {
    Matrice->CompteurExclusionMarkowitz+= f;
    Matrice->NombreDeTermes+= f;
    Matrice->NbFillIn+= f;   
    /* Il y a un "fill-in" de f termes => on scanne la ligne pivot pour pouvoir calculer et 
       stocker les termes crees . Les termes crees sont chaines en fin de ligne */			 
    if ( illDeb + NbTermesLigne + f - 1 > LDernierPossible[Ligne] ) {		
      Demande = NbTermesLigne + f;
      LU_AugmenterLaTailleDeLaMatriceActive( Matrice , Ligne , Demande );
      LIndiceColonne  = Matrice->LIndiceColonne;
      Elm             = Matrice->Elm;
	    ilDebLignePivot = Ldeb[LignePivot];
			il = ilDebLignePivot + i;
			illDeb = Ldeb[Ligne];	
    }

	  illLigne = illDeb + NbTermesLigne;  
		
    /* On commence a 1 car terme diagonal range en premier */
    for ( ill = ilDebLignePivot + 1 ; ill < ilDebLignePivot + NbTermesLignePivot ; ill++ ) {
      Colonne = LIndiceColonne[ill];   
      if ( T[Colonne] == 1 ) {							
        LIndiceColonne[illLigne] = Colonne;
        Elm           [illLigne] = -W[Colonne] * ValTermeColonnePivot;				
				illLigne++;
        NbTermesLigne++;
        f--;
        if ( f == 0 ) { ill++; break;}      
      } 
      else T[Colonne] = 1;
    }						
    while ( ill < ilDebLignePivot + NbTermesLignePivot ) { 
      T[LIndiceColonne[ill]] = 1; 
      ill++;
    }
    /* Mise a jour du nombre de termes de la ligne scannee */
    LNbTerm[Ligne] = NbTermesLigne;			
  }
  else {
    /* Pas de "fill-in" : on remet Matrice.Marqueur aux bonnes valeurs */
    /* Terme diagonal range en premier */
    for ( ill = ilDebLignePivot + 1 ; ill < ilDebLignePivot + NbTermesLignePivot ; ill++ ) {
      T[LIndiceColonne[ill]] = 1; 
    }
  }
		
  FinDuScan:	
  /* Le terme diagonal est toujours range en premier */      
  /* Si le terme diagonal est trop petit on ne reclasse pas la ligne */
  X = fabs( Elm[illDeb] );	
  if ( X > PivotMin ) {
    AbsDuPlusGrandTermeDeLaLigne[Ligne] = -1.0;     
    /* Pas besoin de mettre une valeur negative si < PivotMin car n'est pas classe et ne sera pas
       rencontre dans slection du pivot diagonal */
    AbsValeurDuTermeDiagonal[Ligne] = X;		
    LU_ClasserUneLigne( Matrice , Ligne , NbTermesLigne );      	    
  }
} 

Matrice->IndexLibreDeL = ilL;
  
/* Dans le cas du pivotage diagonal, le terme diagonal est toujours range en premier */
for ( il = ilDebLignePivot + 1 ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
  W[LIndiceColonne[il]] = 0.;
  T[LIndiceColonne[il]] = 0;	
} 

return;
}






































































