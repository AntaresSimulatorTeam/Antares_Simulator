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

   FONCTION: Calculs de classements hascode pour les super lignes
                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# if OUTILS_DE_GESTION_MEMOIRE_DE_PNE_SOLVEUR	
  # include "mem_fonctions.h"
#endif

/*--------------------------------------------------------------------------------------------------*/
/* Premier appel, on calcule les hashcode des lignes non encore eliminees.
   Il n'y a encore aucune super ligne  */

void LU_CreerLesHashCode( MATRICE * Matrice )
{
int Ligne; unsigned int hashCode; unsigned int * HashCodeLigne; int Colonne; 
char * TypeDeClassementHashCodeAFaire; int * PoidsDesColonnes; int * OrdreLigne; int Kp; int il;
int ilMax; int * Ldeb; int * LNbTerm; int * LIndiceColonne;

OrdreLigne = Matrice->OrdreLigne;

Ldeb           = Matrice->Ldeb;
LNbTerm        = Matrice->LNbTerm;
LIndiceColonne = Matrice->LIndiceColonne;

PoidsDesColonnes = Matrice->PoidsDesColonnes;

HashCodeLigne                  = Matrice->HashCodeLigne;
TypeDeClassementHashCodeAFaire = Matrice->TypeDeClassementHashCodeAFaire;

for ( Kp = Matrice->Kp ; Kp < Matrice->Rang ; Kp++ ) {
  Ligne = OrdreLigne[Kp];	
	il = Ldeb[Ligne];
	ilMax = il + LNbTerm[Ligne];
  hashCode = 0;	
	while ( il < ilMax ) {
    Colonne = LIndiceColonne[il];
    hashCode += PoidsDesColonnes[Colonne];
    il++;
	}
  HashCodeLigne[Ligne] = hashCode;
  /* Normalement c'est deja le cas */
  TypeDeClassementHashCodeAFaire[Ligne] = ELEMENT_HASCODE_A_CLASSER;  
}

return;
}

/*--------------------------------------------------------------------------------------------------*/
/* Au premier passage on classe toutes les lignes qui n'ont pas encore ete eliminees */

void LU_ClasserLesElementsEnFonctionDuHashCode( MATRICE * Matrice )
{
int HashModuloSize; int k  ;  int i; int Element             ;
unsigned int * HashCodeLigne; unsigned int * HashCodeSuperLigne; int hashModulo; int hashModuloSuiv;

int * HashNbModuloIdentiques; int * HashModuloPrec; int * HashModuloPrem;
int * HashModuloSuiv        ; char * TypeDeClassementHashCodeAFaire;

HashModuloSize = Matrice->HashModuloSize;

HashCodeLigne      = Matrice->HashCodeLigne;
HashCodeSuperLigne = Matrice->HashCodeSuperLigne;

HashNbModuloIdentiques = Matrice->HashNbModuloIdentiques;    
HashModuloPrec = Matrice->HashModuloPrec;
HashModuloPrem = Matrice->HashModuloPrem;
HashModuloSuiv = Matrice->HashModuloSuiv;
TypeDeClassementHashCodeAFaire = Matrice->TypeDeClassementHashCodeAFaire;

for ( k = 0 ; k < HashModuloSize ; k++ ) {
  HashModuloPrem        [k] = -1;
  HashNbModuloIdentiques[k] = 0;
}

/* Classement des lignes */
Element = 0;
for ( i = 0 ; i < Matrice->Rang ; i++ ) { 
  if ( TypeDeClassementHashCodeAFaire[Element] == ELEMENT_HASCODE_A_DECLASSER ||
       TypeDeClassementHashCodeAFaire[Element] == ELEMENT_HASCODE_A_NE_PAS_CLASSER ) {
    /* Concerne les lignes deja eliminees */
    TypeDeClassementHashCodeAFaire[Element] = ELEMENT_HASCODE_A_NE_PAS_CLASSER;   
    Element++;
    continue;
  }
  TypeDeClassementHashCodeAFaire[Element] = ELEMENT_HASCODE_CLASSE;   
   
 /* Trouver autre chose comme test */
  hashModulo = HashCodeLigne[i] % HashModuloSize;
  
  hashModuloSuiv = HashModuloPrem[hashModulo];
  HashNbModuloIdentiques[hashModulo]++;    
  
  HashModuloPrem[hashModulo] = Element;
    
  HashModuloSuiv[Element] = hashModuloSuiv;
  
  if ( hashModuloSuiv >= 0 ) HashModuloPrec[hashModuloSuiv] = Element;

  Matrice->HashModuloPrec[Element] = -(hashModulo+2); 
  
  Element++;
  
}
/* Classement des super lignes */
Element = Matrice->Rang;
for ( i = 0 ; i < Matrice->NombreDeSuperLignes ; i++ ) {
  if ( TypeDeClassementHashCodeAFaire[Element] == ELEMENT_HASCODE_A_DECLASSER ) {
    TypeDeClassementHashCodeAFaire[Element] = ELEMENT_HASCODE_A_NE_PAS_CLASSER;   
    Element++;
    continue;
  }
  TypeDeClassementHashCodeAFaire[Element] = ELEMENT_HASCODE_CLASSE;   
  
  hashModulo = HashCodeSuperLigne[i] % HashModuloSize;
  
  hashModuloSuiv = HashModuloPrem[hashModulo];
  HashNbModuloIdentiques[hashModulo]++;    
  
  HashModuloPrem[hashModulo] = Element;
    
  HashModuloSuiv[Element] = hashModuloSuiv;
  
  if ( hashModuloSuiv >= 0 ) HashModuloPrec[hashModuloSuiv] = Element;
  
  Matrice->HashModuloPrec[Element] = -(hashModulo+2); 

  Element++;
  
}

return;
}

/*--------------------------------------------------------------------------------------------------*/

void LU_MajClassementDesElementsEnFonctionDuHashCode( MATRICE * Matrice )
{
int i; char AFaire; int Element; char * TypeDeClassementHashCodeAFaire; 

TypeDeClassementHashCodeAFaire = Matrice->TypeDeClassementHashCodeAFaire;

/* Declassement / Classement des lignes */
Element = 0;
for ( i = 0 ; i < Matrice->Rang ; i++ ) {
  AFaire = TypeDeClassementHashCodeAFaire[Element];  
  if ( AFaire == ELEMENT_HASCODE_A_NE_PAS_CLASSER ) {
    Element++;
    continue;
  }  
  if ( AFaire == ELEMENT_HASCODE_A_DECLASSER ) {    
    LU_DeClasserEnFonctionDuHashCode( Matrice , Element );
    TypeDeClassementHashCodeAFaire[Element] = ELEMENT_HASCODE_A_NE_PAS_CLASSER;
    Element++;
    continue;  
  }
  if ( AFaire == ELEMENT_HASCODE_A_CLASSER ) {  
    LU_ClasserEnFonctionDuHashCode( Matrice , Element );
    TypeDeClassementHashCodeAFaire[Element] = ELEMENT_HASCODE_CLASSE;    
    Element++;
    continue;
  }      
  if ( AFaire == ELEMENT_HASCODE_A_RECLASSER ) {  
    LU_DeClasserEnFonctionDuHashCode( Matrice , Element );
    LU_ClasserEnFonctionDuHashCode( Matrice , Element );
    TypeDeClassementHashCodeAFaire[Element] = ELEMENT_HASCODE_CLASSE;    
    Element++;
    continue;
  }  
  Element++;
}
/* Classement des super lignes */
Element = Matrice->Rang;
for ( i = 0 ; i < Matrice->NombreDeSuperLignes ; i++ ) {
  AFaire = TypeDeClassementHashCodeAFaire[Element];
  if ( AFaire == ELEMENT_HASCODE_A_NE_PAS_CLASSER ) {
    Element++;
    continue;
  }
  if ( AFaire == ELEMENT_HASCODE_A_DECLASSER ) {
    LU_DeClasserEnFonctionDuHashCode( Matrice , Element );
    TypeDeClassementHashCodeAFaire[Element] = ELEMENT_HASCODE_A_NE_PAS_CLASSER;
    Element++;
    continue;  
  }
  if ( AFaire == ELEMENT_HASCODE_A_CLASSER ) {
    LU_ClasserEnFonctionDuHashCode( Matrice , Element );
    TypeDeClassementHashCodeAFaire[Element] = ELEMENT_HASCODE_CLASSE;    
    Element++;
    continue;
  }    
  if ( AFaire == ELEMENT_HASCODE_A_RECLASSER ) {
    LU_DeClasserEnFonctionDuHashCode( Matrice , Element );
    LU_ClasserEnFonctionDuHashCode( Matrice , Element );
    TypeDeClassementHashCodeAFaire[Element] = ELEMENT_HASCODE_CLASSE;    
    Element++;
    continue;
  }
  Element++;
}

return;
}

/*--------------------------------------------------------------------------------------------------*/

void LU_ClasserEnFonctionDuHashCode( MATRICE * Matrice , int Element )
{
int HashModulo; int HashModuloSuiv;

if ( Element < Matrice->Rang ) {
  HashModulo = Matrice->HashCodeLigne[Element] % Matrice->HashModuloSize;
}
else {
  HashModulo = Matrice->HashCodeSuperLigne[Element-Matrice->Rang] % Matrice->HashModuloSize;
}

HashModuloSuiv = Matrice->HashModuloPrem[HashModulo];
Matrice->HashNbModuloIdentiques[HashModulo]++;

Matrice->HashModuloPrem[HashModulo] = Element;
Matrice->HashModuloSuiv[Element]    = HashModuloSuiv;

if ( HashModuloSuiv >= 0 ) Matrice->HashModuloPrec[HashModuloSuiv] = Element;
Matrice->HashModuloPrec[Element] = -(HashModulo+2); 

return;
}

/*--------------------------------------------------------------------------------------------------*/

void LU_DeClasserEnFonctionDuHashCode( MATRICE * Matrice , int Element )
{
int HashModuloPrec; int HashModulo; int HashModuloSuiv;

HashModuloPrec = Matrice->HashModuloPrec[Element];

if ( HashModuloPrec < 0 ) {
  HashModulo = -HashModuloPrec;
  HashModulo-=2;
  /* On veut supprimer le premier terme */
  Matrice->HashNbModuloIdentiques[HashModulo]--;  
  Matrice->HashModuloPrem[HashModulo] = Matrice->HashModuloSuiv[Element];  
  Matrice->HashModuloPrec[Element] = -1;
  if ( Matrice->HashModuloSuiv[Element] >= 0 ) {
    Matrice->HashModuloPrec[Matrice->HashModuloSuiv[Element]] = -(HashModulo+2);
  }
  goto FinDeclassement;
}

HashModuloSuiv = Matrice->HashModuloSuiv[Element];
Matrice->HashModuloSuiv[HashModuloPrec] = HashModuloSuiv;
if ( HashModuloSuiv >= 0 ) Matrice->HashModuloPrec[HashModuloSuiv] = HashModuloPrec;

/* Ce n'est pas le premier element et on veut retrouver le HashModulo du groupe */
while ( HashModuloPrec >= 0 ) {
  HashModuloPrec = Matrice->HashModuloPrec[HashModuloPrec];
}

HashModulo = -HashModuloPrec;
HashModulo-=2;
Matrice->HashNbModuloIdentiques[HashModulo]--;  

Matrice->HashModuloPrec[Element] = -1;

FinDeclassement:

return;
}

