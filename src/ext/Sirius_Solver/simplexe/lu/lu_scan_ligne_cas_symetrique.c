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

   FONCTION: Decomposition LU cas symetrique.               
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"  

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

/*--------------------------------------------------------------------------------------------------*/
/* Les termes de la ligne pivot sont compares a ceux de la ligne scannee et les nouvelle valeurs 
   des termes de la ligne scannee sont calculees */

void LU_ScanLigneCasSymetrique( MATRICE * Matrice ,
                                int   Ligne,
									              int   ilDebLignePivot,
				                        int   NbNonuLignePivot, 
                                double ValTermeColonnePivot,
                                int   LignePivot,				                       
                                char * StockageLignesRedimensionne																
                              ) 
{
int il; int ilDeb; double * W; char * T; int Demande; int NbTermesLigne; int NbTermesLignePivot; int Colonne;
int f; char UtiliserLesSuperLignes;  unsigned int * HashCode; int * PoidsDesColonnes; unsigned int DeltaHashCode;
int * LIndiceColonne; double * Elm; int ilLigne;

W = Matrice->W;
T = Matrice->Marqueur;

UtiliserLesSuperLignes = Matrice->UtiliserLesSuperLignes;

ilDeb          = Matrice->Ldeb[Ligne];
NbTermesLigne  = Matrice->LNbTerm[Ligne];
LIndiceColonne = Matrice->LIndiceColonne;
Elm            = Matrice->Elm;

f = NbNonuLignePivot; /* Nombre de termes de la ligne pivot moins le terme pivot */

if ( UtiliserLesSuperLignes == OUI_LU ) {
  HashCode         = &(Matrice->HashCodeLigne[Ligne]);    
  PoidsDesColonnes = Matrice->PoidsDesColonnes;
  DeltaHashCode    = 0;	
}

/* Transformation des termes de la ligne scannee */
il = ilDeb;
Colonne = LIndiceColonne[il];
for ( ; Colonne != LignePivot ; il++ , Colonne = LIndiceColonne[il] ) {
  if ( T[Colonne] == 1 ) {
    T[Colonne] = 0; 
    /* Calcul du terme modifie */
    Elm[il]-= W[Colonne] * ValTermeColonnePivot; 
    /* Comme il y a deja un terme dans la ligne scannee, il n'y a pas de "fill-in" a cet endroit */
    f--;
    if ( f == 0 ) break;  
  }
}
if ( LIndiceColonne[il] == LignePivot ) {
  /* On est sorti pour avoir rencontre le terme colonne pivot */
  NbTermesLigne--;
  Elm           [il] = Elm[ilDeb + NbTermesLigne]; 
  LIndiceColonne[il] = LIndiceColonne[ilDeb + NbTermesLigne];

  if ( UtiliserLesSuperLignes == OUI_LU ) DeltaHashCode-= PoidsDesColonnes[LignePivot];

  for ( ; il < ilDeb + NbTermesLigne ; il++ ) {
    Colonne = LIndiceColonne[il];
    /* Si Colonne est egal a ColonnePivot alors on a T[Colonne] = 0 */
    if ( T[Colonne] == 1 ) {
      /* Calcul du terme modifie */
      Elm[il]-= W[LIndiceColonne[il]] * ValTermeColonnePivot; 
      T[LIndiceColonne[il]] = 0; 
      /* Comme il y a deja un terme dans la ligne scannee, il n'y a pas de "fill-in" a cet endroit */
      f--;
      if ( f == 0 ) break;  
    }
  }
}
else {
  /* On est sorti par f = 0 */
  il++;		
  for ( ; LIndiceColonne[il] != LignePivot ; il++ );	
  NbTermesLigne--;
  Elm           [il] = Elm[ilDeb + NbTermesLigne]; 
  LIndiceColonne[il] = LIndiceColonne[ilDeb + NbTermesLigne];

  if ( UtiliserLesSuperLignes == OUI_LU ) DeltaHashCode-= PoidsDesColonnes[LignePivot];
    
}

NbTermesLignePivot = NbNonuLignePivot + 1;

if ( f > 0 ) {
  /* Il y a un "fill-in" => on scanne la ligne pivot pour pouvoir calculer et 
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
  Matrice->NbFillIn+= f;
  Matrice->CompteurExclusionMarkowitz+= f;        
  Matrice->NombreDeTermes+= f;
	
	ilLigne = ilDeb + NbTermesLigne;
	
  /* On commence a 1 car terme diagonal range en premier */  
  for ( il = ilDebLignePivot + 1 ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
    Colonne = LIndiceColonne[il];
    if ( T[Colonne] == 1 ) {
      /* "Marqueur" est encore egal a 1 => il n'y avait pas de terme a cet IndiceColonne 
         dans la ligne scanne: on a trouve un "fill-in" */      
      Elm           [ilLigne] = -W[Colonne] * ValTermeColonnePivot;   
      LIndiceColonne[ilLigne] = Colonne;      
			ilLigne++;
      NbTermesLigne++;

      if ( UtiliserLesSuperLignes == OUI_LU ) DeltaHashCode+= PoidsDesColonnes[Colonne];

      f--;
      if ( f == 0 ) { il++; break;}            
    } 
    else T[Colonne] = 1;
  }
  while ( il < ilDebLignePivot + NbTermesLignePivot ) { 
    T[LIndiceColonne[il]] = 1;
    il++;
  }	
}
else {
  /* Pas de "fill-in" : on remet Matrice.Marqueur aux bonnes valeurs */
  /* Terme diagonal range en premier */
  for ( il = ilDebLignePivot + 1 ; il < ilDebLignePivot + NbTermesLignePivot ; il++ ) {
    T[LIndiceColonne[il]] = 1; 
  }
}
/* Il faut le laisser la car on a enleve le terme sur la colonne pivot */
Matrice->LNbTerm[Ligne] = NbTermesLigne;

if ( UtiliserLesSuperLignes == OUI_LU ) *HashCode = *HashCode + DeltaHashCode;

return;
}

