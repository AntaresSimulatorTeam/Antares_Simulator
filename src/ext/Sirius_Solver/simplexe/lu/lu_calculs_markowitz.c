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

   FONCTION: Decomposition LU de la base.
             Calculs des nombres de Markowitz.
                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# if OUTILS_DE_GESTION_MEMOIRE_DE_PNE_SOLVEUR	
  # include "mem_fonctions.h"
#endif

/*--------------------------------------------------------------------------------------------------*/

void LU_PlusGrandTermeDeLaLigne( MATRICE * Matrice , int Ligne , int Colonne , int * IndexRecherche )
{
int il; int ilDeb; double X  ; double ElmMax; double * Elm  ; int * LIndiceColonne;
int NombreDeTermes; char LigneDansSuperLigne;

ElmMax = 0.9999 * Matrice->PivotMin;

LigneDansSuperLigne = NON_LU;

if ( Matrice->UtiliserLesSuperLignes == OUI_LU ) {
  if ( Matrice->SuperLigneDeLaLigne[Ligne] != NULL ) {
	  /* Il vaut mieux proceder a une recherche special dans la super ligne plutot que de passer
		   par une recopie */
    LU_RecopierUneLigneDeSuperLigneDansLigne( Matrice, Matrice->SuperLigneDeLaLigne[Ligne], Ligne );
		LigneDansSuperLigne = OUI_LU;
  }
}

ilDeb           = Matrice->Ldeb[Ligne];	
NombreDeTermes  = Matrice->LNbTerm[Ligne];
Elm             = Matrice->Elm;
LIndiceColonne  = Matrice->LIndiceColonne;   

if ( Colonne >= 0 ) {
  for ( il = ilDeb ; il < ilDeb + NombreDeTermes ; il++ ) {
    X = fabs( Elm[il] );		
    if ( X > ElmMax ) ElmMax = X; 
    if ( LIndiceColonne[il] == Colonne ) {
		  if ( LigneDansSuperLigne == NON_LU ) *IndexRecherche = il;
			else *IndexRecherche = il - ilDeb;
      il++;
      break;
    }
  }
  for ( ; il < ilDeb + NombreDeTermes ; il++ ) {  
    X = fabs( Elm[il] );		
    if ( X > ElmMax ) ElmMax = X; 
  }
}
else {
  for ( il = ilDeb ; il < ilDeb + NombreDeTermes ; il++ ) {
    X = fabs( Elm[il] );		
    if ( X > ElmMax ) ElmMax = X; 
  }
}
Matrice->AbsDuPlusGrandTermeDeLaLigne[Ligne] = ElmMax;

return;  
}

/*--------------------------------------------------------------------------------------------------*/

void LU_InitMinMarkowitzDeLigne( MATRICE * Matrice , int Ligne , int * ColonneDeMarkowitz , double * ValeurMarkowitz  )
{
int il; int ilDeb; int Nb; double MinMarkowitz; double Y; int Select; int ColonneDeMinMarkowitz;
double SeuilMarkowitz; int NbTMinCol; int NbTCol; double Ysv; double AbsMaxLig; 
int NombreDeTermes; int * LIndiceColonne; double * Elm; int * CNbTermMatriceActive;
int * Cdeb;

*ColonneDeMarkowitz = -1;
*ValeurMarkowitz    = Matrice->RangAuCarrePlus1 + 1.; /* Car on teste une egalite dans selection pivot */

AbsMaxLig = Matrice->AbsDuPlusGrandTermeDeLaLigne[Ligne];
if ( AbsMaxLig < 0. ) { 
  LU_PlusGrandTermeDeLaLigne( Matrice , Ligne , -1 , &il );
  AbsMaxLig = Matrice->AbsDuPlusGrandTermeDeLaLigne[Ligne];
}

if ( AbsMaxLig < Matrice->PivotMin ) return;

SeuilMarkowitz        = Matrice->SeuilDePivotage * AbsMaxLig;
ColonneDeMinMarkowitz = -1;
NbTMinCol             = Matrice->Rang + 10;
MinMarkowitz          = NbTMinCol;

Ysv = -1.;

if ( Matrice->UtiliserLesSuperLignes == OUI_LU ) {
  if ( Matrice->SuperLigneDeLaLigne[Ligne] != NULL ) {
    LU_RecopierUneLigneDeSuperLigneDansLigne( Matrice, Matrice->SuperLigneDeLaLigne[Ligne], Ligne );	 																					
  }
}

ilDeb          = Matrice->Ldeb[Ligne];	
NombreDeTermes = Matrice->LNbTerm[Ligne];
Elm            = Matrice->Elm;
LIndiceColonne = Matrice->LIndiceColonne;   

Nb = NombreDeTermes - 1;

Cdeb    = Matrice->Cdeb;
CNbTermMatriceActive = Matrice->CNbTermMatriceActive;

for ( il = ilDeb ; il < ilDeb + NombreDeTermes ; il++ ) {		
	NbTCol = CNbTermMatriceActive[LIndiceColonne[il]] - 1;	
  if ( NbTCol <= NbTMinCol ) {  
    Y = fabs( Elm[il] );
    if ( Y > SeuilMarkowitz && Y > Matrice->PivotMin ) {
      Select = OUI_LU;
      if ( NbTCol == NbTMinCol ) {
        if ( Y < Ysv ) Select = NON_LU; 
      }
      /*   */ 
      if ( Select == OUI_LU ) {
        MinMarkowitz          = Nb * NbTCol; 
        ColonneDeMinMarkowitz = LIndiceColonne[il]; 
        NbTMinCol             = NbTCol;
        Ysv                   = Y;
      }
    }
  }
}

if ( ColonneDeMinMarkowitz >= 0 ) {
  *ColonneDeMarkowitz = ColonneDeMinMarkowitz;
  *ValeurMarkowitz    = MinMarkowitz;	
}
else {
  if ( Matrice->ExclureLesEchecsMarkowitz == OUI_LU ) {
    Matrice->LigneRejeteeTemporairementPourPivotage[Ligne] = OUI_LU;
  }
}

return;
}

/*--------------------------------------------------------------------------------------------------*/
/* On elimine les termes qui correspondent a des lignes deja eliminees. En sortie on a
   CNbTerm = CNbTermMatriceActive */
void LU_SupprimerTermesInutilesDansColonne( MATRICE * Matrice, int Colonne, int ic1Deb )
{
int Kp; int NbTerm; int ic1; int ic2; int * InverseOrdreLigne; int * CIndiceLigne;

Kp = Matrice->Kp;
InverseOrdreLigne = Matrice->InverseOrdreLigne;
CIndiceLigne      = Matrice->CIndiceLigne;

NbTerm = 0;
ic2 = ic1Deb + Matrice->CNbTerm[Colonne] - 1;
for ( ic1 = ic1Deb ; ic1 <= ic2 ; ic1++ ) {
  if ( InverseOrdreLigne[CIndiceLigne[ic1]] >= Kp ) {
	  NbTerm++;
	  continue;
	}
  /* La ligne a deja ete eliminee => on peut compacter */
	while ( ic2 > ic1 ) {
		if ( InverseOrdreLigne[CIndiceLigne[ic2]] >= Kp ) {
      /* On peut utiliser le terme */
      CIndiceLigne[ic1] = CIndiceLigne[ic2];
			NbTerm++;
			ic2--;
			break;
		}
    ic2--;
	}
}

if ( NbTerm != Matrice->CNbTermMatriceActive[Colonne] ) {
  printf("BUG dans LU_SupprimerTermesInutilesDansColonne  NbTerm %d  CNbTermMatriceActive %d\n",
	        NbTerm, Matrice->CNbTermMatriceActive[Colonne]);  
}
Matrice->CNbTerm[Colonne] = NbTerm;

return;
}

/*--------------------------------------------------------------------------------------------------*/

void LU_InitMinMarkowitzDeColonne( MATRICE * Matrice , int Colonne , int * LigneDeMarkowitz , double * ValeurMarkowitz )
{
int ic; int icDeb; int ilDeb; int il; int Nb; int * CIndiceLigne; double MinMarkowitz; int Ligne;
double Y; int Select; int LigneDeMinMarkowitz; double SeuilMarkowitz; int NbTMinLig; int NbTLig;
double Ysv; double AbsMaxLig; int NombreDeTermes; int * LIndiceColonne; int * LNbTerm; int * Ldeb;
double * Elm; int * IndiceColonne;

SUPER_LIGNE_DE_LA_MATRICE * SuperLigne; int i  ; char UtiliserLesSuperLignes;
SUPER_LIGNE_DE_LA_MATRICE ** SuperLigneDeLaLigne; 

UtiliserLesSuperLignes = Matrice->UtiliserLesSuperLignes;
SuperLigneDeLaLigne    = Matrice->SuperLigneDeLaLigne;

*LigneDeMarkowitz = -1;
*ValeurMarkowitz  = Matrice->RangAuCarrePlus1 + 1; /* Car on teste une egalite dans selection pivot */

/* On cherche le meilleur puis on verifie les conditions de stabilite. Si elle ne sont pas verifiees alors on fait
   le calcul complet */
LigneDeMinMarkowitz = -1;
NbTMinLig           = Matrice->Rang + 10;
MinMarkowitz        = NbTMinLig;

Ldeb           = Matrice->Ldeb;
LNbTerm        = Matrice->LNbTerm;
LIndiceColonne = Matrice->LIndiceColonne;
Elm            = Matrice->Elm;

icDeb          = Matrice->Cdeb[Colonne];
NombreDeTermes = Matrice->CNbTerm[Colonne];  
CIndiceLigne   = Matrice->CIndiceLigne;

Nb = NombreDeTermes - 1;

if ( NombreDeTermes != Matrice->CNbTermMatriceActive[Colonne] ) {
  LU_SupprimerTermesInutilesDansColonne( Matrice, Colonne, icDeb );
  NombreDeTermes = Matrice->CNbTerm[Colonne];
  Nb = NombreDeTermes - 1;
}

for ( ic = icDeb ; ic < icDeb + NombreDeTermes; ic++ ) {
  Ligne = CIndiceLigne[ic];	
  if ( UtiliserLesSuperLignes == OUI_LU) {
    if ( SuperLigneDeLaLigne[Ligne] != NULL ) {					      
      NbTLig = SuperLigneDeLaLigne[Ligne]->NombreDeTermes - 1;  
    }
    else {
      NbTLig = LNbTerm[Ligne] - 1;  
    }    
  }
  else {
    NbTLig = LNbTerm[Ligne] - 1;
  }
  
  if ( NbTLig < NbTMinLig ) {						        
    MinMarkowitz        = Nb * NbTLig; 
    LigneDeMinMarkowitz = Ligne; 
    NbTMinLig           = NbTLig;
  }
}

if ( LigneDeMinMarkowitz >= 0 ) {
  /* Verification de la condition de stabilite */
  AbsMaxLig = Matrice->AbsDuPlusGrandTermeDeLaLigne[LigneDeMinMarkowitz];  
  if ( AbsMaxLig < 0. ) {  
    LU_PlusGrandTermeDeLaLigne( Matrice , LigneDeMinMarkowitz , Colonne , &il);
    AbsMaxLig = Matrice->AbsDuPlusGrandTermeDeLaLigne[LigneDeMinMarkowitz];
    if ( AbsMaxLig < 0. ) goto RechercheComplete;
  }
  else { 
    /* Recherche de l'index auquel on trouve la valeur du terme pivot potentiel */
    if ( UtiliserLesSuperLignes == OUI_LU) {
      if ( SuperLigneDeLaLigne[LigneDeMinMarkowitz] != NULL ) {
				ilDeb = 0;
        IndiceColonne = SuperLigneDeLaLigne[LigneDeMinMarkowitz]->IndiceColonne;
      }
      else {
			  ilDeb = Ldeb[LigneDeMinMarkowitz];
				IndiceColonne = LIndiceColonne;
      }      
    }
    else {
			ilDeb = Ldeb[LigneDeMinMarkowitz];
			IndiceColonne = LIndiceColonne;
    }
    for ( il = ilDeb ; IndiceColonne[il] != Colonne ; il++ );    
  }
  SeuilMarkowitz = -1.;
  if ( Nb > 0 ) {
    SeuilMarkowitz = Matrice->SeuilDePivotage * AbsMaxLig;    
    /* Comme on ne compare pas les pivots a meme nombre de Markowitz, on augmente
       un peu le seuil */
    SeuilMarkowitz*= 1.5;
  }

  if ( UtiliserLesSuperLignes == OUI_LU) {  
    if ( SuperLigneDeLaLigne[LigneDeMinMarkowitz] != NULL ) {
      SuperLigne = SuperLigneDeLaLigne[LigneDeMinMarkowitz];
      il = SuperLigne->CapaciteDesColonnes * il;
      for ( i = 0 ; i < SuperLigne->NombreDeLignesDeLaSuperLigne ; i++ ) {
        if ( SuperLigne->NumerosDesLignesDeLaSuperLigne[i] == LigneDeMinMarkowitz ) {
          il+= i;
	        break;
	      }
      }
      Y = fabs( SuperLigne->ElmColonneDeSuperLigne[il] );   
    }
    else {
      Y = fabs( Elm[il] );   
    }      
  }
  else {
    Y = fabs( Elm[il] );
  }
   
  if ( Y > SeuilMarkowitz && Y > Matrice->PivotMin ) {
    /* Condition de stabilite verifiee, on peut enteriner le choix */   
    goto FinCalculMinMarkowitzColonne;    
  }  
}

RechercheComplete:
/* Condition de stabilite pas verifiee, on repart sur un calcul complet */
LigneDeMinMarkowitz = -1;
NbTMinLig           = Matrice->Rang + 10;
Ysv   = -1.;

for ( ic = icDeb ; ic < icDeb + NombreDeTermes; ic++ ) {
  Ligne = CIndiceLigne[ic];
  
  if ( UtiliserLesSuperLignes == OUI_LU ) {  
    if ( SuperLigneDeLaLigne[Ligne] != NULL ) {					      
      NbTLig = SuperLigneDeLaLigne[Ligne]->NombreDeTermes - 1;  
    }
    else {
      NbTLig = LNbTerm[Ligne] - 1;  
    }    
  }
  else {
    NbTLig = LNbTerm[Ligne] - 1;  
  }
  
  if ( NbTLig <= NbTMinLig ) {    
    AbsMaxLig = Matrice->AbsDuPlusGrandTermeDeLaLigne[Ligne]; 
    if ( AbsMaxLig < 0. ) {
      LU_PlusGrandTermeDeLaLigne( Matrice , Ligne , Colonne , &il);
      AbsMaxLig = Matrice->AbsDuPlusGrandTermeDeLaLigne[Ligne];
      if ( AbsMaxLig < 0. ) continue;
    }
    else { 
      /* Recherche de l'index auquel on trouve la valeur du terme pivot potentiel */
      if ( UtiliserLesSuperLignes == OUI_LU ) {  
        if ( SuperLigneDeLaLigne[Ligne] != NULL ) {
					ilDeb = 0;   
          IndiceColonne = SuperLigneDeLaLigne[Ligne]->IndiceColonne;
        }
        else {
			    ilDeb = Ldeb[Ligne];
				 	IndiceColonne = LIndiceColonne;
        }      
      }
      else {
			  ilDeb = Ldeb[Ligne];
			  IndiceColonne = LIndiceColonne;
      }
      for ( il = ilDeb ; IndiceColonne[il] != Colonne ; il++ );      
    }
    SeuilMarkowitz = -1.;
    if ( Nb > 0 ) SeuilMarkowitz = Matrice->SeuilDePivotage * AbsMaxLig;

    if ( UtiliserLesSuperLignes == OUI_LU ) {  
      if ( SuperLigneDeLaLigne[Ligne] != NULL ) {
        SuperLigne = SuperLigneDeLaLigne[Ligne];
        il = SuperLigne->CapaciteDesColonnes * il;
        for ( i = 0 ; i < SuperLigne->NombreDeLignesDeLaSuperLigne ; i++ ) {
          if ( SuperLigne->NumerosDesLignesDeLaSuperLigne[i] == Ligne ) {
            il+= i;
	          break;
	        }
        }
        Y = fabs( SuperLigne->ElmColonneDeSuperLigne[il] );   
      }
      else {
				Y = fabs( Elm[il] );   
      }      
    }
    else {
			Y = fabs( Elm[il] );   
    }
    
    if ( Y > SeuilMarkowitz && Y > Matrice->PivotMin ) {
      Select = OUI_LU;      
      if ( NbTLig == NbTMinLig ) {
        if ( Y < Ysv ) Select = NON_LU; 
      }      
      /*    */
      if ( Select == OUI_LU ) {
        MinMarkowitz        = Nb * NbTLig; 
        LigneDeMinMarkowitz = Ligne; 
        NbTMinLig           = NbTLig;
        Ysv                 = Y;
      }
    }
  }
}

FinCalculMinMarkowitzColonne:
if ( LigneDeMinMarkowitz >= 0 ) {
  *LigneDeMarkowitz = LigneDeMinMarkowitz;
  *ValeurMarkowitz  = MinMarkowitz;
}   
else {
  if ( Matrice->ExclureLesEchecsMarkowitz == OUI_LU ) {
    Matrice->ColonneRejeteeTemporairementPourPivotage[Colonne] = OUI_LU;
  }
}

return;
}

