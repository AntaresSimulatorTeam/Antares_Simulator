// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test_api
#define WIN32_LEAN_AND_MEAN

#include <fstream>
#include <memory>

#include <boost/test/unit_test.hpp>

#include "antares/antares/constants.h"
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/study/study.h"
#include "antares/writer/in_memory_writer.h"

#include "in-memory-study.h"
#include "singleProblemGetterImpl.h"

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Optimisation;
using namespace Antares::Expressions::Nodes;
constexpr double EPSILON = 1.e-6;

std::size_t findIndex(const std::vector<std::string>& v, const std::string& value)
{
    std::size_t ret = std::distance(v.begin(), std::find(v.begin(), v.end(), value));
    BOOST_REQUIRE_MESSAGE(ret < v.size(), value + " not found");
    return ret;
}

std::unique_ptr<Study> buildStudy(bool thermal, bool hydro)
{
    StudyBuilder builder;
    builder.simulationBetweenDays(0, 14);
    builder.setNumberMCyears(2);
    auto* area = builder.addAreaToStudy("AREA");
    area->hydro.reservoirManagement = false;
    if (thermal)
    {
        auto thCluster = addClusterToArea(area, "dispatch-cluster");
        ThermalClusterConfig clusterConfig(thCluster);
        clusterConfig.setNominalCapacity(100)
          .setUnitCount(1)
          .setCosts(20.)
          .setAvailablePowerNumberOfTS(1)
          .setAvailablePower(0, 102.);
    }
    if (hydro)
    {
        auto& h = area->hydro;

        TimeSeriesConfigurer genP(h.series->maxHourlyGenPower);
        genP.setDimensions(1).fillColumnWith(0, 100.);

        TimeSeriesConfigurer hydroStorage(h.series->storage);
        hydroStorage.setDimensions(1, DAYS_PER_YEAR).fillColumnWith(0, 10.);

        TimeSeriesConfigurer genE(h.dailyNbHoursAtGenPmax);
        genE.setDimensions(1, DAYS_PER_YEAR).fillColumnWith(0, 24);

        h.reservoirCapacity = 1e4;
        h.reservoirManagement = true;
        h.prepro.release(); // eliminate "prepro" since we don't want TS generation

        // LOAD
        area->load.prepro.release(); // eliminate "prepro" since we don't want TS generation
        TimeSeriesConfigurer loadTSconfig(area->load.series);
        loadTSconfig.setDimensions(1).fillColumnWith(0, 120.);
    }

    // TODO StudyBuilder should have a `run` method that
    // initializeRuntimeInfos
    // auto study = builder.run();
    builder.study->initializeRuntimeInfos();

    // TODO this is HORRIBLE
    // more specifically, this resize is usually done when loading from files. It's all good, except
    // when you DON'T LOAD FILES.
    area->hydro.deltaBetweenFinalAndInitialLevels.resize(builder.study->parameters.nbYears);
    builder.study->resizeAllTimeseriesNumbers(
      1 + builder.study->runtime.rangeLimits.year[Antares::Data::rangeEnd]);

    return std::move(builder.study);
}
BOOST_AUTO_TEST_SUITE(in_memory_check_problem_contents)

BOOST_AUTO_TEST_CASE(single_problem_thermal_first_week_nominal_case)
{
    auto study = buildStudy(true, false);
    Implementation::SingleProblemGetter getter({std::move(study), nullptr});
    const ConstantDataFromAntares constantData = getter.getConstantData();
    // 504 = 3*168, 3 sets of variables :
    // - unsupplied energy
    // - spilled energy
    // - dispatchable production
    BOOST_CHECK_EQUAL(constantData.VariablesCount, 504);
    // 336 = 3*168 : 3 hourly constraints
    // - area balance
    // - fictive loads
    // - Max unsupplied energy
    BOOST_CHECK_EQUAL(constantData.ConstraintesCount, 504);

    const auto unsuppliedVariable = findIndex(constantData.VariablesMeaning,
                                              "PositiveUnsuppliedEnergy::area<area>::hour<0>");
    const auto spilledVariable = findIndex(constantData.VariablesMeaning,
                                           "NegativeUnsuppliedEnergy::area<area>::hour<0>");

    const auto areaBalanceConstraint = findIndex(constantData.ConstraintsMeaning,
                                                 "AreaBalance::area<area>::hour<0>");

    const auto dispatchableVariable = findIndex(
      constantData.VariablesMeaning,
      "DispatchableProduction::area<area>::ThermalCluster<dispatch-cluster>::hour<0>");

    // TODO explain
    BOOST_CHECK_EQUAL(constantData.ColumnIndexes[0], 0);
    BOOST_CHECK_EQUAL(constantData.ColumnIndexes[1], 1);
    BOOST_CHECK_EQUAL(constantData.ConstraintsMatrixCoeff[0], -1);
    BOOST_CHECK_EQUAL(constantData.ConstraintsMatrixCoeff[1], -1);

    const WeeklyDataFromAntares firstWeekData = getter.getWeeklyData({0, 1});
    BOOST_CHECK_EQUAL(firstWeekData.name, "problem-1-1--optim-nb-1");

    // COST
    BOOST_CHECK_CLOSE(firstWeekData.LinearCost[dispatchableVariable],
                      19.999456400134147,
                      EPSILON); // thermal cost 20 + noise
    BOOST_CHECK_CLOSE(firstWeekData.LinearCost[unsuppliedVariable],
                      999.99941243777027,
                      EPSILON); // unsupplied cost
    BOOST_CHECK_CLOSE(firstWeekData.LinearCost[spilledVariable],
                      999.99941243777027,
                      EPSILON); // spilled cost
    // BOUNDS
    BOOST_CHECK_EQUAL(firstWeekData.Xmin[dispatchableVariable], 0.);
    BOOST_CHECK_EQUAL(firstWeekData.Xmin[unsuppliedVariable], 0.);
    BOOST_CHECK_EQUAL(firstWeekData.Xmin[spilledVariable], 0.);

    double infinity = 1.e80;
    BOOST_CHECK_EQUAL(firstWeekData.Xmax[dispatchableVariable], 102.);
    BOOST_CHECK_EQUAL(firstWeekData.Xmax[unsuppliedVariable], infinity);
    BOOST_CHECK_EQUAL(firstWeekData.Xmax[spilledVariable], infinity);
}

BOOST_AUTO_TEST_CASE(single_problem_hydro_two_weeks_nominal_case)
{
    auto study = buildStudy(false, true);
    Implementation::SingleProblemGetter getter({std::move(study), nullptr});
    const ConstantDataFromAntares constantData = getter.getConstantData();
    // Total variable : 1008
    // - 168 unsupplied energy
    // - 168 spilled energy
    // - 168 hydro level
    // - 168 hydro prod
    // - 168 overflow
    BOOST_CHECK_EQUAL(constantData.VariablesCount, 840);
    // Total constaints : 673
    // - 168 area balance
    // - 168 fictive loads
    // - 168 max unsupplied energy
    // - 168 hydro level
    // - 1 hydro power
    BOOST_CHECK_EQUAL(constantData.ConstraintesCount, 673);

    const auto hydroLevelVariable = findIndex(constantData.VariablesMeaning,
                                              "HydroLevel::area<area>::hour<0>");

    const auto hydroProdVariable = findIndex(constantData.VariablesMeaning,
                                             "HydProd::area<area>::hour<0>");

    const auto unsuppliedVariable = findIndex(constantData.VariablesMeaning,
                                              "PositiveUnsuppliedEnergy::area<area>::hour<0>");
    const auto spilledVariable = findIndex(constantData.VariablesMeaning,
                                           "NegativeUnsuppliedEnergy::area<area>::hour<0>");

    const auto areaHydroLevel = findIndex(constantData.ConstraintsMeaning,
                                          "AreaHydroLevel::area<area>::hour<0>");

    const WeeklyDataFromAntares firstWeekData = getter.getWeeklyData({0, 1});
    // COST
    BOOST_CHECK_CLOSE(firstWeekData.LinearCost[hydroLevelVariable],
                      -1.e-6,
                      EPSILON); // default value
    BOOST_CHECK_CLOSE(firstWeekData.LinearCost[hydroProdVariable],
                      -0.00094518442622950813,
                      EPSILON); // hydro cost noise
    BOOST_CHECK_CLOSE(firstWeekData.LinearCost[unsuppliedVariable],
                      999.99941243777027,
                      EPSILON); // unsupplied cost
    BOOST_CHECK_CLOSE(firstWeekData.LinearCost[spilledVariable],
                      999.99941243777027,
                      EPSILON); // spilled cost
    // RHS
    BOOST_CHECK_CLOSE(firstWeekData.RHS[areaHydroLevel],
                      3048.5130614352684,
                      EPSILON); // random initial level

    const WeeklyDataFromAntares secondWeekData = getter.getWeeklyData({0, 2});
    // RHS
    BOOST_CHECK_CLOSE(secondWeekData.RHS[areaHydroLevel],
                      3048.5130614352684,
                      EPSILON); // random initial level

    BOOST_CHECK_EQUAL(secondWeekData.name, "problem-1-2--optim-nb-1");
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(in_memory_check_get_problem_ids)

BOOST_AUTO_TEST_CASE(three_years_two_weeks)
{
    StudyBuilder builder;
    builder.simulationBetweenDays(0, 14);

    builder.setNumberMCyears(3);

    auto study = std::move(builder.study);
    study->initializeRuntimeInfos();

    const Implementation::SingleProblemGetter getter({std::move(study), nullptr});
    auto problem_ids = getter.getProblemIds();
    BOOST_REQUIRE_EQUAL(problem_ids.size(), 3 * 2); // (3 years) x (2 weeks)

    // First problem is (year, week) = (0, 0)
    BOOST_CHECK_EQUAL(problem_ids[0].year, 0);
    BOOST_CHECK_EQUAL(problem_ids[0].week, 1);

    // Last problem is (year, week) = (2, 1)
    BOOST_CHECK_EQUAL(problem_ids[5].year, 2);
    BOOST_CHECK_EQUAL(problem_ids[5].week, 2);
}

BOOST_AUTO_TEST_CASE(three_years_two_weeks_one_disabled_year)
{
    StudyBuilder builder;
    builder.simulationBetweenDays(0, 14);

    builder.setNumberMCyears(3);

    auto study = std::move(builder.study);
    study->initializeRuntimeInfos();
    // Disable year 1 in the playlist
    study->parameters.yearsFilter[1] = false;

    const Implementation::SingleProblemGetter getter({std::move(study), nullptr});
    auto problem_ids = getter.getProblemIds();
    BOOST_REQUIRE_EQUAL(problem_ids.size(), 2 * 2); // (2 years) x (2 weeks), one year is disabled

    // First problem is (year, week) = (0, 0)
    BOOST_CHECK_EQUAL(problem_ids[0].year, 0);
    BOOST_CHECK_EQUAL(problem_ids[0].week, 1);

    // Last problem is (year, week) = (2, 1)
    BOOST_CHECK_EQUAL(problem_ids[3].year, 2);
    BOOST_CHECK_EQUAL(problem_ids[3].week, 2);
}

BOOST_AUTO_TEST_CASE(three_years_two_weeks_one_disabled_year_partial_year)
{
    StudyBuilder builder;
    builder.simulationBetweenDays(7, 14);

    builder.setNumberMCyears(3);

    auto study = std::move(builder.study);
    study->initializeRuntimeInfos();
    // Disable year 1 in the playlist
    study->parameters.yearsFilter[1] = false;

    const Implementation::SingleProblemGetter getter({std::move(study), nullptr});
    auto problem_ids = getter.getProblemIds();
    BOOST_REQUIRE_EQUAL(problem_ids.size(), 2 * 1); // (2 years) x (1 week), one year is disabled

    // First problem is (year, week) = (0, 0)
    BOOST_CHECK_EQUAL(problem_ids[0].year, 0);
    BOOST_CHECK_EQUAL(problem_ids[0].week, 1);

    // Last problem is (year, week) = (2, 1)
    BOOST_CHECK_EQUAL(problem_ids[1].year, 2);
    BOOST_CHECK_EQUAL(problem_ids[1].week, 1);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(file_export)

BOOST_AUTO_TEST_CASE(single_link_ntc_ts_numbers)
{
    StudyBuilder builder;
    builder.setNumberMCyears(5);

    auto* a1 = builder.addAreaToStudy("AREA");
    auto* a2 = builder.addAreaToStudy("ZREA");
    auto link = AreaAddLinkBetweenAreas(a1, a2);

    auto study = std::move(builder.study);
    study->initializeRuntimeInfos();
    Implementation::SingleProblemGetter getter({std::move(study), nullptr});

    // Erase TS numbers for repeatability (no randomness)
    link->timeseriesNumbers.reset(5);
    for (int ii = 0; ii < 5; ii++)
    {
        link->timeseriesNumbers[ii] = ii % 3;
    }

    auto output_dir = std::filesystem::temp_directory_path() / "study" / "output";
    getter.writeNTCTimeSeries(output_dir);
    std::ifstream read(output_dir / "ts-numbers" / "ntc" / "area" / "zrea.txt");
    BOOST_REQUIRE(read);
    std::string content((std::istreambuf_iterator<char>(read)), std::istreambuf_iterator<char>());
    const std::string expected = R"(size:1x5
1
2
3
1
2
)";

    BOOST_CHECK_EQUAL(content, expected);
}

BOOST_AUTO_TEST_CASE(single_link_structure_files)
{
    StudyBuilder builder;

    auto* a1 = builder.addAreaToStudy("AREA");
    auto* a2 = builder.addAreaToStudy("ZREA");
    auto link = AreaAddLinkBetweenAreas(a1, a2);
    auto study = std::move(builder.study);
    study->initializeRuntimeInfos();
    Implementation::SingleProblemGetter getter({std::move(study), nullptr});

    auto output_dir = std::filesystem::temp_directory_path() / "study" / "output";
    getter.writeStudyDescriptionFiles(output_dir);

    // interco-1-1.txt
    {
        std::ifstream interco(output_dir / "interco-1-1.txt");
        BOOST_REQUIRE(interco);
        std::string content((std::istreambuf_iterator<char>(interco)),
                            std::istreambuf_iterator<char>());
        const std::string expected = "0 0 1\n";
        BOOST_CHECK_EQUAL(content, expected);
    }

    // area-1-1.txt
    {
        std::ifstream areas(output_dir / "area-1-1.txt");
        BOOST_REQUIRE(areas);
        std::string content((std::istreambuf_iterator<char>(areas)),
                            std::istreambuf_iterator<char>());
        const std::string expected = "area\nzrea\n";
        BOOST_CHECK_EQUAL(content, expected);
    }
}

BOOST_AUTO_TEST_CASE(about_the_study_directory_structure)
{
    StudyBuilder builder;

    auto* a1 = builder.addAreaToStudy("AREA1");
    auto* a2 = builder.addAreaToStudy("AREA2");
    auto link = AreaAddLinkBetweenAreas(a1, a2);

    auto study = std::move(builder.study);
    study->initializeRuntimeInfos();

    // Create the folderSettings directory structure with generaldata.ini
    auto folderSettings = std::filesystem::temp_directory_path() / "study" / "settings";

    // In-memory studies normally have no input files
    study->folderSettings = folderSettings;

    std::filesystem::create_directories(folderSettings);
    std::ofstream settingsFile(folderSettings / "generaldata.ini");
    settingsFile << "; Test settings file\n";
    settingsFile << "general.mode = 0\n";
    settingsFile.close();

    Implementation::SingleProblemGetter getter({std::move(study), nullptr});

    auto output_dir = std::filesystem::temp_directory_path() / "study" / "output";
    getter.writeStudyDescriptionFiles(output_dir);

    // Verify about-the-study directory exists
    BOOST_CHECK(std::filesystem::exists(output_dir / "about-the-study"));
    BOOST_CHECK(std::filesystem::is_directory(output_dir / "about-the-study"));

    // Verify key files exist
    BOOST_CHECK(std::filesystem::exists(output_dir / "about-the-study" / "study.ini"));
    BOOST_CHECK(std::filesystem::exists(output_dir / "about-the-study" / "parameters.ini"));
    BOOST_CHECK(std::filesystem::exists(output_dir / "info.antares-output"));
}

BOOST_AUTO_TEST_CASE(about_the_study_area_names)
{
    StudyBuilder builder;

    auto* a1 = builder.addAreaToStudy("POWER_PLANT_A");
    auto* a2 = builder.addAreaToStudy("GRID_B");
    auto* a3 = builder.addAreaToStudy("INDUSTRIAL_ZONE_C");

    auto study = std::move(builder.study);
    study->initializeRuntimeInfos();
    Implementation::SingleProblemGetter getter({std::move(study), nullptr});

    auto output_dir = std::filesystem::temp_directory_path() / "study" / "output";
    getter.writeStudyDescriptionFiles(output_dir);

    // Check areas.txt contains all area names
    std::ifstream areas(output_dir / "about-the-study" / "areas.txt");
    BOOST_REQUIRE(areas);
    std::string content((std::istreambuf_iterator<char>(areas)), std::istreambuf_iterator<char>());

    BOOST_CHECK(content.find("POWER_PLANT_A") != std::string::npos);
    BOOST_CHECK(content.find("GRID_B") != std::string::npos);
    BOOST_CHECK(content.find("INDUSTRIAL_ZONE_C") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(about_the_study_links_content)
{
    StudyBuilder builder;

    auto* a1 = builder.addAreaToStudy("AREA_A");
    auto* a2 = builder.addAreaToStudy("AREA_B");
    auto link = AreaAddLinkBetweenAreas(a1, a2);

    auto study = std::move(builder.study);
    study->initializeRuntimeInfos();
    Implementation::SingleProblemGetter getter({std::move(study), nullptr});

    auto output_dir = std::filesystem::temp_directory_path() / "study" / "output";
    getter.writeStudyDescriptionFiles(output_dir);

    // Check links.txt contains link information
    std::ifstream links(output_dir / "about-the-study" / "links.txt");
    BOOST_REQUIRE(links);
    std::string content((std::istreambuf_iterator<char>(links)), std::istreambuf_iterator<char>());

    BOOST_CHECK(content.find("area_a") != std::string::npos);
    BOOST_CHECK(content.find("area_b") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(about_the_study_parameters_file)
{
    StudyBuilder builder;
    builder.simulationBetweenDays(0, 7);
    builder.setNumberMCyears(1);

    auto* area = builder.addAreaToStudy("TEST_AREA");
    area->hydro.reservoirManagement = false;

    auto study = std::move(builder.study);
    study->initializeRuntimeInfos();

    // Create the folderSettings directory structure with generaldata.ini
    auto folderSettings = std::filesystem::temp_directory_path() / "study" / "settings";
    std::filesystem::create_directories(folderSettings);
    std::ofstream settingsFile(folderSettings / "generaldata.ini");
    settingsFile << "; Test settings file\n";
    settingsFile << "general.mode = 0\n";
    settingsFile.close();

    Implementation::SingleProblemGetter getter({std::move(study), nullptr});

    auto output_dir = std::filesystem::temp_directory_path() / "study" / "output";
    getter.writeStudyDescriptionFiles(output_dir);

    // Check parameters.ini file exists and is readable
    std::ifstream params(output_dir / "about-the-study" / "parameters.ini");
    BOOST_REQUIRE(params);
    std::string content((std::istreambuf_iterator<char>(params)), std::istreambuf_iterator<char>());

    // Parameters file should not be empty
    BOOST_CHECK(!content.empty());
}

BOOST_AUTO_TEST_CASE(weeks_independent_no_reservoir_management)
{
    // Test case: No areas with reservoir management
    // Expected: weeks are independent (returns true)
    auto study = buildStudy(true, false);
    Implementation::SingleProblemGetter getter({std::move(study), nullptr});
    BOOST_CHECK_EQUAL(getter.areWeeksIndependent(), true);
}

BOOST_AUTO_TEST_CASE(weeks_independent_with_heuristic_and_no_leeway)
{
    // Test case: All reservoir-managed areas have useHeuristicTarget=true and useLeeway=false
    // Expected: weeks are independent (returns true)
    auto study = buildStudy(false, true);
    auto* area = study->areas.byIndex[0];
    area->hydro.useHeuristicTarget = true;
    area->hydro.useLeeway = false;

    Implementation::SingleProblemGetter getter({std::move(study), nullptr});
    BOOST_CHECK_EQUAL(getter.areWeeksIndependent(), true);
}

BOOST_AUTO_TEST_CASE(weeks_not_independent_with_leeway)
{
    // Test case: Reservoir-managed area has useLeeway=true
    // Expected: weeks are NOT independent (returns false)
    auto study = buildStudy(false, true);
    auto* area = study->areas.byIndex[0];
    area->hydro.useHeuristicTarget = true;
    area->hydro.useLeeway = true;

    Implementation::SingleProblemGetter getter({std::move(study), nullptr});
    BOOST_CHECK_EQUAL(getter.areWeeksIndependent(), false);
}

BOOST_AUTO_TEST_CASE(weeks_not_independent_without_heuristic_target)
{
    // Test case: Reservoir-managed area has useHeuristicTarget=false
    // Expected: weeks are NOT independent (returns false)
    auto study = buildStudy(false, true);
    auto* area = study->areas.byIndex[0];
    area->hydro.useHeuristicTarget = false;
    area->hydro.useLeeway = false;

    Implementation::SingleProblemGetter getter({std::move(study), nullptr});
    BOOST_CHECK_EQUAL(getter.areWeeksIndependent(), false);
}

BOOST_AUTO_TEST_CASE(weeks_not_independent_with_both_conditions_false)
{
    // Test case: Reservoir-managed area has both useHeuristicTarget=false and useLeeway=true
    // Expected: weeks are NOT independent (returns false)
    auto study = buildStudy(false, true);
    auto* area = study->areas.byIndex[0];
    area->hydro.useHeuristicTarget = false;
    area->hydro.useLeeway = true;

    Implementation::SingleProblemGetter getter({std::move(study), nullptr});
    BOOST_CHECK_EQUAL(getter.areWeeksIndependent(), false);
}

BOOST_AUTO_TEST_CASE(weeks_independent_multiple_areas_all_compliant)
{
    // Test case: Multiple areas, all reservoir-managed, all with correct settings
    // Expected: weeks are independent (returns true)
    StudyBuilder builder;
    builder.simulationBetweenDays(0, 14);
    builder.setNumberMCyears(2);

    // Add area 1 with proper hydro settings
    auto* area1 = builder.addAreaToStudy("AREA1");
    area1->hydro.reservoirManagement = true;
    auto& h1 = area1->hydro;
    TimeSeriesConfigurer genP1(h1.series->maxHourlyGenPower);
    genP1.setDimensions(1).fillColumnWith(0, 100.);
    TimeSeriesConfigurer hydroStorage1(h1.series->storage);
    hydroStorage1.setDimensions(1, DAYS_PER_YEAR).fillColumnWith(0, 10.);
    TimeSeriesConfigurer genE1(h1.dailyNbHoursAtGenPmax);
    genE1.setDimensions(1, DAYS_PER_YEAR).fillColumnWith(0, 24);
    h1.reservoirCapacity = 1e4;
    h1.prepro.release();
    area1->load.prepro.release();
    TimeSeriesConfigurer loadTSconfig1(area1->load.series);
    loadTSconfig1.setDimensions(1).fillColumnWith(0, 120.);

    // Add area 2 with proper hydro settings
    auto* area2 = builder.addAreaToStudy("AREA2");
    area2->hydro.reservoirManagement = true;
    auto& h2 = area2->hydro;
    TimeSeriesConfigurer genP2(h2.series->maxHourlyGenPower);
    genP2.setDimensions(1).fillColumnWith(0, 100.);
    TimeSeriesConfigurer hydroStorage2(h2.series->storage);
    hydroStorage2.setDimensions(1, DAYS_PER_YEAR).fillColumnWith(0, 10.);
    TimeSeriesConfigurer genE2(h2.dailyNbHoursAtGenPmax);
    genE2.setDimensions(1, DAYS_PER_YEAR).fillColumnWith(0, 24);
    h2.reservoirCapacity = 1e4;
    h2.prepro.release();
    area2->load.prepro.release();
    TimeSeriesConfigurer loadTSconfig2(area2->load.series);
    loadTSconfig2.setDimensions(1).fillColumnWith(0, 120.);

    builder.study->initializeRuntimeInfos();
    area1->hydro.deltaBetweenFinalAndInitialLevels.resize(builder.study->parameters.nbYears);
    area2->hydro.deltaBetweenFinalAndInitialLevels.resize(builder.study->parameters.nbYears);

    // Set both areas with compliant settings
    area1->hydro.useHeuristicTarget = true;
    area1->hydro.useLeeway = false;
    area2->hydro.useHeuristicTarget = true;
    area2->hydro.useLeeway = false;

    Implementation::SingleProblemGetter getter({std::move(builder.study), nullptr});
    BOOST_CHECK_EQUAL(getter.areWeeksIndependent(), true);
}

BOOST_AUTO_TEST_CASE(weeks_not_independent_multiple_areas_one_non_compliant)
{
    // Test case: Multiple areas, one with non-compliant settings
    // Expected: weeks are NOT independent (returns false)
    StudyBuilder builder;
    builder.simulationBetweenDays(0, 14);
    builder.setNumberMCyears(2);

    // Add area 1 with proper hydro settings
    auto* area1 = builder.addAreaToStudy("AREA1");
    area1->hydro.reservoirManagement = true;
    auto& h1 = area1->hydro;
    TimeSeriesConfigurer genP1(h1.series->maxHourlyGenPower);
    genP1.setDimensions(1).fillColumnWith(0, 100.);
    TimeSeriesConfigurer hydroStorage1(h1.series->storage);
    hydroStorage1.setDimensions(1, DAYS_PER_YEAR).fillColumnWith(0, 10.);
    TimeSeriesConfigurer genE1(h1.dailyNbHoursAtGenPmax);
    genE1.setDimensions(1, DAYS_PER_YEAR).fillColumnWith(0, 24);
    h1.reservoirCapacity = 1e4;
    h1.prepro.release();
    area1->load.prepro.release();
    TimeSeriesConfigurer loadTSconfig1(area1->load.series);
    loadTSconfig1.setDimensions(1).fillColumnWith(0, 120.);

    // Add area 2 with proper hydro settings
    auto* area2 = builder.addAreaToStudy("AREA2");
    area2->hydro.reservoirManagement = true;
    auto& h2 = area2->hydro;
    TimeSeriesConfigurer genP2(h2.series->maxHourlyGenPower);
    genP2.setDimensions(1).fillColumnWith(0, 100.);
    TimeSeriesConfigurer hydroStorage2(h2.series->storage);
    hydroStorage2.setDimensions(1, DAYS_PER_YEAR).fillColumnWith(0, 10.);
    TimeSeriesConfigurer genE2(h2.dailyNbHoursAtGenPmax);
    genE2.setDimensions(1, DAYS_PER_YEAR).fillColumnWith(0, 24);
    h2.reservoirCapacity = 1e4;
    h2.prepro.release();
    area2->load.prepro.release();
    TimeSeriesConfigurer loadTSconfig2(area2->load.series);
    loadTSconfig2.setDimensions(1).fillColumnWith(0, 120.);

    builder.study->initializeRuntimeInfos();
    area1->hydro.deltaBetweenFinalAndInitialLevels.resize(builder.study->parameters.nbYears);
    area2->hydro.deltaBetweenFinalAndInitialLevels.resize(builder.study->parameters.nbYears);

    // Set area 1 with compliant settings
    area1->hydro.useHeuristicTarget = true;
    area1->hydro.useLeeway = false;

    // Set area 2 with NON-compliant settings (useLeeway=true)
    area2->hydro.useHeuristicTarget = true;
    area2->hydro.useLeeway = true;

    Implementation::SingleProblemGetter getter({std::move(builder.study), nullptr});
    BOOST_CHECK_EQUAL(getter.areWeeksIndependent(), false);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(WithModelerData)

ModelerData OneParameterOneVariableOneConstraint()
{
    std::vector<Model> models;
    //---
    std::vector<ModelerStudy::SystemModel::Variable> variables;
    Expressions::Registry<Node> lb_registry;
    auto* lb = lb_registry.create<LiteralNode>(0);
    Expressions::NodeRegistry lb_nodeRegistry(lb, (std::move(lb_registry)));
    Expressions::Registry<Node> ub_registry;
    auto* ub = ub_registry.create<LiteralNode>(55);
    Expressions::NodeRegistry ub_nodeRegistry(ub, (std::move(ub_registry)));

    ModelerStudy::SystemModel::Variable x("x",
                                          Expression("0", std::move(lb_nodeRegistry)),
                                          Expression("55", std::move(ub_nodeRegistry)),
                                          ValueType::FLOAT,
                                          TimeDependent::NO,
                                          ScenarioDependent::NO,
                                          Config::Location::MASTER_AND_SUBPROBLEMS);
    variables.push_back(std::move(x));
    //---
    std::vector<Constraint> constraints;
    Expressions::Registry<Node> constraint_registry;
    auto* x_node = constraint_registry
                     .create<VariableNode>("x", 0, VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
    auto* thirteen = constraint_registry.create<LiteralNode>(13.);

    auto* constraint_node = constraint_registry
                              .create<Expressions::Nodes::LessThanOrEqualNode>(x_node, thirteen);
    Expressions::NodeRegistry constraint_nodeRegistry(constraint_node,
                                                      (std::move(constraint_registry)));

    constraints.emplace_back("constraint",
                             Expression("x < 13", std::move(constraint_nodeRegistry)),
                             Config::Location::MASTER_AND_SUBPROBLEMS);
    auto modelWithParameters = ModelBuilder()
                                 .withId("model")
                                 .withParameters(
                                   {Parameter{"P1", TimeDependent::NO, ScenarioDependent::NO}})
                                 .withVariables(std::move(variables))
                                 .withConstraints(std::move(constraints))
                                 .build();
    models.push_back(std::move(modelWithParameters));
    //---
    std::vector<Library> libraries;
    libraries.emplace_back(
      LibraryBuilder().withId("library").withModels(std::move(models)).build());
    std::map<std::string, ParameterTypeAndValue> parameterValues = {
      {"P1",
       {.id = "id", .type = VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO, .value = "11.03"}}};

    //---
    std::vector<Component> components;
    components.emplace_back(ComponentBuilder()
                              .withId("component")
                              .withIndex(0)
                              .withModel(&libraries.at(0).Models().at("model"))
                              .withParameterValues(parameterValues)
                              .withScenarioGroupId("scenario_group")
                              .build());
    auto system = std::make_unique<ModelerStudy::SystemModel::System>(
      SystemBuilder().withId("system").withComponents(std::move(components)).build());
    std::unique_ptr<LinearProblemApi::ILinearProblemData>
      linearProblemData = std::make_unique<LinearProblemDataImpl::LinearProblemData>(
        LinearProblemDataImpl::DataSeriesRepository{});
    //---
    std::unique_ptr<LinearProblemApi::IScenario>
      scenario = std::make_unique<LinearProblemDataImpl::Scenario>("scenario_group");
    ScenarioGroupRepository scenarioGroupRepository;
    scenarioGroupRepository.addScenario("scenario_group", std::move(scenario));
    //---
    return {
      .libraries = libraries,
      .system = (std::move(system)),
      .dataSeries = std::move(linearProblemData),
      .scenarioGroupRepository = std::move(scenarioGroupRepository),
      .resolutionMode = ResolutionMode::BENDERS_DECOMPOSITION,
      .bendersDecomposition = {},
    };
}

void checkFiles(const std::map<std::string, std::string, std::less<>>& outputs)
{
    // the study has 4 sub-problems mps (2 years x 2 weeks) + structure.txt + master.mps
    BOOST_CHECK_EQUAL(outputs.size(), 6);
    // the master has one variable and one constraint

    static constexpr std::string_view master = R"(* Antares Simulator MPSGenerator
* Number of variables: 1
* Number of constraints: 1
NAME master
ROWS
    N  OBJ
    L  component.constraint
COLUMNS
    component.x  component.constraint  1
RHS
    RHS1  component.constraint  13
RANGES
BOUNDS
    UP BND1 component.x 55
ENDATA
)";
    BOOST_CHECK_EQUAL(outputs.at("master.mps"), master);

    static constexpr std::string_view structure = R"(master	component.x	0
problem-1-1--optim-nb-1	component.x	1008
problem-1-2--optim-nb-1	component.x	1008
problem-2-1--optim-nb-1	component.x	1008
problem-2-2--optim-nb-1	component.x	1008
)";
    BOOST_CHECK_EQUAL(outputs.at("structure.txt"), structure);
}

void checkMasterProblem(const Implementation::SingleProblemGetter& getter)
{
    auto [masterPb, _] = getter.getMasterProblem();
    BOOST_CHECK_EQUAL(masterPb->constraintCount(), 1);
    BOOST_CHECK_EQUAL(masterPb->variableCount(), 1);
    const auto& x = masterPb->getVariables().at(0);
    BOOST_CHECK_EQUAL(x->getName(), "component.x");
    BOOST_CHECK_EQUAL(x->isInteger(), false);
    BOOST_CHECK_EQUAL(x->getLb(), 0);
    BOOST_CHECK_EQUAL(x->getUb(), 55);

    const auto& c = masterPb->getConstraints().at(0);
    BOOST_CHECK_EQUAL(c->getName(), "component.constraint");
    BOOST_CHECK_EQUAL(c->getLb(), -masterPb->infinity());
    BOOST_CHECK_EQUAL(c->getUb(), 13);
    BOOST_CHECK_EQUAL(c->getCoefficient(x.get()), 1);
}

void checkSubProblems(Implementation::SingleProblemGetter& getter)
{
    for (auto id: getter.getProblemIds())
    {
        auto weekly = getter.getWeeklyProblem(id);
        // the last variable is the one from the modeler
        const auto& x = weekly->getVariables().back();
        BOOST_CHECK_EQUAL(x->getName(), "component.x");
        BOOST_CHECK_EQUAL(x->isInteger(), false);
        BOOST_CHECK_EQUAL(x->getLb(), 0);
        BOOST_CHECK_EQUAL(x->getUb(), 55);
        // the last constraint is the one from the modeler
        const auto& c = weekly->getConstraints().back();
        BOOST_CHECK_EQUAL(c->getName(), "component.constraint");
        BOOST_CHECK_EQUAL(c->getLb(), -weekly->infinity());
        BOOST_CHECK_EQUAL(c->getUb(), 13);
        BOOST_CHECK_EQUAL(c->getCoefficient(x.get()), 1);
    }
}

void checkProblems(Implementation::SingleProblemGetter& getter)
{
    checkMasterProblem(getter);
    checkSubProblems(getter);
}

BOOST_AUTO_TEST_CASE(simple_model_one_component)
{
    StudyBuilder builder;

    auto study = buildStudy(true, true);

    study->setModelerData(std::make_unique<ModelerData>(OneParameterOneVariableOneConstraint()));
    auto queueService = std::make_shared<Yuni::Job::QueueService>();
    Benchmarking::DurationCollector durationCollector;
    auto resultWriter = resultWriterFactory(Data::ResultFormat::inMemory,
                                            "",
                                            queueService,
                                            durationCollector);
    Implementation::SingleProblemGetter getter({std::move(study), resultWriter});
    //-- check mps and structure.txt
    getter.printProblems();
    auto* writer = dynamic_cast<InMemoryWriter*>(resultWriter.get());
    auto& outputs = writer->getMap();
    checkFiles(outputs);
    //-- check problem in memory
    checkProblems(getter);
}
BOOST_AUTO_TEST_SUITE_END()
