#define BOOST_TEST_MODULE "test short term storage"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <yuni/io/file.h>
#include <filesystem>

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
    createFileSeries(1.0, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());
    BOOST_CHECK(series.inflows[0] == 1 && series.maxInjectionModulation[8759] == 1
        && series.upperRuleCurve[1343] == 1);
}

BOOST_AUTO_TEST_CASE(check_series_folder_loading_different_values)
{
    createFileSeries(8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());
}

BOOST_AUTO_TEST_CASE(check_series_folder_loading_negative_value)
{
    createFileSeries(-247.0, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(!series.validate());
}

BOOST_AUTO_TEST_CASE(check_series_folder_loading_too_big)
{
    createFileSeries(1.0, 9000);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());
}

BOOST_AUTO_TEST_CASE(check_series_folder_loading_too_small)
{
    createFileSeries(1.0, 100);

    BOOST_CHECK(!series.loadFromFolder(folder));
    BOOST_CHECK(!series.validate());
}

BOOST_AUTO_TEST_CASE(check_series_folder_loading_empty)
{
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
    BOOST_CHECK(cluster.loadSeries(folder));
    BOOST_CHECK(cluster.series->validate());
}

BOOST_AUTO_TEST_CASE(check_cluster_series_load_vector)
{
    createFileSeries(0.5, 8760);

    BOOST_CHECK(cluster.loadSeries(folder));
    BOOST_CHECK(cluster.series->validate());
    BOOST_CHECK(cluster.series->maxWithdrawalModulation[0] == 0.5
        && cluster.series->inflows[2756] == 0.5
        && cluster.series->lowerRuleCurve[6392] == 0.5);
}

BOOST_AUTO_TEST_CASE(check_container_properties_load)
{
    createIniFile();

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));
    BOOST_CHECK(container.storagesByIndex[0]->properties.validate(true));

    removeIniFile();
}

BOOST_AUTO_TEST_CASE(check_container_properties_wrong_value)
{
    createIniFileWrongValue();

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));
    BOOST_CHECK(!container.storagesByIndex[0]->properties.validate(true));

    removeIniFile();
}

BOOST_AUTO_TEST_CASE(check_container_properties_empty_file)
{
    createEmptyIniFile();

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));

    removeIniFile();
}

BOOST_AUTO_TEST_CASE(check_series_lower_curve)
{
    createFileSeries(0.5, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());

    series.lowerRuleCurve[120] = 0.6;

    BOOST_CHECK(!series.validateCycleForWeek(100, 0.5, 20));
}

BOOST_AUTO_TEST_CASE(check_series_upper_curve)
{
    createFileSeries(0.5, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());
    //624 is third week first cycle (100 + 168*3 + 20)
    series.upperRuleCurve[190] = 0.2;

    BOOST_CHECK(!series.validateCycleForWeek(100, 0.5, 45));

}

BOOST_AUTO_TEST_CASE(check_series_interval_lower)
{
    createFileSeries(0.5, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());

    series.lowerRuleCurve[120] = 0.6;

    BOOST_CHECK(!series.validateCycleForWeek(100, std::nullopt, 20));
}

BOOST_AUTO_TEST_CASE(check_series_interval_upper)
{
    createFileSeries(0.5, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());

    series.upperRuleCurve[190] = 0.2;

    BOOST_CHECK(!series.validateCycleForWeek(100, std::nullopt, 30));
}

BOOST_AUTO_TEST_CASE(check_series_sum_inflows_good)
{
    createFileSeries(0.4, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());

    BOOST_CHECK(series.validateInflowsForWeek(100, 8, 1, 1));
}

BOOST_AUTO_TEST_CASE(check_series_sum_inflows_wrong_withdrawal)
{
    createFileSeries(0.4, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());

    std::fill(series.maxWithdrawalModulation.begin(), series.maxWithdrawalModulation.end(), 0.3);

    BOOST_CHECK(!series.validateInflowsForWeek(100, 27, 1, 1));
}

BOOST_AUTO_TEST_CASE(check_series_sum_inflows_wrong_injection)
{
    createFileSeries(0.4, 8760);

    BOOST_CHECK(series.loadFromFolder(folder));
    BOOST_CHECK(series.validate());

    std::fill(series.maxInjectionModulation.begin(), series.maxInjectionModulation.end(), 0.7);

    BOOST_CHECK(!series.validateInflowsForWeek(100, 27, 1, 1));
}

BOOST_AUTO_TEST_SUITE_END()
