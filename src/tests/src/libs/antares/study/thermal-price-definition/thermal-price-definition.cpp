// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test thermal price definition"

#define WIN32_LEAN_AND_MEAN

#include <filesystem>
#include <fstream>

#include <boost/test/unit_test.hpp>

#include <yuni/io/file.h>

#include <antares/checks/checkLoadedInputData.h>
#include <antares/exception/LoadingError.hpp>
#include <antares/study/study.h>
#include "antares/study/parts/thermal/cluster_list.h"

using namespace Antares::Data;
using std::filesystem::temp_directory_path;

// Use RAII to simplify teardown
struct ThermalIniFile
{
    explicit ThermalIniFile()
    {
        const auto folder = temp_directory_path();
        std::ofstream outfile(folder / "list.ini", std::ofstream::out | std::ofstream::trunc);

        outfile << "[some cluster]" << std::endl;
        outfile << "name = some cluster" << std::endl;
        outfile << "group = Lignite" << std::endl;
        outfile << "unitcount = 4" << std::endl;
        outfile << "nominalcapacity = 900.000000" << std::endl;
        outfile << "min-stable-power = 400.000000" << std::endl;
        outfile << "min-up-time = 24" << std::endl;
        outfile << "min-down-time = 24" << std::endl;
        outfile << "co2 = 2.000000" << std::endl;
        outfile << "marginal-cost = 23.000000" << std::endl;
        outfile << "fixed-cost = 1700.000000" << std::endl;
        outfile << "startup-cost = 70000.000000" << std::endl;
        outfile << "market-bid-cost = 35.000000" << std::endl;
        outfile << "costgeneration = useCostTimeseries" << std::endl;
        outfile << "efficiency = 36.00000" << std::endl;
        outfile << "variableomcost = 12.120000" << std::endl;

        outfile.flush();
    }

    ~ThermalIniFile() noexcept
    {
        auto folder = temp_directory_path();
        std::filesystem::remove(folder / "list.ini");
    }
};

struct TimeSeriesFile
{
    TimeSeriesFile(const std::string& name, std::size_t size):
        name_(name)
    {
        folder = temp_directory_path();
        std::ofstream outfile(folder / name, std::ofstream::out | std::ofstream::trunc);

        for (std::size_t i = 0; i < size; i++)
        {
            outfile << 1 << std::endl;
        }
    }

    ~TimeSeriesFile()
    {
        std::filesystem::remove(folder / name_);
    }

    std::string getFolder()
    {
        return folder.string();
    }

private:
    std::filesystem::path folder;
    const std::string name_;
};

// =================
// The fixture
// =================
struct FixtureFull: private ThermalIniFile
{
    FixtureFull(const FixtureFull& f) = delete;
    FixtureFull(const FixtureFull&& f) = delete;
    FixtureFull& operator=(const FixtureFull& f) = delete;
    FixtureFull& operator=(const FixtureFull&& f) = delete;

    FixtureFull()
    {
        area = addAreaToListOfAreas(study->areas, "area");
        study->parameters.include.thermal.minUPTime = true;
        study->parameters.include.thermal.minStablePower = true;
        study->parameters.include.reserve.spinning = true;
        folder = temp_directory_path().string();
    }

    std::string folder;
    ThermalClusterList clusterList;
    Area* area;

    Study::Ptr study = std::make_shared<Study>();
};

struct FixtureStudyOnly
{
    Study::Ptr study = std::make_shared<Study>();
};

// ==================
// Tests section
// ==================

// Here, we need the "lightweight fixture"
BOOST_AUTO_TEST_SUITE(EconomicInputData_loadFromFolder)

BOOST_FIXTURE_TEST_CASE(EconomicInputData_loadFromFolder_OK, FixtureStudyOnly)
{
    TimeSeriesFile fuelCostTSfile("fuelCost.txt", 8760);
    EconomicInputData eco;
    BOOST_CHECK(eco.loadFromFolder(*study, fuelCostTSfile.getFolder()));

    BOOST_CHECK_EQUAL(eco.fuelcost[0][1432], 1);
}

BOOST_FIXTURE_TEST_CASE(EconomicInputData_loadFromFolder_failing_not_enough_values,
                        FixtureStudyOnly)
{
    TimeSeriesFile fuelCostTSfile("fuelCost.txt", 80);
    EconomicInputData eco;
    BOOST_CHECK(!eco.loadFromFolder(*study, fuelCostTSfile.getFolder()));
}

BOOST_FIXTURE_TEST_CASE(EconomicInputData_loadFromFolder_working_with_many_values, FixtureStudyOnly)
{
    TimeSeriesFile co2CostTSfile("CO2Cost.txt", 10000);
    EconomicInputData eco;
    BOOST_CHECK(eco.loadFromFolder(*study, co2CostTSfile.getFolder()));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(ThermalClusterList_loadFromFolder_basic, FixtureFull)
{
    clusterList.loadFromFolder(folder, area);
    auto cluster = clusterList.findInAll("some cluster");

    BOOST_CHECK(cluster->startupCost == 70000.0);
    BOOST_CHECK(cluster->costgeneration == useCostTimeseries);
    BOOST_CHECK(cluster->fuelEfficiency == 36.0);
    BOOST_CHECK(cluster->variableomcost == 12.12);
}

BOOST_FIXTURE_TEST_CASE(checkCo2_checkCO2CostColumnNumber_OK, FixtureFull)
{
    area->thermal.list.loadFromFolder(folder, area);
    auto cluster = area->thermal.list.findInAll("some cluster");

    cluster->series.timeSeries.reset(3, 8760);

    auto& ecoInput = cluster->ecoInput;
    ecoInput.co2cost.reset(3, 8760);

    BOOST_CHECK_NO_THROW(Antares::Check::checkCO2CostColumnNumber(study->areas));
}

BOOST_FIXTURE_TEST_CASE(checkCo2_checkCO2CostColumnNumber_KO, FixtureFull)
{
    area->thermal.list.loadFromFolder(folder, area);
    auto cluster = area->thermal.list.findInAll("some cluster");

    cluster->series.timeSeries.reset(3, 8760);

    auto& ecoInput = cluster->ecoInput;
    ecoInput.co2cost.reset(2, 8760);

    BOOST_CHECK_THROW(Antares::Check::checkCO2CostColumnNumber(study->areas),
                      Antares::Error::IncompatibleCO2CostColumns);
}

BOOST_FIXTURE_TEST_CASE(checkFuelAndCo2_checkColumnNumber_OK, FixtureFull)
{
    area->thermal.list.loadFromFolder(folder, area);
    auto cluster = area->thermal.list.findInAll("some cluster");

    cluster->series.timeSeries.reset(3, 8760);

    cluster->ecoInput.fuelcost.reset(3, 8760);
    cluster->ecoInput.co2cost.reset(3, 8760);

    BOOST_CHECK_NO_THROW(Antares::Check::checkFuelCostColumnNumber(study->areas));
    BOOST_CHECK_NO_THROW(Antares::Check::checkCO2CostColumnNumber(study->areas));
}

// Arbitrary non-constant function, with values in [0, 1]
double marketBidModulation(uint hour)
{
    return hour / 10000;
}

// Arbitrary non-constant function, with values in [0, 1]
double marginalCostModulation(uint hour)
{
    return hour / 20000;
}

BOOST_FIXTURE_TEST_CASE(ThermalCluster_costGenManualCalculationOfMarketBidAndMarginalCostPerHour,
                        FixtureFull)
{
    clusterList.loadFromFolder(folder, area);
    auto cluster = clusterList.findInAll("some cluster");

    cluster->modulation.resize(thermalModulationMax, HOURS_PER_YEAR);
    cluster->costgeneration = Data::setManually;

    auto& cp = cluster->getCostProvider();
    for (uint hour = 0; hour < HOURS_PER_YEAR; hour++)
    {
        cluster->modulation[thermalModulationMarketBid][hour] = marketBidModulation(hour);
        cluster->modulation[thermalModulationCost][hour] = marginalCostModulation(hour);
    }

    for (uint hour = 0; hour < HOURS_PER_YEAR; hour++)
    {
        BOOST_CHECK_EQUAL(cp.getMarketBidCost(hour, 0),
                          cluster->marketBidCost * marketBidModulation(hour));
        BOOST_CHECK_EQUAL(cp.getMarginalCost(hour, 0),
                          cluster->marginalCost * marginalCostModulation(hour));
    }
}

BOOST_FIXTURE_TEST_CASE(
  ThermalCluster_costGenTimeSeriesCalculationOfMarketBidAndMarginalCostPerHour,
  FixtureFull)
{
    TimeSeriesFile fuel("fuelCost.txt", 8760);
    TimeSeriesFile co2("CO2Cost.txt", 8760);

    clusterList.loadFromFolder(folder, area);
    auto cluster = clusterList.findInAll("some cluster");

    cluster->modulation.resize(thermalModulationMax, HOURS_PER_YEAR);
    cluster->modulation.fill(1.);

    cluster->ecoInput.loadFromFolder(*study, folder);

    cluster->tsNumbers.reset(1);

    auto& cp = cluster->getCostProvider();
    BOOST_CHECK_CLOSE(cp.getMarginalCost(0, 0), 24.12, 0.001);
    BOOST_CHECK_CLOSE(cp.getMarketBidCost(2637, 0), 24.12, 0.001);
}

BOOST_FIXTURE_TEST_CASE(computeMarketBidCost_useTimeSeries, FixtureFull)
{
    clusterList.loadFromFolder(folder, area);
    auto cluster = clusterList.findInAll("some cluster");
    BOOST_CHECK_CLOSE(
      computeMarketBidCost(1, cluster->fuelEfficiency, 2, 1, cluster->variableomcost),
      24.12,
      0.001);
}

BOOST_AUTO_TEST_CASE(non_constant_marketbid_modulation)
{
    Area area;
    ThermalCluster cluster(&area);
    cluster.costgeneration = setManually;
    cluster.marketBidCost = 120;

    auto& mod = cluster.modulation;
    mod.resize(thermalModulationMax, HOURS_PER_YEAR);
    mod.fill(1.);

    {
        mod[thermalModulationMarketBid][0] = .5;
        BOOST_CHECK_EQUAL(cluster.getCostProvider().getMarketBidCost(0, 0), .5 * 120);
    }

    {
        mod[thermalModulationMarketBid][1] = .8;
        BOOST_CHECK_EQUAL(cluster.getCostProvider().getMarketBidCost(1, 0), .8 * 120);
    }
}

BOOST_AUTO_TEST_SUITE_END()
