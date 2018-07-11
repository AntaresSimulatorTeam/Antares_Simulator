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
# ifndef PROBLEME_SIMPLEXE_DEJA_DEFINI
/*******************************************************************************************/
/* 
   Le passage des informations a la routine de resolution du simplexe se fait par un pointeur 
   a la structure C definie ci-apres. 
   
   Le fichier spx_constantes_externes.h doit etre inclus dans le code de l'appelant.
   Le fichier spx_definition_arguments.h doit etre inclus dans le code de l'appelant,
   il contient la definition de la structure C exploitee par la fonction.
   Apres avoir renseigne les champs, le module utilisateur appelle la fonction 
   SPX_Simplexe avec, pour argument d'appel:
   - un pointeur à un objet de type PROBLEME_SIMPLEXE: il permet de renseigner les donnees
     du probleme à résoudre
   - un pointeur à un objet de type PROBLEME_SPX. Lorsque la valeur de ce pointeur vaut NULL,
     SPX_Simplexe alloue un nouvel objet de type PROBLEME_SPX sur lequel il travaillera.
     Dans le cas contraire SPX_Simplexe travaillera sur l'objet de type PROBLEME_SPX passé
     par l'appelant. 
     
   SPX_Simplexe renvoie un pointeur à un objet de type PROBLEME_SPX, ce pointeur
   correspond à l'objet sur lequel SPX_Simplexe vient de faire ses calculs.

   L'appelant peut de cette façon travailler sur plusieurs instances de problemes qu'il
   souhaite faire resoudre par le simplexe.

   Exemple d'utilisation :

   PROBLEME_SIMPLEXE Mon_Probleme; <- definition d'une structure "Mon_Probleme" de type PROBLEME_SIMPLEXE   
   void * ProblemeSpx; <- Utiliser void * comme type de pointeur permet à l'appelant d'ignorer la
                          composition de la structure PROBLEME_SPX, qu'il n'a d'ailleurs pas besoin
			  de connaitre.
   .......
   .......
   Remplissage des champs de la structure
   .......
   .......
   .......
   Appel de la fonction:

   ProblemeSpx = SPX_Simplexe( &Mon_Probleme , ProblemeSpx );
    
*/

typedef struct {
  int  Contexte; /* Contexte dans lequel le simplexe est utilise. Cet argument peut prendre 3 valeurs:
                      BRANCH_AND_BOUND_OU_CUT: le simplexe est appelé dans un contexte de Branch And Bound 
		                               ou de Branch And Cut 
                      BRANCH_AND_BOUND_OU_CUT_NOEUD: le simplexe est appelé dans un contexte de Branch And Bound 
		                                     ou de Branch And Cut mais on ne reinitialise pas le probleme 
                      SIMPLEXE_SEUL: le simplexe est appelé hors d'un contexte de Branch and Bound ou de 
		                     Branch And Cut (dans ce cas, certaines sauvegardes particulières ne sont
				     pas faites) */
  int     NombreMaxDIterations; /* Si < 0 , le simplexe prendre sa valeur par defaut */
  double   DureeMaxDuCalcul;     /* Exprime en secondes (attention c'est du double).
                                    Mettre une valeur negative si pas de duree max a prendre en compte */
  double * CoutLineaire;         /* Couts lineaires */
  double * X;                    /* Vecteur des variables */
  double * Xmin;                 /* Bornes min des variables */
  double * Xmax;                 /* Bornes max des variables */
  int     NombreDeVariables;    /* Nombre de variables */
  int   * TypeDeVariable; /* Indicateur du type de variable, il ne doit prendre que les suivantes 
                              (voir le fichier spx_constantes_externes.h mais ne jamais utiliser les valeurs explicites 
			       des constantes): 
                               VARIABLE_FIXE                  , 
                               VARIABLE_BORNEE_DES_DEUX_COTES , 
                               VARIABLE_BORNEE_INFERIEUREMENT , 
                               VARIABLE_BORNEE_SUPERIEUREMENT , 	   
                               VARIABLE_NON_BORNEE
                                             */
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
  /* Choix de l'algorithme */
  int ChoixDeLAlgorithme; /* L'utilisateur doit mettre :  (RQ seul le dual marche)  
                              SPX_PRIMAL s'il veut utiliser l'algorithme primal
                              SPX_DUAL   s'il veut utiliser l'algorithme dual */
  /* Guidage de l'algorithme */
  int TypeDePricing;  /* Le pricing est l'étape du calcul dans laquelle on choisit la variable sortante
			  dans l'algorithme dual (ou la variale entrante dans l'algorithme primal).  
			  Deux choix sont possibles:
			  * PRICING_DANTZIG: c'est la méthode basique, elle est rapide mais dans certains
			                     cas conduit à faire beaucoup d'itérations pour trouver l'optimum.
			  * PRICING_STEEPEST_EDGE: méthode élaborée (Forrest-Goldfarb), elle demande plus de
			    calculs mais permet de réduite significativement le nombre d'itérations. Il est
			    recommander de l'utiliser pour les problèmes difficiles. */
  int FaireDuScaling; /* Vaut OUI_SPX ou NON_SPX. Si l'utilisateur positionne la valeur a OUI_SPX,
		 	  le simplexe fait un scaling du probleme dès le début de la résolution.
			  Le scaling a pour but d'améliorer le conditionnement du problème. Il est
			  recommandé de l'utiliser lorsque les coefficients de la matrice des contraintes
			  sont très différents les un des autres (rapport > 100) */
  int StrategieAntiDegenerescence; /* Vaut AGRESSIF ou PEU_AGRESSIF.
                                       * AGRESSIF: le controle est fait à chaque iterations.
				       * PEU_AGRESSIF: le controle est fait moins souvent.
				       -> Choix recommandé: AGRESSIF
				    */				       
  /* En Entree ou en Sortie */
  int   BaseDeDepartFournie;   /* Vaut OUI_SPX ou NON_SPX */
                               
  int * PositionDeLaVariable;  /* Pour chaque variable, sa position vis a vis de la base. Une variable peut etre de 4 type:
                                   EN_BASE, HORS_BASE_SUR_BORNE_INF,HORS_BASE_SUR_BORNE_SUP,HORS_BASE_A_ZERO*/ 
  int   NbVarDeBaseComplementaires; /* Nombre de variables basiques complementaires (c'est une valeur d'entree mais de sortie aussi) */
  int * ComplementDeLaBase; 

  int   ExistenceDUneSolution; /* En sortie, vaut :
                                   OUI_SPX s'il y a une solution, 
				   NON_SPX s'il n'y a pas de solution admissible 
                                   SPX_ERREUR_INTERNE si probleme a l'execution (saturation memoire par exemple), et 
				                      dans ce cas il n'y a pas de solution
				   SPX_MATRICE_DE_BASE_SINGULIERE si on n'a pas pu construire de matrice de base reguliere,
				                                  et dans ce cas il n'y a pas de solution
			        */
  int   LibererMemoireALaFin;  /* En Entree: 
                                   - Si OUI_SPX la memoire est liberee a la fin du simplexe 
                                   - Si NON_SPX la memoire n'est pas liberee a la fin du simplexe. Cette est utile si l'on 
				     veut conserver temporairement les donnees du probleme pour calculer des coupes de Gomory 
				     par exemple. Attention, l'appelant doit ensuite liberer le memoire par un appel 
				     a "SPX_LibererProbleme" */
  double CoutMax; /* En entree: cette information n'est utilisee que si l'algorithme choisi est l'algorithme dual. 
                     On sait qu'a chaque iteration de l'algorithme dual, le cout courant est un minorant du cout optimal. 
		     Il est donc possible de comparer ce cout à un Cout Max, seuil au dessus duquel on convient d'arreter les 
		     calculs (l'algorithme sort alors avec le verdict "pas de solution").
                     Quelle en est l'utilite (mais il peut y en avoir d'autres) ? 
                     Dans un contexte de branch and bound, des que l'on dispose d'une solution entiere, toutes les resolutions de 
		     probleme relaxé menant a un cout superieur a ce cout sont a rejeter. Donc, si l'on se rend compte au cours de 
		     l'algorithme dual, que la resolution du probleme relaxe va mener a un cout trop grand il est inutile de 
                     poursuivre les calculs. Ceci permet de gagner du temps de calcul. 

                     ATTENTION: comme l'algorithme dual peut etre utilisé en tant que solveur (c'est à dire 
                     ---------  en dehors d'un contexte de branch and bound) ou bien pour resoudre un probleme dont on 
		                de souhaite pas donner de Cout Max parce qu'on ne le connait pas, l'information "CoutMax"
			        n'est utilisee par l'algorithme dual que si l'indicateur "UtiliserCoutMax" (argument suivant) 
				est positionne a "OUI-SPX". */  		  
  int    UtiliserCoutMax;
  /* Les coupes: uniquement dans un contexte BRANCH AND BOUND ET CUT */
  int     NombreDeContraintesCoupes;         
  double * BCoupes;                    /* Valeurs de second membre */
  char   * PositionDeLaVariableDEcartCoupes; 
  int   * MdebCoupes; 
  int   * NbTermCoupes; 
  int   * NuvarCoupes; 
  double * ACoupes;  
  /* En sortie */
  double * CoutsMarginauxDesContraintes; /* Cout marginaux des contraintes (attention, il faut dimensionner ce tableau
                                            a NombreDeContraintes + NombreDeContraintesCoupes */
  double * CoutsReduits; /* Couts reduits des variables hors-base, longueur nombre de variables passees 
                            en entree du probleme. Contient la valeur 0 si la variable est basique */
  /* Traces */
  char     AffichageDesTraces; /* Vaut OUI_SPX ou NON_SPX */ 
				    
} PROBLEME_SIMPLEXE;

/*******************************************************************************************/
# define PROBLEME_SIMPLEXE_DEJA_DEFINI
# endif
# ifdef __cplusplus
  }
# endif







