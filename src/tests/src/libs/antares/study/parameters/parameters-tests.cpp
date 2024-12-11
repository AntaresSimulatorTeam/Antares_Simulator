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
#define BOOST_TEST_MODULE "test parameters"

#include <filesystem>
#include <fstream>
#include <iostream>

#include <boost/test/unit_test.hpp>

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
    BOOST_CHECK_EQUAL(p.optOptions.ortoolsSolver, "sirius");
}

BOOST_FIXTURE_TEST_CASE(loadValid, Fixture)
{
    options.optOptions.ortoolsSolver = "xpress";

    writeValidFile();
    p.loadFromFile(path.string(), version);
    p.validateOptions(options);
    p.fixBadValues();

    BOOST_CHECK_EQUAL(p.nbYears, 5);
    BOOST_CHECK_EQUAL(p.seed[seedTsGenThermal], 5489);
    BOOST_CHECK_EQUAL(p.include.reserve.dayAhead, true);
    BOOST_CHECK_EQUAL(p.optOptions.ortoolsSolver, "xpress");
}

BOOST_FIXTURE_TEST_CASE(fixBadValue, Fixture)
{
    p.nbYears = 100;
    p.derated = true;
    p.nbTimeSeriesThermal = 0;

    p.fixBadValues();

    BOOST_CHECK_EQUAL(p.nbYears, 1);
    BOOST_CHECK_EQUAL(p.nbTimeSeriesThermal, 1);
}

BOOST_FIXTURE_TEST_CASE(invalidValues, Fixture)
{
    writeInvalidFile();
    BOOST_CHECK(p.loadFromFile(path.string(), version));
    p.validateOptions(options);
    p.fixBadValues();

    BOOST_CHECK_EQUAL(p.nbYears, 1);
    BOOST_CHECK_EQUAL(p.useCustomScenario, 0);
    BOOST_CHECK_EQUAL(p.firstWeekday, 0);
    BOOST_CHECK_EQUAL(p.renewableGeneration(), rgUnknown);
}

BOOST_FIXTURE_TEST_CASE(hydroPmax, Fixture)
{
    BOOST_CHECK(p.compatibility.hydroPmax == Parameters::Compatibility::HydroPmax::Daily);

    writeValidFile();
    BOOST_CHECK(p.loadFromFile(path.string(), version));
    p.validateOptions(options);
    p.fixBadValues();

    BOOST_CHECK(p.compatibility.hydroPmax == Parameters::Compatibility::HydroPmax::Hourly);

    BOOST_CHECK_EQUAL(CompatibilityHydroPmaxToCString(p.compatibility.hydroPmax), "hourly");
    BOOST_CHECK_EQUAL(CompatibilityHydroPmaxToCString(Parameters::Compatibility::HydroPmax::Daily),
                      "daily");

    BOOST_CHECK(StringToCompatibilityHydroPmax(p.compatibility.hydroPmax, "daily"));
    BOOST_CHECK(!StringToCompatibilityHydroPmax(p.compatibility.hydroPmax, ""));
    BOOST_CHECK(!StringToCompatibilityHydroPmax(p.compatibility.hydroPmax, "abc"));
}

BOOST_AUTO_TEST_SUITE_END()

void Fixture::writeInvalidFile()
{
    std::ofstream outfile(path);

    outfile << R"([general]
            nbyears = abc
            custom-scenario = 27
            first.weekday = 21

            [other preferences]
            renewable-generation-modelling = abc)";

    outfile.close();
}

void Fixture::writeValidFile()
{
    std::ofstream outfile(path);
    outfile << R"([general]
            mode = Economy
            horizon = 2000
            nbyears = 5
            simulation.start = 1
            simulation.end = 365
            january.1st = Monday
            first-month-in-year = january
            first.weekday = Monday
            leapyear = false
            year-by-year = false
            derated = false
            custom-scenario = false
            user-playlist = false
            thematic-trimming = false
            geographic-trimming = false
            generate = hydro, wind, thermal
            nbtimeseriesload = 1
            nbtimeserieshydro = 1
            nbtimeserieswind = 1
            nbtimeseriesthermal = 1
            nbtimeseriessolar = 1
            refreshtimeseries =
            intra-modal =
            inter-modal =
            refreshintervalload = 0
            refreshintervalhydro = 5
            refreshintervalwind = 5
            refreshintervalthermal = 5
            refreshintervalsolar = 0
            readonly = false

            [input]
            import =

            [output]
            synthesis = false
            storenewset = false
            archives =
            result-format = txt-files

            [optimization]
            simplex-range = day
            transmission-capacities = local-values
            include-constraints = true
            include-hurdlecosts = true
            include-tc-minstablepower = true
            include-tc-min-ud-time = true
            include-dayahead = true
            include-strategicreserve = true
            include-spinningreserve = true
            include-primaryreserve = true
            include-exportmps = none
            include-exportstructure = false
            include-unfeasible-problem-behavior = error-verbose

            [adequacy patch]
            include-adq-patch = false
            set-to-null-ntc-from-physical-out-to-physical-in-for-first-step = true
            set-to-null-ntc-between-physical-out-for-first-step = true
            enable-first-step = true
            price-taking-order = DENS
            include-hurdle-cost-csr = false
            check-csr-cost-function = false
            threshold-initiate-curtailment-sharing-rule = 0.000000
            threshold-display-local-matching-rule-violations = 0.000000
            threshold-csr-variable-bounds-relaxation = 3

            [other preferences]
            hydro-heuristic-policy = accommodate rule curves
            hydro-pricing-mode = fast
            power-fluctuations = free modulations
            shedding-policy = shave peaks
            unit-commitment-mode = fast
            number-of-cores-mode = medium
            renewable-generation-modelling = aggregated

            [advanced parameters]
            accuracy-on-correlation =

            [seeds - Mersenne Twister]
            seed-tsgen-wind = 5489
            seed-tsgen-load = 5489
            seed-tsgen-hydro = 5489
            seed-tsgen-thermal = 5489
            seed-tsgen-solar = 5489
            seed-tsnumbers = 5489
            seed-unsupplied-energy-costs = 6005489
            seed-spilled-energy-costs = 7005489
            seed-thermal-costs = 8005489
            seed-hydro-costs = 9005489
            seed-initial-reservoir-levels = 10005489

            [compatibility]
            hydro-pmax = hourly)";

    outfile.close();
}
