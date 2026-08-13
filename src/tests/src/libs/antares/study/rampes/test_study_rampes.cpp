// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE study
#include <files-system.h>
#include <unit_test_utils.h>
#include <antares/inifile/inifile.h>
using Antares::UnitTests::CaptureAntaresLogs;

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include "antares/study/parts/thermal/cluster.h"
#include "antares/study/study.h"

#include "../../../../../../libs/antares/study/parts/thermal/cluster_list.cpp"

using namespace Antares::Data;

/*!
 * Study with one area named "A"
 */
class OneProblemOneAreaRamping
{
public:
    OneProblemOneAreaRamping()
    {
        study = std::make_unique<Study>();
        areaA = addAreaToListOfAreas(study->areas, "A");
        study->parameters.include.thermal_ramping = true;
    }

    std::unique_ptr<Study> study;
    Area* areaA;
};

class OneProblemOneAreaNotRamping
{
public:
    OneProblemOneAreaNotRamping()
    {
        study = std::make_unique<Study>();
        areaA = addAreaToListOfAreas(study->areas, "A");
        study->parameters.include.thermal_ramping = false;
    }

    std::unique_ptr<Study> study;
    Area* areaA;
};

class OneProblemOneAreaRampingWithLogger: public OneProblemOneAreaRamping, public CaptureAntaresLogs
{
};

static void addThermalCluster(Area* area, const std::string& name)
{
    auto c = std::make_shared<ThermalCluster>(area);
    c->setName(name);
    area->thermal.list.addToCompleteList(c);
}
BOOST_AUTO_TEST_SUITE(ramping_load)

BOOST_FIXTURE_TEST_CASE(test_thermal_load_cluster_ramp_parameters, OneProblemOneAreaRamping)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream file(studyPath / "list.ini");
    file << "[thermal_1]\n";
    file << "name = thermal_1\n";
    file << "ramping-enabled = true\n";
    file << "max-upward-power-ramping-rate = 2.200000\n";
    file << "max-downward-power-ramping-rate = 3.300000\n";
    file << "power-increase-cost = 4.400000\n";
    file << "power-decrease-cost = 5.500000\n";
    file << "\n";
    file << "[thermal_2]\n";
    file << "name = thermal_2\n";
    file << "ramping-enabled = false\n";
    file << "max-upward-power-ramping-rate = 6.600000\n";
    file << "max-downward-power-ramping-rate = 7.700000\n";
    file << "power-increase-cost = 8.800000\n";
    file << "power-decrease-cost = 9.900000\n";
    file.close();

    Antares::IniFile ini;
    BOOST_CHECK_EQUAL(ini.open(studyPath / "list.ini"), true);
    auto* section = ini.firstSection;
    BOOST_CHECK_EQUAL(section->name.empty(), false);

    auto cluster = std::make_shared<ThermalCluster>(areaA);
    auto cluster2 = std::make_shared<ThermalCluster>(areaA);

    ThermalClusterLoadFromSection(areaA->name,
                                  *cluster,
                                  *section,
                                  study->parameters.include.thermal_ramping);
    section = section->next;
    BOOST_CHECK_EQUAL(section->name.empty(), false);
    ThermalClusterLoadFromSection(areaA->name,
                                  *cluster2,
                                  *section,
                                  study->parameters.include.thermal_ramping);
    BOOST_CHECK_EQUAL(cluster->name(), "thermal_1");
    BOOST_CHECK(cluster->ramping.has_value());
    BOOST_CHECK_EQUAL(cluster->ramping->maxUpwardPowerRampingRate, 2.2);
    BOOST_CHECK_EQUAL(cluster->ramping->maxDownwardPowerRampingRate, 3.3);
    BOOST_CHECK_EQUAL(cluster->ramping->powerIncreaseCost, 4.4);
    BOOST_CHECK_EQUAL(cluster->ramping->powerDecreaseCost, 5.5);
    BOOST_CHECK_EQUAL(cluster->ramping->checkValidity(areaA, "thermal_1"), true);

    std::ostringstream rampingStats;
    rampingStats << *cluster->ramping;

    BOOST_CHECK_EQUAL("powerIncreaseCost = 4.4\tpowerDecreaseCost = "
                      "5.5\tmaxUpwardPowerRampingRate = 2.2\tmaxDownwardPowerRampingRate = 3.3",
                      rampingStats.str());

    BOOST_CHECK_EQUAL(cluster2->name(), "thermal_2");
    BOOST_CHECK_EQUAL(cluster2->ramping.has_value(), false);
}

BOOST_FIXTURE_TEST_CASE(test_thermal_load_cluster_no_ramp_parameters, OneProblemOneAreaNotRamping)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream file(studyPath / "list.ini");
    file << "[thermal_1]\n";
    file << "name = thermal_1\n";
    file << "ramping-enabled = true\n";
    file << "max-upward-power-ramping-rate = 2.200000\n";
    file << "max-downward-power-ramping-rate = 3.300000\n";
    file << "power-increase-cost = 4.400000\n";
    file << "power-decrease-cost = 5.500000\n";
    file << "\n";
    file << "[thermal_2]\n";
    file << "name = thermal_2\n";
    file << "ramping-enabled = false\n";
    file << "max-upward-power-ramping-rate = 6.600000\n";
    file << "max-downward-power-ramping-rate = 7.700000\n";
    file << "power-increase-cost = 8.800000\n";
    file << "power-decrease-cost = 9.900000\n";
    file.close();

    Antares::IniFile ini;
    BOOST_CHECK_EQUAL(ini.open(studyPath / "list.ini"), true);
    auto* section = ini.firstSection;
    BOOST_CHECK_EQUAL(section->name.empty(), false);

    auto cluster = std::make_shared<ThermalCluster>(areaA);
    auto cluster2 = std::make_shared<ThermalCluster>(areaA);

    ThermalClusterLoadFromSection(areaA->name,
                                  *cluster,
                                  *section,
                                  study->parameters.include.thermal_ramping);
    section = section->next;
    BOOST_CHECK_EQUAL(section->name.empty(), false);
    ThermalClusterLoadFromSection(areaA->name,
                                  *cluster2,
                                  *section,
                                  study->parameters.include.thermal_ramping);
    BOOST_CHECK_EQUAL(cluster->name(), "thermal_1");
    BOOST_CHECK_EQUAL(cluster->ramping.has_value(), false);

    BOOST_CHECK_EQUAL(cluster2->name(), "thermal_2");
    BOOST_CHECK_EQUAL(cluster2->ramping.has_value(), false);
}

BOOST_FIXTURE_TEST_CASE(test_thermal_load_cluster_invalid_ramp_parameters,
                        OneProblemOneAreaRampingWithLogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream file(studyPath / "list.ini");
    file << "[thermal_1]\n";
    file << "name = thermal_1\n";
    file << "ramping-enabled = true\n";
    file << "max-upward-power-ramping-rate = -2.200000\n";
    file << "max-downward-power-ramping-rate = -3.300000\n";
    file << "power-increase-cost = -4.400000\n";
    file << "power-decrease-cost = -5.500000\n";
    file << "\n";
    file.close();

    Antares::IniFile ini;
    BOOST_CHECK_EQUAL(ini.open(studyPath / "list.ini"), true);
    auto* section = ini.firstSection;
    BOOST_CHECK_EQUAL(section->name.empty(), false);

    auto cluster = std::make_shared<ThermalCluster>(areaA);

    ThermalClusterLoadFromSection(areaA->name,
                                  *cluster,
                                  *section,
                                  study->parameters.include.thermal_ramping);

    BOOST_CHECK_EQUAL(cluster->name(), "thermal_1");
    BOOST_CHECK_EQUAL(cluster->ramping.has_value(), true);

    BOOST_CHECK_EQUAL(cluster->ramping->checkValidity(areaA, "thermal_1"), false);
    BOOST_CHECK_EQUAL(getErrors().size(), 4);
    BOOST_CHECK_EQUAL(getWarnings().size(), 0);
    BOOST_CHECK(getErrors().contains(
      "Thermal cluster: A/thermal_1: The maximum upward power ramping rate "
      "must be greater than zero. Ramping is disabled for this thermal cluster."));
    BOOST_CHECK(getErrors().contains(
      "Thermal cluster: A/thermal_1: The maximum downward power ramping rate "
      "must be greater than zero. Ramping is disabled for this thermal cluster."));
    BOOST_CHECK(
      getErrors().contains("Thermal cluster: A/thermal_1: The ramping power increase cost must be "
                           "positive or null. Ramping is disabled for this thermal cluster."));
    BOOST_CHECK(
      getErrors().contains("Thermal cluster: A/thermal_1: The ramping power decrease cost must be "
                           "positive or null. Ramping is disabled for this thermal cluster."));
}

BOOST_AUTO_TEST_SUITE_END()
