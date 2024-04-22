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

BOOST_AUTO_TEST_SUITE_END()


void Fixture::writeValidFile()
{
    std::ofstream outfile(path);
    outfile << "[general]" << std::endl;
    outfile << "mode = Economy" << std::endl;
    outfile << "horizon = 2000" << std::endl;
    outfile << "nbyears = 5" << std::endl;
    outfile << "simulation.start = 1" << std::endl;
    outfile << "simulation.end = 365" << std::endl;
    outfile << "january.1st = Monday" << std::endl;
    outfile << "first-month-in-year = january" << std::endl;
    outfile << "first.weekday = Monday" << std::endl;
    outfile << "leapyear = false" << std::endl;
    outfile << "year-by-year = false" << std::endl;
    outfile << "derated = false" << std::endl;
    outfile << "custom-scenario = false" << std::endl;
    outfile << "user-playlist = false" << std::endl;
    outfile << "thematic-trimming = false" << std::endl;
    outfile << "geographic-trimming = false" << std::endl;
    outfile << "generate = hydro, wind, thermal" << std::endl;
    outfile << "nbtimeseriesload = 1" << std::endl;
    outfile << "nbtimeserieshydro = 1" << std::endl;
    outfile << "nbtimeserieswind = 1" << std::endl;
    outfile << "nbtimeseriesthermal = 1" << std::endl;
    outfile << "nbtimeseriessolar = 1" << std::endl;
    outfile << "refreshtimeseries = " << std::endl;
    outfile << "intra-modal = " << std::endl;
    outfile << "inter-modal = " << std::endl;
    outfile << "refreshintervalload = 0" << std::endl;
    outfile << "refreshintervalhydro = 5" << std::endl;
    outfile << "refreshintervalwind = 5" << std::endl;
    outfile << "refreshintervalthermal = 5" << std::endl;
    outfile << "refreshintervalsolar = 0" << std::endl;
    outfile << "readonly = false" << std::endl;

    outfile << "[input]" << std::endl;
    outfile << "import = " << std::endl;

    outfile << "[output]" << std::endl;
    outfile << "synthesis = false" << std::endl;
    outfile << "storenewset = false" << std::endl;
    outfile << "archives = " << std::endl;
    outfile << "result-format = txt-files" << std::endl;

    outfile << "[optimization]" << std::endl;
    outfile << "simplex-range = day" << std::endl;
    outfile << "transmission-capacities = local-values" << std::endl;
    outfile << "include-constraints = true" << std::endl;
    outfile << "include-hurdlecosts = true" << std::endl;
    outfile << "include-tc-minstablepower = true" << std::endl;
    outfile << "include-tc-min-ud-time = true" << std::endl;
    outfile << "include-dayahead = true" << std::endl;
    outfile << "include-strategicreserve = true" << std::endl;
    outfile << "include-spinningreserve = true" << std::endl;
    outfile << "include-primaryreserve = true" << std::endl;
    outfile << "include-exportmps = none" << std::endl;
    outfile << "include-exportstructure = false" << std::endl;
    outfile << "include-unfeasible-problem-behavior = error-verbose" << std::endl;

    outfile << "[adequacy patch]" << std::endl;
    outfile << "include-adq-patch = false" << std::endl;
    outfile << "set-to-null-ntc-from-physical-out-to-physical-in-for-first-step = true" << std::endl;
    outfile << "set-to-null-ntc-between-physical-out-for-first-step = true" << std::endl;
    outfile << "enable-first-step = true" << std::endl;
    outfile << "price-taking-order = DENS" << std::endl;
    outfile << "include-hurdle-cost-csr = false" << std::endl;
    outfile << "check-csr-cost-function = false" << std::endl;
    outfile << "threshold-initiate-curtailment-sharing-rule = 0.000000" << std::endl;
    outfile << "threshold-display-local-matching-rule-violations = 0.000000" << std::endl;
    outfile << "threshold-csr-variable-bounds-relaxation = 3" << std::endl;

    outfile << "[other preferences]" << std::endl;
    outfile << "initial-reservoir-levels = cold start" << std::endl;
    outfile << "hydro-heuristic-policy = accommodate rule curves" << std::endl;
    outfile << "hydro-pricing-mode = fast" << std::endl;
    outfile << "power-fluctuations = free modulations" << std::endl;
    outfile << "shedding-policy = shave peaks" << std::endl;
    outfile << "unit-commitment-mode = fast" << std::endl;
    outfile << "number-of-cores-mode = medium" << std::endl;
    outfile << "renewable-generation-modelling = aggregated" << std::endl;

    outfile << "[advanced parameters]" << std::endl;
    outfile << "accuracy-on-correlation = " << std::endl;

    outfile << "[seeds - Mersenne Twister]" << std::endl;
    outfile << "seed-tsgen-wind = 5489" << std::endl;
    outfile << "seed-tsgen-load = 5489" << std::endl;
    outfile << "seed-tsgen-hydro = 5489" << std::endl;
    outfile << "seed-tsgen-thermal = 5489" << std::endl;
    outfile << "seed-tsgen-solar = 5489" << std::endl;
    outfile << "seed-tsnumbers = 5489" << std::endl;
    outfile << "seed-unsupplied-energy-costs = 6005489" << std::endl;
    outfile << "seed-spilled-energy-costs = 7005489" << std::endl;
    outfile << "seed-thermal-costs = 8005489" << std::endl;
    outfile << "seed-hydro-costs = 9005489" << std::endl;
    outfile << "seed-initial-reservoir-levels = 10005489" << std::endl;


    outfile.close();
}
