// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE import_prepro

#include <filesystem>

#include <boost/test/unit_test.hpp>

#include "antares/study/study.h"

#include "files-system.h"

namespace fs = std::filesystem;

using namespace Antares::Data;

/*!
 * Study with one area named "A", and a temp directory set as folderInput.
 * Sub-directories for each time series type are created upfront (needed for
 * load/solar/wind whose saveToFolder does not auto-create intermediate directories).
 */
struct ImportPreproFixture
{
    ImportPreproFixture()
    {
        study = std::make_unique<Study>();
        areaA = addAreaToListOfAreas(study->areas, "A");
        if (areaA)
        {
            areaA->createMissingData();
            areaA->resetToDefaultValues();
        }

        testFolder = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
        study->folderInput = testFolder;

        // Pre-create directories used by load, solar, and wind saveToFolder
        // (hydro and thermal create their own sub-directories internally)
        fs::create_directories(testFolder / "load" / "series");
        fs::create_directories(testFolder / "solar" / "series");
        fs::create_directories(testFolder / "wind" / "series");
    }

    ~ImportPreproFixture()
    {
        fs::remove_all(testFolder);
    }

    std::unique_ptr<Study> study;
    Area* areaA = nullptr;
    fs::path testFolder;
};

BOOST_AUTO_TEST_SUITE(import_timeseries)

BOOST_FIXTURE_TEST_CASE(returns_true_when_nothing_to_import, ImportPreproFixture)
{
    // Default: exportTimeSeriesInInput = 0, timeSeriesToGenerate = 0,
    //          thermalTSRefresh = false  -> nothing to do
    BOOST_CHECK(study->importTimeseriesIntoInput());
}

BOOST_FIXTURE_TEST_CASE(returns_true_when_generate_set_but_export_disabled, ImportPreproFixture)
{
    // timeSeriesToGenerate set but exportTimeSeriesInInput is still 0
    study->parameters.timeSeriesToGenerate = timeSeriesLoad | timeSeriesSolar | timeSeriesWind
                                             | timeSeriesHydro | timeSeriesThermal;
    study->parameters.exportTimeSeriesInInput = 0;
    BOOST_CHECK(study->importTimeseriesIntoInput());
    // No files should have been written
    BOOST_CHECK(!fs::exists(testFolder / "load" / "series" / "load_a.txt"));
}

BOOST_FIXTURE_TEST_CASE(load_timeseries_written_to_correct_path, ImportPreproFixture)
{
    study->parameters.exportTimeSeriesInInput = timeSeriesLoad;
    study->parameters.timeSeriesToGenerate = timeSeriesLoad;

    BOOST_CHECK(study->importTimeseriesIntoInput());

    BOOST_CHECK(fs::exists(testFolder / "load" / "series" / "load_a.txt"));
}

BOOST_FIXTURE_TEST_CASE(solar_timeseries_written_to_correct_path, ImportPreproFixture)
{
    study->parameters.exportTimeSeriesInInput = timeSeriesSolar;
    study->parameters.timeSeriesToGenerate = timeSeriesSolar;

    BOOST_CHECK(study->importTimeseriesIntoInput());

    BOOST_CHECK(fs::exists(testFolder / "solar" / "series" / "solar_a.txt"));
}

BOOST_FIXTURE_TEST_CASE(wind_timeseries_written_to_correct_path, ImportPreproFixture)
{
    study->parameters.exportTimeSeriesInInput = timeSeriesWind;
    study->parameters.timeSeriesToGenerate = timeSeriesWind;

    study->importTimeseriesIntoInput();

    BOOST_CHECK(fs::exists(testFolder / "wind" / "series" / "wind_a.txt"));
}

BOOST_FIXTURE_TEST_CASE(hydro_timeseries_written_to_correct_path, ImportPreproFixture)
{
    study->parameters.exportTimeSeriesInInput = timeSeriesHydro;
    study->parameters.timeSeriesToGenerate = timeSeriesHydro;

    BOOST_CHECK(study->importTimeseriesIntoInput());

    // hydro saves areaID/ror.txt, areaID/mod.txt and areaID/mingen.txt under the series folder
    const fs::path hydroSeriesPath = testFolder / "hydro" / "series" / "a";
    BOOST_CHECK(fs::exists(hydroSeriesPath / "ror.txt"));
    BOOST_CHECK(fs::exists(hydroSeriesPath / "mod.txt"));
    BOOST_CHECK(fs::exists(hydroSeriesPath / "mingen.txt"));
}

BOOST_FIXTURE_TEST_CASE(thermal_timeseries_written_when_thermalTSRefresh_and_export_enabled,
                        ImportPreproFixture)
{
    auto cluster = std::make_shared<ThermalCluster>(areaA);
    cluster->setName("Gas");
    cluster->enabled = true;
    areaA->thermal.list.addToCompleteList(cluster);
    areaA->thermal.list.buildIndexes();

    study->parameters.exportTimeSeriesInInput = timeSeriesThermal;
    study->runtime.thermalTSRefresh = true;

    BOOST_CHECK(study->importTimeseriesIntoInput());

    const fs::path thermalSeriesPath = testFolder / "thermal" / "series" / "a" / "gas";
    BOOST_CHECK(fs::exists(thermalSeriesPath / "series.txt"));
}

BOOST_FIXTURE_TEST_CASE(only_enabled_timeseries_types_are_imported, ImportPreproFixture)
{
    // Only load is configured; solar should not produce a file
    study->parameters.exportTimeSeriesInInput = timeSeriesLoad;
    study->parameters.timeSeriesToGenerate = timeSeriesLoad;

    BOOST_CHECK(study->importTimeseriesIntoInput());

    BOOST_CHECK(fs::exists(testFolder / "load" / "series" / "load_a.txt"));
    BOOST_CHECK(!fs::exists(testFolder / "solar" / "series" / "solar_a.txt"));
}

BOOST_AUTO_TEST_SUITE_END() // import_timeseries
