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

   FONCTION: Decomposition LU de la base. Elimination de la ligne pivot.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

/*--------------------------------------------------------------------------------------------------*/

void LU_EliminationDUneLigne( MATRICE * Matrice , int LignePivot , int ColonnePivot )
{
int il; int ilDeb; int ic; int icDeb; int i; int ilDebLignePivot; double X; int ilDer; int j; int NbNonuLignePivot;
double UnSurValeurDuPivot; int NbTermesLignePivot; int NbTermesColonnePivot;
int Ligne; int Colonne; char * T    ; double * W; int * IndiceColonne     ;
double ValTermeColonnePivot; char FaireDuPivotageDiagonal  ; int CapaciteDesColonnes ;
double * AbsDuPlusGrandTermeDeLaLigne    ; double PivotMin; double * AbsValeurDuTermeDiagonal;
char ContexteDeLaFactorisation; char LignePivotDansSuperLigne;
char StockageLignesRedimensionne; char StockageColonnesRedimensionne;

int * IndiceLigneDeL; double * ElmDeL; int ilL; int ilColonnePivot;

int * Ldeb; int * LNbTerm; int * LDernierPossible; int * LIndiceColonne; double * Elm;
int * Cdeb; int * CNbTermMatriceActive; int * CDernierPossible; int * CIndiceLigne;

/* Pour les super lignes */
SUPER_LIGNE_DE_LA_MATRICE *  SuperLigne         ; SUPER_LIGNE_DE_LA_MATRICE ** SuperLigneDeLaLigne    ;
SUPER_LIGNE_DE_LA_MATRICE ** SuperLigneAScanner ; double * ElmColonneDeSuperLigne                     ;
int * NumerosDesLignesDeLaSuperLigne; int k   ; char * TypeDeClassementHashCodeAFaire               ;
int NombreDeTermesDeLaSuperLigne; int Element ; int icDebColonnePivot; 
double * ValeurDesTermesDeColonnePivot          ; SUPER_LIGNE_DE_LA_MATRICE * SuperLigneDeLaLignePivot;
int * PoidsDesColonnes; int NombreDeSuperLignesAScanner                    ; 
int NombreDePassagesDansSuperLignes            ; unsigned int * HashCodeLigne                       ;
char UtiliserLesSuperLignes; 

/* Fin super lignes */

/*
printf("Matrice->Kp = %d  LignePivot %d ColonnePivot %d rang %d\n",Matrice->Kp,LignePivot,ColonnePivot,Matrice->Rang);
*/

UtiliserLesSuperLignes = Matrice->UtiliserLesSuperLignes;

ContexteDeLaFactorisation = Matrice->ContexteDeLaFactorisation;
 
FaireDuPivotageDiagonal      = Matrice->FaireDuPivotageDiagonal;
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
  HashCodeLigne                  = Matrice->HashCodeLigne;
  PoidsDesColonnes               = Matrice->PoidsDesColonnes;
  /* Si la ligne pivot se trouve dans une SuperLigne, on la recopie dans un tableau particulier,
     et on fait pointer IndiceColonne et Elm vers ces tableaux, puis on l'enleve de la SuperLigne */     
  SuperLigneDeLaLignePivot = SuperLigneDeLaLigne[LignePivot];
  if ( SuperLigneDeLaLigne[LignePivot] != NULL ) {   
    /* Recopie de la ligne pivot vers le stockage standard des lignes */
    LU_RecopierUneLigneDeSuperLigneDansLigne( Matrice, SuperLigneDeLaLigne[LignePivot], LignePivot ); 		
    LignePivotDansSuperLigne = OUI_LU;
    /* Suppression de la LignePivot dans la SuperLigne */
    LU_SupprimerUneLigneDansUneSuperLigne( Matrice , SuperLigneDeLaLigne[LignePivot] , LignePivot , ColonnePivot );    
  }
}

Ldeb             = Matrice->Ldeb;
LNbTerm          = Matrice->LNbTerm;
LDernierPossible = Matrice->LDernierPossible;
LIndiceColonne   = Matrice->LIndiceColonne;
Elm              = Matrice->Elm;
 
Cdeb                = Matrice->Cdeb;
CNbTermMatriceActive = Matrice->CNbTermMatriceActive;
CDernierPossible    = Matrice->CDernierPossible;
CIndiceLigne        = Matrice->CIndiceLigne;

ilDebLignePivot    = Ldeb[LignePivot];
NbTermesLignePivot = LNbTerm[LignePivot];

icDebColonnePivot    = Cdeb[ColonnePivot];
NbTermesColonnePivot = CNbTermMatriceActive[ColonnePivot];

Matrice->IndexLibreDeU += NbTermesLignePivot; /* Servira a allouer le triangle U */

/* On enleve deja le nombre de termes de la ligne pivot */
Matrice->NombreDeTermes-= NbTermesLignePivot;

if ( FaireDuPivotageDiagonal == OUI_LU ) {
  if ( LignePivotDansSuperLigne == OUI_LU ) {
    /* Si la ligne etait dans une super ligne il faut remettre le terme diagonal en premier car il a pu changer de place */
    for ( il = ilDebLignePivot ; LIndiceColonne[il] != ColonnePivot ; il++ );
    LIndiceColonne[il]              = LIndiceColonne[ilDebLignePivot];
    LIndiceColonne[ilDebLignePivot] = ColonnePivot;
    X = Elm[ilDebLignePivot];
		Elm[ilDebLignePivot] = Elm[il];
    Elm[il]              = X;		
	}
}

W = Matrice->W;
T = Matrice->Marqueur;

UnSurValeurDuPivot = 1.; /* Pour pas etre emmerde par les warning a la con */
/* Transfert de la ligne dans un tableau de travail */
if ( NbTermesColonnePivot > 1 ) {
  if ( FaireDuPivotageDiagonal == NON_LU ) {  
    for ( il = ilDebLignePivot ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
      Colonne    = LIndiceColonne[il];
			if ( Colonne == ColonnePivot ) ilColonnePivot = il;
      W[Colonne] = Elm[il];			
      T[Colonne] = 1;			   
      /* Il faut dechainer la colonne du chainage fonction du nombre de termes car son nombre de termes changera */
      LU_DeClasserUneColonne( Matrice , Colonne , CNbTermMatriceActive[Colonne] );
    }  
    UnSurValeurDuPivot = 1. / W[ColonnePivot];
    W[ColonnePivot] = 0.0;
    T[ColonnePivot] = 0;   
  } 
  else {
    /* Si l'on a choisi l'option du pivotage diagonal, le terme diagonal est toujours range en premier */
		ilColonnePivot = ilDebLignePivot;
    for ( il = ilDebLignePivot + 1 ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
      Colonne    = LIndiceColonne[il];
      W[Colonne] = Elm[il];
      T[Colonne] = 1;						
    } 
    UnSurValeurDuPivot = 1. / Elm[ilDebLignePivot];
  }
}
else {
  /* Pas besoin de W et T */
  if ( FaireDuPivotageDiagonal == NON_LU ) {  
    for ( il = ilDebLignePivot ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
      Colonne = LIndiceColonne[il];
      if ( Colonne == ColonnePivot ) {
			  UnSurValeurDuPivot = 1. / Elm[il];
				ilColonnePivot = il;
			}
      LU_DeClasserUneColonne( Matrice , Colonne , CNbTermMatriceActive[Colonne] );
    } 
  }
  else {
    ilColonnePivot = ilDebLignePivot;
	  UnSurValeurDuPivot = 1. / Elm[ilDebLignePivot];
  }
}

/* Il faut dechainer la ligne pivot du chainage fonction du nombre de termes (la colonne pivot a ete dechainee ci-dessus 
   dans la foulee) */
LU_DeClasserUneLigne( Matrice , LignePivot , NbTermesLignePivot );

/* On inverse tout de suite le pivot */
X = Elm[ilDebLignePivot];
Elm[ilDebLignePivot] = UnSurValeurDuPivot;
if ( ilColonnePivot != ilDebLignePivot ) {
  Elm[ilColonnePivot]  = X;
  LIndiceColonne[ilColonnePivot] = LIndiceColonne[ilDebLignePivot];
  LIndiceColonne[ilDebLignePivot] = ColonnePivot;
}

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

if ( UtiliserLesSuperLignes == OUI_LU ) {
  /* Si la ligne pivot est dans une super ligne on ne met pas a jour son type de classement au cas ou
     elle aurait deja ete declassee */
  if ( SuperLigneDeLaLigne[LignePivot] == NULL ) {
    TypeDeClassementHashCodeAFaire[LignePivot] = ELEMENT_HASCODE_A_DECLASSER;
  }
}

NbNonuLignePivot = NbTermesLignePivot - 1;

/* S'il n'y a qu'un seul terme dans la colonne pivot, il n'y a rien a faire */
if ( NbTermesColonnePivot <= 1 ) goto FinDuScanningDesLignes;

StockageLignesRedimensionne   = NON_LU;
StockageColonnesRedimensionne = NON_LU;

ic = icDebColonnePivot;
for ( i = 0 ; i < NbTermesColonnePivot ; i++ , ic++ ) {
  Ligne = CIndiceLigne[ic];       
  if ( Ligne != LignePivot ) {
	
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
    
    /* Recherche de la valeur du terme de la ligne IndiceLigne qui se trouve dans la colonne pivot */
		ilDeb = Ldeb[Ligne];
    for ( il = ilDeb ; LIndiceColonne[il] != ColonnePivot ; il++ );    		
    ValTermeColonnePivot = Elm[il] * UnSurValeurDuPivot;
    /* Triangle L */
    ElmDeL        [ilL] = ValTermeColonnePivot;  
    IndiceLigneDeL[ilL] = Ligne;  
    ilL++;
    /*   */					
    /* Il faut dechainer la ligne du chainage fonction du nombre de termes car son nombre de termes peut changer */
    LU_DeClasserUneLigne( Matrice , Ligne , LNbTerm[Ligne] );
    
    if ( UtiliserLesSuperLignes == OUI_LU ) {
      TypeDeClassementHashCodeAFaire[Ligne] = ELEMENT_HASCODE_A_RECLASSER;       
    }
    
    /* Suppression du terme dans le chainage par ligne */				
    LNbTerm[Ligne]--;		
    ilDer = ilDeb + LNbTerm[Ligne];
    LIndiceColonne[il] = LIndiceColonne[ilDer];    
    Elm           [il] = Elm[ilDer];
    
    Matrice->NombreDeTermes--;    

    if ( UtiliserLesSuperLignes == OUI_LU ) {
      HashCodeLigne[Ligne]-= PoidsDesColonnes[ColonnePivot];
    }
    
    /* Scan de la ligne */
    /* Attention il faut faire le scan ligne meme si ValTermeColonnePivot est egal a 0, car en cas de pivotage
       diagonal sur matrice symetrique en topologie on perd la symetrie si on ne le fait pas */
	  /* Et aussi pour avoir toujous les termes dans les refactorisations (hades) */
	  if ( ContexteDeLaFactorisation != LU_SIMPLEXE ) {		
      if ( NbNonuLignePivot > 0 ) {
        LU_ScanLigne( Matrice, Ligne, ilDebLignePivot, NbNonuLignePivot,  ValTermeColonnePivot, LignePivot,
				              &StockageLignesRedimensionne, &StockageColonnesRedimensionne );			 											
				T[ColonnePivot] = 0;				
      }
		}
		else {
      if ( NbNonuLignePivot > 0 && ValTermeColonnePivot != 0.0 ) {
        LU_ScanLigne( Matrice, Ligne, ilDebLignePivot, NbNonuLignePivot,  ValTermeColonnePivot, LignePivot,
				              &StockageLignesRedimensionne, &StockageColonnesRedimensionne );			 															
				T[ColonnePivot] = 0;
      }
		}
    if ( StockageColonnesRedimensionne == OUI_LU ) {
      CIndiceLigne = Matrice->CIndiceLigne;
      ic = Cdeb[ColonnePivot] + i;			
      StockageColonnesRedimensionne = NON_LU;		
	  }		
    if ( StockageLignesRedimensionne == OUI_LU ) {
      LIndiceColonne  = Matrice->LIndiceColonne;
      Elm             = Matrice->Elm;					
	    ilDebLignePivot = Ldeb[LignePivot];
      StockageLignesRedimensionne = NON_LU;
	  }			        
    /* Si l'on a choisi l'option du pivotage diagonal */
    if ( FaireDuPivotageDiagonal == OUI_LU ) {
      /* Le terme diagonal est toujours range en premier */      
      /* Si le terme diagonal est trop petit on ne reclasse pas la ligne */			
      X = fabs( Elm[Ldeb[Ligne]] );			
      if ( X > PivotMin ) {
        AbsDuPlusGrandTermeDeLaLigne[Ligne] = -1.0;    
        /* Pas besoin de mettre une valeur negative si < a PivotMin car de toute facon c'est pas classe */
        AbsValeurDuTermeDiagonal[Ligne] = X;      
        LU_ClasserUneLigne( Matrice , Ligne , LNbTerm[Ligne] );      	    
      }      
    }
    else {
      /* Preparations pour le calcul du nombre de Markowitz */
      AbsDuPlusGrandTermeDeLaLigne[Ligne] = -1.;    
      /* Pivotage partout */
      /* Remarque: a ce stade IndiceLigne ne peut pas etre egal a LignePivot */
      LU_ClasserUneLigne( Matrice , Ligne , LNbTerm[Ligne] );      
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
      AbsDuPlusGrandTermeDeLaLigne[NumerosDesLignesDeLaSuperLigne[k]] = -1.;
      LU_DeClasserUneLigne( Matrice , NumerosDesLignesDeLaSuperLigne[k] , NombreDeTermesDeLaSuperLigne );      
    }        
         
    /* Suppression dans SuperLigne des termes de la colonne pivot */        
    LU_SupprimerUnTermeDansUneSuperLigne( Matrice , SuperLigne ,icDebColonnePivot , il );
    /* Car on a supprime dans Ligne, le terme d'indice colonne LignePivot */
    Matrice->NombreDeTermes-= SuperLigne->NombreDeLignesDeLaSuperLigne;
    
    NombreDeTermesDeLaSuperLigne--;
    
    /* Scan de la SuperLigne */

    LU_ScanSuperLigne( Matrice, SuperLigne, ilDebLignePivot, NbNonuLignePivot, ValeurDesTermesDeColonnePivot,
			                 &Matrice->HashCodeSuperLigne[SuperLigne->NumeroDeLaSuperLigne] );											
											    
    /* Au cas ou il y aurait eu un redimensionnement */
    ElmColonneDeSuperLigne         = SuperLigne->ElmColonneDeSuperLigne;
    NumerosDesLignesDeLaSuperLigne = SuperLigne->NumerosDesLignesDeLaSuperLigne;
    NombreDeTermesDeLaSuperLigne   = SuperLigne->NombreDeTermes;   		           
    
    Element = SuperLigne->NumeroDeLaSuperLigne + Matrice->Rang;
    if ( TypeDeClassementHashCodeAFaire[Element] != ELEMENT_HASCODE_A_CLASSER ) {
      TypeDeClassementHashCodeAFaire[Element] = ELEMENT_HASCODE_A_RECLASSER;
    }
    
    T[ColonnePivot] = 0;
    SuperLigne->ScannerLaSuperLigne = NON_LU;

    CapaciteDesColonnes = SuperLigne->CapaciteDesColonnes;
    IndiceColonne = SuperLigne->IndiceColonne;
    if ( FaireDuPivotageDiagonal == OUI_LU ) {
      /* Pivotage diagonal */
      for ( k = 0 ; k < SuperLigne->NombreDeLignesDeLaSuperLigne; k++ , ic++ ) {
        il = SuperLigne->IndexDuTermeDiagonal[k];
        X = fabs( ElmColonneDeSuperLigne[ (il * CapaciteDesColonnes) + k ] ); 
        if ( X > PivotMin ) {     
          Ligne = NumerosDesLignesDeLaSuperLigne[k];
          AbsDuPlusGrandTermeDeLaLigne[Ligne] = -1.;    	  
          /* Pas besoin de mettre une valeur negative si < PivotMin car pas classe et donc ne sera pas
	           rencontre dans selection du pivot diagonal */	
          AbsValeurDuTermeDiagonal[Ligne] = X;
          LU_ClasserUneLigne( Matrice , Ligne , NombreDeTermesDeLaSuperLigne );
        } 
      }
    }
    else {
      /* Pivotage Markowitz */
      for ( k = 0 ; k < SuperLigne->NombreDeLignesDeLaSuperLigne; k++ , ic++ ) {
        Ligne = NumerosDesLignesDeLaSuperLigne[k];	     
        /* Preparations pour le calcul du nombre de Markowitz */
        AbsDuPlusGrandTermeDeLaLigne[Ligne] = -1.;    
        LU_ClasserUneLigne( Matrice , Ligne , NombreDeTermesDeLaSuperLigne );
      }
    }        
  }
}

FinDuScanningDesLignes:

Matrice->IndexLibreDeL = ilL;

/* On remet aussi W et Marqueur a 0 */
ilDebLignePivot = Ldeb[LignePivot];
LIndiceColonne  = Matrice->LIndiceColonne;
CIndiceLigne    = Matrice->CIndiceLigne;
			
if ( NbTermesColonnePivot > 1 ) { 
  if ( FaireDuPivotageDiagonal == NON_LU ) {	
    for ( il = ilDebLignePivot + 1 ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
      Colonne = LIndiceColonne[il];
      W[Colonne] = 0.;
      T[Colonne] = 0;      
      /* Mise a jour des colonnes */
		  /* On ne supprime pas les termes de la LignePivot	mais on decremente le nombre de termes utiles */						
			CNbTermMatriceActive[Colonne]--;							
      LU_ClasserUneColonne( Matrice, Colonne, CNbTermMatriceActive[Colonne] );
    }
  }
  else {
    /* Dans le cas du pivotage diagonal, le terme diagonal est toujours range en premier */
    for ( il = ilDebLignePivot + 1 ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
      Colonne = LIndiceColonne[il];
      W[Colonne] = 0.;
      T[Colonne] = 0;    
      /* Mise a jour des colonnes */
		  /* On ne supprime pas les termes de la LignePivot	mais on decremente le nombre de termes utiles */
			CNbTermMatriceActive[Colonne]--;						
    }   
  }
}
else {
  if ( FaireDuPivotageDiagonal == NON_LU ) {
    for ( il = ilDebLignePivot + 1 ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
      Colonne = LIndiceColonne[il];
      /* Mise a jour des colonnes */
		  /* On ne supprime pas les termes de la LignePivot	mais on decremente le nombre de termes utiles */							
			CNbTermMatriceActive[Colonne]--;						
      LU_ClasserUneColonne( Matrice , Colonne , CNbTermMatriceActive[Colonne] );
    }
  }
  else {
    /* Dans le cas du pivotage diagonal, le terme diagonal est toujours range en premier */
    for ( il = ilDebLignePivot + 1 ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
      Colonne = LIndiceColonne[il];
      /* Mise a jour des colonnes */
		  /* On ne supprime pas les termes de la LignePivot	mais on decremente le nombre de termes utiles */
			CNbTermMatriceActive[Colonne]--;									
    } 
  }
}

if ( UtiliserLesSuperLignes == OUI_LU ) SuperLigneDeLaLigne[LignePivot] = NULL;  

/* Remarque:
On peut recupere la place occupee par la colonne eliminee et l'allouer a la colonne precedente mais cela
necessite de reprendre le codage de LU_AugmenterLaTailleDeLaMatriceActiveParColonne et de ne recopier
que les colonnes de la matrice active. On fera ca plus tard. Ci dessous la facon de faire pour recuperer
la place */
/*
ic = Cdeb[ColonnePivot];
Nb = Matrice->CNbTerm[ColonnePivot];
ix = Matrice->CDernierPossible[ColonnePivot];
	
C1 = Matrice->ColonnePrecedente[ColonnePivot];
C2 = Matrice->ColonneSuivante[ColonnePivot];
if ( C1 >= 0 ) {
	Matrice->CDernierPossible[C1] = ix;
	if ( C2 >= 0 ) {
	  Matrice->ColonneSuivante  [C1] = C2;
		Matrice->ColonnePrecedente[C2] = C1;		  
	}
}  
else {
	if ( C2 >= 0 ) Matrice->ColonnePrecedente[C2] = -1;		
}
Matrice->ColonnePrecedente[ColonnePivot] = -1;
Matrice->ColonneSuivante[ColonnePivot] = -1;

CNbTermMatriceActive[ColonnePivot] = 0;
Matrice->CNbTerm[ColonnePivot]     = 0;
*/

return;
}






































































