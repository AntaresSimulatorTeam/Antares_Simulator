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

#define BOOST_TEST_MODULE test_translator
#define WIN32_LEAN_AND_MEAN

#include <span>

#include <boost/test/unit_test.hpp>

#include <antares/solver/optimisation/HebdoProblemToLpsTranslator.h>
#include "antares/solver/utils/opt_period_string_generator.h"

using namespace Antares::Solver;

class StubOptPeriodStringGenerator: public OptPeriodStringGenerator
{
public:
    std::string to_string() const override
    {
        return "Plop";
    }
};

BOOST_AUTO_TEST_CASE(null_hebdo_is_empty_lps)
{
    HebdoProblemToLpsTranslator translator;
    auto ret = translator.translate(nullptr, std::string());
    BOOST_CHECK(ret == WeeklyDataFromAntares());
}

BOOST_AUTO_TEST_CASE(non_null_hebdo_returns_non_empty_lps)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.CoutLineaire.push_back(45);
    auto ret = translator.translate(&problemHebdo, std::string());
    BOOST_CHECK(ret != WeeklyDataFromAntares());
}

BOOST_AUTO_TEST_CASE(Data_properly_copied)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.CoutLineaire = {0, 1, 2};
    problemHebdo.Xmax = {10, 11, 12};
    problemHebdo.Xmin = {20, 21, 22};
    problemHebdo.NomDesVariables = {"a", "b", "c"};
    problemHebdo.NomDesContraintes = {"d", "e", "f"};
    problemHebdo.SecondMembre = {30, 31, 32};

    auto ret = translator.translate(&problemHebdo, std::string());
    BOOST_CHECK(ret.LinearCost == problemHebdo.CoutLineaire);
    BOOST_CHECK(ret.Xmax == problemHebdo.Xmax);
    BOOST_CHECK(ret.Xmin == problemHebdo.Xmin);
    BOOST_CHECK(ret.RHS == problemHebdo.SecondMembre);

    BOOST_CHECK(ret.variables == problemHebdo.NomDesVariables);
    BOOST_CHECK(ret.constraints == problemHebdo.NomDesContraintes);
}

BOOST_AUTO_TEST_CASE(translate_sens)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.Sens = "<=>";

    auto ret = translator.translate(&problemHebdo, std::string());
    BOOST_CHECK(ret.Direction == std::vector({'<', '=', '>'}));
}

BOOST_AUTO_TEST_CASE(translate_name_is_filled)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;

    auto ret = translator.translate(&problemHebdo, "dummy");
    BOOST_CHECK(!ret.name.empty());
}

BOOST_AUTO_TEST_CASE(translate_name_is_properly_filled)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;

    auto ret = translator.translate(&problemHebdo, "problem-Plop--optim-nb-1.mps");
    BOOST_CHECK_EQUAL(ret.name, "problem-Plop--optim-nb-1.mps");
}

BOOST_AUTO_TEST_CASE(empty_problem_empty_const_data)
{
    HebdoProblemToLpsTranslator translator;
    auto ret = translator.commonProblemData(nullptr);
    BOOST_CHECK(ret == ConstantDataFromAntares());
}

template<class T>
static void fillSparseVector(SparseVector<T>& v, int idxMax)
{
    for (int idx = 0; idx < idxMax; idx++)
    {
        v[idx] = idx;
    }
}

BOOST_AUTO_TEST_CASE(common_data_properly_copied)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.NombreDeVariables = 1;
    problemHebdo.NombreDeContraintes = 2;
    problemHebdo.TypeDeVariable = {0, 1, 2};
    problemHebdo.IndicesDebutDeLigne = {0, 3};
    problemHebdo.NombreDeTermesDesLignes = {3, 3};
    fillSparseVector(problemHebdo.CoefficientsDeLaMatriceDesContraintes, 6);
    fillSparseVector(problemHebdo.IndicesColonnes, 6);

    auto ret = translator.commonProblemData(&problemHebdo);

    BOOST_CHECK_EQUAL(ret.VariablesCount, problemHebdo.NombreDeVariables);
    BOOST_CHECK_EQUAL(ret.ConstraintesCount, problemHebdo.NombreDeContraintes);
    BOOST_CHECK(std::ranges::equal(ret.VariablesType, problemHebdo.TypeDeVariable));
    BOOST_CHECK(ret.ConstraintsMatrixCoeff
                == problemHebdo.CoefficientsDeLaMatriceDesContraintes.extract());
    BOOST_CHECK(std::ranges::equal(ret.ColumnIndexes, problemHebdo.IndicesColonnes.extract()));
    auto expectedMdeb = problemHebdo.IndicesDebutDeLigne;
    expectedMdeb.push_back(problemHebdo.CoefficientsDeLaMatriceDesContraintes.size());
    BOOST_CHECK(std::ranges::equal(ret.Mdeb, expectedMdeb));
}

// throw exception if NombreDeVariables is 0
BOOST_AUTO_TEST_CASE(throw_exception_if_NombreDeVariables_is_0)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.NombreDeVariables = 0;
    BOOST_CHECK_THROW(translator.commonProblemData(&problemHebdo), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(throw_exception_if_NombreDeContraintes_is_0)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.NombreDeContraintes = 0;
    BOOST_CHECK_THROW(translator.commonProblemData(&problemHebdo), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(throw_exception_if_IndicesDebutDeLigne_out_of_bound)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.NombreDeVariables = 1;
    problemHebdo.NombreDeContraintes = 3;
    problemHebdo.IndicesDebutDeLigne = {0, 3};
    problemHebdo.NombreDeTermesDesLignes = {0, 3, 6, 7, 8};
    BOOST_CHECK_THROW(translator.commonProblemData(&problemHebdo), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(throw_exception_if_NombreDeTermesDesLignes_out_of_bound)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.NombreDeVariables = 1;
    problemHebdo.NombreDeContraintes = 3;
    problemHebdo.NombreDeTermesDesLignes = {0, 3};
    problemHebdo.IndicesDebutDeLigne = {0, 3, 6, 7, 8};
    BOOST_CHECK_THROW(translator.commonProblemData(&problemHebdo), std::runtime_error);
}

// NombreDeCoefficients
BOOST_AUTO_TEST_CASE(NombreDeCoefficients_is_properly_computed)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.NombreDeVariables = 1;
    problemHebdo.NombreDeContraintes = 3;
    problemHebdo.IndicesDebutDeLigne = {0, 3, 6};
    problemHebdo.NombreDeTermesDesLignes = {3, 3, 3};

    fillSparseVector(problemHebdo.CoefficientsDeLaMatriceDesContraintes, 9);
    fillSparseVector(problemHebdo.IndicesColonnes, 9);

    auto ret = translator.commonProblemData(&problemHebdo);
    BOOST_CHECK_EQUAL(ret.CoeffCount, 9);
}
