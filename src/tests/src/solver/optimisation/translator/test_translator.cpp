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

using namespace Antares::Solver;
BOOST_AUTO_TEST_CASE(null_hebdo_is_empty_lps) {
    HebdoProblemToLpsTranslator translator;
    auto ret = translator.translate(nullptr);
    BOOST_CHECK(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(non_null_hebdo_returns_non_empty_lps) {
    HebdoProblemToLpsTranslator translator;
    PROBLEME_ANTARES_A_RESOUDRE problemHebdo;

    auto ret = translator.translate(&problemHebdo);
    BOOST_CHECK(ret != nullptr);
}

BOOST_AUTO_TEST_CASE(Data_properly_copied) {
    HebdoProblemToLpsTranslator translator;
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

//TODO sens