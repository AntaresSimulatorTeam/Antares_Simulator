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
        outfile << value << std::endl;

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

void removeFileSeries()
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    std::string folder = tmpDir;

    std::filesystem::remove(folder + SEP + "PMAX-injection.txt");
    std::filesystem::remove(folder + SEP + "PMAX-withdrawal.txt");
    std::filesystem::remove(folder + SEP + "inflows.txt");
    std::filesystem::remove(folder + SEP + "lower-rule-curve.txt");
    std::filesystem::remove(folder + SEP + "upper-rule-curve.txt");
}

void createIniFile()
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    std::string folder = tmpDir;

    std::ofstream outfile;
    outfile.open(folder + SEP + "list.ini", std::ofstream::out | std::ofstream::trunc);

    outfile << "[area]" << std::endl;
    outfile << "name = peak" << std::endl;
    outfile << "group = PSP_open" << std::endl;
    outfile << "injectionnominalcapacity = 870.000000" << std::endl;
    outfile << "withdrawalnominalcapacity = 900.000000" << std::endl;
    outfile << "reservoircapacity = 31200.000000" << std::endl;
    outfile << "efficiency = 0.75" << std::endl;
    outfile << "storagecycle = 50" << std::endl;
    outfile << "initiallevel = 0.50000" << std::endl;

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
    Fixture()
    {
        cluster.series = series;
        cluster.properties = properties;
    }

    ~Fixture() = default;
    ShortTermStorage::Series series;
    ShortTermStorage::Properties properties;
    ShortTermStorage::STStorageCluster cluster;
    ShortTermStorage::STStorageInput container;

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

BOOST_AUTO_TEST_CASE(check_series_folder_loading)
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    std::string folder = tmpDir;

    createFileSeries(1.0, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());

    removeFileSeries();
}

BOOST_AUTO_TEST_CASE(check_series_folder_loading_negative_value)
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    std::string folder = tmpDir;

    createFileSeries(-247.0, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());

    removeFileSeries();
}

BOOST_AUTO_TEST_CASE(check_series_folder_loading_too_big)
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    std::string folder = tmpDir;

    createFileSeries(1.0, 9000);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());

    removeFileSeries();
}

BOOST_AUTO_TEST_CASE(check_series_folder_loading_too_small)
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    std::string folder = tmpDir;

    createFileSeries(1.0, 100);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());

    removeFileSeries();
}

BOOST_AUTO_TEST_CASE(check_series_folder_loading_empty)
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    std::string folder = tmpDir;

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(!series.validate());
}

BOOST_AUTO_TEST_CASE(check_series_vector_fill)
{
    series.fillDefaultSeriesIfEmpty();
    BOOST_CHECK(series.validate());
}

BOOST_AUTO_TEST_CASE(check_cluster_series_vector_fill)
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    std::string folder = tmpDir;

    BOOST_CHECK(cluster.loadSeries(folder));
    BOOST_CHECK(cluster.series.validate());
}

BOOST_AUTO_TEST_CASE(check_cluster_series_load_vector)
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    std::string folder = tmpDir;

    createFileSeries(1829.21384, 8760);

    BOOST_CHECK(cluster.loadSeries(folder));
    BOOST_CHECK(cluster.series.validate());

    removeFileSeries();
}

BOOST_AUTO_TEST_SUITE_END()
