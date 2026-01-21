// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test_api
#define WIN32_LEAN_AND_MEAN

#include <fstream>
#include <memory>

#include <boost/test/unit_test.hpp>

#include "antares/antares/constants.h"
#include "antares/study/study.h"

#include "in-memory-study.h"
#include "singleProblemGetterImpl.h"

constexpr double EPSILON = 1.e-6;

std::size_t findIndex(const std::vector<std::string>& v, const std::string& value)
{
    std::size_t ret = std::distance(v.begin(), std::find(v.begin(), v.end(), value));
    BOOST_REQUIRE_MESSAGE(ret < v.size(), value + " not found");
    return ret;
}

std::unique_ptr<Antares::Data::Study> buildStudy(bool thermal, bool hydro)
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
    // calls addScratchpadToEachArea and initializeRuntimeInfos
    // auto study = builder.run();
    builder.study->initializeRuntimeInfos();
    addScratchpadToEachArea(*builder.study);

    // TODO this is HORRIBLE
    // more specifically, this resize is usually done when loading from files. It's all good, except
    // when you DON'T LOAD FILES.
    area->hydro.deltaBetweenFinalAndInitialLevels.resize(builder.study->parameters.nbYears);
    return std::move(builder.study);
}
BOOST_AUTO_TEST_SUITE(in_memory_check_problem_contents)

BOOST_AUTO_TEST_CASE(single_problem_thermal_first_week_nominal_case)
{
    auto study = buildStudy(true, false);
    Antares::Solver::Implementation::SingleProblemGetter getter(std::move(study));
    const Antares::Solver::ConstantDataFromAntares constantData = getter.getConstantData();
    // 504 = 3*168, 3 sets of variables
    // unsupplied energy
    // spilled energy
    // dispatchable production
    BOOST_CHECK_EQUAL(constantData.VariablesCount, 504);
    // 336 = 2*168
    // area balance
    // fictive loads
    BOOST_CHECK_EQUAL(constantData.ConstraintesCount, 336);

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

    const Antares::Solver::WeeklyDataFromAntares firstWeekData = getter.getWeeklyData({0, 1});
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

    BOOST_CHECK_EQUAL(firstWeekData.Xmax[dispatchableVariable], 102.);
    BOOST_CHECK_EQUAL(firstWeekData.Xmax[unsuppliedVariable],
                      1.e-5); // default value when there is no residual load
    BOOST_CHECK_EQUAL(firstWeekData.Xmax[spilledVariable], 1.e80); // infinite
}

BOOST_AUTO_TEST_CASE(single_problem_hydro_two_weeks_nominal_case)
{
    auto study = buildStudy(false, true);
    Antares::Solver::Implementation::SingleProblemGetter getter(std::move(study));
    const Antares::Solver::ConstantDataFromAntares constantData = getter.getConstantData();
    // Total 1008
    // 168 unsupplied energy
    // 168 spilled energy
    // 168 hydro level
    // 168 hydro prod
    // 168 overflow
    BOOST_CHECK_EQUAL(constantData.VariablesCount, 840);
    // Total 505
    // 168 area balance
    // 168 fictive loads
    // 168 hydro level
    // 1 hydro power
    BOOST_CHECK_EQUAL(constantData.ConstraintesCount, 505);

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

    const Antares::Solver::WeeklyDataFromAntares firstWeekData = getter.getWeeklyData({0, 1});
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

    const Antares::Solver::WeeklyDataFromAntares secondWeekData = getter.getWeeklyData({0, 2});
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

    const Antares::Solver::Implementation::SingleProblemGetter getter(std::move(study));
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

    const Antares::Solver::Implementation::SingleProblemGetter getter(std::move(study));
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

    const Antares::Solver::Implementation::SingleProblemGetter getter(std::move(study));
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
    Antares::Solver::Implementation::SingleProblemGetter getter(std::move(study));

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
    Antares::Solver::Implementation::SingleProblemGetter getter(std::move(study));

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

    Antares::Solver::Implementation::SingleProblemGetter getter(std::move(study));

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
    Antares::Solver::Implementation::SingleProblemGetter getter(std::move(study));

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
    Antares::Solver::Implementation::SingleProblemGetter getter(std::move(study));

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

    Antares::Solver::Implementation::SingleProblemGetter getter(std::move(study));

    auto output_dir = std::filesystem::temp_directory_path() / "study" / "output";
    getter.writeStudyDescriptionFiles(output_dir);

    // Check parameters.ini file exists and is readable
    std::ifstream params(output_dir / "about-the-study" / "parameters.ini");
    BOOST_REQUIRE(params);
    std::string content((std::istreambuf_iterator<char>(params)), std::istreambuf_iterator<char>());

    // Parameters file should not be empty
    BOOST_CHECK(!content.empty());
}

BOOST_AUTO_TEST_SUITE_END()
