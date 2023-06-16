#define BOOST_TEST_MODULE "test thermal price definition"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <yuni/io/file.h>
#include <filesystem>
#include <fstream>

#include "container.h"

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

    outfile.close();
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
    Fixture() = default;
    ~Fixture()
    {
    }

    std::string folder = getFolder();

};


// ==================
// Tests section
// ==================

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

BOOST_AUTO_TEST_SUITE_END()
