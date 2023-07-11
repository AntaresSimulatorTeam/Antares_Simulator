#define BOOST_TEST_MODULE "test thermal price definition"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <yuni/io/file.h>
#include <filesystem>
#include <fstream>

#include <study.h>
#include <antares/exception/LoadingError.hpp>

#include "checkApplication.h"
#include "cluster_list.h"
#include "container.h"

const auto SEP = Yuni::IO::Separator;
using namespace Antares::Data;

static std::filesystem::path getFolder()
{
    return std::filesystem::temp_directory_path();
}

// Use RAII to simplify teardown
struct ThermalIniFile
{
    explicit ThermalIniFile()
    {
        auto folder = getFolder();

        std::ofstream outfile(folder / "list.ini", std::ofstream::out | std::ofstream::trunc);

        outfile << "[area]" << std::endl;
        outfile << "name = area" << std::endl;
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
        auto folder = getFolder();
        std::filesystem::remove(folder / "list.ini");
    }
};

struct SeriesFile
{
    SeriesFile(const std::string& name, std::size_t size) : name_(name)
    {
        auto folder = getFolder();
        std::ofstream outfile(folder / name, std::ofstream::out | std::ofstream::trunc);

        for (std::size_t i = 0; i < size; i++)
        {
            outfile << 1 << std::endl;
        }
    }

    ~SeriesFile()
    {
        auto folder = getFolder();
        std::filesystem::remove(folder / name_);
    }

private:
    const std::string name_;
};

struct CO2CostFile : private SeriesFile
{
    CO2CostFile(std::size_t size) : SeriesFile("CO2Cost.txt", size)
    {
    }
};

struct FuelCostFile : private SeriesFile
{
    FuelCostFile(std::size_t size) : SeriesFile("fuelCost.txt", size)
    {
    }
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
struct Fixture : private ThermalIniFile
{
    Fixture(const Fixture& f) = delete;
    Fixture(const Fixture&& f) = delete;
    Fixture& operator=(const Fixture& f) = delete;
    Fixture& operator=(const Fixture&& f) = delete;
    Fixture()
    {
        area = study->areaAdd("area");
        study->parameters.include.thermal.minUPTime = true;
        study->parameters.include.thermal.minStablePower = true;
        study->parameters.include.reserve.spinning = true;
    }

    std::string folder = getFolder().string();
    ThermalClusterList clusterList;
    Area* area;

    Study::Ptr study = std::make_shared<Study>();
};

// ==================
// Tests section
// ==================

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

BOOST_AUTO_TEST_CASE(ThermalClusterList_loadFromFolder_basic)
{
    clusterList.loadFromFolder(*study, folder, area);
    BOOST_CHECK(clusterList.mapping["area"]->startupCost == 70000.0);
    BOOST_CHECK(clusterList.mapping["area"]->costgeneration == useCostTimeseries);
    BOOST_CHECK(clusterList.mapping["area"]->fuelEfficiency == 36.0);
    BOOST_CHECK(clusterList.mapping["area"]->variableomcost == 12.12);
}

BOOST_AUTO_TEST_CASE(EconomicInputData_loadFromFolder_basic)
{
    FuelCostFile f(8760);

    clusterList.loadFromFolder(*study, folder, area);

    EconomicInputData eco;
    BOOST_CHECK(eco.loadFromFolder(*study, folder));

    BOOST_CHECK_EQUAL(eco.fuelcost[0][1432], 1);
}

BOOST_AUTO_TEST_CASE(EconomicInputData_loadFromFolder_failing_not_enough_value)
{
    FuelCostFile f(80);

    clusterList.loadFromFolder(*study, folder, area);

    EconomicInputData eco;
    BOOST_CHECK(!eco.loadFromFolder(*study, folder));
}

BOOST_AUTO_TEST_CASE(EconomicInputData_loadFromFolder_working_with_too_much_value)
{
    CO2CostFile f(10000);

    clusterList.loadFromFolder(*study, folder, area);

    EconomicInputData eco;
    BOOST_CHECK(eco.loadFromFolder(*study, folder));
}

BOOST_AUTO_TEST_CASE(checkFuelAndCo2_basic_working)
{
    area->thermal.list.loadFromFolder(*study, folder, area);
    area->thermal.list.mapping["area"]->series = new DataSeriesCommon;
    area->thermal.list.mapping["area"]->series->timeSeries.reset(1, 8760);

    area->thermal.prepareAreaWideIndexes();

    FuelCostFile fuel(8760);
    CO2CostFile co2(8760);

    BOOST_CHECK(area->thermal.list.mapping["area"]->ecoInput.loadFromFolder(*study, folder));

    AreaList l(*study);
    l.add(area);

    BOOST_CHECK_NO_THROW(Antares::Check::checkFuelAndCo2ColumnNumber(l));

    l.areas.erase("area");
}

BOOST_AUTO_TEST_CASE(checkFuelAndCo2_failing_different_economic_input_width)
{
    area->thermal.list.loadFromFolder(*study, folder, area);
    area->thermal.list.mapping["area"]->series = new DataSeriesCommon;
    area->thermal.list.mapping["area"]->series->timeSeries.reset(1, 8760);

    area->thermal.prepareAreaWideIndexes();

    FuelCostFile fuel(8760);
    CO2CostFile co2(8760);

    BOOST_CHECK(area->thermal.list.mapping["area"]->ecoInput.loadFromFolder(*study, folder));
    area->thermal.list.mapping["area"]->ecoInput.fuelcost.width = 3;
    area->thermal.list.mapping["area"]->ecoInput.co2cost.width = 5;
    AreaList l(*study);
    l.add(area);

    BOOST_CHECK_THROW(Antares::Check::checkFuelAndCo2ColumnNumber(l),
                      Error::IncompatibleFuelOrCo2CostColumns);

    area->thermal.list.mapping["area"]->ecoInput.fuelcost.width = 1;
    area->thermal.list.mapping["area"]->ecoInput.co2cost.width = 1;
    l.areas.erase("area");
}

BOOST_AUTO_TEST_CASE(checkFuelAndCo2_working_same_economic_input_and_time_series_width)
{
    area->thermal.list.loadFromFolder(*study, folder, area);
    area->thermal.list.mapping["area"]->series = new DataSeriesCommon;
    area->thermal.list.mapping["area"]->series->timeSeries.reset(3, 8760);

    area->thermal.prepareAreaWideIndexes();

    FuelCostFile fuel(8760);
    CO2CostFile co2(8760);

    BOOST_CHECK(area->thermal.list.mapping["area"]->ecoInput.loadFromFolder(*study, folder));
    area->thermal.list.mapping["area"]->ecoInput.fuelcost.width = 3;
    area->thermal.list.mapping["area"]->ecoInput.co2cost.width = 3;
    AreaList l(*study);
    l.add(area);

    BOOST_CHECK_NO_THROW(Antares::Check::checkFuelAndCo2ColumnNumber(l));

    area->thermal.list.mapping["area"]->ecoInput.fuelcost.width = 1;
    area->thermal.list.mapping["area"]->ecoInput.co2cost.width = 1;
    l.areas.erase("area");
}

BOOST_AUTO_TEST_CASE(ThermalCluster_costGenManualCalculationOfMarketBidAndMarginalCostPerHour)
{
    clusterList.loadFromFolder(*study, folder, area);
    clusterList.mapping["area"]->costGenManualCalculationOfMarketBidAndMarginalCostPerHour();
    BOOST_CHECK_EQUAL(
      clusterList.mapping["area"]->thermalEconomicTimeSeries[0].marketBidCostPerHourTs[2637], 35);
    BOOST_CHECK_EQUAL(
      clusterList.mapping["area"]->thermalEconomicTimeSeries[0].marginalCostPerHourTs[6737], 23);
}

BOOST_AUTO_TEST_CASE(ThermalCluster_costGenTimeSeriesCalculationOfMarketBidAndMarginalCostPerHour)
{
    FuelCostFile fuel(8760);
    CO2CostFile co2(8760);

    clusterList.loadFromFolder(*study, folder, area);
    clusterList.mapping["area"]->modulation.reset(1, 8760);
    clusterList.mapping["area"]->ecoInput.loadFromFolder(*study, folder);
    fillThermalEconomicTimeSeries(clusterList.mapping["area"].get());

    clusterList.mapping["area"]->costGenTimeSeriesCalculationOfMarketBidAndMarginalCostPerHour();

    BOOST_CHECK_CLOSE(
      clusterList.mapping["area"]->thermalEconomicTimeSeries[0].marketBidCostPerHourTs[0],
      24.12,
      0.001);
    BOOST_CHECK_CLOSE(
      clusterList.mapping["area"]->thermalEconomicTimeSeries[0].marketBidCostPerHourTs[2637],
      24.12,
      0.001);
}

BOOST_AUTO_TEST_CASE(computeMarketBidCost)
{
    clusterList.loadFromFolder(*study, folder, area);
    BOOST_CHECK_CLOSE(clusterList.mapping["area"]->computeMarketBidCost(1, 2, 1), 24.12, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()
