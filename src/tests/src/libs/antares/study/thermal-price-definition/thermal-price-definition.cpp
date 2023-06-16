#define BOOST_TEST_MODULE "test thermal price definition"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <yuni/io/file.h>
#include <filesystem>
#include <fstream>

#include <study.h>

#include "cluster_list.h"

#define SEP Yuni::IO::Separator

using namespace std;
using namespace Antares::Data;

std::string getFolder()
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    return tmpDir.string();
}

void createIniFile()
{
    std::string folder = getFolder();

    std::ofstream outfile;
    outfile.open(folder + SEP + "list.ini", std::ofstream::out | std::ofstream::trunc);

    outfile << "[area]" << std::endl;
    outfile << "name = area" << std::endl;
    outfile << "group = Lignite" << std::endl;
    outfile << "unitcount = 4" << std::endl;
    outfile << "nominalcapacity = 900.000000" << std::endl;
    outfile << "min-stable-power = 400.000000" << std::endl;
    outfile << "min-up-time = 24" << std::endl;
    outfile << "min-down-time = 24" << std::endl;
    outfile << "co2 = 1.200000" << std::endl;
    outfile << "marginal-cost = 35.000000" << std::endl;
    outfile << "fixed-cost = 1700.000000" << std::endl;
    outfile << "startup-cost = 70000.000000" << std::endl;
    outfile << "market-bid-cost = 35.000000" << std::endl;
    outfile << "costgeneration = useCostTimeseries" << std::endl;
    outfile << "efficiency = 35.350000" << std::endl;
    outfile << "variableomcost = 12.120000" << std::endl;

    outfile.close();
}

void removeIniFile()
{
    std::string folder = getFolder();
    std::filesystem::remove(folder + SEP + "list.ini");
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
        area = study->areaAdd("Area");
        /* study->usedByTheSolver = true; */
        study->parameters.include.thermal.minUPTime = true;
        study->parameters.include.thermal.minStablePower = true;
        study->parameters.include.reserve.spinning = true;

    }
    ~Fixture()
    {
    }

    std::string folder = getFolder();
    ThermalClusterList clusterList;
    Area* area;

    Study::Ptr study = std::make_shared<Study>();

};


// ==================
// Tests section
// ==================

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

BOOST_AUTO_TEST_CASE(load_from_folder_basic)
{
    createIniFile();

    clusterList.loadFromFolder(*study, folder, area);
    BOOST_CHECK(clusterList.mapping["area"]->startupCost == 70000.0);
    BOOST_CHECK(clusterList.mapping["area"]->costgeneration == useCostTimeseries);
    BOOST_CHECK(clusterList.mapping["area"]->fuelEfficiency == 35.35);
    BOOST_CHECK(clusterList.mapping["area"]->variableomcost == 12.12);

    removeIniFile();
}

BOOST_AUTO_TEST_SUITE_END()
