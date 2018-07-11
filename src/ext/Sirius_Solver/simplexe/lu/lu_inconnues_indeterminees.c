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

   FONCTION: Ce sous-programme retourne la liste des inconnues que l'on 
             ne peut pas determiner lorsque la matrice est singuliere. 
             C'est la sous-matrice active. 
             La fonction retourne: 
             - le nombre de variables indeterminees,
             - le nombre de triangles observables,
             - un pointeur sur une table contenant 
                 * la valeur "VARIABLE_INDETERMINEE" si 
                   l'inconnue est indeterminee
                 * le numero du triangle si l'inconnue appartient a
                   un triangle qui a pu etre factorise           
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

# define PREMIER_NUMERO_DE_TRIANGLE       1
# define VARIABLE_INDETERMINEE           -1
# define VARIABLE_PAS_ENCORE_AFFECTEE    -10
# define NUMERO_DE_TRIANGLE_NON_DEFINI	 -10000
	      
/*-------------------------------------------------------------------------------------------------------------*/
/* Attention cette fonction n'est plus utilisee. C'est Sylvain qui reconstiutue ce dont il a besoin */

int * LU_InconnuesIndeterminees( MATRICE * Matrice , int * NombreDeVariablesIndeterminees , int * NombreDeTrianglesFactorises )
{
int Kp; int ilDeb  ; int ilFin; int il; int ilMax; int Inconnue                ;
int NumeroDeTriangle; int NumeroDeTriangleCourant   ; int NbVariablesIndeterminees;   

/*Matrice = Mat;*/

printf("Attention: la fonction LU_InconnuesIndeterminees est obsolete, on ne rentre pas dedans\n");
return( NULL );

*NombreDeVariablesIndeterminees = 0;
*NombreDeTrianglesFactorises	= 0;

Matrice->NumeroDeTriangleDeLaVariable = (int *) malloc( Matrice->Rang * sizeof( int ) ); 
if ( Matrice->NumeroDeTriangleDeLaVariable == NULL ) {
  printf("Factorisation LU, sous-programme LU_InconnuesIndeterminees: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Matrice->AnomalieDetectee = SATURATION_MEMOIRE;
  longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 
}    


for ( Kp = 0 ; Kp < Matrice->Rang ; Kp++ ) Matrice->NumeroDeTriangleDeLaVariable[Kp] = VARIABLE_PAS_ENCORE_AFFECTEE;

NbVariablesIndeterminees = 0;
for ( Kp = Matrice->EtapeSinguliere ; Kp < Matrice->Rang ; Kp++ ) {
  Matrice->NumeroDeTriangleDeLaVariable[Matrice->OrdreColonne[Kp]] = VARIABLE_INDETERMINEE;
  NbVariablesIndeterminees++;
}
			  
/* Etablissement des numero de triangles factorises: parcours de la triangulaire superieure */

NumeroDeTriangleCourant = PREMIER_NUMERO_DE_TRIANGLE - 1;

for ( Kp = Matrice->EtapeSinguliere - 1 ; Kp >= 0 ; Kp-- ) {
  ilDeb = Matrice->LdebParLigneDeU[Kp];
  ilFin = ilDeb + Matrice->NbTermesParLigneDeU[Kp];   
  /* Recherche d'un numero de triangle */
  il    = ilDeb;
  ilMax = ilFin;
  NumeroDeTriangle = NUMERO_DE_TRIANGLE_NON_DEFINI;
  while ( il != ilMax ) {
    if ( Matrice->ElmDeU[il] != 0.0 ) {      	     
      Inconnue = Matrice->IndiceColonneDeU[il];
      if ( Matrice->NumeroDeTriangleDeLaVariable[Inconnue] >= PREMIER_NUMERO_DE_TRIANGLE ) {
        NumeroDeTriangle = Matrice->NumeroDeTriangleDeLaVariable[Inconnue];
        break;
      } 
    }
    il++;
  }
  /* On n'a pas trouve de numero de zone alors on en cree un */
  if ( NumeroDeTriangle == NUMERO_DE_TRIANGLE_NON_DEFINI ) { 
    NumeroDeTriangleCourant++; 
    NumeroDeTriangle = NumeroDeTriangleCourant;
  }

  il    = ilDeb;
  ilMax = ilFin;
  while ( il != ilMax ) {
    if ( Matrice->ElmDeU[il] != 0.0 ) {      	     
      Inconnue = Matrice->IndiceColonneDeU[il];
      if ( Matrice->NumeroDeTriangleDeLaVariable[Inconnue] != VARIABLE_INDETERMINEE ) {
        Matrice->NumeroDeTriangleDeLaVariable[Inconnue] = NumeroDeTriangle;
      }
    } 
    il++;
  }
}
   
*NombreDeVariablesIndeterminees = NbVariablesIndeterminees;
if ( NumeroDeTriangleCourant < PREMIER_NUMERO_DE_TRIANGLE ) NumeroDeTriangleCourant = 0;
*NombreDeTrianglesFactorises	= NumeroDeTriangleCourant;

return( Matrice->NumeroDeTriangleDeLaVariable );

}
	      
/*----------------------------------------------------------------------*/
/*    Routine experimentale, uniquement pour les besoins du PRESOLVE    */
/* On utilise NumeroDeTriangleDeLaVariable mais en realite on met les
   numeros de lignes non factorisees */   

int * LU_Indeterminees( MATRICE * Matrice, int * NombreDIndeterminees, char Choix, char LIGNE, char COLONNE )
{
int Kp; int NbDIndeterminees; int * Ordre;

/*printf("LU_Indeterminees Choix %d  LIGNE %d  COLONNE %d\n",Choix,LIGNE,COLONNE);*/

if ( Choix == LIGNE ) Ordre = Matrice->OrdreLigne;
else if ( Choix == COLONNE ) Ordre = Matrice->OrdreColonne;
else {
  *NombreDIndeterminees = 0;
  return( NULL );
}

*NombreDIndeterminees = 0;

Matrice->NumeroDeTriangleDeLaVariable = (int *) malloc( Matrice->Rang * sizeof( int ) ); 
if ( Matrice->NumeroDeTriangleDeLaVariable == NULL ) {
  return( NULL );
}    

NbDIndeterminees = 0;
for ( Kp = Matrice->EtapeSinguliere ; Kp < Matrice->Rang ; Kp++ ) {
  Matrice->NumeroDeTriangleDeLaVariable[NbDIndeterminees] = Ordre[Kp];
  NbDIndeterminees++;
}
			  
*NombreDIndeterminees = NbDIndeterminees;

return( Matrice->NumeroDeTriangleDeLaVariable );

}

