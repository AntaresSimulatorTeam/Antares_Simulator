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
# ifndef DEFINITIONS_CONSTANTES_INTERNES_LU_FAITE  
/***********************************************************************************************************************/
    
# define LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE 
# undef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE
    
# define VERBOSE_LU       0              
# define VERBOSE_SCALING  0
   
# define HYPER_CREUX
  /*# undef HYPER_CREUX*/	
# define NOEUD_HORS_LISTE   0 /* Pour l'hyper creux */
# define NOEUD_DANS_LISTE   1 /* Pour l'hyper creux */

# define COMPACT_LU             1 /* Les valeurs non nulles sont compactees au debut du vecteur */
# define ADRESSAGE_INDIRECT_LU  2 /* Adressage indirect des valeurs non nulles */
# define VECTEUR_LU             3 /* Pas d'adressage particulier */

# define RECURSION_OK      1
# define RECURSION_NOT_OK  2
# define POURSUITE_RESOLUTION_HYPER_CREUX 3
# define ARRET_RESOLUTION_HYPER_CREUX     4  

# define NOMBRE_MAX_ITER_DE_RAFFINEMENT        100  
										           
# define NOMBRE_INITIAL_DE_VECTEURS_H_ALLOUES  150  /*150*/   /* grand nombre => cycle conduit par le fillin */ 
									       
# define MARGE_MINIMALE_POUR_CREATION_DE_TERMES  5 /*10*/         		   

# define L_INCREMENT_DALLOCATION   10000
# define C_INCREMENT_DALLOCATION   10000

# define INCREMENT_DALLOCATION_DE_L   10000
# define INCREMENT_DALLOCATION_DE_U   10000

# define SHIFT_MAX_INCREMENT 5
 
/* Pour les super lignes */     
  # define ELEMENT_HASCODE_A_NE_PAS_CLASSER  1  /* Attention c'est pour mettre dans un char */
  # define ELEMENT_HASCODE_A_DECLASSER       2  /* Attention c'est pour mettre dans un char */
  # define ELEMENT_HASCODE_A_RECLASSER       3  /* Attention c'est pour mettre dans un char */
  # define ELEMENT_HASCODE_CLASSE            4  /* Attention c'est pour mettre dans un char */
  # define ELEMENT_HASCODE_A_CLASSER         5  /* Attention c'est pour mettre dans un char */
   
  # define SEUIL_NB_SUPER_LIGNES_A_GROUPER   2
  # define SEUIL_1_NB_LIGNES_A_GROUPER       5   
  # define SEUIL_2_NB_LIGNES_A_GROUPER       2 
/* Fin super lignes */     
 
# define PIVOT_MIN_SIMPLEXE          1.e-6  /*6*/           
# define PIVOT_MIN_EXTREME_SIMPLEXE  1.e-10 /*10*/     
# define PIVOT_MIN_FINAL             1.e-15 /*1.e-15 Valeur de pivot min pour l'elimination du dernier pivot */    

# define PIVOT_MIN_AUTRES_CONTEXTES          1.e-9       
# define PIVOT_MIN_EXTREME_AUTRES_CONTEXTES  1.e-10    	     
	   
# define NOMBRE_MAX_DE_CHANGEMENTS_DE_SEUIL_DE_PIVOTAGE_SIMPLEXE          4    
# define NOMBRE_MAX_DE_CHANGEMENTS_DE_SEUIL_DE_PIVOTAGE_AUTRES_CONTEXTES  0     
	  
# define PREMIER_SEUIL_DE_PIVOTAGE       0.050 /*0.050*/        
# define INCREMENT_DE_SEUIL_DE_PIVOTAGE  0.025 /*0.025*/
        				
# define SEUIL_DE_REINTRODUCTION  10.0    

# define POURCENTAGE_MAX_DE_TERMES_CREES_PAR_LU_UPDATE  0.4  /*0.1*/ /* Il ne faut pas etre trop restrictif sinon, dans le
                                                                        cas ou le remplissage est important pendant la
																																			  factorisation, on sera conduit a refactoriser trop
																																			  souvent pendant la LU update */

# define LINFINI_LU 1.e+80

/* Pour le reglage des zeros ci-dessous il faudrait une methode automatique */
# define ZERO_POUR_LE_RESULTAT_DE_LA_RESOLUTION             1.e-20 /*1.e-18*/ /*1.e-17*/ /*1.e-16*/  /* Que pour les Simplexe */
# define ZERO_POUR_LE_RESULTAT_DE_LA_RESOLUTION_TRANSPOSEE  1.e-20 /*1.e-18*/ /*1.e-17*/ /*1.e-16*/  /* Que pour les Simplexe */
# define ZERO_POUR_SPIKE  1.e-20 /*1.e-19*/          
# define VALEUR_NULLE_DE_LAMBDA_POUR_LU_UPDATE 1.e-30       

/*****************************************************************************/
/* Super lignes dans le cas de la factorisation des matrices non symetriques: en test
   utilise pour diminuer le temps passe dans scan ligne */
   
# define INCREMENT_ALLOC_NB_LIGNES_DE_SUPER_LIGNE   0   /* Correspond aux lignes ajoutees dans super ligne */
# define INCREMENT_ALLOC_NB_COLONNES_DE_SUPER_LIGNE 50  /* Correspond aux nouveaux termes crees */

/*****************************************************************************/

/*******************************************************************************************/
# define DEFINITIONS_CONSTANTES_INTERNES_LU_FAITE  
# endif


 


