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
# ifndef MATRICE_A_FACTORISER_DEJA_DEFINI  
/*******************************************************************************************/
/* 
   Le passage des informations a la routine de factorisation lu se fait par un pointeur 
   a la structure C definie ci-apres. 
   
   Le fichier lu_definition_arguments.h doit etre inclus dans le code de l'appelant,
   il contient la definition de la structure C exploitee par la fonction de factorisation.
   Apres avoir renseigne les champs, le module utilisateur appelle la fonction 
   LU_Factorisation avec, pour argument d'appel, un pointeur a la structure ci-dessous.
   LU_Factorisation retourne un pointeur à un objet de type MATRICE. En cas d'erreur interne
   telle que saturation memoire, la valeur de ce pointeur vaut NULL.
   

   Exemple d'utilisation :   
 
   MATRICE_A_FACTORISER Ma_Matrice; <- definition d'une structure "Ma_Matrice" de type MATRICE_A_FACTORISER 

   MATRICE * MatriceFactorisee; <- Contient la valuer du pointeur à un objet de type MATRICE renvoyé
                                   par LU_Factorisation
   
   Remplissage des champs de la structure: 

   Ma_Matrice.ValeurDesTermesDeLaMatrice = adresse du vecteur contenant les termes de la matrice a factoriser;
   Ma_Matrice.IndicesDeLigne             = adresse du vecteur contenant les indices de lignes;
   Ma_Matrice.IndexDebutDesColonnes      = adresse du vecteur contenant les index debut des colonnes;
   Ma_Matrice.NbTermesDesColonnes        = adresse du vecteur contenant le nombre de termes de chaque colonne;
   etc..

   Appel de la fonction:

   MatriceFactorisee = LU_Factorisation( &Matrice ); 
    
*/

typedef struct {
  /* Contexte d'utilisation de la factorisation */
  char     ContexteDeLaFactorisation; /* L'utilisateur doit positionner cet indicateur a l'une des
                                         valeurs suivantes:
                                       * LU_SIMPLEXE         si la factorisation est utilisee par le simplexe
                                       * LU_POINT_INTERIEUR  si la factorisation est utilisee par le point interieur
                                       * LU_GENERAL          si la factorisation est utilisee dans les autres cas
                                      */
  char     UtiliserLesSuperLignes;            /* Vaut OUI_LU ou NON_LU */
  char     LaMatriceEstSymetriqueEnStructure; /* Vaut OUI_LU ou NON_LU */
  /* La matrice a factoriser */
  double * ValeurDesTermesDeLaMatrice; /* Tout est dit dans le nom (c'est un long vecteur) */
  int   * IndicesDeLigne;  /* Vecteur parallele a "ValeurDesTermesDeLaMatrice". 
                               Pour chaque terme d'une colonne il donne son indice
                               de ligne. Attention, les termes d'une meme colonne
                               doivent etre ranges dans l'ordre croissant des
                               indices de ligne */
  int   * IndexDebutDesColonnes;  /* Pour chaque colonne de la matrice, index debut des valeurs de ses termes 
                                      dans "ValeurDesTermesDeLaMatrice" */
  int   * NbTermesDesColonnes;
  int     NombreDeColonnes;  /* On suppose que le nombre de lignes est egal au nombre de colonnes */				       
  /* Code retour de la factorisation */				                                                                  
  int     ProblemeDeFactorisation;  /* Le code retour ( NON_LU si tout s'est bien passe ) */

  /* Les options de factorisation */
  int     FaireScalingDeLaMatrice; /* L'utilisateur doit positionner cet indicateur a:
                                       * OUI_LU s'il veut que la factorisation fasse du 
                                         scaling automatiquement.
                                       * NON_LU dans le cas contraire */  

  char   UtiliserLesValeursDePivotNulParDefaut; /* Vaut OUI_LU ou NON_LU ( valeur conseillee: OUI_LU ) */                                 
  double ValeurDuPivotMin;         /* Si UtiliserLesValeursDePivotNulParDefaut est egal a NON_LU, alors le module */
  double ValeurDuPivotMinExtreme;  /* de factorisation utilise ces valeurs de pivot min */

  char   SeuilPivotMarkowitzParDefaut; /* Vaut OUI_LU ou NON_LU ( valeur conseillee: OUI_LU ) */                                 
  double ValeurDuPivotMarkowitz;       /* Lorsque "SeuilPivotMarkowitzParDefaut" est positionne a OUI_LU,
                                          La valeur "ValeurDuPivotMarkowitz" remplace la valeur par defaut.
					  "ValeurDuPivotMarkowitz" doit etre inferieure ou egale a 1.
                                          - Lorsqu'elle vaut 1 l'algorithme ne peut que pivoter sur le plus grand
					    terme de la ligne ce qui conduit a un remplissage important
					    de la matrice factorisee.
					  - Plus la valeur est petite, moins la factorisee sera pleine. Mais un seuil
					    trop petit peut conduire a des instabilites numeriques.
					  - Avec une grande valeur, plus la factorisee sera numeriquement plus stable,
					    avec en contrepartie un factorisee plus pleine. Petit bemol: ceci c'est de la
					    theorie car en pratique si le nombre de termes de la factorisee augmente,
					    le nombre d'operations aussi et donc l'impact des erreurs d'arrondi
					    machine aussi. Donc, rester raisonnable SVP.
					  - Il est conseille de ne jamais descendre en dessous de 0.01 Plus*/

  char   FaireDuPivotageDiagonal; /* Vaut OUI_LU ou NON_LU */
  char   LaMatriceEstSymetrique;  /* Vaut OUI_LU ou NON_LU, cette information n'est prise en compte
                                     que si FaireDuPivotageDiagonal est egal a OUI_LU */
																		 
	/*************************************************************************************************/
	/* Ci-dessous resultats specifiques pour l'observabilite numerique de l'estimation d'etat:
	   ces infos ne sont plus utilisees par l'estimation d'etat. On les garde encore quelque temps
		 au cas ou ..*/
	
  /* En retour, les triangles L et U de la factorisee */
  /* Le triangle L stocke par colonne */
  /* Le premier terme de chaque colonne vaut 1. Les termes suivant de la colonne ne sont 
     pas classes dans un ordre specifique. */
  int   * IndexDebutDesColonnesDeL;
  int   * NbTermesDesColonnesDeL;
  double * ValeurDesTermesDeL;		        					  
  int   * IndicesDeLigneDeL; /* Remarque: correspond aussi a l'indice ligne "natif" du terme */

  /* Le triangle U stocke par ligne */
  /* Le premier terme de chaque ligne est egal a l'inverse du pivot. Les termes suivants
     de la ligne ne sont pas classes dans un ordre specifique. */
  int   * IndexDebutDesLignesDeU;
  int   * NbTermesDesLignesDeU;
  double * ValeurDesTermesDeU;		        					  
  int   * IndicesDeColonneDeU; /* Remarque: correspond ausss a l'indice colonne "natif" du terme */
	
	/* Fin des resultats specifiques pour l'observabilite numerique de l'estimation d'etat */
	/*************************************************************************************************/																	 

	/*************************************************************************************************/																	 
	/* Ci-dessous resultats specifiques pour le point interieur du solveur */
		 
	/* Informations supplementaires qui sont utilisees uniquement pour le point interieur c'est a dire
	   lorsque "ContexteDeLaFactorisation" vaut "LU_POINT_INTERIEUR" */
  double   ValeurDeRegularisation; /* Valeur de remplacement du pivot nul: a fournir par l'utilisateur.
	                                    Typiquement mettre 1.e-8 mais pas en dessous */
  char     OnARegularise;          /* Information renvoyee par la factorisation. Elle vaut OUI_LU si
	                                    la factorisation a ete obligee de regularise, NON_LU sinon. */
	double * TermeDeRegularisation;  /* Ce vecteur doit etre alloue par l'utilisation. Sa dimension
	                                    doit etre egale au nombre de colonnes (ou de lignes) de la matrice.
																		  L'utilisateur doit imperativement initialiser ce vecteur.
																		  La premiere fois il doit l'initialiser a 0. Les fois suivantes,
																		  il peut, soit reinitialiser a 0, soit conserver les valeurs qu'il y a
																		  deja dedans.
																		  Lorsque la factorisation est obligee de regulariser, i.e. remplacer
																		  un terme diagonal nul par une petite valeur, elle ajoute aussi cette
																		  valeur de regularisation a la valeur contenue dans le vecteur (pour
																		  la colonne consideree. */
	/* Fin des resultats specifiques pour le point interieur du solveur */
	/*************************************************************************************************/																	 
	    
} MATRICE_A_FACTORISER;

/*******************************************************************************************/
# define MATRICE_A_FACTORISER_DEJA_DEFINI	
# endif
# ifdef __cplusplus
  }
# endif
		
