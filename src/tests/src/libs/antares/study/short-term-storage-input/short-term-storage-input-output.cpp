/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#define BOOST_TEST_MODULE "test short term storage"

#define WIN32_LEAN_AND_MEAN

#include <filesystem>
#include <fstream>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <yuni/io/file.h>

#include "antares/antares/constants.h"
#include "antares/study/parts/short-term-storage/additionalConstraints.h"
#include "antares/study/parts/short-term-storage/container.h"

using namespace std;
using namespace Antares::Data;

namespace fs = std::filesystem;

namespace
{

struct PenaltyCostOnVariation
{
    bool injection = false;
    bool withdrawal = false;
};

fs::path getFolder()
{
    return fs::temp_directory_path();
}

void resizeFillVectors(ShortTermStorage::Series& series, double value, unsigned int size)
{
    series.maxInjectionModulation.resize(size, value);
    series.maxWithdrawalModulation.resize(size, value);
    series.inflows.reset(1, size);
    series.inflows.fill(value);
    series.lowerRuleCurve.resize(size, value);
    series.upperRuleCurve.resize(size, value);

    series.costInjection.resize(size, value);
    series.costWithdrawal.resize(size, value);
    series.costLevel.resize(size, value);

    series.costVariationInjection.resize(size, value);
    series.costVariationWithdrawal.resize(size, value);
}

void createIndividualFileSeries(const fs::path& path, double value, unsigned int size)
{
    std::ofstream outfile(path);

    for (unsigned int i = 0; i < size; i++)
    {
        outfile << value << std::endl;
    }

    outfile.close();
}

void createIndividualFileSeries(const fs::path& path, unsigned int size)
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
    fs::path folder = getFolder();

    createIndividualFileSeries(folder / "PMAX-injection.txt", value, size);
    createIndividualFileSeries(folder / "PMAX-withdrawal.txt", value, size);
    createIndividualFileSeries(folder / "inflows.txt", value, size);
    createIndividualFileSeries(folder / "lower-rule-curve.txt", value, size);
    createIndividualFileSeries(folder / "upper-rule-curve.txt", value, size);

    createIndividualFileSeries(folder / "cost-injection.txt", value, size);
    createIndividualFileSeries(folder / "cost-withdrawal.txt", value, size);
    createIndividualFileSeries(folder / "cost-level.txt", value, size);
    createIndividualFileSeries(folder / "cost-variation-injection.txt", value, size);
    createIndividualFileSeries(folder / "cost-variation-withdrawal.txt", value, size);
}

void createFileSeries(unsigned int size)
{
    fs::path folder = getFolder();

    createIndividualFileSeries(folder / "PMAX-injection.txt", size);
    createIndividualFileSeries(folder / "PMAX-withdrawal.txt", size);
    createIndividualFileSeries(folder / "inflows.txt", size);
    createIndividualFileSeries(folder / "lower-rule-curve.txt", size);
    createIndividualFileSeries(folder / "upper-rule-curve.txt", size);

    createIndividualFileSeries(folder / "cost-injection.txt", size);
    createIndividualFileSeries(folder / "cost-withdrawal.txt", size);
    createIndividualFileSeries(folder / "cost-level.txt", size);

    createIndividualFileSeries(folder / "cost-variation-injection.txt", size);
    createIndividualFileSeries(folder / "cost-variation-withdrawal.txt", size);
}

void createIniFile(bool enabled)
{
    fs::path folder = getFolder();

    std::ofstream outfile;
    outfile.open(folder / "list.ini", std::ofstream::out | std::ofstream::trunc);

    outfile << "[area]" << std::endl;
    outfile << "name = area" << std::endl;
    outfile << "group = PSP_open" << std::endl;
    outfile << "injectionnominalcapacity = 870.000000" << std::endl;
    outfile << "withdrawalnominalcapacity = 900.000000" << std::endl;
    outfile << "reservoircapacity = 31200.000000" << std::endl;
    outfile << "efficiency = 0.75" << std::endl;
    outfile << "efficiencywithdrawal = 0.9" << std::endl;
    outfile << "initiallevel = 0.50000" << std::endl;
    outfile << "enabled = " << (enabled ? "true" : "false") << std::endl;
    outfile.close();
}

void createIniFile(const PenaltyCostOnVariation& penaltyCostOnVariation)
{
    fs::path folder = getFolder();

    std::ofstream outfile;
    outfile.open(folder / "list.ini", std::ofstream::out | std::ofstream::trunc);

    outfile << "[area]" << std::endl;
    outfile << "name = area" << std::endl;
    outfile << "group = PSP_open" << std::endl;
    outfile << "penalize-variation-injection = " << std::boolalpha
            << penaltyCostOnVariation.injection << std::endl;
    outfile << "penalize-variation-withdrawal = " << std::boolalpha
            << penaltyCostOnVariation.withdrawal << std::endl;
    outfile.close();
}

void createIniFileWrongValue()
{
    fs::path folder = getFolder();

    std::ofstream outfile;
    outfile.open(folder / "list.ini", std::ofstream::out | std::ofstream::trunc);

    outfile << "[area]" << std::endl;
    outfile << "name = area" << std::endl;
    outfile << "group = abcde" << std::endl;
    outfile << "injectionnominalcapacity = -870.000000" << std::endl;
    outfile << "withdrawalnominalcapacity = -900.000000" << std::endl;
    outfile << "reservoircapacity = -31200.000000" << std::endl;
    outfile << "efficiency = 4" << std::endl;
    outfile << "efficiencywithdrawal = -2" << std::endl;
    outfile << "initiallevel = -0.50000" << std::endl;

    outfile.close();
}

void createEmptyIniFile()
{
    fs::path folder = getFolder();

    std::ofstream outfile;
    outfile.open(folder / "list.ini", std::ofstream::out | std::ofstream::trunc);

    outfile.close();
}

void removeIniFile()
{
    fs::remove(getFolder() / "list.ini");
}
} // namespace

// =================
// The fixture
// =================
struct Fixture
{
    Fixture(const Fixture& f) = delete;
    Fixture(const Fixture&& f) = delete;
    Fixture& operator=(const Fixture& f) = delete;
    Fixture& operator=(const Fixture&& f) = delete;
    Fixture() = default;

    ~Fixture()
    {
        fs::remove(folder / "PMAX-injection.txt");
        fs::remove(folder / "PMAX-withdrawal.txt");
        fs::remove(folder / "inflows.txt");
        fs::remove(folder / "lower-rule-curve.txt");
        fs::remove(folder / "upper-rule-curve.txt");

        fs::remove(folder / "cost-injection.txt");
        fs::remove(folder / "cost-withdrawal.txt");
        fs::remove(folder / "cost-level.txt");

        fs::remove(folder / "cost-variation-injection.txt");
        fs::remove(folder / "cost-variation-withdrawal.txt");
    }

    bool loadFromFolder(StudyVersion version)
    {
        return series.loadFromFolder(folder, version);
    }

    fs::path folder = getFolder();
    ShortTermStorage::Series series;
    ShortTermStorage::Properties properties;
    ShortTermStorage::STStorageCluster cluster;
    ShortTermStorage::STStorageInput container;

    PenaltyCostOnVariation penaltyCostOnVariation;
};

// ==================
// Tests section
// ==================

BOOST_AUTO_TEST_SUITE(s)

// We only check the 1st element
void checkSizeFirst(const std::vector<double>& in, double v)
{
    BOOST_CHECK_EQUAL(in.size(), HOURS_PER_YEAR);
    BOOST_CHECK_EQUAL(in[0], v);
}

void checkSizeFirst(const TimeSeries& series, double value)
{
    BOOST_CHECK_EQUAL(series.timeSeries.height, HOURS_PER_YEAR);
    BOOST_CHECK_EQUAL(series.getCoefficient(0, 0), value);
}

BOOST_FIXTURE_TEST_CASE(check_empty, Fixture)
{
    createFileSeries(0); // Empty files
    loadFromFolder(StudyVersion(9, 2));
    series.fillDefaultSeriesIfEmpty();

    // version<9.2
    checkSizeFirst(series.maxInjectionModulation, 1.0);
    checkSizeFirst(series.maxWithdrawalModulation, 1.0);
    checkSizeFirst(series.inflows, 0.0);
    checkSizeFirst(series.lowerRuleCurve, 0.0);
    checkSizeFirst(series.upperRuleCurve, 1.0);

    // version>=9.2
    checkSizeFirst(series.costInjection, 0.0);
    checkSizeFirst(series.costWithdrawal, 0.0);
    checkSizeFirst(series.costLevel, -1.e-6);
    checkSizeFirst(series.costVariationInjection, 0.0);
    checkSizeFirst(series.costVariationWithdrawal, 0.0);
}

BOOST_FIXTURE_TEST_CASE(check_vector_sizes, Fixture)
{
    resizeFillVectors(series, 0.0, 12);
    BOOST_CHECK(!series.validate("", StudyVersion::latest()));

    resizeFillVectors(series, 0.0, 8760);
    BOOST_CHECK(series.validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading, Fixture)
{
    createFileSeries(1.0, 8760);

    BOOST_CHECK(loadFromFolder(StudyVersion::latest()));
    BOOST_CHECK(series.validate("", StudyVersion::latest()));
    BOOST_CHECK(series.inflows.getCoefficient(0, 0) == 1 && series.maxInjectionModulation[8759] == 1
                && series.upperRuleCurve[1343] == 1 && series.costVariationInjection[0] == 1
                && series.costVariationWithdrawal[0] == 1);
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_880, Fixture)
{
    createFileSeries(1.0, 8760);

    BOOST_CHECK(loadFromFolder(StudyVersion(8, 8)));
    BOOST_CHECK(series.validate("", StudyVersion(8, 8)));
    BOOST_CHECK(series.inflows.getCoefficient(0, 0) == 1 && series.maxInjectionModulation[8759] == 1
                && series.upperRuleCurve[1343]);

    // New elements should NOT be loaded if the study version is < 9.2
    BOOST_CHECK(series.costVariationInjection.empty());
    BOOST_CHECK(series.costVariationWithdrawal.empty());
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_different_values_880, Fixture)
{
    createFileSeries(8760);

    BOOST_CHECK(loadFromFolder(StudyVersion(8, 8)));
    BOOST_CHECK(series.validate("", StudyVersion(8, 8)));
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_different_values, Fixture)
{
    createFileSeries(8760);

    BOOST_CHECK(loadFromFolder(StudyVersion::latest()));
    BOOST_CHECK(series.validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_negative_value, Fixture)
{
    createFileSeries(-247.0, 8760);

    BOOST_CHECK(loadFromFolder(StudyVersion::latest()));
    BOOST_CHECK(!series.validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_too_big, Fixture)
{
    createFileSeries(1.0, 9000);

    BOOST_CHECK(loadFromFolder(StudyVersion::latest()));
    BOOST_CHECK(series.validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_too_big_880, Fixture)
{
    createFileSeries(1.0, 9000);

    BOOST_CHECK(loadFromFolder(StudyVersion(8, 8)));
    BOOST_CHECK(series.validate("", StudyVersion(8, 8)));
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_too_small, Fixture)
{
    createFileSeries(1.0, 100);

    BOOST_CHECK(!loadFromFolder(StudyVersion::latest()));
    BOOST_CHECK(!series.validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_empty, Fixture)
{
    BOOST_CHECK(loadFromFolder(StudyVersion::latest()));
    BOOST_CHECK(!series.validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_series_vector_fill, Fixture)
{
    series.fillDefaultSeriesIfEmpty();
    BOOST_CHECK(series.validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_cluster_series_vector_fill, Fixture)
{
    BOOST_CHECK(cluster.loadSeries(folder, StudyVersion::latest()));
    BOOST_CHECK(cluster.series->validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_cluster_series_load_vector, Fixture)
{
    createFileSeries(0.5, 8760);

    BOOST_CHECK(cluster.loadSeries(folder, StudyVersion::latest()));
    BOOST_CHECK(cluster.series->validate("", StudyVersion::latest()));
    BOOST_CHECK(cluster.series->maxWithdrawalModulation[0] == 0.5
                && cluster.series->inflows.getCoefficient(0, 2756) == 0.5
                && cluster.series->lowerRuleCurve[6392] == 0.5
                && cluster.series->costVariationInjection[15] == 0.5
                && cluster.series->costVariationWithdrawal[756] == 0.5);
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_enabled_load, Fixture)
{
    createIniFile(true);

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));

    auto& properties = container.storagesByIndex[0].properties;

    BOOST_CHECK(properties.enabled);
    BOOST_CHECK_EQUAL(container.count(), 1);
    BOOST_CHECK(properties.validate());
    BOOST_CHECK(!properties.penalizeVariationInjection);
    BOOST_CHECK(!properties.penalizeVariationWithdrawal);

    removeIniFile();
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_enabled_load_with_cost_variation_injection,
                        Fixture)
{
    penaltyCostOnVariation = {.injection = true, .withdrawal = false};
    createIniFile(penaltyCostOnVariation);

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));

    auto& properties = container.storagesByIndex[0].properties;

    BOOST_CHECK(properties.penalizeVariationInjection);

    removeIniFile();
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_enabled_load_with_cost_variation_withdrawal,
                        Fixture)
{
    penaltyCostOnVariation = {.injection = false, .withdrawal = true};
    createIniFile(penaltyCostOnVariation);

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));

    auto& properties = container.storagesByIndex[0].properties;

    BOOST_CHECK(properties.penalizeVariationWithdrawal);

    removeIniFile();
}

BOOST_FIXTURE_TEST_CASE(
  check_container_properties_enabled_load_with_cost_variation_injection_and_withdrawal,
  Fixture)
{
    penaltyCostOnVariation = {.injection = true, .withdrawal = true};
    createIniFile(penaltyCostOnVariation);

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));

    auto& properties = container.storagesByIndex[0].properties;

    BOOST_CHECK(properties.penalizeVariationInjection);
    BOOST_CHECK(properties.penalizeVariationWithdrawal);

    removeIniFile();
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_disabled_load, Fixture)
{
    createIniFile(false);

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));

    auto& properties = container.storagesByIndex[0].properties;

    BOOST_CHECK(!properties.enabled);
    BOOST_CHECK_EQUAL(container.count(), 0);
    BOOST_CHECK(properties.validate());

    removeIniFile();
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_wrong_value, Fixture)
{
    createIniFileWrongValue();

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));
    BOOST_CHECK(!container.storagesByIndex[0].properties.validate());

    removeIniFile();
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_empty_file, Fixture)
{
    createEmptyIniFile();

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));

    removeIniFile();
}

#ifdef BUILD_UI
BOOST_FIXTURE_TEST_CASE(check_file_save, Fixture)
{
    createIniFile(true);

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));

    removeIniFile();

    BOOST_CHECK(container.saveToFolder(folder.string()));

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(folder));

    removeIniFile();
}

BOOST_FIXTURE_TEST_CASE(check_series_save, Fixture)
{
    resizeFillVectors(series, 0.123456789, 8760);

    BOOST_CHECK(series.saveToFolder(folder.string()));
    resizeFillVectors(series, 0, 0);

    BOOST_CHECK(loadFromFolder(StudyVersion::latest()));
    BOOST_CHECK(series.validate("", StudyVersion::latest()));
}
#endif

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(AdditionalConstraintsTests)

BOOST_AUTO_TEST_CASE(Validate_ClusterIdEmpty)
{
    ShortTermStorage::AdditionalConstraints
      constraints("name", "name", "", "injection", "less", true, {});

    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, false);
    BOOST_CHECK_EQUAL(error_msg, "Cluster ID is empty.");
}

BOOST_AUTO_TEST_CASE(Validate_InvalidVariable)
{
    ShortTermStorage::AdditionalConstraints
      constraints("name", "name", "ClusterA", "invalid", "less", true, {});
    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, false);
    BOOST_CHECK_EQUAL(error_msg,
                      "Invalid variable type. Must be 'injection', 'withdrawal', or 'netting'.");
}

BOOST_AUTO_TEST_CASE(Validate_InvalidOperatorType)
{
    ShortTermStorage::AdditionalConstraints
      constraints("name", "name", "ClusterA", "injection", "invalid", true, {});

    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, false);
    BOOST_CHECK_EQUAL(error_msg, "Invalid operator type. Must be 'less', 'equal', or 'greater'.");
}

BOOST_AUTO_TEST_CASE(Validate_InvalidHours_Empty)
{
    ShortTermStorage::SingleAdditionalConstraint constraint;
    // Case : Empty hours
    constraint.hours = {}; // Invalid: empty

    ShortTermStorage::AdditionalConstraints
      constraints("name", "name", "ClusterA", "injection", "less", true, {constraint});

    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, false);
    BOOST_CHECK_EQUAL(error_msg, "Hours sets contains invalid values. Must be between 1 and 168.");
}

BOOST_AUTO_TEST_CASE(Validate_InvalidHours_Out_of_range)
{
    ShortTermStorage::AdditionalConstraints
      constraints("name", "name", "ClusterA", "injection", "less", true, {});

    // Case: Out of range
    ShortTermStorage::SingleAdditionalConstraint constraint;
    constraint.hours = {120, 169}; // Invalid: out of range
    constraints.constraints.push_back(constraint);

    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, false);
    BOOST_CHECK_EQUAL(error_msg, "Hours sets contains invalid values. Must be between 1 and 168.");
}

BOOST_AUTO_TEST_CASE(Validate_InvalidHours_Below_minimum)
{
    ShortTermStorage::AdditionalConstraints
      constraints("name", "name", "ClusterA", "injection", "less", true, {});

    // Case : Below minimum
    ShortTermStorage::SingleAdditionalConstraint constraint;
    constraint.hours = {0, 1}; // Invalid: below minimum
    constraints.constraints.push_back(constraint);

    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, false);
    BOOST_CHECK_EQUAL(error_msg, "Hours sets contains invalid values. Must be between 1 and 168.");
}

BOOST_AUTO_TEST_CASE(Validate_ValidConstraints)
{
    ShortTermStorage::AdditionalConstraints
      constraints("name", "name", "ClusterA", "injection", "less", true, {});

    ShortTermStorage::SingleAdditionalConstraint constraint1;
    constraint1.hours = {1, 2, 3}; // Valid hours

    ShortTermStorage::SingleAdditionalConstraint constraint2;
    constraint2.hours = {100, 150, 168}; // Valid hours

    constraints.constraints = {constraint1, constraint2};

    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, true);
    BOOST_CHECK(error_msg.empty());
}

BOOST_AUTO_TEST_CASE(loadAdditionalConstraints_ValidFile)
{
    std::filesystem::path testPath = getFolder() / "test_data";
    std::filesystem::create_directories(testPath / "cluster1");

    std::ofstream iniFile(testPath / "cluster1" / "additional-constraints.ini");
    iniFile << "[constraint1]\n";
    iniFile << "variable=injection\n";
    iniFile << "operator=less\n";
    iniFile << "hours=[1,2,3]\n";
    iniFile.close();

    ShortTermStorage::STStorageInput storageInput;
    ShortTermStorage::STStorageCluster cluster;
    cluster.id = "cluster1";
    storageInput.storagesByIndex.push_back(cluster);

    bool result = storageInput.loadAdditionalConstraints(testPath);

    BOOST_CHECK_EQUAL(result, true);
    BOOST_CHECK_EQUAL(storageInput.storagesByIndex[0].additionalConstraints.size(), 1);
    BOOST_CHECK_EQUAL(storageInput.storagesByIndex[0].additionalConstraints[0]->name,
                      "constraint1");

    std::filesystem::remove_all(testPath);
}

BOOST_AUTO_TEST_CASE(loadAdditionalConstraints_InvalidHours)
{
    std::filesystem::path testPath = getFolder() / "test_data";
    std::filesystem::create_directories(testPath / "ClusterA");

    std::ofstream iniFile(testPath / "ClusterA" / "additional-constraints.ini");
    iniFile << "[constraint1]\n";
    iniFile << "variable=injection\n";
    iniFile << "operator=less\n";
    iniFile << "hours=[0,1]\n"; // Invalid hours
    iniFile.close();

    ShortTermStorage::STStorageInput storageInput;
    ShortTermStorage::STStorageCluster cluster;
    cluster.id = "ClusterA";
    storageInput.storagesByIndex.push_back(cluster);

    bool result = storageInput.loadAdditionalConstraints(testPath);
    BOOST_CHECK_EQUAL(result, false);

    std::filesystem::remove_all(testPath);
}

BOOST_AUTO_TEST_CASE(loadAdditionalConstraints_MissingFile)
{
    ShortTermStorage::STStorageInput storageInput;
    bool result = storageInput.loadAdditionalConstraints("nonexistent_path");
    BOOST_CHECK_EQUAL(result, true);
}

BOOST_AUTO_TEST_CASE(loadAdditionalConstraints_InvalidConstraint)
{
    std::filesystem::path testPath = getFolder() / "test_data";
    std::filesystem::create_directories(testPath / "cluster1");

    std::ofstream iniFile(testPath / "cluster1" / "additional-constraints.ini");
    iniFile << "[constraint1]\n";
    iniFile << "variable=invalid\n"; // Invalid variable
    iniFile << "operator=less\n";
    iniFile << "hours=[1,2,3]\n";
    iniFile.close();

    ShortTermStorage::STStorageInput storageInput;
    ShortTermStorage::STStorageCluster cluster;
    cluster.id = "cluster1";
    storageInput.storagesByIndex.push_back(cluster);

    bool result = storageInput.loadAdditionalConstraints(testPath);
    BOOST_CHECK_EQUAL(result, false);

    std::filesystem::remove_all(testPath);
}

BOOST_AUTO_TEST_CASE(loadAdditionalConstraints_ValidRhs)
{
    std::filesystem::path testPath = getFolder() / "test_data";
    std::filesystem::create_directories(testPath / "cluster1");

    std::ofstream iniFile(testPath / "cluster1" / "additional-constraints.ini");
    iniFile << "[constraint1]\n";
    iniFile << "variable=injection\n";
    iniFile << "operator=less\n";
    iniFile << "hours=[1,2,3]\n";
    iniFile.close();

    std::ofstream rhsFile(testPath / "cluster1" / "rhs_constraint1.txt");
    for (unsigned int i = 0; i < HOURS_PER_YEAR; ++i)
    {
        rhsFile << i * 1.0 << "\n";
    }
    rhsFile.close();

    ShortTermStorage::STStorageInput storageInput;
    ShortTermStorage::STStorageCluster cluster;
    cluster.id = "cluster1";
    storageInput.storagesByIndex.push_back(cluster);

    bool result = storageInput.loadAdditionalConstraints(testPath);

    BOOST_CHECK_EQUAL(result, true);
    const auto& constraint1Rhs = storageInput.storagesByIndex[0].additionalConstraints[0]->rhs();
    BOOST_CHECK_EQUAL(constraint1Rhs.timeSeries.height, HOURS_PER_YEAR);
    BOOST_CHECK_EQUAL(constraint1Rhs.getCoefficient(0, 0), 0.0);
    BOOST_CHECK_EQUAL(constraint1Rhs.getCoefficient(0, HOURS_PER_YEAR - 1), HOURS_PER_YEAR - 1);

    std::filesystem::remove_all(testPath);
}

BOOST_AUTO_TEST_CASE(Load2ConstraintsFromIniFile)
{
    std::filesystem::path testPath = getFolder() / "test_data";
    std::filesystem::create_directories(testPath / "cluster1");

    std::ofstream iniFile(testPath / "cluster1" / "additional-constraints.ini");
    iniFile << R"([constraint1]
                  variable=injection
                  operator=less
                  hours=[1,2,3]
                  [constraint2]
                  variable=withdrawal
                  operator=greater
                  hours=[5,33])";
    iniFile.close();

    std::ofstream rhsFile(testPath / "cluster1" / "rhs_constraint1.txt");
    for (unsigned int i = 0; i < HOURS_PER_YEAR; ++i)
    {
        rhsFile << i * 1.0 << "\n";
    }
    rhsFile.close();

    ShortTermStorage::STStorageInput storageInput;
    ShortTermStorage::STStorageCluster cluster;
    cluster.id = "cluster1";
    storageInput.storagesByIndex.push_back(cluster);

    bool result = storageInput.loadAdditionalConstraints(testPath);

    BOOST_CHECK_EQUAL(result, true);
    BOOST_CHECK_EQUAL(storageInput.storagesByIndex[0].additionalConstraints.size(), 2);

    //------- constraint1 ----------
    const auto& constraint1 = *storageInput.storagesByIndex[0].additionalConstraints[0];
    BOOST_CHECK_EQUAL(constraint1.name, "constraint1");
    BOOST_CHECK_EQUAL(constraint1.operatorType, "less");
    BOOST_CHECK_EQUAL(constraint1.variable, "injection");
    BOOST_CHECK_EQUAL(constraint1.cluster_id, cluster.id);

    const auto& constraint1Rhs = constraint1.rhs();
    BOOST_CHECK_EQUAL(constraint1Rhs.timeSeries.height, HOURS_PER_YEAR);
    BOOST_CHECK_EQUAL(constraint1Rhs.getCoefficient(0, 0), 0.0);
    BOOST_CHECK_EQUAL(constraint1Rhs.getCoefficient(0, HOURS_PER_YEAR - 1), HOURS_PER_YEAR - 1);

    //------- constraint2 ----------

    const auto& constraint2 = *storageInput.storagesByIndex[0].additionalConstraints[1];
    BOOST_CHECK_EQUAL(constraint2.name, "constraint2");
    BOOST_CHECK_EQUAL(constraint2.operatorType, "greater");
    BOOST_CHECK_EQUAL(constraint2.variable, "withdrawal");
    BOOST_CHECK_EQUAL(constraint2.cluster_id, cluster.id);

    const auto& constraint2Rhs = constraint2.rhs();
    BOOST_CHECK_EQUAL(constraint2Rhs.timeSeries.height, HOURS_PER_YEAR);
    BOOST_CHECK_EQUAL(constraint2Rhs.getCoefficient(0, 0), 0.0);
    BOOST_CHECK_EQUAL(constraint2Rhs.getCoefficient(0, HOURS_PER_YEAR - 1), 0);

    std::filesystem::remove_all(testPath);
}

BOOST_AUTO_TEST_CASE(loadAdditionalConstraints_MissingRhsFile)
{
    std::filesystem::path testPath = getFolder() / "test_data";
    std::filesystem::create_directories(testPath / "cluster1");

    std::ofstream iniFile(testPath / "cluster1" / "additional-constraints.ini");
    iniFile << "[constraint1]\n";
    iniFile << "variable=injection\n";
    iniFile << "operator=less\n";
    iniFile << "hours=[1,2,3]\n";
    iniFile.close();

    ShortTermStorage::STStorageInput storageInput;
    ShortTermStorage::STStorageCluster cluster;
    cluster.id = "cluster1";
    storageInput.storagesByIndex.push_back(cluster);

    bool result = storageInput.loadAdditionalConstraints(testPath);

    BOOST_CHECK_EQUAL(result, true);
    const auto& constraintRhs = storageInput.storagesByIndex[0].additionalConstraints[0]->rhs();
    BOOST_CHECK_EQUAL(constraintRhs.timeSeries.height, HOURS_PER_YEAR);
    BOOST_CHECK_EQUAL(constraintRhs.getCoefficient(0, 0), 0.0);

    std::filesystem::remove_all(testPath);
}

BOOST_AUTO_TEST_CASE(loadAdditionalConstraints_MalformedRhsFile)
{
    std::filesystem::path testPath = getFolder() / "test_data";
    std::filesystem::create_directories(testPath / "cluster1");

    std::ofstream iniFile(testPath / "cluster1" / "additional-constraints.ini");
    iniFile << "[constraint1]\n";
    iniFile << "variable=injection\n";
    iniFile << "operator=less\n";
    iniFile << "hours=[1,2,3]\n";
    iniFile.close();

    std::ofstream rhsFile(testPath / "cluster1" / "rhs_constraint1.txt");
    rhsFile << "1.0\n2.0\ninvalid\n4.0\n"; // Malformed line
    rhsFile.close();

    ShortTermStorage::STStorageInput storageInput;
    ShortTermStorage::STStorageCluster cluster;
    cluster.id = "cluster1";
    storageInput.storagesByIndex.push_back(cluster);

    bool result = storageInput.loadAdditionalConstraints(testPath);
    BOOST_CHECK_EQUAL(result, false);
    std::filesystem::remove_all(testPath);
}

BOOST_AUTO_TEST_CASE(loadAdditionalConstraints_IncompleteRhsFile)
{
    std::filesystem::path testPath = getFolder() / "test_data";
    std::filesystem::create_directories(testPath / "cluster1");

    std::ofstream iniFile(testPath / "cluster1" / "additional-constraints.ini");
    iniFile << "[constraint1]\n";
    iniFile << "variable=injection\n";
    iniFile << "operator=less\n";
    iniFile << "hours=[1,2,3]\n";
    iniFile.close();

    std::ofstream rhsFile(testPath / "cluster1" / "rhs_constraint1.txt");
    for (int i = 0; i < 10; ++i)
    {
        rhsFile << i * 1.0 << "\n";
    }
    rhsFile.close();

    ShortTermStorage::STStorageInput storageInput;
    ShortTermStorage::STStorageCluster cluster;
    cluster.id = "cluster1";
    storageInput.storagesByIndex.push_back(cluster);

    bool result = storageInput.loadAdditionalConstraints(testPath);
    BOOST_CHECK_EQUAL(result, false);

    std::filesystem::remove_all(testPath);
}

// Test data for parameterization
namespace bdata = boost::unit_test::data;

BOOST_DATA_TEST_CASE(Validate_AllVariableOperatorCombinations,
                     bdata::make({"injection", "withdrawal", "netting"})
                       ^ bdata::make({"less", "equal", "greater"}),
                     variable,
                     op)
{
    ShortTermStorage::AdditionalConstraints
      constraints("name", "name", "clusterA", variable, op, true, {});

    // Create constraints with valid hours
    constraints.constraints.push_back(ShortTermStorage::SingleAdditionalConstraint{{1, 2, 3}});
    constraints.constraints.push_back(ShortTermStorage::SingleAdditionalConstraint{{50, 100, 150}});
    constraints.constraints.push_back(
      ShortTermStorage::SingleAdditionalConstraint{{120, 121, 122}});

    // Validate the constraints
    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, true);
    BOOST_CHECK(error_msg.empty());
}

BOOST_DATA_TEST_CASE(Validate_AllVariableOperatorCombinationsFromFile,
                     bdata::make({"injection", "withdrawal", "netting"})
                       * bdata::make({"less", "equal", "greater"}),
                     variable,
                     op)
{
    // Define the path for the test data
    std::filesystem::path testPath = std::filesystem::temp_directory_path() / "test_data";
    std::filesystem::create_directories(testPath / "clustera");

    // Write the `.ini` file for this test case
    std::ofstream iniFile(testPath / "clustera" / "additional-constraints.ini");
    iniFile << "[constraint1]\n";
    iniFile << "variable=" << variable << "\n";
    iniFile << "operator=" << op << "\n";
    iniFile << "enabled=true\n";
    iniFile << "hours=[1,2,3]\n";
    iniFile.close();

    // Write the `rhs_constraint1.txt` file
    std::ofstream rhsFile(testPath / "clustera" / "rhs_constraint1.txt");
    for (unsigned int i = 0; i < HOURS_PER_YEAR; ++i)
    {
        rhsFile << i * 1.0 << "\n";
    }
    rhsFile.close();

    // Setup storage input and cluster
    ShortTermStorage::STStorageInput storageInput;
    ShortTermStorage::STStorageCluster cluster;
    cluster.id = "clustera";
    storageInput.storagesByIndex.push_back(cluster);

    // Load constraints from the `.ini` file
    bool result = storageInput.loadAdditionalConstraints(testPath);
    BOOST_CHECK_EQUAL(storageInput.cumulativeConstraintCount(), 1);

    // Assertions
    BOOST_CHECK_EQUAL(result, true);
    // Validate loaded constraints
    auto& built_cluster = storageInput.storagesByIndex[0];
    BOOST_REQUIRE_EQUAL(built_cluster.additionalConstraints.size(), 1);

    const auto& loadedConstraint = *built_cluster.additionalConstraints[0];

    // Check variable, operator type, and rhs values
    BOOST_CHECK_EQUAL(loadedConstraint.variable, variable);
    BOOST_CHECK_EQUAL(loadedConstraint.operatorType, op);
    const auto& rhs = loadedConstraint.rhs();
    BOOST_REQUIRE_EQUAL(rhs.timeSeries.height, HOURS_PER_YEAR);

    unsigned int i = 0;
    do
    {
        BOOST_CHECK_CLOSE(rhs.getCoefficient(0, i), i * 1.0, 0.001);
        // Check rhs values within a tolerance

        i += HOURS_PER_YEAR / 5;
    } while (i < HOURS_PER_YEAR);
}

BOOST_AUTO_TEST_CASE(Load_disabled)
{
    // Define the path for the test data
    std::filesystem::path testPath = std::filesystem::temp_directory_path() / "test_data";
    std::filesystem::create_directories(testPath / "clustera");

    // Write the `.ini` file for this test case
    std::ofstream iniFile(testPath / "clustera" / "additional-constraints.ini");
    iniFile << "[constraint1]\n";
    iniFile << "variable=injection\n";
    iniFile << "operator=less\n";
    iniFile << "enabled=false\n";
    iniFile << "hours=[1,2,3]\n";
    iniFile.close();

    // Setup storage input and cluster
    ShortTermStorage::STStorageInput storageInput;
    ShortTermStorage::STStorageCluster cluster;
    cluster.id = "clustera";
    storageInput.storagesByIndex.push_back(cluster);

    // Load constraints from the `.ini` file
    bool result = storageInput.loadAdditionalConstraints(testPath);
    BOOST_CHECK_EQUAL(storageInput.cumulativeConstraintCount(), 0);

    // Assertions
    BOOST_CHECK_EQUAL(result, true);
    // Validate loaded constraints
    auto& built_cluster = storageInput.storagesByIndex[0];
    BOOST_REQUIRE_EQUAL(built_cluster.additionalConstraints.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
