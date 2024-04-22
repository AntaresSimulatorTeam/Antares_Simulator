/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#define BOOST_TEST_MODULE "test parameters"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <antares/study/study.h>

using namespace Antares::Data;

// =================
// The fixture
// =================
struct Fixture
{
    Parameters p;
};


// ==================
// Tests section
// ==================

BOOST_AUTO_TEST_SUITE(parameters_tests)

BOOST_FIXTURE_TEST_CASE(reset, Fixture)
{
    p.reset();
    BOOST_CHECK_EQUAL(p.simulationDays.first, 0);
    BOOST_CHECK_EQUAL(p.nbTimeSeriesThermal, 1);
    BOOST_CHECK_EQUAL(p.synthesis, true);
    BOOST_CHECK_EQUAL(p.ortoolsSolver, "sirius");
}

BOOST_AUTO_TEST_SUITE_END()
