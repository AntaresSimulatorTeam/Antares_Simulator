// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test_translator
#define WIN32_LEAN_AND_MEAN

#include <span>

#include <boost/test/unit_test.hpp>

#include <antares/solver/optimisation/HebdoProblemToLpsTranslator.h>
#include "antares/solver/utils/opt_period_string_generator.h"

using namespace Antares::Solver;

class StubOptPeriodStringGenerator final: public OptPeriodStringGenerator
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
}

BOOST_AUTO_TEST_CASE(translate_sens)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.Sens = "<=>";
    problemHebdo.NombreDeContraintes = 3;

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
static void fillVector(T& v, int idxMax)
{
    v.resize(idxMax);
    for (int idx = 0; idx < idxMax; idx++)
    {
        v[idx] = idx;
    }
}

BOOST_AUTO_TEST_CASE(common_data_properly_copied)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.NombreDeVariables = 3;
    problemHebdo.NombreDeContraintes = 2;
    problemHebdo.IndicesDebutDeLigne = {0, 3};
    problemHebdo.NombreDeTermesDesLignes = {3, 3};
    problemHebdo.NomDesVariables = {"a", "b", "c"};
    problemHebdo.NomDesContraintes = {"d", "e"};
    fillVector(problemHebdo.CoefficientsDeLaMatriceDesContraintes, 6);
    fillVector(problemHebdo.IndicesColonnes, 6);

    auto ret = translator.commonProblemData(&problemHebdo);

    BOOST_CHECK_EQUAL(ret.VariablesCount, problemHebdo.NombreDeVariables);
    BOOST_CHECK_EQUAL(ret.ConstraintesCount, problemHebdo.NombreDeContraintes);
    BOOST_CHECK(ret.ConstraintsMatrixCoeff == problemHebdo.CoefficientsDeLaMatriceDesContraintes);
    BOOST_CHECK(std::ranges::equal(ret.ColumnIndexes, problemHebdo.IndicesColonnes));
    auto expectedMdeb = problemHebdo.IndicesDebutDeLigne;
    expectedMdeb.push_back(problemHebdo.CoefficientsDeLaMatriceDesContraintes.size());
    BOOST_CHECK(std::ranges::equal(ret.Mdeb, expectedMdeb));

    BOOST_CHECK(ret.VariablesMeaning == problemHebdo.NomDesVariables);
    BOOST_CHECK(ret.ConstraintsMeaning == problemHebdo.NomDesContraintes);
}

// throw exception if NombreDeVariables is 0
BOOST_AUTO_TEST_CASE(throw_exception_if_NombreDeVariables_is_0)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.NombreDeVariables = 0;
    BOOST_CHECK_THROW((void)translator.commonProblemData(&problemHebdo), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(throw_exception_if_NombreDeContraintes_is_0)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.NombreDeContraintes = 0;
    BOOST_CHECK_THROW((void)translator.commonProblemData(&problemHebdo), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(throw_exception_if_IndicesDebutDeLigne_out_of_bound)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.NombreDeVariables = 1;
    problemHebdo.NombreDeContraintes = 3;
    problemHebdo.IndicesDebutDeLigne = {0, 3};
    problemHebdo.NombreDeTermesDesLignes = {0, 3, 6, 7, 8};
    BOOST_CHECK_THROW((void)translator.commonProblemData(&problemHebdo), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(throw_exception_if_NombreDeTermesDesLignes_out_of_bound)
{
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.NombreDeVariables = 1;
    problemHebdo.NombreDeContraintes = 3;
    problemHebdo.NombreDeTermesDesLignes = {0, 3};
    problemHebdo.IndicesDebutDeLigne = {0, 3, 6, 7, 8};
    BOOST_CHECK_THROW((void)translator.commonProblemData(&problemHebdo), std::runtime_error);
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

    fillVector(problemHebdo.CoefficientsDeLaMatriceDesContraintes, 9);
    fillVector(problemHebdo.IndicesColonnes, 9);

    auto ret = translator.commonProblemData(&problemHebdo);
    BOOST_CHECK_EQUAL(ret.CoeffCount, 9);
}
