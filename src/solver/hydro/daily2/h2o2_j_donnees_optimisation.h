/*
** Copyright 2007-2023 RTE
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
#ifndef __SOLVER_H2O2_J_STRUCTURE_INTERNE__
#define __SOLVER_H2O2_J_STRUCTURE_INTERNE__

#ifdef __CPLUSPLUS
extern "C"
{
#endif

#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#ifdef __CPLUSPLUS
}

#endif
#include "../daily/h2o_j_donnees_optimisation.h"
#include <antares/study/study.h>
#include <antares/mersenne-twister/mersenne-twister.h>

#define LINFINI 1.e+80

#define JOURS_28 28
#define JOURS_29 29
#define JOURS_30 30
#define JOURS_31 31
#define NOMBRE_DE_TYPE_DE_MOIS 4

/*--------------------------------------------------------------------------------------*/
/* Matrice des contraintes: il y aura une seule instance pour tous les reservoirs */
/* Dans ce struct il n'y a que des donnees qui sont lues et surtout pas ecrites   */
/* Ce struct est instancie une seule fois                                         */
typedef struct
{
    int NombreDeVariables;
    std::vector<double> CoutLineaire;
    std::vector<int> TypeDeVariable; /* Indicateur du type de variable, il ne doit prendre que les suivantes
                                                        (voir le fichier spx_constantes_externes.h
                            mais ne jamais utiliser les valeurs explicites des constantes):
                                                        VARIABLE_FIXE                  ,
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
} PROBLEME_LINEAIRE_ETENDU_PARTIE_FIXE;

/* Partie variable renseignee avant le lancement de l'optimisation de chaque reservoir */
typedef struct
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
} PROBLEME_LINEAIRE_ETENDU_PARTIE_VARIABLE;

/* Les correspondances des variables */
typedef struct
{
    std::vector<int> NumeroVar_Turbine; /* Turbines */

    std::vector<int> NumeroVar_niveauxFinJours; // Niveaux fin jours
    int NumeroVar_waste;            // Waste
    std::vector<int> NumeroVar_overflow;   // Deversements (ecarts journaliers entre niveaux et les 100 % du
                               // reservoir)
    std::vector<int> NumeroVar_deviations; // Deviations (ecarts journaliers entre turbin?s et cr?dits cibles
                               // brutes)
    std::vector<int> NumeroVar_violations; // Violations (ecarts journaliers entre niveaux et courbes guides sup
                               // et inf)
    int NumeroVar_deviationMax; // Deviation max sur le mois
    int NumeroVar_violationMax; // Violation max sur le mois
} CORRESPONDANCE_DES_VARIABLES_PB_ETENDU;

/* Structure uniquement exploitee par l'optimisation (donc a ne pas acceder depuis l'exterieur) */
typedef struct
{
    char LesCoutsOntEteInitialises; /* Vaut OUI ou NON */

    int NombreDeProblemes;
    std::vector<int> NbJoursDUnProbleme;

    std::vector<CORRESPONDANCE_DES_VARIABLES_PB_ETENDU> CorrespondanceDesVariables;

    std::vector<PROBLEME_LINEAIRE_ETENDU_PARTIE_FIXE> ProblemeLineaireEtenduPartieFixe;
    std::vector<PROBLEME_LINEAIRE_ETENDU_PARTIE_VARIABLE> ProblemeLineaireEtenduPartieVariable;

    std::vector<PROBLEME_SPX*> ProblemeSpx; /* Il y en a 1 par reservoir. Un probleme couvre 1 mois */
} PROBLEME_HYDRAULIQUE_ETENDU;

namespace Antares::Constants
{
constexpr double noiseAmplitude = 1e-3;
constexpr unsigned int seed = 0x79683264; // "hyd2" in hexa
} // namespace Antares::Constants

class Hydro_problem_costs
{
public:
    Hydro_problem_costs(const Data::Parameters& parameters);

    inline double get_end_days_levels_cost() const
    {
        return end_days_levels + noiseGenerator() * Constants::noiseAmplitude;
    }
    inline double get_overflow_cost() const
    {
        return overflow + noiseGenerator() * Constants::noiseAmplitude;
    }
    inline double get_deviations_cost() const
    {
        return deviations + noiseGenerator() * Constants::noiseAmplitude;
    }
    inline double get_violations_cost() const
    {
        return violations + noiseGenerator() * Constants::noiseAmplitude;
    }
    inline double get_waste_cost() const
    {
        return waste + noiseGenerator() * Constants::noiseAmplitude;
    }
    inline double get_deviationMax_cost() const
    {
        return deviationMax + noiseGenerator() * Constants::noiseAmplitude;
    }
    inline double get_violationMax_cost() const
    {
        return violationMax + noiseGenerator() * Constants::noiseAmplitude;
    }

private:
    double end_days_levels;
    double overflow;
    double deviations;
    double violations;
    double waste;
    double deviationMax;
    double violationMax;

    mutable Antares::MersenneTwister noiseGenerator;
};

#endif
