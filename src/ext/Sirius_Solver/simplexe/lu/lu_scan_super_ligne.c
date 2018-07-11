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

   FONCTION: Decomposition LU de la base. Modification des lignes
             suite a l'elimination de la ligne pivot dans le cas ou
	     la ligne scannee est une SuperLigne.
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# if OUTILS_DE_GESTION_MEMOIRE_DE_PNE_SOLVEUR	
  # include "mem_fonctions.h"
#endif   

/*--------------------------------------------------------------------------------------------------*/
/* Les termes de la ligne pivot sont compares a ceux de la ligne scannee et les nouvelle valeurs 
   des termes de la ligne scannee sont calculees */

void LU_ScanSuperLigne( MATRICE * Matrice,
                        SUPER_LIGNE_DE_LA_MATRICE * SuperLigne,
									      int   ilDebLignePivot,											
                        int   NbNonuLignePivot,
		 	                  double * ValeurDesTermesDeColonnePivot,
			                  unsigned int * hashCode			                  
                      )											
{
int il     ; int f    ; int i ; int NbTermesLignePivot; int NbTCol; unsigned int DeltaHashCode;
int CapaciteMinDemandee; int Colonne; double X  ; int ic; int * PoidsDesColonnes;
double * W; char * T; double * ElmColonneDeSuperLigne  ; int * IndiceColonne; int CapaciteDesColonnes;
int NombreDeLignesDeLaSuperLigne; int * NumerosDesLignesDeLaSuperLigne; int * CNbTermMatriceActive;
int NombreDeTermes; int icDeb; char Augmenter;
int * LIndiceColonne; int * Cdeb; int * CNbTerm; int * CIndiceLigne; int * CDernierPossible;

W = Matrice->W;
T = Matrice->Marqueur;

LIndiceColonne = Matrice->LIndiceColonne;

ElmColonneDeSuperLigne       = SuperLigne->ElmColonneDeSuperLigne;
IndiceColonne                = SuperLigne->IndiceColonne;
CapaciteDesColonnes          = SuperLigne->CapaciteDesColonnes;
NombreDeLignesDeLaSuperLigne = SuperLigne->NombreDeLignesDeLaSuperLigne;
NombreDeTermes               = SuperLigne->NombreDeTermes;
  
/* f sert a mesurer le "fill-in" */
f = NbNonuLignePivot; /* Nombre de termes de la ligne pivot moins le terme pivot */

/* Transformation des termes de la ligne scannee */
for ( il = 0 ; il < NombreDeTermes ; il++ ) {
  Colonne = IndiceColonne[il];
  /* Si Colonne est egal a ColonnePivot alors on a Matrice.Marqueur[Colonne] = 0 */
  if ( T[Colonne] == 1 ) { 
    T[Colonne] = 0; 
    /* Calcul du terme modifie: on balaye la colonne de la SuperLigne */
    ic = CapaciteDesColonnes * il;  
    X  = W[Colonne];    
    for ( i = 0 ; i < NombreDeLignesDeLaSuperLigne ; i++ , ic++ ) {
      ElmColonneDeSuperLigne[ic]-= X * ValeurDesTermesDeColonnePivot[i]; 
    }            
    /* Comme il y a deja un terme dans la ligne scannee, il n'y a pas de "fill-in" a cet endroit */
    f--;
    if ( f == 0 ) goto ControleFillIn;  
  }
}

/* Controle du fill in */
ControleFillIn:

NbTermesLignePivot = NbNonuLignePivot + 1;

if ( f > 0 ) {

  DeltaHashCode = 0;

  Matrice->CompteurExclusionMarkowitz+= f;
  Matrice->NombreDeTermes+=             f * NombreDeLignesDeLaSuperLigne;
  Matrice->NbFillIn+=                   f * NombreDeLignesDeLaSuperLigne;   
     	
	Cdeb                 = Matrice->Cdeb;
  CNbTerm              = Matrice->CNbTerm;
  CNbTermMatriceActive = Matrice->CNbTermMatriceActive;
  CIndiceLigne         = Matrice->CIndiceLigne;  
  CDernierPossible     = Matrice->CDernierPossible;
	
  /* Il y a un "fill-in" => on scanne la ligne pivot pour pouvoir calculer et 
     stocker les termes crees . Les termes crees sont chaines en fin de ligne */
  if ( NombreDeTermes + f > SuperLigne->Capacite ) { 
    CapaciteMinDemandee = NombreDeTermes + f;     
    LU_AugmenterCapaciteDeSuperLigne( Matrice , SuperLigne , CapaciteMinDemandee );
    ElmColonneDeSuperLigne = SuperLigne->ElmColonneDeSuperLigne;
    IndiceColonne          = SuperLigne->IndiceColonne;    
  }
  
  NumerosDesLignesDeLaSuperLigne = SuperLigne->NumerosDesLignesDeLaSuperLigne; 
  PoidsDesColonnes = Matrice->PoidsDesColonnes;
  for ( il = ilDebLignePivot ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
    Colonne = LIndiceColonne[il];
    if ( T[Colonne] == 1 ) {
      /* "Marqueur" est encore egal a 1 => il n'y avait pas de terme a cet IndiceColonne 
         dans la SuperLigne scannee: on a trouve un "fill-in" */
      X = W[Colonne];    
      ic = CapaciteDesColonnes * NombreDeTermes;
      for ( i = 0 ; i < NombreDeLignesDeLaSuperLigne ; i++ , ic++ ) {
        ElmColonneDeSuperLigne[ic] = -X * ValeurDesTermesDeColonnePivot[i];	
      }
      IndiceColonne[NombreDeTermes] = Colonne;
      NombreDeTermes++;

      DeltaHashCode+= PoidsDesColonnes[Colonne];
			
      /* On met a jour le stockage par colonne pour y ajouter NombreDeLignesDeLaSuperLigne termes */
      NbTCol = CNbTerm[Colonne];
      if ( Cdeb[Colonne] + NbTCol + NombreDeLignesDeLaSuperLigne > CDernierPossible[Colonne] ) {

        Augmenter = OUI_LU;
        /* On verifie d'abord si en compactant la colonne il reste de la place */
        if ( NbTCol != CNbTermMatriceActive[Colonne] ) {
          LU_SupprimerTermesInutilesDansColonne( Matrice , Colonne , Cdeb[Colonne] );
				  /* Il faut reinitialiser car il est possible qu'on ait compacte la colonne */
			    NbTCol = CNbTerm[Colonne];
          /* On refait le test afin d'augmenter la taille de la colonne si le compactage n'a pas suffit */
					if ( Cdeb[Colonne] + NbTCol + NombreDeLignesDeLaSuperLigne > CDernierPossible[Colonne] ) Augmenter = OUI_LU;
					else Augmenter = NON_LU;
        }

			  if ( Augmenter == OUI_LU ) {			
          LU_AugmenterLaTailleDeLaMatriceActiveParColonne( Matrice, Colonne, NbTCol + NombreDeLignesDeLaSuperLigne );
          CIndiceLigne = Matrice->CIndiceLigne;
			    /* Il faut reinitialiser car il est possible qu'on ait compacte la colonne */
			    NbTCol = CNbTerm[Colonne];				
				}
      }
			icDeb = Cdeb[Colonne];
      for ( i = 0 ; i < NombreDeLignesDeLaSuperLigne ; i++ ) {
        /* On ajoute le terme dans la colonne */
        CIndiceLigne[icDeb + NbTCol] = NumerosDesLignesDeLaSuperLigne[i];
        NbTCol++;
      }
      CNbTerm[Colonne] = NbTCol;
      CNbTermMatriceActive[Colonne] = NbTCol;		
                  
      f--;
      if ( f <= 0 ) { il++; break;}
    } 
    else T[Colonne] = 1;
  }
  while ( il < ilDebLignePivot + NbTermesLignePivot ) { 
    T[LIndiceColonne[il]] = 1; 
    il++;
  }
  
  *hashCode = *hashCode + DeltaHashCode;
  
  SuperLigne->NombreDeTermes = NombreDeTermes;
  
}
else {
  /* Pas de "fill-in" : on remet Matrice.Marqueur aux bonnes valeurs */
  for ( il = ilDebLignePivot ; il != ilDebLignePivot + NbTermesLignePivot ; il++ ) {
    T[LIndiceColonne[il]] = 1; 
  }
}

return;
}

