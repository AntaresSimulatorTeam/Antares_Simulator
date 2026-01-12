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

#include "antares/antares/constants.h"
#include "antares/study/parts/short-term-storage/additionalConstraints.h"
#include "antares/study/parts/short-term-storage/container.h"

using namespace Antares::Data;
using namespace Antares::Data::ShortTermStorage;

namespace fs = std::filesystem;

struct PenaltyCostOnVariation
{
    bool injection = false;
    bool withdrawal = false;
};

void resizeFillVectors(ShortTermStorage::Series& series, double value, unsigned size)
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

void createIndividualFileSeries(const fs::path& path, double value, unsigned size)
{
    std::ofstream outfile(path);

    for (unsigned i = 0; i < size; i++)
    {
        outfile << value << std::endl;
    }

    outfile.close();
}

void createIndividualFileSeries(const fs::path& path, unsigned size)
{
    std::ofstream outfile;
    outfile.open(path, std::ofstream::out | std::ofstream::trunc);

    for (unsigned i = 0; i < size; i++)
    {
        double value = i * 0.0001;
        outfile << value << std::endl;
    }

    outfile.close();
}

// =================
// The fixture
// =================
struct WorkDirCreationFixture
{
    WorkDirCreationFixture()
    {
        work_dir = fs::temp_directory_path() / "testWorkDir";
        fs::create_directories(work_dir);
    }

    ~WorkDirCreationFixture()
    {
        fs::remove_all(work_dir);
    }

    fs::path work_dir;
};

struct Fixture: public WorkDirCreationFixture
{
    Fixture() = default;
    void createFileSeries(double value, unsigned size);
    void createFileSeries(unsigned size);
    void createIniFileWrongValue();
    void createIniFile(bool enabled);
    void createIniFile(const PenaltyCostOnVariation& penaltyCostOnVariation);
    void createEmptyIniFile();

    ShortTermStorage::Series series;
    ShortTermStorage::Properties properties;
    ShortTermStorage::STStorageCluster cluster;
    ShortTermStorage::STStorageInput container;

    PenaltyCostOnVariation penaltyCostOnVariation;

private:
    std::ofstream iniFile_;
};

void Fixture::createFileSeries(double value, unsigned size)
{
    createIndividualFileSeries(work_dir / "PMAX-injection.txt", value, size);
    createIndividualFileSeries(work_dir / "PMAX-withdrawal.txt", value, size);
    createIndividualFileSeries(work_dir / "inflows.txt", value, size);
    createIndividualFileSeries(work_dir / "lower-rule-curve.txt", value, size);
    createIndividualFileSeries(work_dir / "upper-rule-curve.txt", value, size);

    createIndividualFileSeries(work_dir / "cost-injection.txt", value, size);
    createIndividualFileSeries(work_dir / "cost-withdrawal.txt", value, size);
    createIndividualFileSeries(work_dir / "cost-level.txt", value, size);
    createIndividualFileSeries(work_dir / "cost-variation-injection.txt", value, size);
    createIndividualFileSeries(work_dir / "cost-variation-withdrawal.txt", value, size);
}

void Fixture::createFileSeries(unsigned size)
{
    createIndividualFileSeries(work_dir / "PMAX-injection.txt", size);
    createIndividualFileSeries(work_dir / "PMAX-withdrawal.txt", size);
    createIndividualFileSeries(work_dir / "inflows.txt", size);
    createIndividualFileSeries(work_dir / "lower-rule-curve.txt", size);
    createIndividualFileSeries(work_dir / "upper-rule-curve.txt", size);

    createIndividualFileSeries(work_dir / "cost-injection.txt", size);
    createIndividualFileSeries(work_dir / "cost-withdrawal.txt", size);
    createIndividualFileSeries(work_dir / "cost-level.txt", size);

    createIndividualFileSeries(work_dir / "cost-variation-injection.txt", size);
    createIndividualFileSeries(work_dir / "cost-variation-withdrawal.txt", size);
}

void Fixture::createIniFile(bool enabled)
{
    iniFile_.open(work_dir / "list.ini", std::ofstream::out | std::ofstream::trunc);
    iniFile_ << "[area]" << std::endl;
    iniFile_ << "name = area" << std::endl;
    iniFile_ << "group = PSP_open" << std::endl;
    iniFile_ << "injectionnominalcapacity = 870.000000" << std::endl;
    iniFile_ << "withdrawalnominalcapacity = 900.000000" << std::endl;
    iniFile_ << "reservoircapacity = 31200.000000" << std::endl;
    iniFile_ << "efficiency = 0.75" << std::endl;
    iniFile_ << "efficiencywithdrawal = 0.9" << std::endl;
    iniFile_ << "initiallevel = 0.50000" << std::endl;
    iniFile_ << "enabled = " << (enabled ? "true" : "false") << std::endl;
    iniFile_.close();
}

void Fixture::createIniFile(const PenaltyCostOnVariation& penaltyCostOnVariation)
{
    iniFile_.open(work_dir / "list.ini", std::ofstream::out | std::ofstream::trunc);
    iniFile_ << "[area]" << std::endl;
    iniFile_ << "name = area" << std::endl;
    iniFile_ << "group = PSP_open" << std::endl;
    iniFile_ << "penalize-variation-injection = " << std::boolalpha
             << penaltyCostOnVariation.injection << std::endl;
    iniFile_ << "penalize-variation-withdrawal = " << std::boolalpha
             << penaltyCostOnVariation.withdrawal << std::endl;
    iniFile_.close();
}

void Fixture::createEmptyIniFile()
{
    iniFile_.open(work_dir / "list.ini", std::ofstream::out | std::ofstream::trunc);
    iniFile_.close();
}

void Fixture::createIniFileWrongValue()
{
    iniFile_.open(work_dir / "list.ini", std::ofstream::out | std::ofstream::trunc);
    iniFile_ << "[area]" << std::endl;
    iniFile_ << "name = area" << std::endl;
    iniFile_ << "group = abcde" << std::endl;
    iniFile_ << "injectionnominalcapacity = -870.000000" << std::endl;
    iniFile_ << "withdrawalnominalcapacity = -900.000000" << std::endl;
    iniFile_ << "reservoircapacity = -31200.000000" << std::endl;
    iniFile_ << "efficiency = 4" << std::endl;
    iniFile_ << "efficiencywithdrawal = -2" << std::endl;
    iniFile_ << "initiallevel = -0.50000" << std::endl;
    iniFile_.close();
}

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
    series.loadFromFolder(work_dir, StudyVersion(9, 2));
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

    resizeFillVectors(series, 0.0, HOURS_PER_YEAR);
    BOOST_CHECK(series.validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading, Fixture)
{
    createFileSeries(1.0, HOURS_PER_YEAR);

    BOOST_CHECK(series.loadFromFolder(work_dir, StudyVersion::latest()));
    BOOST_CHECK(series.validate("", StudyVersion::latest()));
    BOOST_CHECK(series.inflows.getCoefficient(0, 0) == 1);
    BOOST_CHECK(series.maxInjectionModulation[8759] == 1);
    BOOST_CHECK(series.upperRuleCurve[1343] == 1);
    BOOST_CHECK(series.costVariationInjection[0] == 1);
    BOOST_CHECK(series.costVariationWithdrawal[0] == 1);
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_880, Fixture)
{
    createFileSeries(1.0, HOURS_PER_YEAR);

    BOOST_CHECK(series.loadFromFolder(work_dir, StudyVersion(8, 8)));
    BOOST_CHECK(series.validate("", StudyVersion(8, 8)));
    BOOST_CHECK(series.inflows.getCoefficient(0, 0) == 1);
    BOOST_CHECK(series.maxInjectionModulation[8759] == 1);
    BOOST_CHECK(series.upperRuleCurve[1343]);

    // New elements should NOT be loaded if the study version is < 9.2
    BOOST_CHECK(series.costVariationInjection.empty());
    BOOST_CHECK(series.costVariationWithdrawal.empty());
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_different_values_880, Fixture)
{
    createFileSeries(HOURS_PER_YEAR);

    BOOST_CHECK(series.loadFromFolder(work_dir, StudyVersion(8, 8)));
    BOOST_CHECK(series.validate("", StudyVersion(8, 8)));
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_different_values, Fixture)
{
    createFileSeries(HOURS_PER_YEAR);

    BOOST_CHECK(series.loadFromFolder(work_dir, StudyVersion::latest()));
    BOOST_CHECK(series.validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_negative_value, Fixture)
{
    createFileSeries(-247.0, HOURS_PER_YEAR);

    BOOST_CHECK(series.loadFromFolder(work_dir, StudyVersion::latest()));
    BOOST_CHECK(!series.validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_too_big, Fixture)
{
    createFileSeries(1.0, 9000);

    BOOST_CHECK(series.loadFromFolder(work_dir, StudyVersion::latest()));
    BOOST_CHECK(series.validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_too_big_880, Fixture)
{
    createFileSeries(1.0, 9000);

    BOOST_CHECK(series.loadFromFolder(work_dir, StudyVersion(8, 8)));
    BOOST_CHECK(series.validate("", StudyVersion(8, 8)));
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_too_small, Fixture)
{
    createFileSeries(1.0, 100);

    BOOST_CHECK(!series.loadFromFolder(work_dir, StudyVersion::latest()));
    BOOST_CHECK(!series.validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_series_folder_loading_empty, Fixture)
{
    BOOST_CHECK(series.loadFromFolder(work_dir, StudyVersion::latest()));
    BOOST_CHECK(!series.validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_series_vector_fill, Fixture)
{
    series.fillDefaultSeriesIfEmpty();
    BOOST_CHECK(series.validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_cluster_series_vector_fill, Fixture)
{
    BOOST_CHECK(cluster.loadSeries(work_dir, StudyVersion::latest()));
    BOOST_CHECK(cluster.series->validate("", StudyVersion::latest()));
}

BOOST_FIXTURE_TEST_CASE(check_cluster_series_load_vector, Fixture)
{
    createFileSeries(0.5, HOURS_PER_YEAR);

    BOOST_CHECK(cluster.loadSeries(work_dir, StudyVersion::latest()));
    BOOST_CHECK(cluster.series->validate("", StudyVersion::latest()));
    BOOST_CHECK(cluster.series->maxWithdrawalModulation[0] == 0.5);
    BOOST_CHECK(cluster.series->inflows.getCoefficient(0, 2756) == 0.5);
    BOOST_CHECK(cluster.series->lowerRuleCurve[6392] == 0.5);
    BOOST_CHECK(cluster.series->costVariationInjection[15] == 0.5);
    BOOST_CHECK(cluster.series->costVariationWithdrawal[756] == 0.5);
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_enabled_load, Fixture)
{
    createIniFile(true);

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(work_dir));

    auto& properties = container.storagesByIndex[0].properties;

    BOOST_CHECK(properties.enabled);
    BOOST_CHECK_EQUAL(container.count(), 1);
    BOOST_CHECK(properties.validate());
    BOOST_CHECK(!properties.penalizeVariationInjection);
    BOOST_CHECK(!properties.penalizeVariationWithdrawal);
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_enabled_load_with_cost_variation_injection,
                        Fixture)
{
    penaltyCostOnVariation = {.injection = true, .withdrawal = false};
    createIniFile(penaltyCostOnVariation);

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(work_dir));

    auto& properties = container.storagesByIndex[0].properties;

    BOOST_CHECK(properties.penalizeVariationInjection);
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_enabled_load_with_cost_variation_withdrawal,
                        Fixture)
{
    penaltyCostOnVariation = {.injection = false, .withdrawal = true};
    createIniFile(penaltyCostOnVariation);

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(work_dir));

    auto& properties = container.storagesByIndex[0].properties;

    BOOST_CHECK(properties.penalizeVariationWithdrawal);
}

BOOST_FIXTURE_TEST_CASE(
  check_container_properties_enabled_load_with_cost_variation_injection_and_withdrawal,
  Fixture)
{
    penaltyCostOnVariation = {.injection = true, .withdrawal = true};
    createIniFile(penaltyCostOnVariation);

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(work_dir));

    auto& properties = container.storagesByIndex[0].properties;

    BOOST_CHECK(properties.penalizeVariationInjection);
    BOOST_CHECK(properties.penalizeVariationWithdrawal);
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_disabled_load, Fixture)
{
    createIniFile(false);

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(work_dir));

    auto& properties = container.storagesByIndex[0].properties;

    BOOST_CHECK(!properties.enabled);
    BOOST_CHECK_EQUAL(container.count(), 0);
    BOOST_CHECK(properties.validate());
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_wrong_value, Fixture)
{
    createIniFileWrongValue();

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(work_dir));
    BOOST_CHECK(!container.storagesByIndex[0].properties.validate());
}

BOOST_FIXTURE_TEST_CASE(check_container_properties_empty_file, Fixture)
{
    createEmptyIniFile();

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(work_dir));
}

#ifdef BUILD_UI
BOOST_FIXTURE_TEST_CASE(check_file_save, Fixture)
{
    createIniFile(true);

    BOOST_CHECK(container.createSTStorageClustersFromIniFile(work_dir));

    fs::remove(work_dir / "list.ini");

    BOOST_CHECK(container.saveToFolder(work_dir.string()));
    BOOST_CHECK(container.createSTStorageClustersFromIniFile(work_dir));
}

BOOST_FIXTURE_TEST_CASE(check_series_save, Fixture)
{
    resizeFillVectors(series, 0.123456789, HOURS_PER_YEAR);

    BOOST_CHECK(series.saveToFolder(work_dir.string()));
    resizeFillVectors(series, 0, 0);

    BOOST_CHECK(series.loadFromFolder(work_dir, StudyVersion::latest()));
    BOOST_CHECK(series.validate("", StudyVersion::latest()));
}
#endif

BOOST_AUTO_TEST_SUITE_END()

// Test data for parameterization
namespace bdata = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(ValidatingAdditionalConstraints)

BOOST_AUTO_TEST_CASE(Validate_ClusterIdEmpty)
{
    AdditionalConstraints constraints("name", "name", "", "injection", "less", true, {});

    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, false);
    BOOST_CHECK_EQUAL(error_msg, "Cluster ID is empty.");
}

BOOST_AUTO_TEST_CASE(Validate_InvalidVariable)
{
    AdditionalConstraints constraints("name", "name", "ClusterA", "invalid", "less", true, {});
    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, false);
    BOOST_CHECK_EQUAL(error_msg,
                      "Invalid variable type. Must be 'injection', 'withdrawal', or 'netting'.");
}

BOOST_AUTO_TEST_CASE(Validate_InvalidOperatorType)
{
    AdditionalConstraints constraints("name", "name", "ClusterA", "injection", "invalid", true, {});

    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, false);
    BOOST_CHECK_EQUAL(error_msg, "Invalid operator type. Must be 'less', 'equal', or 'greater'.");
}

BOOST_AUTO_TEST_CASE(Validate_InvalidHours_Empty)
{
    SingleAdditionalConstraint constraint;
    // Case : Empty hours
    constraint.hours = {}; // Invalid: empty

    AdditionalConstraints
      constraints("name", "name", "ClusterA", "injection", "less", true, {constraint});

    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, false);
    BOOST_CHECK_EQUAL(error_msg, "Hours sets contains invalid values. Must be between 1 and 168.");
}

BOOST_AUTO_TEST_CASE(Validate_InvalidHours_Out_of_range)
{
    AdditionalConstraints constraints("name", "name", "ClusterA", "injection", "less", true, {});

    // Case: Out of range
    SingleAdditionalConstraint constraint;
    constraint.hours = {120, 169}; // Invalid: out of range
    constraints.constraints.push_back(constraint);

    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, false);
    BOOST_CHECK_EQUAL(error_msg, "Hours sets contains invalid values. Must be between 1 and 168.");
}

BOOST_AUTO_TEST_CASE(Validate_InvalidHours_Below_minimum)
{
    AdditionalConstraints constraints("name", "name", "ClusterA", "injection", "less", true, {});

    // Case : Below minimum
    SingleAdditionalConstraint constraint;
    constraint.hours = {0, 1}; // Invalid: below minimum
    constraints.constraints.push_back(constraint);

    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, false);
    BOOST_CHECK_EQUAL(error_msg, "Hours sets contains invalid values. Must be between 1 and 168.");
}

BOOST_AUTO_TEST_CASE(Validate_ValidConstraints)
{
    AdditionalConstraints constraints("name", "name", "ClusterA", "injection", "less", true, {});

    SingleAdditionalConstraint constraint1;
    constraint1.hours = {1, 2, 3}; // Valid hours

    SingleAdditionalConstraint constraint2;
    constraint2.hours = {100, 150, 168}; // Valid hours

    constraints.constraints = {constraint1, constraint2};

    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, true);
    BOOST_CHECK(error_msg.empty());
}

BOOST_DATA_TEST_CASE(Validate_AllVariableOperatorCombinations,
                     bdata::make({"injection", "withdrawal", "netting"})
                       ^ bdata::make({"less", "equal", "greater"}),
                     variable,
                     op)
{
    AdditionalConstraints constraints("name", "name", "clusterA", variable, op, true, {});

    // Create constraints with valid hours
    constraints.constraints.push_back(SingleAdditionalConstraint{{1, 2, 3}});
    constraints.constraints.push_back(SingleAdditionalConstraint{{50, 100, 150}});
    constraints.constraints.push_back(SingleAdditionalConstraint{{120, 121, 122}});

    // Validate the constraints
    auto [ok, error_msg] = validate(constraints);
    BOOST_CHECK_EQUAL(ok, true);
    BOOST_CHECK(error_msg.empty());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LoadingAdditionalConstraints)

struct IniConstraint
{
    std::string name;
    std::string variable;
    std::string op;
    std::string hours;
    std::string enabled = "true";
};

void makeAdditConstrIniFile(fs::path& dir, const std::vector<IniConstraint>& ini_constraints)
{
    std::ofstream iniFile(dir / "additional-constraints.ini");
    for (const auto& c: ini_constraints)
    {
        iniFile << "[" + c.name + "]\n";
        iniFile << "variable=" + c.variable + "\n";
        iniFile << "operator=" + c.op + "\n";
        iniFile << "hours=" + c.hours + "\n";
        iniFile << "enabled=" + c.enabled + "\n";
    }
    iniFile.close();
}

void makeRHSforConstraint(fs::path& dir, unsigned nb_lines, const std::string& constraint_name)
{
    std::string filename = "rhs_" + constraint_name + ".txt";
    std::ofstream rhsFile(dir / filename);

    for (unsigned i = 0; i < nb_lines; ++i)
    {
        rhsFile << i << "\n";
    }
    rhsFile.close();
}

// =========================================
// Fixture : AdditConstrFixture
// =========================================

template<unsigned nb_sts>
struct AdditConstrFixture: public WorkDirCreationFixture
{
    AdditConstrFixture();

    STStorageInput storageInput;
    std::vector<fs::path> pathsToSTS;
    std::vector<std::string> sts_names;
};

template<unsigned nb_sts>
AdditConstrFixture<nb_sts>::AdditConstrFixture()
{
    for (unsigned i = 1; i <= nb_sts; i++)
    {
        std::string sts_name = "my-sts-" + std::to_string(i);
        sts_names.push_back(sts_name);

        fs::path sts_dir = work_dir / sts_name;
        pathsToSTS.push_back(work_dir / sts_name); // Storing path tp STS data

        fs::create_directories(sts_dir);

        STStorageCluster sts;
        sts.id = sts_name;
        storageInput.storagesByIndex.push_back(sts);
    }
}

// =========================================
// Test on loading additional constraints
// =========================================
BOOST_FIXTURE_TEST_CASE(constraint_has_a_unknown_key___loading_returns_false, AdditConstrFixture<1>)
{
    std::ofstream iniFile(pathsToSTS[0] / "additional-constraints.ini");
    iniFile << "[some constraint]\n";
    iniFile << "blabla=some value\n";
    iniFile.close();

    BOOST_CHECK(!storageInput.loadAdditionalConstraints(work_dir));
}

BOOST_FIXTURE_TEST_CASE(loadAdditionalConstraints_ValidFile, AdditConstrFixture<1>)
{
    makeAdditConstrIniFile(pathsToSTS[0], {{"my_constr", "injection", "less", "[1,2,3]"}});

    BOOST_CHECK(storageInput.loadAdditionalConstraints(work_dir));
    BOOST_CHECK_EQUAL(storageInput.storagesByIndex[0].additionalConstraints.size(), 1);
    BOOST_CHECK_EQUAL(storageInput.storagesByIndex[0].additionalConstraints[0]->name, "my_constr");
}

BOOST_FIXTURE_TEST_CASE(loadAdditionalConstraints_InvalidHours, AdditConstrFixture<1>)
{
    makeAdditConstrIniFile(pathsToSTS[0],
                           {{"my_constr", "injection", "less", "[0,1]"}}); // Invalid hours

    BOOST_CHECK(!storageInput.loadAdditionalConstraints(work_dir));
}

BOOST_AUTO_TEST_CASE(loadAdditionalConstraints_MissingFile)
{
    STStorageInput storageInput;
    BOOST_CHECK(storageInput.loadAdditionalConstraints("nonexistent_path"));
}

BOOST_FIXTURE_TEST_CASE(loadAdditionalConstraints_InvalidConstraint, AdditConstrFixture<1>)
{
    makeAdditConstrIniFile(pathsToSTS[0],
                           {{"my_constr", "invalid", "less", "[1,2,3]"}}); // Invalid variable

    BOOST_CHECK(!storageInput.loadAdditionalConstraints(work_dir));
}

BOOST_FIXTURE_TEST_CASE(loadAdditionalConstraints_ValidRhs, AdditConstrFixture<1>)
{
    makeAdditConstrIniFile(pathsToSTS[0], {{"my_constr", "injection", "less", "[1,2,3]"}});
    makeRHSforConstraint(pathsToSTS[0], HOURS_PER_YEAR, "my_constr");

    BOOST_CHECK(storageInput.loadAdditionalConstraints(work_dir));

    const auto& rhs = storageInput.storagesByIndex[0].additionalConstraints[0]->rhs();
    BOOST_CHECK_EQUAL(rhs.timeSeries.height, HOURS_PER_YEAR);
    BOOST_CHECK_EQUAL(rhs.getCoefficient(0, 0), 0.0);
    BOOST_CHECK_EQUAL(rhs.getCoefficient(0, HOURS_PER_YEAR - 1), HOURS_PER_YEAR - 1);
}

BOOST_FIXTURE_TEST_CASE(Load2ConstraintsFromIniFile, AdditConstrFixture<1>)
{
    makeAdditConstrIniFile(pathsToSTS[0],
                           {{"constraint1", "injection", "less", "[1,2,3]"},
                            {"constraint2", "withdrawal", "greater", "[5,33]"}});

    makeRHSforConstraint(pathsToSTS[0], HOURS_PER_YEAR, "constraint1");

    BOOST_CHECK(storageInput.loadAdditionalConstraints(work_dir));
    BOOST_CHECK_EQUAL(storageInput.storagesByIndex[0].additionalConstraints.size(), 2);

    //------- constraint1 ----------
    const auto& constraint1 = storageInput.storagesByIndex[0].additionalConstraints[0];
    BOOST_CHECK_EQUAL(constraint1->name, "constraint1");
    BOOST_CHECK_EQUAL(constraint1->operatorType, "less");
    BOOST_CHECK_EQUAL(constraint1->variable, "injection");
    BOOST_CHECK_EQUAL(constraint1->cluster_id, sts_names[0]);

    const auto& rhs1 = constraint1->rhs();
    BOOST_CHECK_EQUAL(rhs1.timeSeries.height, HOURS_PER_YEAR);
    BOOST_CHECK_EQUAL(rhs1.getCoefficient(0, 0), 0.0);
    BOOST_CHECK_EQUAL(rhs1.getCoefficient(0, HOURS_PER_YEAR - 1), HOURS_PER_YEAR - 1);

    //------- constraint2 ----------
    const auto& constraint2 = storageInput.storagesByIndex[0].additionalConstraints[1];
    BOOST_CHECK_EQUAL(constraint2->name, "constraint2");
    BOOST_CHECK_EQUAL(constraint2->operatorType, "greater");
    BOOST_CHECK_EQUAL(constraint2->variable, "withdrawal");
    BOOST_CHECK_EQUAL(constraint2->cluster_id, sts_names[0]);

    const auto& rhs2 = constraint2->rhs();
    BOOST_CHECK_EQUAL(rhs2.timeSeries.height, HOURS_PER_YEAR);
    BOOST_CHECK_EQUAL(rhs2.getCoefficient(0, 0), 0.0);
    BOOST_CHECK_EQUAL(rhs2.getCoefficient(0, HOURS_PER_YEAR - 1), 0);
}

BOOST_FIXTURE_TEST_CASE(loadAdditionalConstraints_MissingRhsFile, AdditConstrFixture<1>)
{
    makeAdditConstrIniFile(pathsToSTS[0], {{"my_constr", "injection", "less", "[1,2,3]"}});

    BOOST_CHECK(storageInput.loadAdditionalConstraints(work_dir));

    const auto& constraintRhs = storageInput.storagesByIndex[0].additionalConstraints[0]->rhs();
    BOOST_CHECK_EQUAL(constraintRhs.timeSeries.height, HOURS_PER_YEAR);
    BOOST_CHECK_EQUAL(constraintRhs.getCoefficient(0, 0), 0.0);
}

BOOST_FIXTURE_TEST_CASE(loadAdditionalConstraints_MalformedRhsFile, AdditConstrFixture<1>)
{
    makeAdditConstrIniFile(pathsToSTS[0], {{"my_constr", "injection", "less", "[1,2,3]"}});

    std::ofstream rhsFile(pathsToSTS[0] / "rhs_my_constr.txt");
    rhsFile << "1.0\n2.0\ninvalid\n4.0\n"; // Malformed line
    rhsFile.close();

    BOOST_CHECK(!storageInput.loadAdditionalConstraints(work_dir));
}

BOOST_FIXTURE_TEST_CASE(loadAdditionalConstraints_IncompleteRhsFile, AdditConstrFixture<1>)
{
    makeAdditConstrIniFile(pathsToSTS[0], {{"my_constr", "injection", "less", "[1,2,3]"}});

    makeRHSforConstraint(pathsToSTS[0], 10, "my_constr");

    BOOST_CHECK(!storageInput.loadAdditionalConstraints(work_dir));
}

BOOST_DATA_TEST_CASE_F(AdditConstrFixture<1>,
                       Validate_AllVariableOperatorCombinationsFromFile,
                       bdata::make({"injection", "withdrawal", "netting"})
                         * bdata::make({"less", "equal", "greater"}),
                       variable,
                       op)
{
    makeAdditConstrIniFile(pathsToSTS[0], {{"my_constr", variable, op, "[1,2,3]"}});
    makeRHSforConstraint(pathsToSTS[0], HOURS_PER_YEAR, "my_constr");

    BOOST_CHECK(storageInput.loadAdditionalConstraints(work_dir));
    BOOST_CHECK_EQUAL(storageInput.cumulativeConstraintCount(), 1);
    BOOST_REQUIRE_EQUAL(storageInput.storagesByIndex[0].additionalConstraints.size(), 1);

    const auto loadedConstraint = storageInput.storagesByIndex[0].additionalConstraints[0];
    BOOST_CHECK_EQUAL(loadedConstraint->variable, variable);
    BOOST_CHECK_EQUAL(loadedConstraint->operatorType, op);

    const auto& rhs = loadedConstraint->rhs();
    BOOST_REQUIRE_EQUAL(rhs.timeSeries.height, HOURS_PER_YEAR);

    unsigned i = 0;
    while (i < HOURS_PER_YEAR)
    {
        BOOST_CHECK_CLOSE(rhs.getCoefficient(0, i), i, 0.001 /* tolerance */);
        i += HOURS_PER_YEAR / 5;
    }
}

BOOST_FIXTURE_TEST_CASE(Load_disabled, AdditConstrFixture<1>)
{
    makeAdditConstrIniFile(pathsToSTS[0], {{"my_constr", "injection", "less", "[1,2,3]", "false"}});

    BOOST_CHECK(storageInput.loadAdditionalConstraints(work_dir));
    BOOST_CHECK_EQUAL(storageInput.cumulativeConstraintCount(), 0);
}

BOOST_FIXTURE_TEST_CASE(multiple_sts__one_sts_has_no_additional_constraint__all_constr_fully_loaded,
                        AdditConstrFixture<2> /* 2 STS are built here */)
{
    // Fixture builds 2 short term storage here.
    // We make 2 constraints for the first one.
    makeAdditConstrIniFile(pathsToSTS[1],
                           {{"constr_2a", "injection", "less", "[1]"},
                            {"constr_2b", "netting", "greater", "[1,3]"}});
    makeRHSforConstraint(pathsToSTS[1], HOURS_PER_YEAR, "constr_2a");
    makeRHSforConstraint(pathsToSTS[1], HOURS_PER_YEAR, "constr_2b");

    BOOST_CHECK(storageInput.loadAdditionalConstraints(work_dir));

    BOOST_CHECK_EQUAL(storageInput.cumulativeConstraintCount(), 2);
    BOOST_REQUIRE_EQUAL(storageInput.storagesByIndex[0].additionalConstraints.size(), 0);
    BOOST_REQUIRE_EQUAL(storageInput.storagesByIndex[1].additionalConstraints.size(), 2);

    const auto& constr_2a = storageInput.storagesByIndex[1].additionalConstraints[0];
    const auto& constr_2b = storageInput.storagesByIndex[1].additionalConstraints[1];

    BOOST_REQUIRE_EQUAL(constr_2a->id, "constr_2a");
    BOOST_REQUIRE_EQUAL(constr_2b->id, "constr_2b");
}

BOOST_FIXTURE_TEST_CASE(one_sts_with_many_constraints_one_is_disabled__all_constr_fully_loaded,
                        AdditConstrFixture<1> /* 1 sts here */)
{
    makeAdditConstrIniFile(pathsToSTS[0],
                           {{"constr_1", "withdrawal", "less", "[1]", "false"}, // Disabled
                            {"constr_2", "injection", "less", "[1,6]"},
                            {"constr_3", "netting", "greater", "[1,3]"}});
    makeRHSforConstraint(pathsToSTS[0], HOURS_PER_YEAR, "constr_2");
    makeRHSforConstraint(pathsToSTS[0], HOURS_PER_YEAR, "constr_3");

    BOOST_CHECK(storageInput.loadAdditionalConstraints(work_dir));

    BOOST_CHECK_EQUAL(storageInput.cumulativeConstraintCount(), 2);
    BOOST_REQUIRE_EQUAL(storageInput.storagesByIndex[0].additionalConstraints.size(), 2);

    const auto& constr_2 = storageInput.storagesByIndex[0].additionalConstraints[0];
    const auto& constr_3 = storageInput.storagesByIndex[0].additionalConstraints[1];

    BOOST_REQUIRE_EQUAL(constr_2->id, "constr_2");
    BOOST_REQUIRE_EQUAL(constr_3->id, "constr_3");
}

BOOST_AUTO_TEST_SUITE_END()
