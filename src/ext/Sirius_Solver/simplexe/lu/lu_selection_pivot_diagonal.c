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

   FONCTION: Choix du pivot sur la diagonale.
                   
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# if OUTILS_DE_GESTION_MEMOIRE_DE_PNE_SOLVEUR	
  # include "mem_fonctions.h"
#endif

/*--------------------------------------------------------------------------------------------------*/
/*             Plus grand terme d'une ligne             */

double LU_TrouverPlusGrandTermeDeLigne( MATRICE * Matrice , int LignePivot )
{
int il; int ilDebLignePivot; double X; double Xmx; double * Elm;

SUPER_LIGNE_DE_LA_MATRICE * SuperLigneDeLaLignePivot; int * NumerosDesLignesDeLaSuperLigne;

int NombreDeLignesDeLaSuperLigne; int NombreDeTermes; int ic;
double * ElmColonneDeSuperLigne  ; int CapaciteDesColonnes;            

if ( Matrice->UtiliserLesSuperLignes == OUI_LU ) {
  SuperLigneDeLaLignePivot = Matrice->SuperLigneDeLaLigne[LignePivot];     
  if ( SuperLigneDeLaLignePivot != NULL ) {  
    /* Recopie de la ligne pivot vers un tableau compact */    
    NombreDeLignesDeLaSuperLigne   = SuperLigneDeLaLignePivot->NombreDeLignesDeLaSuperLigne;
    NumerosDesLignesDeLaSuperLigne = SuperLigneDeLaLignePivot->NumerosDesLignesDeLaSuperLigne;
    for ( ic = 0 ; ic < NombreDeLignesDeLaSuperLigne ; ic++ ) {
      if ( NumerosDesLignesDeLaSuperLigne[ic] == LignePivot ) {
        /* ic est l'index qui correspond a la ligne */
        break;
      }
    }    
    NombreDeTermes         = SuperLigneDeLaLignePivot->NombreDeTermes;
    CapaciteDesColonnes    = SuperLigneDeLaLignePivot->CapaciteDesColonnes;
    ElmColonneDeSuperLigne = SuperLigneDeLaLignePivot->ElmColonneDeSuperLigne;
    
    Xmx = -1.;
    for ( il = 0 ; il < NombreDeTermes ; il++ ) {
      X = fabs( ElmColonneDeSuperLigne[ic] );
      if ( X > Xmx ) Xmx = X;      
      ic+= CapaciteDesColonnes;
    }

    Matrice->AbsDuPlusGrandTermeDeLaLigne[LignePivot] = Xmx;
    
    return( Xmx );
    
  }
}

Elm = Matrice->Elm;
ilDebLignePivot = Matrice->Ldeb[LignePivot];
Xmx = fabs( Elm[ilDebLignePivot] );
for ( il = ilDebLignePivot + 1 ; il < ilDebLignePivot + Matrice->LNbTerm[LignePivot]; il++ ) {  
  X = fabs( Elm[il] );
  if ( X > Xmx ) Xmx = X; 
}

Matrice->AbsDuPlusGrandTermeDeLaLigne[LignePivot] = Xmx;

return( Xmx );
}

/*--------------------------------------------------------------------------------------------------*/
/*                           Selection du pivot sur la diagonale                                    */

void LU_SelectionDuPivotDiagonal( MATRICE * Matrice,
                                  int * LignePivotChoisie, 
                                  int * ColonnePivotChoisie 
                                )
{
int k; int Ligne ; int Colonne; int il         ; 
double MinMarkowitz; double Mu   ; double MuTest   ; int NuScan     ; double X  ; double Xsv     ;
char MinMarkowitzSimplifie       ; int * PremLigne; int * SuivLigne; double Pgt; double CoeffPgt;
int MaxScan; double PivotMin; int kFin;
int LignePivot; int ColonnePivot; char TestDeStabilite                  ;
double * AbsValeurDuTermeDiagonal ; double * AbsDuPlusGrandTermeDeLaLigne ;
int * Ldeb; double * Elm; int * CNbTerm; int * CNbTermMatriceActive;

int NombreDeLignesDeLaSuperLigne; 
SUPER_LIGNE_DE_LA_MATRICE * SuperLigneDeLaLigne      ; int * NumerosDesLignesDeLaSuperLigne ;                  
double * ElmColonneDeSuperLigne ; int ic            ; int CapaciteDesColonnes; char RegulOk;              

TestDeStabilite = 1;

RechercheDuPivot:

LignePivot   = -1;
ColonnePivot = -1;

Mu       = Matrice->RangAuCarrePlus1; 
MuTest   = Mu;
NuScan   = 0;
MaxScan  = Matrice->MaxScan;
PivotMin = Matrice->PivotMin;

CoeffPgt = PivotMin;

if ( Matrice->LaMatriceEstSymetrique == OUI_LU || Matrice->LaMatriceEstSymetriqueEnStructure == OUI_LU ) {
  MinMarkowitzSimplifie = OUI_LU;
}
else {
  MinMarkowitzSimplifie = NON_LU;
}

PremLigne = Matrice->PremLigne;
SuivLigne = Matrice->SuivLigne;

AbsValeurDuTermeDiagonal     = Matrice->AbsValeurDuTermeDiagonal;
AbsDuPlusGrandTermeDeLaLigne = Matrice->AbsDuPlusGrandTermeDeLaLigne;

CNbTerm = Matrice->CNbTerm;
CNbTermMatriceActive = Matrice->CNbTermMatriceActive;

kFin = Matrice->Rang - Matrice->Kp;
Xsv = -1.;
for ( k = 1 ; k <= kFin ; k++ ) {
  /* On a trouve un pivot ? */
  if ( Mu < MuTest ) goto FinSelectionDuPivotDiagonal;
  /* Balayage des lignes qui on k termes */
  Ligne = PremLigne[k];
  while ( Ligne >= 0 ) {
    MinMarkowitz = MuTest;
    X = AbsValeurDuTermeDiagonal[Ligne];
    if ( X > PivotMin ) { /* Controle supplementaire car au debut on classe aussi les pivots nuls */      
      Colonne = Ligne;      
      if ( MinMarkowitzSimplifie == NON_LU ) {
        MinMarkowitz = ( k - 1 ) * ( CNbTermMatriceActive[Colonne] - 1 );
      }
      else {
        MinMarkowitz = k;
      }
			if ( TestDeStabilite == 0 ) {
        LignePivot   = Ligne;
        ColonnePivot = Colonne;
        break;
			}
      goto Compare;
    }	
    goto NextLigne;
        
    Compare:
    /* Terme diagonal eventuellement trouve */
    if ( MinMarkowitz == Mu ) {
      if ( X > Xsv ) {
        /* Methode en test : on fait un controle grossier de stabilite. Resultat des tests: ca marche bien */
        if ( X < 1. ) {
					Pgt = AbsDuPlusGrandTermeDeLaLigne[Ligne];
					if ( Pgt < 0. ) {
						Pgt = LU_TrouverPlusGrandTermeDeLigne( Matrice , Ligne );
						/*AbsDuPlusGrandTermeDeLaLigne[Ligne] = Pgt;*/	    
					}
					if ( X < CoeffPgt * Pgt ) {
						goto NextLigne;
					}
        }      
        /* Fin methode en test */         
        Xsv = X;
        LignePivot   = Ligne;
        ColonnePivot = Colonne;
      }
    }     
    else if ( MinMarkowitz < Mu ) {
      /* Methode en test : on fait un controle grossier de stabilite. Resultat des tests: ca marche bien */   
      if ( X < 1. ) {
        Pgt = AbsDuPlusGrandTermeDeLaLigne[Ligne];
				if ( Pgt < 0. ) {
					Pgt = LU_TrouverPlusGrandTermeDeLigne( Matrice , Ligne );
					/*AbsDuPlusGrandTermeDeLaLigne[Ligne] = Pgt;*/	    
				}      
				if ( X < CoeffPgt * Pgt ) {
					goto NextLigne;
        }
      }         
      /* Fin methode en test */    
      Mu  = MinMarkowitz;
      Xsv = X;
      LignePivot   = Ligne;
      ColonnePivot = Colonne;
			/* Dans le cas hors point interieur on peut se contenter du premier pivot correct rencontre */
			/* donc chercher a remettre le test ci-dessous en service */
      /* goto FinSelectionDuPivotDiagonal; */
    }    
           
    if ( MinMarkowitz < MuTest ) NuScan++;
    
    if ( NuScan == MaxScan ) goto FinSelectionDuPivotDiagonal;
    
    NextLigne:
    Ligne = SuivLigne[Ligne];
  }

  /* On change de k: si on a trouve un pivot on arrete */
  if ( NuScan > 0 ) goto FinSelectionDuPivotDiagonal;
  
}

FinSelectionDuPivotDiagonal:

*LignePivotChoisie   = LignePivot;
*ColonnePivotChoisie = ColonnePivot;

if ( LignePivot >= 0 && ColonnePivot >= 0 ) {
  if ( MinMarkowitzSimplifie == NON_LU ) {  
    if ( CNbTerm[ColonnePivot] != CNbTermMatriceActive[ColonnePivot] ) {
      LU_SupprimerTermesInutilesDansColonne( Matrice, ColonnePivot, Matrice->Cdeb[ColonnePivot] );
    }
	}
  return;
}

if ( Matrice->OnPeutRegulariser == NON_LU ) return;
if ( Matrice->ValeurDeRegularisation <= Matrice->PivotMin ) return;

/* On peut regulariser, donc on est dans le point interieur. Avant de se lancer dans la recgularisation, on
   regarde s'il est possible de trouver un pivot sans faire le test de staibilite. En effet, il y a le risque
	 que l'on n'ai pas choisi de pivot a cause de la condition de stabilite, tout en ayant des pivot de valeur
	 plus grande que le terme de regularisation lui-même. */
if ( TestDeStabilite == 1 ) {
  TestDeStabilite = 0;
	goto RechercheDuPivot;
}

/* Si on est tombe sur un pivot nul et qu'on est dans un contexte de point interieur, on regularise */

Ldeb = Matrice->Ldeb;
Elm  = Matrice->Elm;

RegulOk = NON_LU;
for ( k = 1 ; k <= kFin ; k++ ) {
  /* Balayage des lignes qui on k termes */
  Ligne = PremLigne[k];
  while ( Ligne >= 0 ) {			
    AbsValeurDuTermeDiagonal[Ligne] = Matrice->ValeurDeRegularisation;
    Matrice->TermeDeRegularisation[Ligne]+= Matrice->ValeurDeRegularisation;		
	  RegulOk = OUI_LU;
    Matrice->OnARegularise = OUI_LU;		
		*LignePivotChoisie   = Ligne;
    *ColonnePivotChoisie = Ligne;
		/* On modifie aussi la valeur du terme diagonal pour etre en accord */
    if ( Matrice->UtiliserLesSuperLignes == OUI_LU ) {
      SuperLigneDeLaLigne = Matrice->SuperLigneDeLaLigne[Ligne];     
      if ( SuperLigneDeLaLigne != NULL ) {  
        NombreDeLignesDeLaSuperLigne   = SuperLigneDeLaLigne->NombreDeLignesDeLaSuperLigne;
        NumerosDesLignesDeLaSuperLigne = SuperLigneDeLaLigne->NumerosDesLignesDeLaSuperLigne;
        CapaciteDesColonnes            = SuperLigneDeLaLigne->CapaciteDesColonnes;
        ElmColonneDeSuperLigne         = SuperLigneDeLaLigne->ElmColonneDeSuperLigne;				
        for ( ic = 0 ; ic < NombreDeLignesDeLaSuperLigne ; ic++ ) {
          if ( NumerosDesLignesDeLaSuperLigne[ic] == Ligne ) {
            /* ic est l'index qui correspond a la ligne */
            il = SuperLigneDeLaLigne->IndexDuTermeDiagonal[ic];
            ElmColonneDeSuperLigne[ (il * CapaciteDesColonnes) + ic ] = Matrice->ValeurDeRegularisation;						
            break;
          }
        }             
		  }
		  else {    
				Elm[Ldeb[Ligne]] = Matrice->ValeurDeRegularisation;
		  }
	  }
	  else {
			Elm[Ldeb[Ligne]] = Matrice->ValeurDeRegularisation;			
	  }
	  if ( RegulOk == OUI_LU ) break;
    Ligne = SuivLigne[Ligne];		
	}   
	if ( RegulOk == OUI_LU ) break;	
}

return;

}































































