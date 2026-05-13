/*
 * Copyright 2007-2026, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 */

#define BOOST_TEST_MODULE test cluster

#include <files-system.h>
#include <filesystem>
#include <fstream>

#include <boost/test/unit_test.hpp>

#include <antares/antares/constants.h>
#include <antares/study/parts/thermal/cluster.h>
#include <antares/study/study.h>

using namespace Antares::Data;
namespace fs = std::filesystem;

BOOST_AUTO_TEST_CASE(disabled_thermal_cluster_skips_series_loading)
{
    auto study = std::make_shared<Study>();
    auto area = addAreaToListOfAreas(study->areas, "area1");

    auto cluster = std::make_shared<ThermalCluster>(area);
    cluster->setName("my_cluster");
    cluster->enabled = false;
    area->thermal.list.addToCompleteList(cluster);

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    fs::create_directories(working_tmp_dir / area->id / cluster->id());

    std::ofstream seriesFile(working_tmp_dir / area->id / cluster->id() / "series.txt");
    for (int i = 0; i < HOURS_PER_YEAR; ++i)
    {
        seriesFile << "100\n";
    }
    seriesFile.close();

    bool ret = cluster->loadDataSeriesFromFolder(*study, working_tmp_dir);
    BOOST_CHECK_EQUAL(ret, true);
    BOOST_CHECK_EQUAL(cluster->series.timeSeries.width, 0);
}

BOOST_AUTO_TEST_CASE(enabled_thermal_cluster_loads_series)
{
    auto study = std::make_shared<Study>();
    auto area = addAreaToListOfAreas(study->areas, "area1");

    auto cluster = std::make_shared<ThermalCluster>(area);
    cluster->setName("my_cluster");
    cluster->enabled = true;
    area->thermal.list.addToCompleteList(cluster);

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    fs::create_directories(working_tmp_dir / area->id / cluster->id());

    std::ofstream seriesFile(working_tmp_dir / area->id / cluster->id() / "series.txt");
    for (int i = 0; i < HOURS_PER_YEAR; ++i)
    {
        seriesFile << "100\n";
    }
    seriesFile.close();

    bool ret = cluster->loadDataSeriesFromFolder(*study, working_tmp_dir);
    BOOST_CHECK_EQUAL(ret, true);
    BOOST_CHECK_EQUAL(cluster->series.timeSeries.width, 1);
    BOOST_CHECK_EQUAL(cluster->series.timeSeries.height, HOURS_PER_YEAR);
    BOOST_CHECK_EQUAL(cluster->series.timeSeries[0][0], 100);
}
