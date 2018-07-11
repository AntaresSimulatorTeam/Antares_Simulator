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
# ifdef __cplusplus
  extern "C"
	{
# endif
# ifndef PROBLEME_POINT_INTERIEUR_DEJA_DEFINI  
/*******************************************************************************************/
/* 
   Le passage des informations a la routine de resolution du point interieur se fait par un pointeur 
   a la structure C definie ci-apres. 
   
   Le fichier pi_constantes_externes.h doit etre inclus dans le code de l'appelant.
   Le fichier pi_definition_arguments.h doit etre inclus dans le code de l'appelant,
   il contient la definition de la structure C exploitee par la fonction.
   Apres avoir renseigne les champs, le module utilisateur appelle la fonction 
   PI_Quamin avec, pour argument d'appel, un pointeur a la structure ci-dessous. 

   Exemple d'utilisation :

   PROBLEME_POINT_INTERIEUR Mon_Probleme; <- definition d'une structure "Mon_Probleme" de type PROBLEME_POINT_INTERIEUR   
   .......
   .......
   Remplissage des champs de la structure
   .......
   .......
   .......
   Appel de la fonction:

   PI_Quamin( &Mon_Probleme ); Attention maintenant c'est comme pour le simplexe (pour marcher en multi threading)
    
*/

typedef struct {

  double * CoutQuadratique;
  double * CoutLineaire;
	
  double * X;
  double * Xmin;
  double * Xmax;
  int     NombreDeVariables;  
  int   * TypeDeVariable;  /* Indicateur du type de variable, il ne doit prendre que les suivantes 
                              (voir le fichier pi_constantes_externes.h mais ne jamais utiliser les valeurs explicites 
			       des constantes): 
                               VARIABLE_FIXE                  , 
                               VARIABLE_BORNEE_DES_DEUX_COTES , 
                               VARIABLE_BORNEE_INFERIEUREMENT , 
                               VARIABLE_BORNEE_SUPERIEUREMENT , 	
                               VARIABLE_NON_BORNEE     	                                               */
  char   * VariableBinaire; /* Vaut OUI_PI ou NON_PI */
  /* La matrice des contraintes */
  int   NombreDeContraintes;      /* Nombre de contraintes */
  int * IndicesDebutDeLigne;      /* Pointeur sur le debut de chaque ligne de la matrice des contraintes */
  int * NombreDeTermesDesLignes;  /* Nombre de termes non nuls de chaque ligne */
  int * IndicesColonnes;          /* Indice colonne des termes de la matrice des contraintes.
                                      Attention, les termes de la ligne doivent etre ranges dans l'ordre 
                                      croissant des indices de colonnes */
  double * CoefficientsDeLaMatriceDesContraintes;  /* Les termes de la matrice des contraintes */

  /* Le second membre */
  char   * Sens;          /* Sens de contrainte: '<' ou '>' ou '=' */
  double * SecondMembre;  /* Valeurs de second membre */

  /* Parametres de controle */
  int    NombreMaxDIterations; /* Si < 0 , le point interieur prendre sa valeur par defaut */   
  int    AffichageDesTraces; /* Vaut OUI_PI ou NON_PI */ 

  int    UtiliserLaToleranceDAdmissibiliteParDefaut; 
  double  ToleranceDAdmissibilite;
  
  int    UtiliserLaToleranceDeStationnariteParDefaut; 
  double  ToleranceDeStationnarite;
	
  int    UtiliserLaToleranceDeComplementariteParDefaut; 
  double  ToleranceDeComplementarite; 
  
  /* Indicateur de deroulement (sortie) */ 
  int   ExistenceDUneSolution; /* En sortie, vaut :
                                   OUI_PI s'il y a une solution, 
				   NON_PI si pas de solution trouvee
                                   PI_ERREUR_INTERNE si probleme a l'execution (saturation memoire par exemple), et 
				                      dans ce cas il n'y a pas de solution
			        */

  /* Variables duales (sortie) */ 
  double * CoutsMarginauxDesContraintes; 
  double * CoutsMarginauxDesContraintesDeBorneInf; 
  double * CoutsMarginauxDesContraintesDeBorneSup;   
  /* */

} PROBLEME_POINT_INTERIEUR;
 

/*******************************************************************************************/
# define PROBLEME_POINT_INTERIEUR_DEJA_DEFINI	
# endif
# ifdef __cplusplus
  }
# endif







