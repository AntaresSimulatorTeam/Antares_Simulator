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
#include <filesystem>
#include <iostream>
#include <fstream>

#include <antares/study/study.h>

using namespace Antares::Data;

// =================
// The fixture
// =================
struct Fixture
{
    Parameters p;
    std::filesystem::path path;
    StudyLoadOptions options;
    StudyVersion version = StudyVersion::latest();

    Fixture()
    {
        path = std::filesystem::temp_directory_path() / "generaldata.ini";
    }

    void writeValidFile();
    void writeInvalidFile();
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

BOOST_FIXTURE_TEST_CASE(loadValid, Fixture)
{
    options.ortoolsSolver = "xpress";

    writeValidFile();
    p.loadFromFile(path.string(), version, options);

    BOOST_CHECK_EQUAL(p.nbYears, 5);
    BOOST_CHECK_EQUAL(p.seed[seedTsGenThermal], 5489);
    BOOST_CHECK_EQUAL(p.include.reserve.dayAhead, true);
    BOOST_CHECK_EQUAL(p.ortoolsSolver, "xpress");
}

BOOST_FIXTURE_TEST_CASE(fixBadValue, Fixture)
{
    p.nbYears = 100;
    p.derated = true;
    p.nbTimeSeriesThermal = 0;

    p.fixBadValues();

    BOOST_CHECK_EQUAL(p.nbYears, 1);
    BOOST_CHECK_EQUAL(p.nbTimeSeriesThermal, 1);

    p.nbYears = 10000000;
    p.derated = false;

    p.fixBadValues();

    BOOST_CHECK_EQUAL(p.nbYears, 100000);
}

BOOST_FIXTURE_TEST_CASE(invalidValues, Fixture)
{
    writeInvalidFile();
    p.loadFromFile(path.string(), version, options);

    BOOST_CHECK_EQUAL(p.nbYears, 1);
    BOOST_CHECK_EQUAL(p.useCustomScenario, 0);
    BOOST_CHECK_EQUAL(p.firstWeekday, 0);
    BOOST_CHECK_EQUAL(p.renewableGeneration(), rgUnknown);
    std::cout << p.firstWeekday;
}

BOOST_AUTO_TEST_SUITE_END()

void Fixture::writeInvalidFile()
{
    std::ofstream outfile(path);

    outfile << "[general]" << std::endl
            << "nbyears = abc" << std::endl
            << "custom-scenario = 27" << std::endl
            << "first.weekday = 21" << std::endl

            << "[other preferences]" << std::endl
            << "renewable-generation-modelling = abc" << std::endl;

    outfile.close();
}

void Fixture::writeValidFile()
{
    std::ofstream outfile(path);
    outfile << "[general]" << std::endl
            << "mode = Economy" << std::endl
            << "horizon = 2000" << std::endl
            << "nbyears = 5" << std::endl
            << "simulation.start = 1" << std::endl
            << "simulation.end = 365" << std::endl
            << "january.1st = Monday" << std::endl
            << "first-month-in-year = january" << std::endl
            << "first.weekday = Monday" << std::endl
            << "leapyear = false" << std::endl
            << "year-by-year = false" << std::endl
            << "derated = false" << std::endl
            << "custom-scenario = false" << std::endl
            << "user-playlist = false" << std::endl
            << "thematic-trimming = false" << std::endl
            << "geographic-trimming = false" << std::endl
            << "generate = hydro, wind, thermal" << std::endl
            << "nbtimeseriesload = 1" << std::endl
            << "nbtimeserieshydro = 1" << std::endl
            << "nbtimeserieswind = 1" << std::endl
            << "nbtimeseriesthermal = 1" << std::endl
            << "nbtimeseriessolar = 1" << std::endl
            << "refreshtimeseries = " << std::endl
            << "intra-modal = " << std::endl
            << "inter-modal = " << std::endl
            << "refreshintervalload = 0" << std::endl
            << "refreshintervalhydro = 5" << std::endl
            << "refreshintervalwind = 5" << std::endl
            << "refreshintervalthermal = 5" << std::endl
            << "refreshintervalsolar = 0" << std::endl
            << "readonly = false" << std::endl

            << "[input]" << std::endl
            << "import = " << std::endl

            << "[output]" << std::endl
            << "synthesis = false" << std::endl
            << "storenewset = false" << std::endl
            << "archives = " << std::endl
            << "result-format = txt-files" << std::endl

            << "[optimization]" << std::endl
            << "simplex-range = day" << std::endl
            << "transmission-capacities = local-values" << std::endl
            << "include-constraints = true" << std::endl
            << "include-hurdlecosts = true" << std::endl
            << "include-tc-minstablepower = true" << std::endl
            << "include-tc-min-ud-time = true" << std::endl
            << "include-dayahead = true" << std::endl
            << "include-strategicreserve = true" << std::endl
            << "include-spinningreserve = true" << std::endl
            << "include-primaryreserve = true" << std::endl
            << "include-exportmps = none" << std::endl
            << "include-exportstructure = false" << std::endl
            << "include-unfeasible-problem-behavior = error-verbose" << std::endl

            << "[adequacy patch]" << std::endl
            << "include-adq-patch = false" << std::endl
            << "set-to-null-ntc-from-physical-out-to-physical-in-for-first-step = true" << std::endl
            << "set-to-null-ntc-between-physical-out-for-first-step = true" << std::endl
            << "enable-first-step = true" << std::endl
            << "price-taking-order = DENS" << std::endl
            << "include-hurdle-cost-csr = false" << std::endl
            << "check-csr-cost-function = false" << std::endl
            << "threshold-initiate-curtailment-sharing-rule = 0.000000" << std::endl
            << "threshold-display-local-matching-rule-violations = 0.000000" << std::endl
            << "threshold-csr-variable-bounds-relaxation = 3" << std::endl

            << "[other preferences]" << std::endl
            << "initial-reservoir-levels = cold start" << std::endl
            << "hydro-heuristic-policy = accommodate rule curves" << std::endl
            << "hydro-pricing-mode = fast" << std::endl
            << "power-fluctuations = free modulations" << std::endl
            << "shedding-policy = shave peaks" << std::endl
            << "unit-commitment-mode = fast" << std::endl
            << "number-of-cores-mode = medium" << std::endl
            << "renewable-generation-modelling = aggregated" << std::endl

            << "[advanced parameters]" << std::endl
            << "accuracy-on-correlation = " << std::endl

            << "[seeds - Mersenne Twister]" << std::endl
            << "seed-tsgen-wind = 5489" << std::endl
            << "seed-tsgen-load = 5489" << std::endl
            << "seed-tsgen-hydro = 5489" << std::endl
            << "seed-tsgen-thermal = 5489" << std::endl
            << "seed-tsgen-solar = 5489" << std::endl
            << "seed-tsnumbers = 5489" << std::endl
            << "seed-unsupplied-energy-costs = 6005489" << std::endl
            << "seed-spilled-energy-costs = 7005489" << std::endl
            << "seed-thermal-costs = 8005489" << std::endl
            << "seed-hydro-costs = 9005489" << std::endl
            << "seed-initial-reservoir-levels = 10005489" << std::endl;

    outfile.close();
}
