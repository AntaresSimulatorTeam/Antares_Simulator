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

#define BOOST_TEST_MODULE test translator
#define BOOST_TEST_DYN_LINK

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>
#include <antares/solver/optimisation/HebdoProblemToLpsTranslator.h>
#include "antares/solver/optimisation/opt_period_string_generator_base.h"

using namespace Antares::Solver;

class StubOptPeriodStringGenerator : public OptPeriodStringGenerator {
public:
    std::string to_string() const override {
        return "Plop";
    }
};

BOOST_AUTO_TEST_CASE(null_hebdo_is_empty_lps) {
    HebdoProblemToLpsTranslator translator(std::make_shared<StubOptPeriodStringGenerator>());
    auto ret = translator.translate(nullptr);
    BOOST_CHECK(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(non_null_hebdo_returns_non_empty_lps) {
    HebdoProblemToLpsTranslator translator(std::make_shared<StubOptPeriodStringGenerator>());
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;

    auto ret = translator.translate(&problemHebdo);
    BOOST_CHECK(ret != nullptr);
}

BOOST_AUTO_TEST_CASE(Data_properly_copied) {
    HebdoProblemToLpsTranslator translator(std::make_shared<StubOptPeriodStringGenerator>());
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.CoutLineaire = {0, 1, 2};
    problemHebdo.Xmax = {10, 11, 12};
    problemHebdo.Xmin = {20, 21, 22};
    problemHebdo.NomDesVariables = {"a", "b", "c"};
    problemHebdo.NomDesContraintes = {"d", "e", "f"};
    problemHebdo.SecondMembre = {30, 31, 32};

    auto ret = translator.translate(&problemHebdo);
    BOOST_CHECK(ret->CoutLineaire == problemHebdo.CoutLineaire);
    BOOST_CHECK(ret->Xmax == problemHebdo.Xmax);
    BOOST_CHECK(ret->Xmin == problemHebdo.Xmin);
    BOOST_CHECK(ret->SecondMembre == problemHebdo.SecondMembre);

    BOOST_CHECK(ret->variables == problemHebdo.NomDesVariables);
    BOOST_CHECK(ret->constraints == problemHebdo.NomDesContraintes);
}

BOOST_AUTO_TEST_CASE(translate_sens) {
    HebdoProblemToLpsTranslator translator(std::make_shared<StubOptPeriodStringGenerator>());
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.Sens = "<=>";

    auto ret = translator.translate(&problemHebdo);
    BOOST_CHECK(ret->Sens == std::vector({'<','=','>'}));
}

BOOST_AUTO_TEST_CASE(translate_name_is_filled) {
    HebdoProblemToLpsTranslator translator(std::make_shared<StubOptPeriodStringGenerator>());
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;

    auto ret = translator.translate(&problemHebdo);
    BOOST_CHECK(!ret->name.empty());
}

BOOST_AUTO_TEST_CASE(translate_name_is_properly_filled) {
    HebdoProblemToLpsTranslator translator(std::make_shared<StubOptPeriodStringGenerator>(), 1);
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;

    auto ret = translator.translate(&problemHebdo);
    BOOST_CHECK_EQUAL(ret->name, "problem-Plop--optim-nb-1.mps");
}

BOOST_AUTO_TEST_CASE(empty_problem_empty_const_data) {
        HebdoProblemToLpsTranslator translator(std::make_shared<StubOptPeriodStringGenerator>());
        auto ret = translator.commonProblemData(nullptr);
        BOOST_CHECK(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(common_data_properly_copied) {
    HebdoProblemToLpsTranslator translator(std::make_shared<StubOptPeriodStringGenerator>());
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;
    problemHebdo.NombreDeVariables = 1;
    problemHebdo.NombreDeContraintes = 2;
    problemHebdo.TypeDeVariable = {0, 1, 2};
    problemHebdo.IndicesDebutDeLigne = {0, 3};
    problemHebdo.NombreDeTermesDesLignes = {3, 3};
    problemHebdo.CoefficientsDeLaMatriceDesContraintes = {0, 1, 2, 3, 4, 5};
    problemHebdo.IndicesColonnes = {0, 1, 2, 3, 4, 5};



    auto ret = translator.commonProblemData(&problemHebdo);
    BOOST_CHECK_EQUAL(ret->NombreDeVariables, problemHebdo.NombreDeVariables);
    BOOST_CHECK_EQUAL(ret->NombreDeContraintes, problemHebdo.NombreDeContraintes);
    BOOST_CHECK(ret->TypeDeVariable == problemHebdo.TypeDeVariable);
    BOOST_CHECK(ret->CoefficientsDeLaMatriceDesContraintes == problemHebdo.CoefficientsDeLaMatriceDesContraintes);
    BOOST_CHECK(ret->IndicesColonnes == problemHebdo.IndicesColonnes);
    BOOST_CHECK(ret->Mdeb == problemHebdo.IndicesDebutDeLigne);
    BOOST_CHECK_EQUAL(ret->NombreDeCoefficients, 6);
}

//Test NombreDeCoefficients
