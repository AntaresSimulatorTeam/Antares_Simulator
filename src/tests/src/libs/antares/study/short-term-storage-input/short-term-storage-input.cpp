#define BOOST_TEST_MODULE "test short term storage"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <yuni/io/file.h>
#include <filesystem>
#include <random>
#include <fstream>

#include <study.h>

#include "series.h"

#define SEP Yuni::IO::Separator

using namespace std;
using namespace Antares::Data;

std::string getFolder()
{
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
    return tmpDir.string();
}

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
    std::ofstream outfile(path);

    for (unsigned int i = 0; i < size; i++)
        outfile << value << std::endl;

    outfile.close();
}

void createIndividualFileSeries(const std::string& path, unsigned int size)
{
    std::default_random_engine rnd{std::random_device{}()};
    std::uniform_real_distribution<double> dist(0, 1);

    std::ofstream outfile;
    outfile.open(path, std::ofstream::out | std::ofstream::trunc);

    for (unsigned int i = 0; i < size; i++)
        outfile << dist(rnd) << std::endl;

    outfile.close();
}

void createFileSeries(double value, unsigned int size)
{
    std::string folder = getFolder();

    createIndividualFileSeries(folder + SEP + "PMAX-injection.txt", value, size);
    createIndividualFileSeries(folder + SEP + "PMAX-withdrawal.txt", value, size);
    createIndividualFileSeries(folder + SEP + "inflows.txt", value, size);
    createIndividualFileSeries(folder + SEP + "lower-rule-curve.txt", value, size);
    createIndividualFileSeries(folder + SEP + "upper-rule-curve.txt", value, size);
}

void createFileSeries(unsigned int size)
{
    std::string folder = getFolder();

    createIndividualFileSeries(folder + SEP + "PMAX-injection.txt", size);
    createIndividualFileSeries(folder + SEP + "PMAX-withdrawal.txt", size);
    createIndividualFileSeries(folder + SEP + "inflows.txt", size);
    createIndividualFileSeries(folder + SEP + "lower-rule-curve.txt", size);
    createIndividualFileSeries(folder + SEP + "upper-rule-curve.txt", size);
}

void removeFileSeries()
{
    std::string folder = getFolder();

    std::filesystem::remove(folder + SEP + "PMAX-injection.txt");
    std::filesystem::remove(folder + SEP + "PMAX-withdrawal.txt");
    std::filesystem::remove(folder + SEP + "inflows.txt");
    std::filesystem::remove(folder + SEP + "lower-rule-curve.txt");
    std::filesystem::remove(folder + SEP + "upper-rule-curve.txt");
}

void createIniFile()
{
    std::string folder = getFolder();

    std::ofstream outfile;
    outfile.open(folder + SEP + "list.ini", std::ofstream::out | std::ofstream::trunc);

    outfile << "[area]" << std::endl;
    outfile << "name = area" << std::endl;
    outfile << "group = PSP_open" << std::endl;
    outfile << "injectionnominalcapacity = 870.000000" << std::endl;
    outfile << "withdrawalnominalcapacity = 900.000000" << std::endl;
    outfile << "reservoircapacity = 31200.000000" << std::endl;
    outfile << "efficiency = 0.75" << std::endl;
    outfile << "storagecycle = 50" << std::endl;
    outfile << "initiallevel = 0.50000" << std::endl;

    outfile.close();
}

void createIniFileWrongValue()
{
    std::string folder = getFolder();

    std::ofstream outfile;
    outfile.open(folder + SEP + "list.ini", std::ofstream::out | std::ofstream::trunc);

    outfile << "[area]" << std::endl;
    outfile << "name = area" << std::endl;
    outfile << "group = abcde" << std::endl;
    outfile << "injectionnominalcapacity = -870.000000" << std::endl;
    outfile << "withdrawalnominalcapacity = -900.000000" << std::endl;
    outfile << "reservoircapacity = -31200.000000" << std::endl;
    outfile << "efficiency = 4" << std::endl;
    outfile << "storagecycle = 50" << std::endl;
    outfile << "initiallevel = -0.50000" << std::endl;

    outfile.close();
}

void createEmptyIniFile()
{
    std::string folder = getFolder();

    std::ofstream outfile;
    outfile.open(folder + SEP + "list.ini", std::ofstream::out | std::ofstream::trunc);

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
    std::string folder = getFolder();

    createFileSeries(1.0, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());
    BOOST_CHECK(series.inflows[0] == 1 && series.maxInjectionModulation[8759] == 1
        && series.upperRuleCurve[1343] == 1);

    std::ifstream infile(folder + SEP + "inflows.txt");
    if (!infile.is_open())
        std::cout << "File not oppened";

    std::string line;
    int count = 0;
    while (std::getline(infile, line))
    {
        std::cout << count++ << " : " << line << std::endl;
    }
    infile.close();
    removeFileSeries();
}

BOOST_AUTO_TEST_CASE(check_series_folder_loading_random_value)
{
    std::string folder = getFolder();

    createFileSeries(8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());

    removeFileSeries();
}

BOOST_AUTO_TEST_CASE(check_series_folder_loading_negative_value)
{
    std::string folder = getFolder();

    createFileSeries(-247.0, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(!series.validate());

    removeFileSeries();
}

BOOST_AUTO_TEST_CASE(check_series_folder_loading_too_big)
{
    std::string folder = getFolder();

    createFileSeries(1.0, 9000);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());

    removeFileSeries();
}

BOOST_AUTO_TEST_CASE(check_series_folder_loading_too_small)
{
    std::string folder = getFolder();

    createFileSeries(1.0, 100);

    BOOST_CHECK(!series.loadFromFolder(folder));
    BOOST_CHECK(!series.validate());

    removeFileSeries();
}

BOOST_AUTO_TEST_CASE(check_series_folder_loading_empty)
{
    std::string folder = getFolder();

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
    std::string folder = getFolder();

    BOOST_CHECK(cluster.loadSeries(folder));
    BOOST_CHECK(cluster.series->validate());
}

BOOST_AUTO_TEST_CASE(check_cluster_series_load_vector)
{
    std::string folder = getFolder();

    createFileSeries(0.5, 8760);

    BOOST_CHECK(cluster.loadSeries(folder));
    BOOST_CHECK(cluster.series->validate());
    BOOST_CHECK(cluster.series->maxWithdrawalModulation[0] == 0.5
        && cluster.series->inflows[2756] == 0.5
        && cluster.series->lowerRuleCurve[6392] == 0.5);


    removeFileSeries();
}

BOOST_AUTO_TEST_CASE(check_container_properties_load)
{
    std::string folder = getFolder();

    createIniFile();

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));
    BOOST_CHECK(container.storagesByIndex[0]->properties.validate());

    removeIniFile();
}

BOOST_AUTO_TEST_CASE(check_container_properties_wrong_value)
{
    std::string folder = getFolder();

    createIniFileWrongValue();

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));
    BOOST_CHECK(!container.storagesByIndex[0]->properties.validate());

    removeIniFile();
}

BOOST_AUTO_TEST_CASE(check_container_properties_empty_file)
{
    std::string folder = getFolder();

    createEmptyIniFile();

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));

    removeIniFile();
}

BOOST_AUTO_TEST_SUITE_END()
