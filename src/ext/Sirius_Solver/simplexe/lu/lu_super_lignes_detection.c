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

   FONCTION: Detection des super lignes
                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

/*--------------------------------------------------------------------------------------------------*/

void LU_RechercherUnGroupeDeLignes( MATRICE * Matrice, int * NbElements, int * NumeroDElement,
                                    int ElementRef   , int hashCodeRef , int NbTermesRef     ,
				                            int * T          , int * NbLignesAGrouper                ,
				                            int * NbSuperLignesAGrouper
				                          )				    
{ int Element; int il; int ilMax; int k; int NbEl; int Rang; int NbLignes; int NbSuperLignes;
SUPER_LIGNE_DE_LA_MATRICE * ptSuperLigne; 
int * HashModuloSuiv; unsigned int *  HashCodeLigne; unsigned int * HashCodeSuperLigne;
SUPER_LIGNE_DE_LA_MATRICE ** SuperLigne; int * SIndiceColonne;
int * Ldeb; int * LNbTerm; int * LIndiceColonne; 

Ldeb           = Matrice->Ldeb;
LNbTerm        = Matrice->LNbTerm;
LIndiceColonne = Matrice->LIndiceColonne;
  
HashModuloSuiv     = Matrice->HashModuloSuiv;
HashCodeLigne      = Matrice->HashCodeLigne;
HashCodeSuperLigne = Matrice->HashCodeSuperLigne;

SuperLigne       = Matrice->SuperLigne;

Rang = Matrice->Rang;

NbEl          = *NbElements;
NbLignes      = *NbLignesAGrouper;
NbSuperLignes = *NbSuperLignesAGrouper;

Element = HashModuloSuiv[ElementRef];
while ( Element >= 0 ) {  
  if ( Element < Rang ) {
    /* C'est une ligne */
    if ( HashCodeLigne[Element] != (unsigned int) hashCodeRef ) goto AutreElement;
				
    k = LNbTerm[Element];		
    if ( k != NbTermesRef ) goto AutreElement;
		
	  il = Ldeb[Element];
	  ilMax = il + k;
    while ( il < ilMax ) {		
      if ( T[LIndiceColonne[il]] == 0 ) goto AutreElement;   
		  il++;
		}		
  }    
  else {
    /* C'est une super ligne */
    if ( HashCodeSuperLigne[Element - Rang] != (unsigned int) hashCodeRef ) goto AutreElement;
    ptSuperLigne = SuperLigne[Element - Rang];
    k = ptSuperLigne->NombreDeTermes;
    if ( k != NbTermesRef ) goto AutreElement;
    SIndiceColonne = ptSuperLigne->IndiceColonne;      
    for ( il = 0 ; il < k ; il++ ) {
      if ( T[SIndiceColonne[il]] == 0 ) goto AutreElement;   
    }
  }       
  NumeroDElement[NbEl] = Element;
  NbEl++;

  if ( Element < Rang ) NbLignes++;
  else NbSuperLignes++;
    
  AutreElement:
  Element = HashModuloSuiv[Element];    
}

*NbElements            = NbEl;
*NbLignesAGrouper      = NbLignes;
*NbSuperLignesAGrouper = NbSuperLignes;

return;
}

/*--------------------------------------------------------------------------------------------------*/

void LU_DetectionSuperLignes( MATRICE * Matrice , char * SuperLignesReamenagees )

{ int k; int Ligne; int il; int * T; int Element; int ilS; int hashCodeRef; int NbTermesRef; int Rang; 
  int ii;
	
int * NumeroDElement; int NbElements; int ElementRef; int NumeroSuperLigne1; int NumeroSuperLigne2;
int NbSuperLignesAGrouper; int NbLignesAGrouper; int SeuilNbModuloIdentiques;

unsigned int * HashCodeLigne; unsigned int * HashCodeSuperLigne; int * IndiceColonne;
int * HashModuloPrem; int * HashNbModuloIdentiques; int * HashModuloSuiv; char * TypeDeClassementHashCodeAFaire;

SUPER_LIGNE_DE_LA_MATRICE * SuperLigne ; 
SUPER_LIGNE_DE_LA_MATRICE ** SuperLigneDeLaMatrice;
SUPER_LIGNE_DE_LA_MATRICE * ptSuperLigne;

int ilMax; int * Ldeb; int * LNbTerm; int * LIndiceColonne; int ilDeb;

Ldeb           = Matrice->Ldeb;
LNbTerm        = Matrice->LNbTerm;
LIndiceColonne = Matrice->LIndiceColonne;

/* Classement de toutes les lignes en fonction de leur hashcode */
if ( Matrice->NombreDePassagesDansSuperLignes == 0 ) {
  LU_CreerLesHashCode( Matrice );
  LU_ClasserLesElementsEnFonctionDuHashCode( Matrice );
}
else {
  LU_MajClassementDesElementsEnFonctionDuHashCode( Matrice );
}
Matrice->NombreDePassagesDansSuperLignes++;
*SuperLignesReamenagees = NON_LU;

Rang = Matrice->Rang;

T = (int *) Matrice->W;
memset( (char *) T , 0 , Matrice->Rang * sizeof( int ) );

HashCodeLigne          = Matrice->HashCodeLigne;
HashCodeSuperLigne     = Matrice->HashCodeSuperLigne;
HashModuloPrem         = Matrice->HashModuloPrem;
HashNbModuloIdentiques = Matrice->HashNbModuloIdentiques;
HashModuloSuiv         = Matrice->HashModuloSuiv;
TypeDeClassementHashCodeAFaire = Matrice->TypeDeClassementHashCodeAFaire;

SuperLigneDeLaMatrice = Matrice->SuperLigne;

NumeroDElement = (int *) Matrice->SolutionIntermediaire;

if ( Matrice->NombreDeSuperLignes <= 0 ) {
  SeuilNbModuloIdentiques = Matrice->SeuilNombreDeLignesAGrouper;
}
else {
  if ( Matrice->SeuilNombreDeSuperLigneAGrouper < Matrice->SeuilNombreDeLignesAGrouper ) {
    SeuilNbModuloIdentiques = Matrice->SeuilNombreDeSuperLigneAGrouper;
  }
  else {
    SeuilNbModuloIdentiques = Matrice->SeuilNombreDeLignesAGrouper;
  }
}

for ( k = 0 ; k < Matrice->HashModuloSize ; k++ ) {
  if ( HashNbModuloIdentiques[k] < SeuilNbModuloIdentiques ) continue; 
  Element = HashModuloPrem[k];
  while ( Element >= 0 ) {  
    NumeroDElement[0] = Element;        
    NbElements        = 1; 
    ElementRef        = Element;
    NbLignesAGrouper      = 0;
    NbSuperLignesAGrouper = 0;    
   
    if ( Element < Rang ) {
      /* C'est une ligne */
      hashCodeRef   = HashCodeLigne[Element];			
	    ilDeb = Ldeb[Element];
      NbTermesRef   = LNbTerm[Element];
			IndiceColonne = LIndiceColonne;
			il    = ilDeb;
	    ilMax = il + NbTermesRef;
      NbLignesAGrouper++;
    }    
    else {    
      /* C'est une super ligne */
      hashCodeRef   = HashCodeSuperLigne[Element - Rang];
      ptSuperLigne  = SuperLigneDeLaMatrice[Element - Rang];
      NbTermesRef   = ptSuperLigne->NombreDeTermes;
      IndiceColonne = ptSuperLigne->IndiceColonne;
			ilDeb = 0;
			ilMax = NbTermesRef;
      NbSuperLignesAGrouper++;
    }

    for ( il = ilDeb , ii = 0 ; il < ilMax ; il++ , ii++ ) T[IndiceColonne[il]] = ii + 1;

    LU_RechercherUnGroupeDeLignes( Matrice   , &NbElements, NumeroDElement,
                                   ElementRef, hashCodeRef, NbTermesRef   , T,
				                           &NbLignesAGrouper , &NbSuperLignesAGrouper );	
				   
    /* Il est imperatif d'avoir plus de 1 ligne dans SuperLigne a cause de LU_SelectionPivot */
    SuperLigne = NULL;

    if ( NbSuperLignesAGrouper == 0 ) {    
      if ( NbLignesAGrouper >= Matrice->SeuilNombreDeLignesAGrouper ) {
        *SuperLignesReamenagees = OUI_LU;
	      /*
	      printf("Creation de la super ligne %d a %d lignes\n",Matrice->NombreDeSuperLignes,NbElements);
       	*/
        SuperLigne = LU_CreerUneSuperLigne( Matrice , T , NbElements , NumeroDElement , NbTermesRef );
	
        /* Il faut reinitialiser T avant de liberer IndiceColonne */
				
        for ( il = ilDeb ; il < ilMax ; il++ ) T[IndiceColonne[il]] = 0; 

				
	      /* Il faut dechainer les lignes sauf le premier element car c'est l'element courant de la boucle */		
	      for ( il = 1 ; il < NbElements ; il++ ) {
	        /* Il faut dechainer la ligne */
	        Ligne = NumeroDElement[il];	  	  
          LU_DeClasserEnFonctionDuHashCode( Matrice , Ligne );

	        TypeDeClassementHashCodeAFaire[Ligne] = ELEMENT_HASCODE_A_NE_PAS_CLASSER;   

          /* La ligne a ete mise dans la super ligne, on peut liberer de la place dans la structure ligne */  
          /* On ne libere pas la ligne pour pouvoir la reutiliser plus tard dans Ldeb etc .. */														
	      }
	
	      TypeDeClassementHashCodeAFaire[NumeroDElement[0]] = ELEMENT_HASCODE_A_DECLASSER;
	
	     /* Il est inutile de classer la super ligne pour ce coup-ci. Cependant on positionne son
	        indicateur comme etant a classe */
        il = Matrice->NombreDeSuperLignes - 1; /* c'est le numero de la super ligne que l'on vient de creer */
	      il+= Rang;
        TypeDeClassementHashCodeAFaire[il] = ELEMENT_HASCODE_A_CLASSER;
        goto ElementSuivant;      	
      }
      goto RazT;      
    }
    /* Il y a des superlignes a grouper */
    if ( NbLignesAGrouper == 0 ) {
      /* Que des superlignes */
      if ( NbSuperLignesAGrouper >= Matrice->SeuilNombreDeSuperLigneAGrouper ) {
        *SuperLignesReamenagees = OUI_LU;
        /* Dans ce cas on fusionne toutes les super lignes */      
        il = 0;
        NumeroSuperLigne1 = NumeroDElement[il] - Rang;
	      il++;
	      for ( ; il < NbElements ; il++ ) {
          NumeroSuperLigne2 = NumeroDElement[il] - Rang;
	        /*
	        printf("QUE_DES_SUPER_LIGNES Fusion de la super ligne %d dans la super ligne %d\n",NumeroSuperLigne2,NumeroSuperLigne1);
	        */
	        LU_FusionnerDeuxSuperLignes( Matrice, T, NumeroSuperLigne1, NumeroSuperLigne2 );	  
	        /* On ne doit pas declasser le premier element (celui pour lequel il = 0 ) car c'est l'element
	           courant de la boucle */
          LU_DeClasserEnFonctionDuHashCode( Matrice , NumeroDElement[il] );
          TypeDeClassementHashCodeAFaire[NumeroDElement[il]] = ELEMENT_HASCODE_A_NE_PAS_CLASSER;	  
	      }
        /* Il faut reinitialiser T */
        for ( il = ilDeb ; il < ilMax ; il++ ) T[IndiceColonne[il]] = 0; 
        goto ElementSuivant;           
      }
      goto RazT;           
    }
    /* Il y a des super lignes et des lignes ( au moins 1 de chaque ) */
    if ( NbSuperLignesAGrouper >= Matrice->SeuilNombreDeSuperLigneAGrouper ||
         NbLignesAGrouper      >= Matrice->SeuilNombreDeLignesAGrouper ) {
      *SuperLignesReamenagees = OUI_LU;
    }
    /* Il y a des lignes et des super lignes. On fusionne d'abord toutes les super lignes
       puis on ajoute toutes les lignes dans la super ligne qui remplace les fusionnees */
    ilS = 0;
		NumeroSuperLigne1 = 0;
		for ( il = 0 ; il < NbElements ; il++ ) {
      if ( NumeroDElement[il] >= Rang ) {
        NumeroSuperLigne1 = NumeroDElement[il] - Rang;	    
        ilS = il + 1;
        break;
      }
    }
		
    /* Constitution du nouveau vecteur T concernant la super ligne qu'on garde */
    for ( il = ilDeb ; il < NbTermesRef ; il++ ) T[IndiceColonne[il]] = 0;
		
    ptSuperLigne  = SuperLigneDeLaMatrice[NumeroSuperLigne1];
    NbTermesRef   = ptSuperLigne->NombreDeTermes;
    IndiceColonne = ptSuperLigne->IndiceColonne;
		ilDeb = 0;
    ilMax = NbTermesRef;
		
    for ( il = ilDeb , ii = 0 ; il < ilMax ; il++ , ii++ ) T[IndiceColonne[il]] = ii + 1;  
	
    /* Il faut dechainer les elements sauf le premier element car c'est l'element courant de la boucle */	
    for ( il = ilS ; il < NbElements ; il++ ) {
      if ( NumeroDElement[il] >= Rang ) {
        NumeroSuperLigne2 = NumeroDElement[il] - Rang;
        /*
        printf("Fusion de la super ligne %d dans la super ligne %d\n",NumeroSuperLigne2,NumeroSuperLigne1);
        */
        LU_FusionnerDeuxSuperLignes( Matrice, T, NumeroSuperLigne1, NumeroSuperLigne2 );	    
        /* On ne doit pas declasser le premier element (celui pour lequel il = 0 ) */
        LU_DeClasserEnFonctionDuHashCode( Matrice , NumeroDElement[il] );
        TypeDeClassementHashCodeAFaire[NumeroDElement[il]] = ELEMENT_HASCODE_A_NE_PAS_CLASSER;	    
      }
    }
	
    for ( il = 0 ; il < NbElements ; il++ ) {
      if ( NumeroDElement[il] < Rang ) {
        Ligne = NumeroDElement[il];
        /*
        printf("Ajout de la ligne %d dans la super ligne %d\n",Ligne,NumeroSuperLigne1);
        */
        LU_AjouterUneLigneDansUneSuperLigne( Matrice, Matrice->SuperLigne[NumeroSuperLigne1] , T , Ligne );
        /* Il faut dechainer la ligne sauf si c'est la premier element car c'est l'element courant de
	         la boucle */
        if ( il != 0 ) {
	        LU_DeClasserEnFonctionDuHashCode( Matrice , Ligne );
	        TypeDeClassementHashCodeAFaire[Ligne] = ELEMENT_HASCODE_A_NE_PAS_CLASSER;
	      }
        else TypeDeClassementHashCodeAFaire[Ligne] = ELEMENT_HASCODE_A_DECLASSER;
	    
        /* La ligne a ete mise dans la super ligne, on ne libere pas la place dans Ldeb etc.. */  
      }
    }
	
    /* Il faut reinitialiser T */
    for ( il = ilDeb ; il < ilMax ; il++ ) T[IndiceColonne[il]] = 0;	
    goto ElementSuivant;           
            
    RazT:
    /* Rien trouve */        
    for ( il = ilDeb ; il < ilMax ; il++ ) T[IndiceColonne[il]] = 0;
    
    ElementSuivant:
    Element = HashModuloSuiv[Element];    
  }
}

/* printf("-----------------> Nombre de super lignes %d\n",Matrice->NombreDeSuperLignes); */

/* Si on a detecte des superlignes, on change le seuil concernant les lignes */
Matrice->SeuilNombreDeLignesAGrouper = SEUIL_2_NB_LIGNES_A_GROUPER;

return;
}

