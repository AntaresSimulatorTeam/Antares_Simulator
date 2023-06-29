#define BOOST_TEST_MODULE "test thermal price definition"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <yuni/io/file.h>
#include <filesystem>
#include <fstream>

#include <study.h>
#include <antares/exception/LoadingError.hpp>

#include "../checks/checkApplication.h"
#include "cluster_list.h"
#include "container.h"

#define SEP Yuni::IO::Separator

using namespace std;
using namespace Antares::Data;

std::filesystem::path getFolder()
{
    return std::filesystem::temp_directory_path();
}

void createIniFile()
{
    std::filesystem::path folder = getFolder();

    std::ofstream outfile;
    outfile.open(folder / "list.ini", std::ofstream::out | std::ofstream::trunc);

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

    outfile.close();
}

void createFuelCostFile(int size)
{
    std::filesystem::path folder = getFolder();

    std::ofstream outfile;
    outfile.open(folder / "fuelCost.txt", std::ofstream::out | std::ofstream::trunc);

    for (int i = 0; i < size; i++)
    {
        outfile << 1 << std::endl;
    }
}

void createCo2CostFile(int size)
{
    std::filesystem::path folder = getFolder();

    std::ofstream outfile;
    outfile.open(folder / "CO2Cost.txt", std::ofstream::out | std::ofstream::trunc);

    for (int i = 0; i < size; i++)
    {
        outfile << 1 << std::endl;
    }
}

void removeIniFile()
{
    std::filesystem::path folder = getFolder();
    std::filesystem::remove(folder / "list.ini");
}

void removeCostFiles()
{
    std::filesystem::path folder = getFolder();
    std::filesystem::remove(folder / "fuelCost.txt");
    std::filesystem::remove(folder / "CO2Cost.txt");
}

void fillThermalEconomicTimeSeries(ThermalCluster *c)
{
    c->thermalEconomicTimeSeries[0].productionCostTs.fill(1);
    c->thermalEconomicTimeSeries[0].marketBidCostPerHourTs.fill(1);
    c->thermalEconomicTimeSeries[0].marginalCostPerHourTs.fill(1);
}


// =================
// The fixture
// =================
struct Fixture
{
    Fixture(const Fixture & f) = delete;
    Fixture(const Fixture && f) = delete;
    Fixture & operator= (const Fixture & f) = delete;
    Fixture& operator= (const Fixture && f) = delete;
    Fixture()
    {
        area = study->areaAdd("area");
        study->parameters.include.thermal.minUPTime = true;
        study->parameters.include.thermal.minStablePower = true;
        study->parameters.include.reserve.spinning = true;

    }
    ~Fixture()
    {
        removeIniFile();
        removeCostFiles();
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
    createIniFile();

    clusterList.loadFromFolder(*study, folder, area);
    BOOST_CHECK(clusterList.mapping["area"]->startupCost == 70000.0);
    BOOST_CHECK(clusterList.mapping["area"]->costgeneration == useCostTimeseries);
    BOOST_CHECK(clusterList.mapping["area"]->fuelEfficiency == 36.0);
    BOOST_CHECK(clusterList.mapping["area"]->variableomcost == 12.12);

}

BOOST_AUTO_TEST_CASE(EconomicInputData_loadFromFolder_basic)
{
    createIniFile();
    createFuelCostFile(8760);

    clusterList.loadFromFolder(*study, folder, area);

    EconomicInputData eco;
    BOOST_CHECK(eco.loadFromFolder(*study, folder));

    BOOST_CHECK(eco.fuelcost[0][1432] == 1);
}

BOOST_AUTO_TEST_CASE(EconomicInputData_loadFromFolder_failing_not_enough_value)
{
    createIniFile();
    createFuelCostFile(80);

    clusterList.loadFromFolder(*study, folder, area);

    EconomicInputData eco;
    BOOST_CHECK(!eco.loadFromFolder(*study, folder));
}

BOOST_AUTO_TEST_CASE(EconomicInputData_loadFromFolder_working_with_too_much_value)
{
    createIniFile();
    createFuelCostFile(10000);

    clusterList.loadFromFolder(*study, folder, area);

    EconomicInputData eco;
    BOOST_CHECK(eco.loadFromFolder(*study, folder));
}

BOOST_AUTO_TEST_CASE(checkFuelAndCo2_basic_working)
{
    createIniFile();
    area->thermal.list.loadFromFolder(*study, folder, area);
    area->thermal.list.mapping["area"]->series = new DataSeriesCommon;
    area->thermal.list.mapping["area"]->series->timeSeries.reset(1, 8760);

    area->thermal.prepareAreaWideIndexes();

    createFuelCostFile(8760);
    createCo2CostFile(8760);

    BOOST_CHECK(area->thermal.list.mapping["area"]->ecoInput.loadFromFolder(*study, folder));

    AreaList l(*study);
    l.add(area);

    BOOST_CHECK_NO_THROW(Antares::Check::checkFuelAndCo2ColumnNumber(l));

    l.areas.erase("area");
}

BOOST_AUTO_TEST_CASE(checkFuelAndCo2_failing_different_economic_input_width)
{
    createIniFile();
    area->thermal.list.loadFromFolder(*study, folder, area);
    area->thermal.list.mapping["area"]->series = new DataSeriesCommon;
    area->thermal.list.mapping["area"]->series->timeSeries.reset(1, 8760);

    area->thermal.prepareAreaWideIndexes();

    createFuelCostFile(8760);
    createCo2CostFile(8760);

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
    createIniFile();
    area->thermal.list.loadFromFolder(*study, folder, area);
    area->thermal.list.mapping["area"]->series = new DataSeriesCommon;
    area->thermal.list.mapping["area"]->series->timeSeries.reset(3, 8760);

    area->thermal.prepareAreaWideIndexes();

    createFuelCostFile(8760);
    createCo2CostFile(8760);

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
    createIniFile();
    clusterList.loadFromFolder(*study, folder, area);
    clusterList.mapping["area"]->costGenManualCalculationOfMarketBidAndMarginalCostPerHour();
    BOOST_CHECK(clusterList.mapping["area"]->thermalEconomicTimeSeries[0]
        .marketBidCostPerHourTs[2637] == 35);
    BOOST_CHECK(clusterList.mapping["area"]->thermalEconomicTimeSeries[0]
        .marginalCostPerHourTs[6737] == 23);
}

BOOST_AUTO_TEST_CASE(ThermalCluster_costGenTimeSeriesCalculationOfMarketBidAndMarginalCostPerHour)
{
    createIniFile();
    createFuelCostFile(8760);
    createCo2CostFile(8760);

    clusterList.loadFromFolder(*study, folder, area);
    clusterList.mapping["area"]->modulation.reset(1, 8760);
    clusterList.mapping["area"]->ecoInput.loadFromFolder(*study, folder);
    fillThermalEconomicTimeSeries(clusterList.mapping["area"].get());

    clusterList.mapping["area"]->costGenTimeSeriesCalculationOfMarketBidAndMarginalCostPerHour();

    BOOST_CHECK(clusterList.mapping["area"]->thermalEconomicTimeSeries[0]
        .marketBidCostPerHourTs[0] - 24.12 < 0.000001);
    BOOST_CHECK(clusterList.mapping["area"]->thermalEconomicTimeSeries[0]
        .marketBidCostPerHourTs[2637] - 24.12 < 0.000001);
}


BOOST_AUTO_TEST_SUITE_END()
