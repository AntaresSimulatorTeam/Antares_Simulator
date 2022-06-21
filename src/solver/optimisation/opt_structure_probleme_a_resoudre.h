/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __SOLVER_OPTIMISATION_STRUCTURE_PROBLEME_A_RESOUDRE_H__
#define __SOLVER_OPTIMISATION_STRUCTURE_PROBLEME_A_RESOUDRE_H__

#include <vector>
#include <string>

#define PREMIERE_OPTIMISATION 1
#define DEUXIEME_OPTIMISATION 2

/* # define CONTRAINTE_DE_RESERVE       NON_ANTARES */ /* OUI_ANTARES ou NON_ANTARES */

#define RESERVE_J_MOINS_1 OUI_ANTARES

/*
# define CONTRAINTE_HORAIRE       1
# define CONTRAINTE_MULTI_HORAIRE 2
*/

#define CONTRAINTE_HORAIRE 1
#define CONTRAINTE_JOURNALIERE 2
#define CONTRAINTE_HEBDOMADAIRE 3

#define PAS_DE_LISSAGE_HYDRAULIQUE 0
#define LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS 1
#define LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX 2

#define ALTRUISTE 1
#define EGOISTE 2

#define ANTARES_SIMPLEXE 1
#define ANTARES_PNE 2

#define VARIABLES_MMOINS_MOINS_BORNEES_DES_2_COTES OUI_ANTARES

#define SUBSTITUTION_DE_LA_VARIABLE_MPLUS                                          \
    NON_ANTARES /* Attention, dans le cas de OUI_ANTARES, la partie pour recuperer \
                   la valeur de MPLUS n'est pas encore codee */

/*--------------------------------------------------------------------------------------*/

/* Les problemes Simplexe */
typedef struct
{
    void** ProblemeSpx;
    void** ProblemeMip;
} PROBLEMES_SIMPLEXE;

/* Le probleme a resoudre */
typedef struct
{
    /* Pour la prise en compte des PMIN */
    char NumeroDOptimisation; /* Vaut	PREMIERE_OPTIMISATION ou DEUXIEME_OPTIMISATION */

    /* La matrice des contraintes */
    int NombreDeVariables;
    int NombreDeContraintes; /* Il est egal a :
                                 (  NombreDePays contraintes de bilan
                                  + NombreDePays contraintes de reserve
                                  + NombreDeContraintesCouplantes
                                 )* NombreDePasDeTempsDUneJournee
                                  + NombreDePays contraintes de turbine */
    char* Sens;
    int* IndicesDebutDeLigne;
    int* NombreDeTermesDesLignes;
    double* CoefficientsDeLaMatriceDesContraintes;
    int* IndicesColonnes;
    int NombreDeTermesAllouesDansLaMatriceDesContraintes;
    int IncrementDAllocationMatriceDesContraintes;
    int NombreDeTermesDansLaMatriceDesContraintes;
    /* Donnees variables de la matrice des contraintes */
    double* CoutQuadratique;
    double* CoutLineaire;
    int* TypeDeVariable; /* Indicateur du type de variable, il ne doit prendre que les suivantes
                             (voir le fichier spx_constantes_externes.h mais ne jamais utiliser les
                            valeurs explicites des constantes): VARIABLE_FIXE                  ,
                              VARIABLE_BORNEE_DES_DEUX_COTES ,
                              VARIABLE_BORNEE_INFERIEUREMENT ,
                              VARIABLE_BORNEE_SUPERIEUREMENT ,
                              VARIABLE_NON_BORNEE
                                            */
    double* Xmin;
    double* Xmax;
    double* SecondMembre;
    /* Tableau de pointeur a des doubles. Ce tableau est parallele a X, il permet
       de renseigner directement les structures de description du reseau avec les
       resultats contenus dans X */
    double** AdresseOuPlacerLaValeurDesVariablesOptimisees;
    /* Resultat */
    double* X;
    /* Tableau de pointeur a des doubles. Ce tableau est parallele a CoutsMarginauxDesContraintes,
       il permet de renseigner directement les structures de description du reseau avec les
       resultats sur les couts marginaux */
    double** AdresseOuPlacerLaValeurDesCoutsMarginaux;
    double* CoutsMarginauxDesContraintes;
    /* Tableau de pointeur a des doubles. Ce tableau est parallele a CoutsMarginauxDesContraintes,
       il permet de renseigner directement les structures de description du reseau avec les
       resultats sur les couts reduits */
    double** AdresseOuPlacerLaValeurDesCoutsReduits;
    double* CoutsReduits;
    /* En Entree ou en Sortie */
    int ExistenceDUneSolution; /* En sortie, vaut :
                                   OUI_SPX s'il y a une solution,
                                   NON_SPX s'il n'y a pas de solution admissible
                                   SPX_ERREUR_INTERNE si probleme a l'execution (saturation memoire
                                  par exemple), et dans ce cas il n'y a pas de solution
                                   SPX_MATRICE_DE_BASE_SINGULIERE si on n'a pas pu construire de
                                  matrice de base reguliere, et dans ce cas il n'y a pas de solution
                                */

    PROBLEMES_SIMPLEXE* ProblemesSpx;

    int* PositionDeLaVariable; /* Vecteur a passer au Simplexe pour recuperer la base optimale */
    int* ComplementDeLaBase;   /* Vecteur a passer au Simplexe pour recuperer la base optimale */

    /* Vecteurs de travail pour contruire la matrice des contraintes lineaires */
    double* Pi;
    int* Colonne;

    /* Nommage des variables & contraintes */
    std::vector<std::string> NomDesVariables;
    std::vector<std::string> NomDesContraintes;

    std::vector<bool> VariablesEntieres; // true = int, false = continuous

} PROBLEME_ANTARES_A_RESOUDRE;

#endif /* __SOLVER_OPTIMISATION_STRUCTURE_PROBLEME_A_RESOUDRE_H__ */
