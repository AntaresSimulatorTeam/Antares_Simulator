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

   FONCTION: Elimination d'une ligne dans la cas symetrique (pivotage
             diagonal).                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

void LU_EliminationDUneLigneMatricePleineCasSymetrique( MATRICE * , int , int );

/*--------------------------------------------------------------------------------------------------*/

void LU_EliminationDUneLigneCasSymetrique( MATRICE * Matrice , int LignePivot , int ColonnePivot )
{ 
int il; int ic; int ilL; double X; char Contexte;
int * IndiceColonne; int NbNonuLignePivot; double UnSurValeurDuPivot; 
double ValTermeColonnePivot; char * T; double * W; char MajValeurPivotDiagonal;   
int NbTermesLignePivot; int Ligne; int NbTermesColonnePivot; int Colonne;
double * ElmDeL; int * IndiceLigneDeL   ; 
double * AbsValeurDuTermeDiagonal; double * AbsDuPlusGrandTermeDeLaLigne   ; double PivotMin; 

int * Ldeb; int * LNbTerm; int * LDernierPossible; int * LIndiceColonne; double * Elm;

char LignePivotDansSuperLigne; int ilDebLignePivot; char StockageLignesRedimensionne;

/* Pour les super lignes */
SUPER_LIGNE_DE_LA_MATRICE *  SuperLigne        ; SUPER_LIGNE_DE_LA_MATRICE ** SuperLigneDeLaLigne    ;
SUPER_LIGNE_DE_LA_MATRICE ** SuperLigneAScanner; double * ElmColonneDeSuperLigne                     ;
int * NumerosDesLignesDeLaSuperLigne          ; char * TypeDeClassementHashCodeAFaire               ;
int CapaciteDesColonnes;int k;int j; int i ; double * ValeurDesTermesDeColonnePivot              ;
int NombreDeTermesDeLaSuperLigne              ; int Element               ;
int NombreDeSuperLignesAScanner               ; SUPER_LIGNE_DE_LA_MATRICE * SuperLigneDeLaLignePivot;
int icDebColonnePivot   ; int NombreDePassagesDansSuperLignes                ;
char UtiliserLesSuperLignes                    ;
/* Fin super lignes */

UtiliserLesSuperLignes = Matrice->UtiliserLesSuperLignes;

if ( UtiliserLesSuperLignes == OUI_LU ) {
  if ( Matrice->MatricePleineDansUneSeuleSuperLigne == OUI_LU ) {	  
    LU_EliminationDUneLigneMatricePleineCasSymetrique( Matrice , LignePivot , ColonnePivot );
    return;		
  }
} 

Contexte = Matrice->ContexteDeLaFactorisation;

PivotMin                     = Matrice->PivotMin;
AbsValeurDuTermeDiagonal     = Matrice->AbsValeurDuTermeDiagonal;
AbsDuPlusGrandTermeDeLaLigne = Matrice->AbsDuPlusGrandTermeDeLaLigne;

LignePivotDansSuperLigne = NON_LU;

if ( UtiliserLesSuperLignes == OUI_LU ) {
  NombreDePassagesDansSuperLignes = Matrice->NombreDePassagesDansSuperLignes;
  /* Utilisation des super lignes */
  NombreDeSuperLignesAScanner = 0;
  SuperLigneDeLaLigne            = Matrice->SuperLigneDeLaLigne;
  SuperLigneAScanner             = Matrice->SuperLigneAScanner;
  TypeDeClassementHashCodeAFaire = Matrice->TypeDeClassementHashCodeAFaire;
  /* Si la ligne pivot se trouve dans une SuperLigne, on la recopie dans un tableau particulier,
     et on fait pointer IndiceColonne et Elm vers ces tableaux, puis on l'enleve de la SuperLigne */     
  SuperLigneDeLaLignePivot = SuperLigneDeLaLigne[LignePivot];     
  if ( SuperLigneDeLaLignePivot != NULL ) {  
    /* Recopie de la ligne pivot vers un tableau compact */
    LU_RecopierUneLigneDeSuperLigneDansLigne( Matrice, SuperLigneDeLaLigne[LignePivot], LignePivot ); 		
    LignePivotDansSuperLigne = OUI_LU;
    /* Suppression de la LignePivot dans la SuperLigne */    
    LU_SupprimerUneLigneDansUneSuperLigne( Matrice , SuperLigneDeLaLignePivot , LignePivot , ColonnePivot );    
  }
}

Ldeb             = Matrice->Ldeb;
LNbTerm          = Matrice->LNbTerm;
LDernierPossible = Matrice->LDernierPossible;
LIndiceColonne   = Matrice->LIndiceColonne;
Elm              = Matrice->Elm;

ilDebLignePivot    = Ldeb[LignePivot];
NbTermesLignePivot = LNbTerm[LignePivot];

Matrice->IndexLibreDeU += NbTermesLignePivot; /* Servira a allouer le triangle U */

/* On enleve deja le nombre de termes de la ligne pivot */
Matrice->NombreDeTermes-= NbTermesLignePivot;
if ( LignePivotDansSuperLigne == OUI_LU ) {
  /* Si la ligne etait dans une super ligne il faut remettre le terme diagonal en premier car il a pu changer de place */
  for ( il = ilDebLignePivot ; LIndiceColonne[il] != ColonnePivot ; il++ );
  LIndiceColonne[il]              = LIndiceColonne[ilDebLignePivot];
  LIndiceColonne[ilDebLignePivot] = ColonnePivot;
  X = Elm[ilDebLignePivot];
	Elm[ilDebLignePivot] = Elm[il];
  Elm[il]              = X;		
}

icDebColonnePivot    = ilDebLignePivot;
NbTermesColonnePivot = NbTermesLignePivot;

ElmDeL           = Matrice->ElmDeL; 
IndiceLigneDeL   = Matrice->IndiceLigneDeL;

W = Matrice->W;
T = Matrice->Marqueur;

/* Transfert de la ligne dans un tableau de travail */
/* Le terme diagonal est range en premier */
UnSurValeurDuPivot = 1. / Elm[ilDebLignePivot];

/* On inverse tout de suite le terme pivot */
Elm[ilDebLignePivot] = UnSurValeurDuPivot;

for ( il = ilDebLignePivot + 1 ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
  Colonne    = LIndiceColonne[il];  
  W[Colonne] = Elm[il];
  T[Colonne] = 1;
}

/* Il faut dechainer la ligne pivot du chainage fonction du nombre de termes (la colonne pivot a ete dechainee
   ci-dessus dans la foulee) */
LU_DeClasserUneLigne( Matrice , LignePivot , NbTermesLignePivot );

if ( UtiliserLesSuperLignes == OUI_LU ) {
  /* Si la ligne pivot est dans une super ligne on ne met pas a jour son type de classement au cas ou
     elle aurait deja ete declassee */
  if ( SuperLigneDeLaLignePivot == NULL ) {  
    TypeDeClassementHashCodeAFaire[LignePivot] = ELEMENT_HASCODE_A_DECLASSER;    
  } 
}

NbNonuLignePivot = NbTermesLignePivot - 1;

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
Matrice->ElmDeL[ilL] = 1.;
Matrice->IndiceLigneDeL[ilL] = LignePivot;
ilL++;

/* Maintenant on balaye toutes les lignes de la matrice active qui ont un terme non nul dans la colonne 
   pivot et on les compare a la ligne pivot (ce qui revient a balayer les termes de la ligne pivot) */
StockageLignesRedimensionne   = NON_LU;
ic = ilDebLignePivot + 1;
for ( i = 1 ; i < NbTermesLignePivot ; i++ , ic++ ) {  
  /* Le terme diagonal est toujours range en premier */
  /* En fait, il y a aussi un terme d'indice ligne egal a cette "Colonne" dans la colonne pivot */
  Ligne = LIndiceColonne[ic];    

  if ( UtiliserLesSuperLignes == OUI_LU ) {
    /* Si la Ligne est dans une SuperLigne on ne fait rien */
    if ( NombreDePassagesDansSuperLignes > 0 ) {
      if ( SuperLigneDeLaLigne[Ligne] != NULL ) {      
        if ( SuperLigneDeLaLigne[Ligne]->ScannerLaSuperLigne == NON_LU ) {	
          SuperLigneAScanner[NombreDeSuperLignesAScanner] = SuperLigneDeLaLigne[Ligne];
          NombreDeSuperLignesAScanner++;
          SuperLigneDeLaLigne[Ligne]->ScannerLaSuperLigne = OUI_LU;
        }
        continue;
      }
    }
  }
  	
  LU_DeClasserUneLigne( Matrice , Ligne , LNbTerm[Ligne] );
	    
  if ( UtiliserLesSuperLignes == OUI_LU ) {
    TypeDeClassementHashCodeAFaire[Ligne] = ELEMENT_HASCODE_A_RECLASSER;
  }
           
  ValTermeColonnePivot = Elm[ic] * UnSurValeurDuPivot;
  ElmDeL        [ilL] = ValTermeColonnePivot;
  IndiceLigneDeL[ilL] = Ligne;
  ilL++;
        
  /* La suppression du terme de la colonne pivot dans Ligne est fait dans ScanLigneCasSymetrique */				    
  LU_ScanLigneCasSymetrique( Matrice , Ligne, ilDebLignePivot, NbNonuLignePivot, ValTermeColonnePivot,
                             LignePivot, &StockageLignesRedimensionne ); 
  if ( StockageLignesRedimensionne == OUI_LU ) {
    LIndiceColonne  = Matrice->LIndiceColonne;
    Elm             = Matrice->Elm;					
	  ilDebLignePivot = Ldeb[LignePivot];
    ic = ilDebLignePivot + i;					
    StockageLignesRedimensionne = NON_LU;
	}	
														 
  MajValeurPivotDiagonal = OUI_LU;

  /* Dans le cas symetrique le terme diagonal est forcement modifie. Balayer la ligne plutot que de gerer cela dans
     "ScanLigne" et dans une eventuelle extension du stockage de la matrice n'est pas penalisant. */
  if ( MajValeurPivotDiagonal == OUI_LU ) {
    /* Car on a supprime dans Ligne, le terme d'indice colonne LignePivot */
    Matrice->NombreDeTermes--;
    /*  */
    /* Le terme diagonal est toujours range en premier */      
    /* Si le terme diagonal est trop petit on ne reclasse pas la ligne */
    X = fabs( Elm[Ldeb[Ligne]] );
    if ( X > PivotMin || Contexte == LU_POINT_INTERIEUR ) {
       AbsDuPlusGrandTermeDeLaLigne[Ligne] = -1; /* Il a pu etre modifie */
      /* Pas besoin de mettre une valeur negative si < PivotMin car pas classe et donc ne sera pas
         rencontre dans selection du pivot diagonal */
      AbsValeurDuTermeDiagonal[Ligne] = X;
      LU_ClasserUneLigne( Matrice, Ligne, LNbTerm[Ligne] );
    }    
  }  
}

if ( UtiliserLesSuperLignes == OUI_LU ) {
	
  for ( j = 0 ; j < NombreDeSuperLignesAScanner ; j++ ) {
    SuperLigne = SuperLigneAScanner[j];

    ElmColonneDeSuperLigne         = SuperLigne->ElmColonneDeSuperLigne;
    NumerosDesLignesDeLaSuperLigne = SuperLigne->NumerosDesLignesDeLaSuperLigne;
    NombreDeTermesDeLaSuperLigne   = SuperLigne->NombreDeTermes;
    IndiceColonne                  = SuperLigne->IndiceColonne;      
    
    for ( il = 0 ; il < NombreDeTermesDeLaSuperLigne ; il++ ) {
      if ( IndiceColonne[il] == ColonnePivot ) { 
        icDebColonnePivot = SuperLigne->CapaciteDesColonnes * il;
        break;
      }
    }

    /* Modification des termes de la colonne pivot et stockage dans le triangle L */
    ic = icDebColonnePivot;
    ValeurDesTermesDeColonnePivot = &(ElmDeL[ilL]);
    for ( k = 0 ; k < SuperLigne->NombreDeLignesDeLaSuperLigne; k++ , ic++ ) {
      ElmDeL        [ilL] = ElmColonneDeSuperLigne[ic] * UnSurValeurDuPivot;  
      IndiceLigneDeL[ilL] = NumerosDesLignesDeLaSuperLigne[k];			
      ilL++;
      /* Il faut dechainer la ligne du chainage fonction du nombre de termes car son nombre de termes peut changer */
      LU_DeClasserUneLigne( Matrice , NumerosDesLignesDeLaSuperLigne[k] , NombreDeTermesDeLaSuperLigne );      
    }
       
    /* Suppression dans SuperLigne des termes de la colonne pivot */
    LU_SupprimerUnTermeDansUneSuperLigne( Matrice , SuperLigne ,icDebColonnePivot , il );
    /* Car on a supprime dans Ligne, le terme d'indice colonne LignePivot */
    Matrice->NombreDeTermes-= SuperLigne->NombreDeLignesDeLaSuperLigne;
    
    NombreDeTermesDeLaSuperLigne--;
    
    /* Scan de la SuperLigne */
    /* On a deja scanne la super ligne de la ligne pivot */
    if ( SuperLigne != SuperLigneDeLaLignePivot || 1 ) {   
      LU_ScanSuperLigneCasSymetrique( Matrice, SuperLigne, ilDebLignePivot, NbNonuLignePivot, ValeurDesTermesDeColonnePivot,
			                                &Matrice->HashCodeSuperLigne[SuperLigne->NumeroDeLaSuperLigne] );				 									    
      /* Au cas ou il y aurait eu un redimensionnement */
      ElmColonneDeSuperLigne         = SuperLigne->ElmColonneDeSuperLigne;
      NumerosDesLignesDeLaSuperLigne = SuperLigne->NumerosDesLignesDeLaSuperLigne; /* Normalement ca change pas */
      NombreDeTermesDeLaSuperLigne   = SuperLigne->NombreDeTermes;
      IndiceColonne                  = SuperLigne->IndiceColonne;      
    }
    
    Element = SuperLigne->NumeroDeLaSuperLigne + Matrice->Rang;
    if ( TypeDeClassementHashCodeAFaire[Element] != ELEMENT_HASCODE_A_CLASSER ) {
      TypeDeClassementHashCodeAFaire[Element] = ELEMENT_HASCODE_A_RECLASSER;
    }
    
    T[ColonnePivot] = 0;
    SuperLigne->ScannerLaSuperLigne = NON_LU;

    CapaciteDesColonnes = SuperLigne->CapaciteDesColonnes;
    for ( k = 0 ; k < SuperLigne->NombreDeLignesDeLaSuperLigne; k++ , ic++ ) {      
      il = SuperLigne->IndexDuTermeDiagonal[k];			
      X = fabs( ElmColonneDeSuperLigne[ (il * CapaciteDesColonnes) + k ] ); 
      if ( X > PivotMin || Contexte == LU_POINT_INTERIEUR ) {     
        Ligne = NumerosDesLignesDeLaSuperLigne[k];
        /* Pas besoin de mettre une valeur negative si < PivotMin car pas classe et donc ne sera pas
	   rencontre dans selection du pivot diagonal */
        AbsDuPlusGrandTermeDeLaLigne[Ligne] = -1.0;	   
        AbsValeurDuTermeDiagonal[Ligne] = X;								
        LU_ClasserUneLigne( Matrice , Ligne , NombreDeTermesDeLaSuperLigne );
      }
    }    
  }
}

Matrice->IndexLibreDeL = ilL;

ilDebLignePivot = Ldeb[LignePivot];
LIndiceColonne  = Matrice->LIndiceColonne;

/* Dans le cas du pivotage diagonal, le terme diagonal est toujours range en premier */
for ( il = ilDebLignePivot + 1 ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
  Colonne = LIndiceColonne[il];
  /* On remet W et Marqueur a 0 */
  W[Colonne] = 0.;
  T[Colonne] = 0;    
}      

if ( UtiliserLesSuperLignes == OUI_LU ) SuperLigneDeLaLigne[LignePivot] = NULL;  

return;
}

/*--------------------------------------------------------------------------------------------------*/
/* La matrice est symetrique pleine et tout est dans une seule super ligne */

void LU_EliminationDUneLigneMatricePleineCasSymetrique( MATRICE * Matrice , int LignePivot , int ColonnePivot )
{
int il; int ic; int ilL; double X; int ilDebLignePivot; int ilColonnePivot; int ilElm;
int * IndiceColonne; double UnSurValeurDuPivot; int NbTermesLignePivot; int Ligne; int NbTermesColonnePivot; 
double * ElmDeL; int * IndiceLigneDeL; int CapaciteDesColonnes; int i; int NombreDeLignesDeLaSuperLigne;
int NumeroDeColonne; char Contexte; int icDebColonnePivot; double * ElmColonneDeSuperLigne;
int * NumerosDesLignesDeLaSuperLigne; int NombreDeTermesDeLaSuperLigne;
double * ValeurDesTermesDeColonnePivot; int k; int NombreDeLignesRestantes;
SUPER_LIGNE_DE_LA_MATRICE * SuperLigneDeLaLignePivot; double PivotMin;
int * Ldeb; int * LNbTerm; int * LIndiceColonne; double * Elm;

/*printf("Matrice->Kp = %d LignePivot %d \n", Matrice->Kp, LignePivot);*/

Contexte = Matrice->ContexteDeLaFactorisation;
PivotMin = Matrice->PivotMin;

SuperLigneDeLaLignePivot = Matrice->SuperLigneDeLaLigne[LignePivot];
  
Matrice->NombreDeTermes-= SuperLigneDeLaLignePivot->NombreDeTermes;
NombreDeLignesRestantes = SuperLigneDeLaLignePivot->NombreDeLignesDeLaSuperLigne - 1;
Matrice->NombreDeTermes-= NombreDeLignesRestantes;    

/* Recopie de la ligne pivot vers un tableau compact */																		 
LU_RecopierUneLigneDeSuperLigneDansLigne( Matrice, SuperLigneDeLaLignePivot, LignePivot ); 		
/* Suppression de la LignePivot dans la SuperLigne */    
LU_SupprimerUneLigneDansUneSuperLigne( Matrice , SuperLigneDeLaLignePivot , LignePivot , ColonnePivot );    

Ldeb             = Matrice->Ldeb;
LNbTerm          = Matrice->LNbTerm;
LIndiceColonne   = Matrice->LIndiceColonne;
Elm              = Matrice->Elm;

ilDebLignePivot      = Ldeb[LignePivot];
NbTermesLignePivot   = LNbTerm[LignePivot];
NbTermesColonnePivot = NbTermesLignePivot;

Matrice->IndexLibreDeU += NbTermesLignePivot; /* Servira a allouer le triangle U */

/* On repere le terme diagonal mais on ne le place pas tout de suite en premiere position */
for ( il = ilDebLignePivot ; LIndiceColonne[il] != ColonnePivot ; il++ );
ilColonnePivot = il;
UnSurValeurDuPivot = 1. / Elm[ilColonnePivot];
NumeroDeColonne    = ilColonnePivot - ilDebLignePivot;

ElmDeL           = Matrice->ElmDeL; 
IndiceLigneDeL   = Matrice->IndiceLigneDeL;

/* Recopies dans les triangles */
/* Triangle L */
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
Matrice->ElmDeL[ilL] = 1.;
Matrice->IndiceLigneDeL[ilL] = LignePivot;
ilL++;

/* Il faut dechainer la ligne pivot du chainage fonction du nombre de termes (la colonne pivot a ete dechainee
   ci-dessus dans la foulee) */
LU_DeClasserUneLigne( Matrice , LignePivot , NbTermesLignePivot );

if ( NombreDeLignesRestantes <= 0 ) {
  Matrice->IndexLibreDeL = ilL;
  goto FinScanSuperLigne;
}

ElmColonneDeSuperLigne         = SuperLigneDeLaLignePivot->ElmColonneDeSuperLigne;
NumerosDesLignesDeLaSuperLigne = SuperLigneDeLaLignePivot->NumerosDesLignesDeLaSuperLigne;
NombreDeTermesDeLaSuperLigne   = SuperLigneDeLaLignePivot->NombreDeTermes;
IndiceColonne                  = SuperLigneDeLaLignePivot->IndiceColonne;      
CapaciteDesColonnes            = SuperLigneDeLaLignePivot->CapaciteDesColonnes;
NombreDeLignesDeLaSuperLigne   = SuperLigneDeLaLignePivot->NombreDeLignesDeLaSuperLigne;

icDebColonnePivot = CapaciteDesColonnes * NumeroDeColonne; 

/* On scanne la super ligne. Il n'y a pas de creation de terme et il n'y a pas a s'occuper du hashcode */
ValeurDesTermesDeColonnePivot = &(ElmColonneDeSuperLigne[icDebColonnePivot]);

ilElm = ilDebLignePivot;
for ( il = 0 ; il < NbTermesLignePivot ; il++ , ilElm++ ) {

  if ( il == NumeroDeColonne ) continue;
 
  X = Elm[ilElm] * UnSurValeurDuPivot;

  ElmDeL        [ilL] = X;  
  IndiceLigneDeL[ilL] = LIndiceColonne[ilElm];	
  ilL++;
  
  /* Calcul du terme modifie: on balaye la colonne de la SuperLigne */
  ic = CapaciteDesColonnes * il;  
  for ( i = 0 ; i < NombreDeLignesDeLaSuperLigne ; i++ , ic++ ) {
    ElmColonneDeSuperLigne[ic]-= X * ValeurDesTermesDeColonnePivot[i]; 
  }             
}

Matrice->IndexLibreDeL = ilL;

/* Dans ce cas, ce n'est qu'apres que l'on enleve le terme de la colonne pivot */
/* Suppression dans SuperLigne des termes de la colonne pivot */
LU_SupprimerUnTermeDansUneSuperLigne( Matrice , SuperLigneDeLaLignePivot , icDebColonnePivot , NumeroDeColonne );
NombreDeTermesDeLaSuperLigne--;				        

goto UtilisationIndexDuTermeDiagonalMatricePleine;

for ( k = 0 ; k < NombreDeLignesDeLaSuperLigne; k++ , ic++ ) {
  Ligne = NumerosDesLignesDeLaSuperLigne[k];
  LU_DeClasserUneLigne( Matrice , Ligne , NbTermesLignePivot );     
  for ( il = 0 ; il < NombreDeTermesDeLaSuperLigne ; il++ ) { 
    if ( IndiceColonne[il] == Ligne) {
      X = fabs( ElmColonneDeSuperLigne[ (il * CapaciteDesColonnes) + k ] ); 
      /* Si le terme diagonal est trop petit on ne reclasse pas la ligne */
      if ( X > PivotMin || Contexte == LU_POINT_INTERIEUR ) {
        Matrice->AbsDuPlusGrandTermeDeLaLigne[Ligne] = -1.0;    
        Matrice->AbsValeurDuTermeDiagonal[Ligne] = X;
        LU_ClasserUneLigne( Matrice , Ligne , NombreDeTermesDeLaSuperLigne );
      }
      break;
    }
  }
}
goto FinScanSuperLigne;

UtilisationIndexDuTermeDiagonalMatricePleine:
for ( k = 0 ; k < NombreDeLignesDeLaSuperLigne; k++ , ic++ ) {
  Ligne = NumerosDesLignesDeLaSuperLigne[k];
  LU_DeClasserUneLigne( Matrice , Ligne , NbTermesLignePivot );     
  il = SuperLigneDeLaLignePivot->IndexDuTermeDiagonal[k];
  X = fabs( ElmColonneDeSuperLigne[ (il * CapaciteDesColonnes) + k ] ); 
  if ( X > PivotMin || Contexte == LU_POINT_INTERIEUR ) {
    Matrice->AbsDuPlusGrandTermeDeLaLigne[Ligne] = -1.0;     
    Matrice->AbsValeurDuTermeDiagonal[Ligne] = X;
    LU_ClasserUneLigne( Matrice , Ligne , NombreDeTermesDeLaSuperLigne );
  }
}

FinScanSuperLigne:

/* La ligne etait dans une super ligne il faut remettre le terme diagonal en premier car il a pu changer de place */

if ( ilColonnePivot == ilDebLignePivot ) {
  Elm[ilDebLignePivot] = UnSurValeurDuPivot;  
}
else {
  LIndiceColonne[ilColonnePivot]  = LIndiceColonne[ilDebLignePivot];
  LIndiceColonne[ilDebLignePivot] = ColonnePivot;
  X = Elm[ilDebLignePivot];
  /* On inverse le terme pivot */
  Elm[ilDebLignePivot] = UnSurValeurDuPivot;
  Elm[ilColonnePivot]  = X;
}
		 
if ( Matrice->UtiliserLesSuperLignes == OUI_LU ) Matrice->SuperLigneDeLaLigne[LignePivot] = NULL;

return;
}

