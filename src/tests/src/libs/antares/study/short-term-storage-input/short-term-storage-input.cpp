#define BOOST_TEST_MODULE "test short term storage"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <yuni/io/file.h>
#include <filesystem>
#include <fstream>

#include <study.h>

#include "series.h"

#define SEP Yuni::IO::Separator

using namespace std;
using namespace Antares::Data;


void resizeFillVectors(ShortTermStorage::Series& series, double value, unsigned int size)
{
    series.maxInjectionModulation.resize(size, value);
    series.maxWithdrawalModulation.resize(size, value);
    series.inflows.resize(size, value);
    series.lowerRuleCurve.resize(size, value);
    series.upperRuleCurve.resize(size, value);
}

void createIndividualFileSeries(const std::string& path, double value, unsigned int size)
{
    std::ofstream outfile;
    outfile.open(path, std::ofstream::out | std::ofstream::trunc);

    for (unsigned int i = 0; i < size; i++)
    {
        outfile << value << std::endl;
    }

    outfile.close();
}

void createFileSeries(double value, unsigned int size)
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    std::string folder = tmpDir;

    createIndividualFileSeries(folder + SEP + "PMAX-injection.txt", value, size);
    createIndividualFileSeries(folder + SEP + "PMAX-withdrawal.txt", value, size);
    createIndividualFileSeries(folder + SEP + "inflows.txt", value, size);
    createIndividualFileSeries(folder + SEP + "lower-rule-curve.txt", value, size);
    createIndividualFileSeries(folder + SEP + "upper-rule-curve.txt", value, size);
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
    }

    ~Fixture() = default;
    ShortTermStorage::Series series;
    ShortTermStorage::Properties properties;

};


// ==================
// Tests section
// ==================

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

BOOST_AUTO_TEST_CASE(check_vector_sizes)
{
    resizeFillVectors(series, 0.0, 12);
    BOOST_CHECK(!series.validate());

    resizeFillVectors(series, 0.0, 8760);
    BOOST_CHECK(series.validate());
}

BOOST_AUTO_TEST_CASE(check_folder_loading)
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    std::string folder = tmpDir;

    createFileSeries(1.0, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());
}

BOOST_AUTO_TEST_CASE(check_folder_loading_too_big)
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    std::string folder = tmpDir;

    createFileSeries(1.0, 9000);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());
}

BOOST_AUTO_TEST_SUITE_END()
