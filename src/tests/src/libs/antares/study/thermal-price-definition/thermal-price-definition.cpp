#define BOOST_TEST_MODULE "test thermal price definition"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <yuni/io/file.h>
#include <filesystem>
#include <fstream>

#include <study.h>
#include <antares/logs.h>
#include <antares/exception/LoadingError.hpp>

#include "checkLoadedInputData.h"
#include "cluster_list.h"
#include "container.h"

const auto SEP = Yuni::IO::Separator;
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
    TimeSeriesFile(const std::string& name, std::size_t size) : name_(name)
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

    std::string getFolder() {
        return folder.string();
    }

private:
    std::filesystem::path folder;
    const std::string name_;
};

void fillThermalEconomicTimeSeries(ThermalCluster* c)
{
    c->thermalEconomicTimeSeries[0].productionCostTs.fill(1);
    c->thermalEconomicTimeSeries[0].marketBidCostPerHourTs.fill(1);
    c->thermalEconomicTimeSeries[0].marginalCostPerHourTs.fill(1);
}

// =================
// The fixture
// =================
struct FixtureFull : private ThermalIniFile
{
    FixtureFull(const FixtureFull& f) = delete;
    FixtureFull(const FixtureFull&& f) = delete;
    FixtureFull& operator=(const FixtureFull& f) = delete;
    FixtureFull& operator=(const FixtureFull&& f) = delete;
    FixtureFull()
    {
        area = study->areaAdd("area");
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
BOOST_FIXTURE_TEST_SUITE(EconomicInputData_loadFromFolder, FixtureStudyOnly)
BOOST_AUTO_TEST_CASE(EconomicInputData_loadFromFolder_OK)
{
    TimeSeriesFile fuelCostTSfile("fuelCost.txt", 8760);
    EconomicInputData eco;
    BOOST_CHECK(eco.loadFromFolder(*study, fuelCostTSfile.getFolder()));

    BOOST_CHECK_EQUAL(eco.fuelcost[0][1432], 1);
}

BOOST_AUTO_TEST_CASE(EconomicInputData_loadFromFolder_failing_not_enough_values)
{
    TimeSeriesFile fuelCostTSfile("fuelCost.txt", 80);
    EconomicInputData eco;
    BOOST_CHECK(!eco.loadFromFolder(*study, fuelCostTSfile.getFolder()));
}

BOOST_AUTO_TEST_CASE(EconomicInputData_loadFromFolder_working_with_many_values)
{
    TimeSeriesFile co2CostTSfile("CO2Cost.txt", 10000);
    EconomicInputData eco;
    BOOST_CHECK(eco.loadFromFolder(*study, co2CostTSfile.getFolder()));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(s, FixtureFull)
BOOST_AUTO_TEST_CASE(ThermalClusterList_loadFromFolder_basic)
{
    clusterList.loadFromFolder(*study, folder, area);
    BOOST_CHECK(clusterList.mapping["some cluster"]->startupCost == 70000.0);
    BOOST_CHECK(clusterList.mapping["some cluster"]->costgeneration == useCostTimeseries);
    BOOST_CHECK(clusterList.mapping["some cluster"]->fuelEfficiency == 36.0);
    BOOST_CHECK(clusterList.mapping["some cluster"]->variableomcost == 12.12);
}

BOOST_AUTO_TEST_CASE(checkCo2_checkCO2CostColumnNumber_OK)
{
    area->thermal.list.loadFromFolder(*study, folder, area);
    area->thermal.list.mapping["some cluster"]->series = new DataSeriesCommon;
    area->thermal.list.mapping["some cluster"]->series->timeSeries.reset(3, 8760);

    area->thermal.prepareAreaWideIndexes();

    auto& ecoInput = area->thermal.list.mapping["some cluster"]->ecoInput;
    ecoInput.co2cost.reset(3, 8760);

    BOOST_CHECK_NO_THROW(Antares::Check::checkCO2CostColumnNumber(study->areas));
}

BOOST_AUTO_TEST_CASE(checkCo2_checkCO2CostColumnNumber_KO)
{
    area->thermal.list.loadFromFolder(*study, folder, area);
    area->thermal.list.mapping["some cluster"]->series = new DataSeriesCommon;
    area->thermal.list.mapping["some cluster"]->series->timeSeries.reset(3, 8760);

    area->thermal.prepareAreaWideIndexes();

    auto& ecoInput = area->thermal.list.mapping["some cluster"]->ecoInput;
    ecoInput.co2cost.reset(2, 8760);

    BOOST_CHECK_THROW(Antares::Check::checkCO2CostColumnNumber(study->areas),
                      Antares::Error::IncompatibleCO2CostColumns);
}

BOOST_AUTO_TEST_CASE(checkFuelAndCo2_checkColumnNumber_OK)
{
    area->thermal.list.loadFromFolder(*study, folder, area);
    area->thermal.list.mapping["some cluster"]->series = new DataSeriesCommon;
    area->thermal.list.mapping["some cluster"]->series->timeSeries.reset(3, 8760);

    area->thermal.prepareAreaWideIndexes();

    area->thermal.list.mapping["some cluster"]->ecoInput.fuelcost.reset(3, 8760);
    area->thermal.list.mapping["some cluster"]->ecoInput.co2cost.reset
      (3, 8760);

    BOOST_CHECK_NO_THROW(Antares::Check::checkFuelCostColumnNumber(study->areas));
    BOOST_CHECK_NO_THROW(Antares::Check::checkCO2CostColumnNumber(study->areas));
}

BOOST_AUTO_TEST_CASE(ThermalCluster_costGenManualCalculationOfMarketBidAndMarginalCostPerHour)
{
    clusterList.loadFromFolder(*study, folder, area);
    clusterList.mapping["some cluster"]->costGenManualCalculationOfMarketBidAndMarginalCostPerHour();
    BOOST_CHECK_EQUAL(
      clusterList.mapping["some cluster"]->thermalEconomicTimeSeries[0].marketBidCostPerHourTs[2637], 35);
    BOOST_CHECK_EQUAL(
      clusterList.mapping["some cluster"]->thermalEconomicTimeSeries[0].marginalCostPerHourTs[6737], 23);
}

BOOST_AUTO_TEST_CASE(ThermalCluster_costGenTimeSeriesCalculationOfMarketBidAndMarginalCostPerHour)
{
    TimeSeriesFile fuel("fuelCost.txt", 8760);
    TimeSeriesFile co2("CO2Cost.txt", 8760);

    clusterList.loadFromFolder(*study, folder, area);
    clusterList.mapping["some cluster"]->modulation.reset(1, 8760);
    clusterList.mapping["some cluster"]->ecoInput.loadFromFolder(*study, folder);
    fillThermalEconomicTimeSeries(clusterList.mapping["some cluster"].get());

    clusterList.mapping["some cluster"]->costGenTimeSeriesCalculationOfMarketBidAndMarginalCostPerHour();

    BOOST_CHECK_CLOSE(
      clusterList.mapping["some cluster"]->thermalEconomicTimeSeries[0].marketBidCostPerHourTs[0],
      24.12,
      0.001);
    BOOST_CHECK_CLOSE(
      clusterList.mapping["some cluster"]->thermalEconomicTimeSeries[0].marketBidCostPerHourTs[2637],
      24.12,
      0.001);
}

BOOST_AUTO_TEST_CASE(computeMarketBidCost)
{
    clusterList.loadFromFolder(*study, folder, area);
    BOOST_CHECK_CLOSE(clusterList.mapping["some cluster"]->computeMarketBidCost(1, 2, 1), 24.12, 0.001);
}
BOOST_AUTO_TEST_SUITE_END()
