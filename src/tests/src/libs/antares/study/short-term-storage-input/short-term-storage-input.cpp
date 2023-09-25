#define BOOST_TEST_MODULE "test short term storage"
#define BOOST_TEST_DYN_LINK

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>
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
    std::ofstream outfile;
    outfile.open(path, std::ofstream::out | std::ofstream::trunc);

    for (unsigned int i = 0; i < size; i++)
    {
        double value = i * 0.0001;
        outfile << value << std::endl;
    }

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
    Fixture() = default;
    ~Fixture()
    {
        std::filesystem::remove(folder + SEP + "PMAX-injection.txt");
        std::filesystem::remove(folder + SEP + "PMAX-withdrawal.txt");
        std::filesystem::remove(folder + SEP + "inflows.txt");
        std::filesystem::remove(folder + SEP + "lower-rule-curve.txt");
        std::filesystem::remove(folder + SEP + "upper-rule-curve.txt");
    }

    std::string folder = getFolder();

    ShortTermStorage::Series series;
    ShortTermStorage::Properties properties;
    ShortTermStorage::STStorageCluster cluster;
    ShortTermStorage::STStorageInput container;
};


// ==================
// Tests section
// ==================

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(check_vector_sizes, Fixture)
{
    resizeFillVectors(series, 0.0, 12);
    BOOST_CHECK(!series.validate());

    resizeFillVectors(series, 0.0, 8760);
    BOOST_CHECK(series.validate());
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading, Fixture)
{
    createFileSeries(1.0, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());
    BOOST_CHECK(series.inflows[0] == 1 && series.maxInjectionModulation[8759] == 1
        && series.upperRuleCurve[1343] == 1);
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_different_values, Fixture)
{
    createFileSeries(8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_negative_value, Fixture)
{
    createFileSeries(-247.0, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(!series.validate());
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_too_big, Fixture)
{
    createFileSeries(1.0, 9000);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_too_small, Fixture)
{
    createFileSeries(1.0, 100);

    BOOST_CHECK(!series.loadFromFolder(folder));
    BOOST_CHECK(!series.validate());
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_empty, Fixture)
{
    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(!series.validate());
}

BOOST_FIXTURE_TEST_CASE(check_series_vector_fill, Fixture)
{
    series.fillDefaultSeriesIfEmpty();
    BOOST_CHECK(series.validate());
}

BOOST_FIXTURE_TEST_CASE(check_cluster_series_vector_fill, Fixture)
{
    BOOST_CHECK(cluster.loadSeries(folder));
    BOOST_CHECK(cluster.series->validate());
}

BOOST_FIXTURE_TEST_CASE(check_cluster_series_load_vector, Fixture)
{
    createFileSeries(0.5, 8760);

    BOOST_CHECK(cluster.loadSeries(folder));
    BOOST_CHECK(cluster.series->validate());
    BOOST_CHECK(cluster.series->maxWithdrawalModulation[0] == 0.5
        && cluster.series->inflows[2756] == 0.5
        && cluster.series->lowerRuleCurve[6392] == 0.5);
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_load, Fixture)
{
    createIniFile();

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));
    BOOST_CHECK(container.storagesByIndex[0]->properties.validate());

    removeIniFile();
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_wrong_value, Fixture)
{
    createIniFileWrongValue();

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));
    BOOST_CHECK(!container.storagesByIndex[0]->properties.validate());

    removeIniFile();
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_empty_file, Fixture)
{
    createEmptyIniFile();

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));

    removeIniFile();
}

BOOST_FIXTURE_TEST_CASE(check_file_save, Fixture)
{
    createIniFile();

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));

    removeIniFile();

    BOOST_CHECK(container.saveToFolder(folder));

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));

    removeIniFile();
}

BOOST_FIXTURE_TEST_CASE(check_series_save, Fixture)
{
    resizeFillVectors(series, 0.123456789, 8760);

    BOOST_CHECK(series.saveToFolder(folder));
    resizeFillVectors(series, 0, 0);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());
}

BOOST_AUTO_TEST_SUITE_END()
