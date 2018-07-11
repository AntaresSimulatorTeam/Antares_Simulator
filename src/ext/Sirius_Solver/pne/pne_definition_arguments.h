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
# ifndef PROBLEME_DEJA_DEFINI
/*******************************************************************************************/
/* 
   Le passage des informations a la routine de resolution du probleme se fait par un pointeur 
   a la structure C definie ci-apres. 
   
   Le fichier pne_definition_arguments.h doit etre inclus dans le code de l'appelant,
   il contient la definition de la structure C exploitee par la fonction.
   Apres avoir renseigne les champs, le module utilisateur appelle la fonction 
   PNE_Solveur avec, pour argument d'appel, un pointeur a la structure ci-dessous. 

   Exemple d'utilisation :

   PROBLEME_A_RESOUDRE Mon_Probleme; <- definition d'une structure "Mon_Probleme" de type PROBLEME_A_RESOUDRE   
   .......
   .......
   Remplissage des champs de la structure
   .......
   .......
   .......
   Appel de la fonction:

   PNE_Solveur( &Mon_Probleme );
    
*/

typedef struct {

  int     NombreDeVariables;   /* Nombre de variables */
  int   * TypeDeVariable   ;   /* ENTIER ou REEL attention dans le cas des variables entieres, le seul cas traite 
                                   est celui des variables {0,1} */
  int   * TypeDeBorneDeLaVariable; /* Indicateur du type de variable, il ne doit prendre que les suivantes 
                                     (voir le fichier pne_constantes_externes.h mais ne jamais utiliser les 
				                             valeurs explicites des constantes): 
                                     VARIABLE_FIXE                  , 
                                     VARIABLE_BORNEE_DES_DEUX_COTES , 
                                     VARIABLE_BORNEE_INFERIEUREMENT , 
                                     VARIABLE_BORNEE_SUPERIEUREMENT , 	
                                     VARIABLE_NON_BORNEE     	  
                                                    */
  double * X 	               ;   /* Vecteur des inconnues: en sortie, il contient la solution s'il y en a une */
  double * Xmax 	       ;   /* Borne sup de chaque inconnue */
  double * Xmin 	       ;   /* Borne inf de chaque inconnue */
  double * CoutLineaire        ;   /* Vecteur des couts lineaires */
  /* Description des contraintes */
  int     NombreDeContraintes;    /* Nombre de contraintes */
  double * SecondMembre       ;   /* Vecteur des second membres */
  char   * Sens	              ;   /* Sens de chaque contrainte :
                                     pour une contraintes de type inferieur ou egal mettre le caractere '<' 
                                     pour une contraintes de type superieur ou egal mettre le caractere '>'
                                     pour une contrainte d'egalite mettre le caractere "=" */

/* 
La matrice des contrainte est decrite par les 4 vecteurs qui suivent. Elle doit etre decrite par ligne.
     -> Les coefficients de la matrice des contraintes doivent etre donnes dans un vecteur double precision.
     -> En parallele du vecteur des coefficient, il faut donner l'indice colonne du coefficient. 
     -> Pour chaque ligne (ou premier membre de la contrainte) il faut donner sont indice début dans le vecteur
        double precision qui contient les coefficients de la contraintes, et le nombre de coefficients non nuls. 	
*/
  int   * IndicesDebutDeLigne 	                 ; /* Pour chaque ligne, indice debut de la ligne dans le
                                                      vecteur des coefficients */
  int   * NombreDeTermesDesLignes	               ; /* Nombre de termes non nuls de la ligne */
  double * CoefficientsDeLaMatriceDesContraintes ; /* Coefficients de la matrice des contraintes */
  int   * IndicesColonnes                        ; /* Vecteur parallele au precedent. Il contient l'indice 
                                                      colonne de chaque coefficient */

  /* En retour */
  double * VariablesDualesDesContraintes;  /* Pointeur sur un vecteur dans lequel le solveur va mettre
					      les variables duales des contraintes. Attention, ce tableau
					      doit etre alloue par l'appelant */

  int     ExistenceDUneSolution;   /* Indicateur d'existence de solution. Valeurs possible: 
				                              -> PAS_DE_SOLUTION_TROUVEE    
                                      -> SOLUTION_OPTIMALE_TROUVEE  
				                              -> SOLUTION_OPTIMALE_TROUVEE_MAIS_QUELQUES_CONTRAINTES_SONT_VIOLEES
                                      -> PROBLEME_INFAISABLE        
                                      -> PROBLEME_NON_BORNE         
                                      -> ARRET_PAR_LIMITE_DE_TEMPS_AVEC_SOLUTION_ADMISSIBLE_DISPONIBLE  
                                      -> ARRET_CAR_ERREUR_INTERNE (saturation memoire par exemple)  
				    */
  /* Options */
  char     AlgorithmeDeResolution; /* Doit valoir SIMPLEXE ou POINT_INTERIEUR */
                                   /* Attention, le choix POINT_INTERIEUR ne peut être utilise que dans le cas
				                              d'un probleme ne comportant pas de varaibles entieres */
  char     AffichageDesTraces; /* Peut valoir OUI_PNE ou NON_PNE */ 
  char     SortirLesDonneesDuProbleme; /* Peut valoir OUI_PNE ou NON_PNE. 
                                          Mettre OUI_PNE pour sortir les donnees du probleme dans un fichier au format mps */
  char     FaireDuPresolve; /* Peut valoir OUI_PNE ou NON_PNE */
                            /* La valeur hautement conseillee est OUI_PNE */
  int     TempsDExecutionMaximum; /* Temps (en secondes) au bout duquel la resolution du probleme est arretee meme si la 
                                     solution optimale n'a pas ete trouvee. Attention, cette grandeur n'est prise en compte 
				                             que si le probleme contient des variables entieres */
                                  /* Mettre 0 si le temps est illimite */
  int     NombreMaxDeSolutionsEntieres; /* Lorsque le nombre de solutions entieres est egal à la valeur de ce
                                           parametre, le solveur s'arrete et donne la meilleure solution rencontree.
					                                 Remarque: mettre une valeur strictement negative pour que ce parametre n'ai pas
					                                 de rôle.
					                              */
  double   ToleranceDOptimalite;  /* Si l'écart relatif entre le cout de la solution entiere trouvee et le plus petit minorant
                                     est inférieur à ToleranceDOptimalite, le solveur s'arrete et considère que la solution
				                             entiere trouvee est la solution optimale.
				                             Convention: ToleranceDOptimalite doit etre exprimé en %.
				                             Conseil   : mettre 0 %.
                                  */
  char     CoupesLiftAndProject;   /* Utile que s'il y a des variables entieres dans le probleme.
                                      Peut valoir OUI_PNE ou NON_PNE. Lorsque cette option vaut OUI_PNE
				                              le calcul des coupes de type lift and project est activé.
				                               - Choix conseillé: NON_PNE car le calcul de ce type de coupe peut être
				                                 couteux.
				                               - Mettre OUI_PNE si le probleme est difficile a resoudre.
				                           */																	 
} PROBLEME_A_RESOUDRE;


/* Prototype du la fonction d'entree */
void PNE_Solveur( PROBLEME_A_RESOUDRE * );

/*******************************************************************************************/
# define PROBLEME_DEJA_DEFINI	
# endif
# ifdef __cplusplus
  }
# endif		







