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
             suite a l'elimination de la ligne pivot.                
 
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

void LU_ScanLigne( MATRICE * Matrice,  
                   int   Ligne,
									 int   ilDebLignePivot,
                   int   NbNonuLignePivot, 
                   double ValTermeColonnePivot,									 
		               int   LignePivot,									  									 
									 char * StockageLignesRedimensionne,
									 char * StockageColonnesRedimensionne									 
                  ) 
{
int il; int ilDeb; int ilLigne; int f; int Demande; int NbTermesLigne; int NbTermesLignePivot;
int Colonne; int NbTermesColonne; char UtiliserLesSuperLignes; int * CNbTermMatriceActive;
double * W; char * T; char Augmenter;
int * LIndiceColonne; double * Elm;
int * Cdeb; int * CNbTerm; int * CIndiceLigne; int * CDernierPossible;
	
unsigned int * HashCode; int * PoidsDesColonnes; unsigned int DeltaHashCode;

UtiliserLesSuperLignes = Matrice->UtiliserLesSuperLignes;

ilDeb          = Matrice->Ldeb[Ligne];
NbTermesLigne  = Matrice->LNbTerm[Ligne];
LIndiceColonne = Matrice->LIndiceColonne;
Elm            = Matrice->Elm;
   
W = Matrice->W;
T = Matrice->Marqueur;

*StockageLignesRedimensionne = NON_LU;
*StockageColonnesRedimensionne = NON_LU;

/* f sert a mesurer le "fill-in" */
f = NbNonuLignePivot; /* Nombre de termes de la ligne pivot moins le terme pivot */

/* Transformation des termes de la ligne scannee */				
for ( il = ilDeb ; il < ilDeb + NbTermesLigne ; il++ ) {
  Colonne = LIndiceColonne[il];  
  /* Si Colonne est egal a ColonnePivot alors on a Matrice.Marqueur[Colonne] = 0 */
  if ( T[Colonne] == 1 ) {
    T[Colonne] = 0; 
    /* Calcul du terme modifie */
    Elm[il]-= W[Colonne] * ValTermeColonnePivot;		
    /* Comme il y a deja un terme dans la ligne scannee, il n'y a pas de "fill-in" a cet endroit */
    f--;
    if ( f == 0 ) goto ControleFillIn;  
  }
}

/* Controle du fill in */
ControleFillIn:

NbTermesLignePivot = NbNonuLignePivot + 1;

if ( f > 0 ) {
  if ( UtiliserLesSuperLignes == OUI_LU ) {
    HashCode         = &(Matrice->HashCodeLigne[Ligne]);    
    PoidsDesColonnes = Matrice->PoidsDesColonnes;
    DeltaHashCode    = 0;
  }
  
  Matrice->NbFillIn += f;
  Matrice->CompteurExclusionMarkowitz += f;      
  Matrice->NombreDeTermes += f;
  	
  /* Il y a un "fill-in" de f termes => on scanne la ligne pivot pour pouvoir calculer et 
     stocker les termes crees . Les termes crees sont chaines en fin de ligne */
  if ( ilDeb + NbTermesLigne + f - 1 > Matrice->LDernierPossible[Ligne] ) {		
    Demande = NbTermesLigne + f;
    LU_AugmenterLaTailleDeLaMatriceActive( Matrice , Ligne , Demande );
    *StockageLignesRedimensionne = OUI_LU;		
    ilDeb           = Matrice->Ldeb[Ligne];
    LIndiceColonne  = Matrice->LIndiceColonne;
    Elm             = Matrice->Elm;		
		ilDebLignePivot = Matrice->Ldeb[LignePivot];			
  }

	ilLigne = ilDeb + NbTermesLigne;

	Cdeb                 = Matrice->Cdeb;
  CNbTerm              = Matrice->CNbTerm;
  CNbTermMatriceActive = Matrice->CNbTermMatriceActive;
  CIndiceLigne         = Matrice->CIndiceLigne;  
  CDernierPossible     = Matrice->CDernierPossible; 
	
  for ( il = ilDebLignePivot ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
    Colonne = LIndiceColonne[il];
    if ( T[Colonne] == 1 ) {				 			
		  /* On ajoute un terme dans la ligne */			
      Elm           [ilLigne] = -W[Colonne] * ValTermeColonnePivot;			
      LIndiceColonne[ilLigne] = Colonne;
			ilLigne++;
      NbTermesLigne++;
			
      /* "Marqueur" est encore egal a 1 => il n'y avait pas de terme a cet IndiceColonne 
         dans la ligne scannee: on a trouve un "fill-in" */	 				
			NbTermesColonne = CNbTerm[Colonne];
      if ( Cdeb[Colonne] + NbTermesColonne > CDernierPossible[Colonne] ) {
        Augmenter = OUI_LU;
        /* On verifie d'abord si en compactant la colonne il reste de la place */
        if ( NbTermesColonne != CNbTermMatriceActive[Colonne] ) {
          LU_SupprimerTermesInutilesDansColonne( Matrice , Colonne , Cdeb[Colonne] );
				  /* Il faut reinitialiser car il est possible qu'on ait compacte la colonne */
			    NbTermesColonne = CNbTerm[Colonne];
          /* On refait le test afin d'augmenter la taille de la colonne si le compactage n'a pas suffit */
          if ( Cdeb[Colonne] + NbTermesColonne > CDernierPossible[Colonne] ) Augmenter = OUI_LU;
					else Augmenter = NON_LU;
        }
			  if ( Augmenter == OUI_LU ) {
          LU_AugmenterLaTailleDeLaMatriceActiveParColonne( Matrice, Colonne, NbTermesColonne + 1);
          *StockageColonnesRedimensionne = OUI_LU;			
          CIndiceLigne = Matrice->CIndiceLigne;
				  /* Il faut reinitialiser car il est possible qu'on ait compacte la colonne */
			    NbTermesColonne = CNbTerm[Colonne];
				}
      }

      if ( UtiliserLesSuperLignes == OUI_LU ) DeltaHashCode+= PoidsDesColonnes[Colonne];
           
      /* Mise a jour du chainage par colonne: le nouveau terme doit etre chaine dans le 
         chainage par colonne */
      CIndiceLigne[ Cdeb[Colonne] + NbTermesColonne] = Ligne;
			
      CNbTerm[Colonne]++;
      CNbTermMatriceActive[Colonne]++;		
      			
      f--;
      if ( f == 0 ) { il++; break;}      
    } 
    else T[Colonne] = 1;
  }
  while ( il < ilDebLignePivot + NbTermesLignePivot ) { 
    T[LIndiceColonne[il]] = 1; 
    il++;
  }
  Matrice->LNbTerm[Ligne] = NbTermesLigne;
  
  if ( UtiliserLesSuperLignes == OUI_LU ) *HashCode = *HashCode + DeltaHashCode;
  
}
else {
  /* Pas de "fill-in" : on remet Matrice.Marqueur aux bonnes valeurs */
  for ( il = ilDebLignePivot ; il != ilDebLignePivot + NbTermesLignePivot ; il++ ) {
    T[LIndiceColonne[il]] = 1; 
  }
}

return;
}

