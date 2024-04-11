
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

struct ProblemHebdoId
{
    unsigned int year;
    unsigned int week;
    //Order of comparison is order of member declaration
    auto operator<=>(const ProblemHebdoId& other) const = default;
};

// Type de données inutile car les matrices de tous les pbs hebdo sont
// identiques. Cela pourra changer à l'avenir si des coefficients de contraintes
// couplantes peuvent varier au cours du temps (ex: rendement d'une pompe à
// chaleur qui varie selon la température, FlowBased ?, etc)

class ConstantDataFromAntares
{
public:
    int NombreDeVariables; // Mathématiquement : Nb colonnes de la matrice,
    // Informatiquement = TypeDeVariable.size()
    int NombreDeContraintes; // Mathématiqument : Nb lignes de la matrice,
    // Informatiquement = Mdeb.size()
    int NombreDeCoefficients; // Mathématiquement : Nb coeffs non nuls de la
    // matrice, Informatiquement = Nbterm.size() =
    // IndicesColonnes.size()=
    // CoefficientsDeLaMatriceDesContraintes.size()

    std::vector<int> TypeDeVariable; // Variables entières ou biniaires
    std::vector<int> Mdeb;           // Indique dans les indices dans le vecteur IndicesColonnes qui
    // correspondent au début de chaque ligne. Ex : Mdeb[3] = 8 et
    // Mdeb[4] = 13 -> Les termes IndicesColonnes[8] à
    // IndicesColonnes[12] correspondent à des Id de colonnes de la
    // ligne 3 de la matrice (en supposant que les lignes sont indexées
    // à partir de 0)
    std::vector<int> Nbterm; // Nombre de termes non nuls sur chaque ligne.
    // Inutile car NbTerm[i] = Mdeb[i+1] - Mdeb[i]
    std::vector<int> IndicesColonnes; // Id des colonnes des termes de
    // CoefficientsDeLaMatriceDesContraintes : Ex
    // IndicesColonnes[3] = 8 ->
    // CoefficientsDeLaMatriceDesContraintes[8] donne la
    // valeur du terme de la colonne 8, et de la ligne i où
    // i est tel que Mdeb[i] <= 3 < Mdeb[i+1]
    std::vector<double> CoefficientsDeLaMatriceDesContraintes; // Coefficients de la matrice

    std::vector<std::string> SignificationMetierDesVariables;
    std::vector<std::string> SignificationMetierDesContraintes;
};

class HebdoDataFromAntares
{
public:
    std::vector<char> Sens; // Sens de la contrainte : < ou > ou =, taille =
    // NombreDeContraintes
    std::vector<double> Xmax; // Borne max des variables de la semaine
    // considérée, taille = NombreDeVariables
    std::vector<double> Xmin; // Borne min des variables de la semaine
    // considérée, taille =  NombreDeVariables
    std::vector<double> CoutLineaire; // Coefficients du vecteur de coût de la fonction objectif,
    // taille = NombreDeVariables
    std::vector<double> SecondMembre; // Vecteur des second membre des contraintes, taille =
    // NombreDeContraintes
    std::string name;

    std::vector<std::string> variables;
    std::vector<std::string> constraints;
};

using ConstantDataFromAntaresPtr = std::unique_ptr<ConstantDataFromAntares>;
using HebdoDataFromAntaresPtr = std::unique_ptr<HebdoDataFromAntares>;
using YearWeekHebdoDataFromAntares = std::map<ProblemHebdoId, HebdoDataFromAntaresPtr>;

class LpsFromAntares
{
public:
    ConstantDataFromAntaresPtr _constant;
    YearWeekHebdoDataFromAntares _hebdo;
    bool empty() const;
    void replaceConstantData(ConstantDataFromAntaresPtr uniquePtr);
    void addHebdoData(ProblemHebdoId id, HebdoDataFromAntaresPtr uniquePtr);
    const HebdoDataFromAntares* hebdoData(ProblemHebdoId id) const;
};

}