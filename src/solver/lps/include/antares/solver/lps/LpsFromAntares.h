
/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once
#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Antares::Solver
{

/**
 * @struct WeeklyProblemId
 * @brief The WeeklyProblemId struct is used to identify a weekly problem by year and week.
 */
struct WeeklyProblemId
{
    unsigned int year = 0;
    unsigned int week = 0;
    // Order of comparison is order of member declaration
    auto operator<=>(const WeeklyProblemId& other) const = default;
};

// Type de données inutile car les matrices de tous les pbs Weekly sont
// identiques. Cela pourra changer à l'avenir si des coefficients de contraintes
// couplantes peuvent varier au cours du temps (ex: rendement d'une pompe à
// chaleur qui varie selon la température, FlowBased ?, etc)
/**
 * @class ConstantDataFromAntares
 * @brief The ConstantDataFromAntares class is used to store constant data across all weeks
 * of Antares problems.
 */
struct ConstantDataFromAntares
{
    unsigned VariablesCount = 0; // Mathématiquement : Nb colonnes de la matrice,
    // Informatiquement = TypeDeVariable.size()
    unsigned ConstraintesCount = 0; // Mathématiqument : Nb lignes de la matrice,
    // Informatiquement = Mdeb.size()
    unsigned CoeffCount = 0; // Mathématiquement : Nb coeffs non nuls de la
    // matrice, Informatiquement = Nbterm.size() =
    // IndicesColonnes.size()=
    // CoefficientsDeLaMatriceDesContraintes.size()

    std::vector<unsigned> VariablesType; // Variables entières ou biniaires
    std::vector<unsigned> Mdeb; // Indique dans les indices dans le vecteur IndicesColonnes qui
    // correspondent au début de chaque ligne. Ex : Mdeb[3] = 8 et
    // Mdeb[4] = 13 -> Les termes IndicesColonnes[8] à
    // IndicesColonnes[12] correspondent à des Id de colonnes de la
    // ligne 3 de la matrice (en supposant que les lignes sont indexées
    // à partir de 0)
    std::vector<unsigned> NotNullTermCount; // Nombre de termes non nuls sur chaque ligne.
    // Inutile car NbTerm[i] = Mdeb[i+1] - Mdeb[i]
    std::vector<unsigned> ColumnIndexes; // Id des colonnes des termes de
    // CoefficientsDeLaMatriceDesContraintes : Ex
    // IndicesColonnes[3] = 8 ->
    // CoefficientsDeLaMatriceDesContraintes[8] donne la
    // valeur du terme de la colonne 8, et de la ligne i où
    // i est tel que Mdeb[i] <= 3 < Mdeb[i+1]
    std::vector<double> ConstraintsMatrixCoeff; // Coefficients de la matrice

    std::vector<std::string> VariablesMeaning;
    std::vector<std::string> ConstraintsMeaning;

    auto operator<=>(const ConstantDataFromAntares& other) const = default;
};

/**
 * @class WeeklyDataFromAntares
 * @brief The WeeklyDataFromAntares class is used to store weekly data for an Antares Problem.
 */
struct WeeklyDataFromAntares
{
    std::vector<char> Direction; // Sens de la contrainte : < ou > ou =, taille =
    // NombreDeContraintes
    std::vector<double> Xmax; // Borne max des variables de la semaine
    // considérée, taille = NombreDeVariables
    std::vector<double> Xmin; // Borne min des variables de la semaine
    // considérée, taille =  NombreDeVariables
    std::vector<double> LinearCost; // Coefficients du vecteur de coût de la fonction objectif,
    // taille = NombreDeVariables
    std::vector<double> RHS; // Vecteur des second membre des contraintes, taille =
    // NombreDeContraintes
    std::string name;

    std::vector<std::string> variables;
    std::vector<std::string> constraints;

    auto operator<=>(const WeeklyDataFromAntares& other) const = default;
};

using WeeklyDataByYearWeek = std::map<WeeklyProblemId, WeeklyDataFromAntares>;

/**
 * @class LpsFromAntares
 * @brief The LpsFromAntares class is used to manage the constant and weekly data for Antares
 * problems.
 */
class LpsFromAntares
{
public:
    /*
     * @brief Checks if the LpsFromAntares object is empty.
     * Emptiness is defined by either the constant data or the weekly data being empty.
     */
    bool empty() const;
    /*
     * @brief Replaces the constant data in the LpsFromAntares object.
     * Copy happens
     */
    void setConstantData(const ConstantDataFromAntares& data);
    /*
     * @brief Adds weekly data to the LpsFromAntares object.
     */
    void addWeeklyData(WeeklyProblemId id, const WeeklyDataFromAntares& data);
    /*
     * @brief Retrieves weekly data from the LpsFromAntares object.
     */
    const WeeklyDataFromAntares& weeklyData(WeeklyProblemId id) const;
    /*
     * @brief Retrieves the number of weeks in the LpsFromAntares object.
     */
    [[nodiscard]] size_t weekCount() const noexcept;

    ConstantDataFromAntares constantProblemData;
    WeeklyDataByYearWeek weeklyProblems;
};

} // namespace Antares::Solver
