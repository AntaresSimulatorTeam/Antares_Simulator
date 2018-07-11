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
# ifndef CONSTANTES_EXTERNES_SIMPLEXE_DEJA_DEFINIES  
/*******************************************************************************************/
/* 
  Definition des constantes symboliques a utiliser par le module appelant le simplexe 
*/

# define SPX_ERREUR_INTERNE              2
# define SPX_MATRICE_DE_BASE_SINGULIERE  3
/* Constantes symboliques du OUI et du NON */
# define OUI_SPX  1
# define NON_SPX  0
/* */  

# define PRICING_DANTZIG        1 
# define PRICING_STEEPEST_EDGE  0

# define UTILISER_LA_BASE_DU_PROBLEME_SPX  2   

# define AGRESSIF      1
# define PEU_AGRESSIF  2

/* Type de borne des variables fournies en entree */
# define VARIABLE_FIXE     		  1 
# define VARIABLE_BORNEE_DES_DEUX_COTES   2  	
# define VARIABLE_BORNEE_INFERIEUREMENT   3  	
# define VARIABLE_BORNEE_SUPERIEUREMENT   4  	
# define VARIABLE_NON_BORNEE     	  5
	      		    
/* Contexte d'utilisation du simplexe */
# define BRANCH_AND_BOUND_OU_CUT          1  /* Branch and Bound ou Branch and Cut avec reinitialisation du probleme */
# define BRANCH_AND_BOUND_OU_CUT_NOEUD    2  /* Branch and Bound ou Branch and Cut en un noeud particulier (i.e. sans reinitialisation du probleme) */ 
# define SIMPLEXE_SEUL     	          3

/* Pour choisir l'algorithme */  
# define SPX_PRIMAL  1		        
# define SPX_DUAL    2 				    

/* */
# ifndef CREUX
  # define CREUX    1
# endif
# ifndef COMPACT
  # define COMPACT  2
# endif

# define PHASE_1  1		        
# define PHASE_2  2 	 

/* Origine des variables du simplexe */
# define NATIVE		       1  /* Variable native (elle correspond a une variable du probleme d'entree */
# define ECART		       2  /* Variable d'ecart creee pour mettre une contrainte d'inegalite sous la forme standard */
# define BASIQUE_ARTIFICIELLE  3  /* Variable basique de depart creee si necessaire pour construire la 
                                     premiere base inversible */	   

/* Position des variables */
# define EN_BASE                 0  
# define EN_BASE_LIBRE           1  
# define EN_BASE_SUR_BORNE_INF   2
# define EN_BASE_SUR_BORNE_SUP   3
# define HORS_BASE_SUR_BORNE_INF 4
# define HORS_BASE_SUR_BORNE_SUP 5 
# define HORS_BASE_A_ZERO        6  /* Pour les variables non bornees qui restent hors base */ 

/*******************************************************************************************/
# define CONSTANTES_EXTERNES_SIMPLEXE_DEJA_DEFINIES	
# endif

