/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __SOLVER_H2O_J_STRUCTURE_INTERNE__
#define __SOLVER_H2O_J_STRUCTURE_INTERNE__

#ifdef __CPLUSPLUS
extern "C"
{
#endif

#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#ifdef __CPLUSPLUS
}
#endif

#define LINFINI 1.e+80

#define JOURS_28 28
#define JOURS_29 29
#define JOURS_30 30
#define JOURS_31 31
#define NOMBRE_DE_TYPE_DE_MOIS 4

#include <string>
#include <vector>

/*--------------------------------------------------------------------------------------*/
/* Matrice des contraintes: il y aura une seule instance pour tous les reservoirs */
/* Dans ce struct il n'y a que des donnees qui sont lues et surtout pas ecrites   */
/* Ce struct est instancie une seule fois                                         */
struct PROBLEME_LINEAIRE_PARTIE_FIXE
{
    int NombreDeVariables;
    std::vector<double> CoutLineaire;
    std::vector<int> TypeDeVariable; /* Indicateur du type de variable, il ne doit prendre que les suivantes
                           (voir le fichier spx_constantes_externes.h mais ne jamais utiliser les
                           valeurs explicites des constantes): VARIABLE_FIXE                  ,
                            VARIABLE_BORNEE_DES_DEUX_COTES ,
                            VARIABLE_BORNEE_INFERIEUREMENT ,
                            VARIABLE_BORNEE_SUPERIEUREMENT ,
                            VARIABLE_NON_BORNEE
                                           */
    /* La matrice des contraintes */
    int NombreDeContraintes;
    std::vector<char> Sens;
    std::vector<int> IndicesDebutDeLigne;
    std::vector<int> NombreDeTermesDesLignes;
    std::vector<double> CoefficientsDeLaMatriceDesContraintes;
    std::vector<int> IndicesColonnes;
    int NombreDeTermesAlloues;
};

/* Partie variable renseignee avant le lancement de l'optimisation de chaque reservoir */
struct PROBLEME_LINEAIRE_PARTIE_VARIABLE
{
    /* Donnees variables de la matrice des contraintes */
    /* On met quand-meme les bornes dans la partie variable pour le cas ou on voudrait avoir
             un jour des bornes min et max variables dans le temps et en fonction des reservoirs */
    std::vector<double> Xmin;
    std::vector<double> Xmax;
    std::vector<double> SecondMembre;
    /* Tableau de pointeur a des doubles. Ce tableau est parallele a X, il permet
       de renseigner directement les structures de description du reseau avec les
       resultats contenus dans X */
    std::vector<double*> AdresseOuPlacerLaValeurDesVariablesOptimisees;
    /* Resultat */
    std::vector<double> X;
    /* En Entree ou en Sortie */
    int ExistenceDUneSolution; /* En sortie, vaut :
                                  OUI_SPX s'il y a une solution,
                                                          NON_SPX s'il n'y a pas de solution
                                  admissible SPX_ERREUR_INTERNE si probleme a l'execution
                                  (saturation memoire par exemple), et dans ce cas il n'y a pas de
                                  solution SPX_MATRICE_DE_BASE_SINGULIERE si on n'a pas pu
                                  construire de matrice de base reguliere, et dans ce cas il n'y a
                                  pas de solution
                                                 */

    std::vector<int> PositionDeLaVariable; /* Vecteur a passer au Simplexe pour recuperer la base optimale */
    std::vector<int> ComplementDeLaBase;   /* Vecteur a passer au Simplexe pour recuperer la base optimale */
    std::vector<double> CoutsReduits;      /* Vecteur a passer au Simplexe pour recuperer les couts reduits */
    std::vector<double> CoutsMarginauxDesContraintes; /* Vecteur a passer au Simplexe pour recuperer les couts
                                             marginaux */
};

/* Les correspondances fixes des contraintes */
struct CORRESPONDANCE_DES_CONTRAINTES
{
    int NumeroDeContrainteDEnergieMensuelle;
    std::vector<int> NumeroDeContrainteSurXi;
};

/* Les correspondances des variables */
struct CORRESPONDANCE_DES_VARIABLES
{
    std::vector<int> NumeroDeVariableTurbine; /* Turbines */
    int NumeroDeLaVariableMu;     /* Variable de deversement (total sur la periode) */
    int NumeroDeLaVariableXi; /* Variable decrivant l'ecart max au turbine cible quand le turbine
                                 est inferieur au turbine cible */
};

/* Structure uniquement exploitee par l'optimisation (donc a ne pas acceder depuis l'exterieur) */
struct PROBLEME_HYDRAULIQUE
{
    char LesCoutsOntEteInitialises; /* Vaut OUI ou NON */

    int NombreDeProblemes;
    std::vector<int> NbJoursDUnProbleme;

    std::vector<CORRESPONDANCE_DES_VARIABLES> CorrespondanceDesVariables;
    std::vector<CORRESPONDANCE_DES_CONTRAINTES> CorrespondanceDesContraintes;

    std::vector<PROBLEME_LINEAIRE_PARTIE_FIXE> ProblemeLineairePartieFixe;
    std::vector<PROBLEME_LINEAIRE_PARTIE_VARIABLE> ProblemeLineairePartieVariable;

    std::vector<PROBLEME_SPX*> ProblemeSpx; /* Il y en a 1 par reservoir. Un probleme couvre 1 mois */
};

#endif
