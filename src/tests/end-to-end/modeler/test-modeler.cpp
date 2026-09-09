// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE testE2EModeler
#include <chrono>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/ILoader.h>
#include <antares/solver/modeler/Modeler.h>
#include <antares/study/system-model-base/variabilityType.h>
#include "antares/expressions/nodes/GreaterThanOrEqualNode.h"
#include "antares/io/outputs/SimulationTable.h"
#include "antares/optimisation/linear-problem-api/mipSolution.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"

#include "inmemory-modeler.h"

using namespace Antares::Expressions;
using namespace Antares::Solver;
using namespace Antares::LinearProblem;
using namespace Antares::IO::Outputs;
using PTV = ParameterTypeAndValue;
using VV = VariabilityType;

class ConstantDataSeries: public Api::ILinearProblemData
{
public:
    explicit ConstantDataSeries(double value):
        value_(value)
    {
    }

    double getData(const std::string& /*dataSetId*/,
                   unsigned /*year*/,
                   unsigned /*hour*/) const override
    {
        return value_;
    }

    [[nodiscard]] std::span<const double> getData(const std::string& /*dataSetId*/,
                                                  unsigned /*timeSeriesNumber*/,
                                                  unsigned /*firstHour*/,
                                                  unsigned /*lastHour*/) const override

    {
        return vector_;
    }

private:
    double value_{0.};
    std::vector<double> vector_{value_};
};

ConstantDataSeries emptyDataSeries(0.);

Component copyComponent(const Component& c)
{
    ComponentBuilder builder;
    return builder.withId(c.Id())
      .withModel(c.getModel())
      .withScenarioGroupId(c.getScenarioGroupId())
      .withParameterValues(c.getParameterValues())
      .build();
}

class DefaultScenario final: public Api::IScenario
{
public:
    using IScenario::IScenario;

    [[nodiscard]] TimeSeriesNumber getData(Year) const override
    {
        return 1; // Default rank for empty groupId
    }
};

using Models = std::unordered_map<std::string, Model>;

class InMemoryLoader final: public ILoader
{
public:
    ModelerParameters loadParameters() override
    {
        return {.solver = "sirius",
                .solverLogs = false,
                .solverParameters = "DUMMY",
                .noOutput = false,
                .firstTimeStep = timeSteps.first,
                .lastTimeStep = timeSteps.second};
    }

    std::optional<ModelerData> loadAll() override
    {
        auto objective = fixture.variable("x", 0);
        auto var_node = fixture.variable("x", 0);
        auto zero = fixture.literal(0);
        auto ct_node = fixture.nodeRegistry.template create<Nodes::GreaterThanOrEqualNode>(var_node,
                                                                                           zero);
        if (masterOnly)
        {
            // Variable located in the master problem only: subproblem builds return null.
            fixture.createModel("some_model",
                                parameterIds,
                                {{"x",
                                  ValueType::FLOAT,
                                  lower_bound,
                                  fixture.literal(10),
                                  timeDependent,
                                  false,
                                  Config::Location::MASTER}},
                                {{"ct1", ct_node}},
                                objective);
        }
        else
        {
            fixture.createModelWithOneFloatVar("some_model",
                                               parameterIds,
                                               "x",
                                               lower_bound,
                                               fixture.literal(10),
                                               {{"ct1", ct_node}},
                                               objective,
                                               timeDependent);
        }

        LibraryBuilder library_builder;
        auto&& library = library_builder.withId("dummy-library")
                           .withDescription("")
                           // We should set models in the library
                           // However doing so will move the model, reseting the fixture.models
                           // Also invaliding the component reference to the model
                           //.withModelsMap(std::move(fixture.models))
                           .build();
        auto groupIt = groupes.find("some_component");
        if (groupIt != groupes.end())
        {
            fixture.createComponent("some_model", "some_component", parameters, groupIt->second);
        }
        else
        {
            fixture.createComponent("some_model", "some_component", parameters);
        }
        for (auto& [id, scenario]: pendingScenarios_)
        {
            scenarioGroupRepository.addScenario(id, std::move(scenario));
        }
        setComponents(fixture.components); // Component model may not be the system model
        SystemBuilder builder;
        auto system = builder.withId("dummy-system").withComponents(std::move(components)).build();
        ModelerData md;
        md.libraries = {library};
        md.system = std::make_unique<System>(std::move(system));
        md.dataSeries = std::move(data);
        md.scenarioGroupRepository = std::move(scenarioGroupRepository);
        md.scenarioScope = scenarioScope;

        return md;
    }

    void setComponents(const std::vector<Component>& compos)
    {
        components = compos;
    }

    void setModels(Models&& map)
    {
        models = std::move(map);
    }

    void setLowerBoundToParameter(const std::string& parameterId)
    {
        lower_bound = fixture.parameter(parameterId, VV::VARYING_IN_TIME_ONLY);
    }

    void addParameter(const std::string& str, const VV& type = VV::VARYING_IN_TIME_AND_SCENARIO)
    {
        parameters.emplace(Test::Modeler::build_context_parameter_with(str, "GROUPA", type));
        parameterIds.push_back(str);
    }

    void addScenario(const std::string& str, int year, int timeSeriesNumber)
    {
        auto [it, inserted] = pendingScenarios_.try_emplace(str,
                                                            std::make_unique<DataImpl::Scenario>(
                                                              str));
        it->second->setTimeSerieNumber(year, timeSeriesNumber);
    }

    Models models;
    std::vector<Component> components;
    Test::Modeler::LinearProblemBuildingFixture fixture;
    std::unique_ptr<Api::ILinearProblemData> data = std::make_unique<ConstantDataSeries>(0.);
    Nodes::Node* lower_bound = fixture.literal(0.0);
    bool timeDependent{false};
    bool masterOnly{false};
    std::map<std::string, PTV> parameters{};
    std::vector<std::string> parameterIds{};
    ScenarioGroupRepository scenarioGroupRepository{};
    std::map<std::string, std::unique_ptr<DataImpl::Scenario>> pendingScenarios_;
    std::unordered_map<std::string, std::string> groupes;
    std::pair<unsigned int, unsigned int> timeSteps{0, 0};
    ScenarioScope scenarioScope{};
};

struct Solution
{
    double objectiveValue{0.0};
};

BOOST_AUTO_TEST_CASE(Minimal_system_minimize_to_0)
{
    InMemoryLoader inMemoryLoader;
    Modeler modeler(inMemoryLoader, {}, TableFormat::CSV);
    modeler.run();
    auto* solution = modeler.subProbSolution();
    BOOST_CHECK_EQUAL(solution->getObjectiveValue(), 0);
}

BOOST_AUTO_TEST_CASE(system_with_one_constant_serie_value_10)
{
    InMemoryLoader inMemoryLoader;
    inMemoryLoader.timeDependent = true;
    inMemoryLoader.setLowerBoundToParameter("paramA");
    inMemoryLoader.addParameter("paramA");

    inMemoryLoader.data = std::make_unique<ConstantDataSeries>(5);

    Modeler modeler(inMemoryLoader, {}, TableFormat::CSV);
    modeler.run();
    auto* solution = modeler.subProbSolution();
    BOOST_CHECK_EQUAL(solution->getObjectiveValue(), 5);
}

struct TSDimensions
{
    int nRows{1};
    int nCols{1};
};

DataImpl::TimeSeriesSet constantTimeSeriesSets(const std::string& id,
                                               std::span<double> values,
                                               unsigned int nRows = 1)
{
    DataImpl::TimeSeriesSet timeSeriesSet(id, nRows);
    for (double value: values)
    {
        if (nRows == 0)
        {
            return timeSeriesSet;
        }
        std::vector<double> set(nRows, value);
        timeSeriesSet.add(set);
    }

    return timeSeriesSet;
}

DataImpl::TimeSeriesSet constantTimeSeriesSet(const std::string& id,
                                              double value = 0.,
                                              TSDimensions dims = {1, 1})
{
    std::vector<double> values(dims.nCols, value);
    return constantTimeSeriesSets(id, values, dims.nRows);
}

BOOST_AUTO_TEST_CASE(system_with_two_time_series_use_default_first_all_2)
{
    InMemoryLoader inMemoryLoader;
    inMemoryLoader.timeDependent = true;
    inMemoryLoader.setLowerBoundToParameter("paramA");
    inMemoryLoader.addParameter("paramA");

    DataImpl::DataSeriesRepository data_series_repository;
    std::vector<double> values = {2, 3, 4};
    data_series_repository.addDataSeries(
      std::make_unique<DataImpl::TimeSeriesSet>(constantTimeSeriesSets("GROUPA", values, 1)));
    inMemoryLoader.data = std::make_unique<DataImpl::LinearProblemData>(
      std::move(data_series_repository));

    Modeler modeler(inMemoryLoader, {}, TableFormat::CSV);
    modeler.run();
    auto* solution = modeler.subProbSolution();
    BOOST_CHECK_EQUAL(solution->getObjectiveValue(), 2);
}

BOOST_AUTO_TEST_CASE(system_with_three_time_series_use_second_one_all_3)
{
    InMemoryLoader inMemoryLoader;
    inMemoryLoader.timeDependent = true;
    inMemoryLoader.setLowerBoundToParameter("paramA");
    inMemoryLoader.addParameter("paramA");

    DataImpl::DataSeriesRepository data_series_repository;
    std::vector<double> values = {2, 3, 4};
    data_series_repository.addDataSeries(
      std::make_unique<DataImpl::TimeSeriesSet>(constantTimeSeriesSets("GROUPA", values, 1)));
    inMemoryLoader.data = std::make_unique<DataImpl::LinearProblemData>(
      std::move(data_series_repository));

    inMemoryLoader.addScenario("GROUPA", 0, 2); // Year 0, timeseriesNumber 1
    inMemoryLoader.groupes["some_component"] = "GROUPA";

    Modeler modeler(inMemoryLoader, {}, TableFormat::CSV);
    modeler.run();
    auto* solution = modeler.subProbSolution();
    BOOST_CHECK_EQUAL(solution->getObjectiveValue(), 3);
}

class ScalingLoader: public ILoader
{
public:
    explicit ScalingLoader(unsigned int timeSteps):
        timeSteps_(timeSteps)
    {
    }

    ModelerParameters loadParameters() override
    {
        return {.solver = "sirius",
                .solverLogs = false,
                .solverParameters = "DUMMY",
                .noOutput = false,
                .firstTimeStep = 0,
                .lastTimeStep = timeSteps_ - 1};
    }

    std::optional<ModelerData> loadAll() override
    {
        auto var_node = fixture.variable("x", 0);

        // GOOD linear scaling for this expression
        auto objective = fixture.add(fixture.Sum(var_node), fixture.Sum(var_node));

        // GOOD quadratic scaling for this expression
        auto lhs = fixture.Sum(fixture.add(var_node, var_node));

        auto zero = fixture.literal(0);
        auto ct_node = fixture.nodeRegistry.template create<Nodes::GreaterThanOrEqualNode>(lhs,
                                                                                           zero);
        fixture.createModelWithOneFloatVar("some_model",
                                           parameterIds,
                                           "x",
                                           lower_bound_,
                                           fixture.literal(10),
                                           {{"ct1", ct_node}},
                                           objective,
                                           true);

        LibraryBuilder library_builder;
        auto&& library = library_builder.withId("dummy-library").withDescription("").build();

        fixture.createComponent("some_model", "some_component", parameters);

        SystemBuilder builder;
        auto system = builder.withId("dummy-system")
                        .withComponents(std::move(fixture.components))
                        .build();
        ModelerData md;
        md.libraries = {library};
        md.system = std::make_unique<System>(std::move(system));
        md.dataSeries = std::make_unique<ConstantDataSeries>(0.);

        return md;
    }

    Test::Modeler::LinearProblemBuildingFixture fixture;
    std::map<std::string, PTV> parameters{};
    std::vector<std::string> parameterIds{};
    Nodes::Node* lower_bound_ = fixture.literal(0.0);
    unsigned int timeSteps_;
};

BOOST_DATA_TEST_CASE(modeler_scaling_by_time_steps,
                     boost::unit_test::data::make({1e3, 1e4, 1e5, 1e6}),
                     nTimeSteps)
{
    ScalingLoader loader(nTimeSteps);

    auto start_total = std::chrono::high_resolution_clock::now();
    Modeler modeler(loader, {}, TableFormat::CSV);
    modeler.run();
    auto end_total = std::chrono::high_resolution_clock::now();

    auto total_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_total
                                                                               - start_total)
                           .count();

    std::cout << "\n========================================" << std::endl;
    std::cout << "Modeler Scaling Test Results:" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Number of time steps: " << nTimeSteps << std::endl;
    std::cout << "Total wall clock time: " << total_time_ms << " ms" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

// The simulation table header is:
// block,component,output,absolute_time_index,block_time_index,scenario_index,value,basis_status
std::vector<unsigned> readScenarioIndexes(const std::filesystem::path& csvPath)
{
    std::vector<unsigned> scenarioIndexes;
    std::ifstream in(csvPath);
    std::string line;
    std::getline(in, line); // skip header
    while (std::getline(in, line))
    {
        if (line.empty())
        {
            continue;
        }
        std::size_t start = 0;
        bool malformed = false;
        for (int column = 0; column < 5; ++column)
        {
            const auto delimiter = line.find(',', start);
            if (delimiter == std::string::npos)
            {
                malformed = true;
                break;
            }
            start = delimiter + 1;
        }
        if (malformed)
        {
            continue;
        }
        const auto end = line.find(',', start);
        const std::string field = line.substr(start,
                                              end == std::string::npos ? std::string::npos
                                                                       : end - start);
        try
        {
            std::size_t consumed = 0;
            const unsigned long long value = std::stoull(field, &consumed);
            if (consumed != field.size() || value > std::numeric_limits<unsigned>::max())
            {
                continue;
            }
            scenarioIndexes.push_back(static_cast<unsigned>(value));
        }
        catch (const std::exception&)
        {
            // Non-numeric or out-of-range scenario index: skip the row.
        }
    }
    return scenarioIndexes;
}

BOOST_AUTO_TEST_CASE(multi_scenario_produces_one_table_per_scenario)
{
    namespace fs = std::filesystem;
    InMemoryLoader inMemoryLoader;
    inMemoryLoader.scenarioScope.include = {"1", "2"};

    const auto outputDir = fs::temp_directory_path() / "antares-modeler-e2e-multi-scenario";
    std::error_code ec;
    fs::remove_all(outputDir, ec);
    fs::create_directories(outputDir, ec);

    Modeler modeler(inMemoryLoader, outputDir, TableFormat::CSV);
    modeler.run();

    // The solution is owned by the last retained subproblem and must stay usable after run().
    BOOST_CHECK(modeler.subProbSolution() != nullptr);
    BOOST_CHECK_EQUAL(modeler.subProbSolution()->getObjectiveValue(), 0);
    BOOST_CHECK(fs::exists(outputDir / "simulation-table-1.csv"));
    BOOST_CHECK(fs::exists(outputDir / "simulation-table-2.csv"));
    BOOST_CHECK(!fs::exists(outputDir / "simulation-table.csv"));

    for (const unsigned scenario: {1, 2})
    {
        const auto indices = readScenarioIndexes(
          outputDir / ("simulation-table-" + std::to_string(scenario) + ".csv"));
        BOOST_CHECK(!indices.empty());
        for (const auto index: indices)
        {
            BOOST_CHECK_EQUAL(index, scenario);
        }
    }

    fs::remove_all(outputDir, ec);
}

// A model whose only variable lives in the master problem yields a null subproblem for
// every scenario; buildProblems() must not dereference those null entries.
BOOST_AUTO_TEST_CASE(build_problems_with_all_null_subproblems_does_not_crash)
{
    InMemoryLoader inMemoryLoader;
    inMemoryLoader.masterOnly = true;

    Modeler modeler(inMemoryLoader, {}, TableFormat::CSV);
    modeler.buildProblems();

    const auto& subproblems = modeler.subproblems();
    BOOST_REQUIRE_EQUAL(subproblems.size(), 1u);
    for (const auto& problem: subproblems)
    {
        BOOST_CHECK(problem == nullptr);
    }
}

BOOST_AUTO_TEST_CASE(validation_reports_component_group_missing_selected_year)
{
    InMemoryLoader inMemoryLoader;
    inMemoryLoader.scenarioScope.include = {"0", "1"};
    inMemoryLoader.addScenario("GROUPA", 0, 0); // no time series for year 1
    inMemoryLoader.groupes["some_component"] = "GROUPA";

    try
    {
        Modeler modeler(inMemoryLoader, {}, TableFormat::CSV);
        BOOST_FAIL("expected a ModelerError");
    }
    catch (const Modeler::ModelerError& e)
    {
        const std::string message = e.what();
        BOOST_CHECK(message.find("GROUPA") != std::string::npos);
        BOOST_CHECK(message.find("some_component") != std::string::npos);
    }
}

BOOST_AUTO_TEST_CASE(validation_passes_when_component_group_covers_selected_years)
{
    InMemoryLoader inMemoryLoader;
    inMemoryLoader.scenarioScope.include = {"0", "1"};
    inMemoryLoader.addScenario("GROUPA", 0, 0);
    inMemoryLoader.addScenario("GROUPA", 1, 1);
    inMemoryLoader.groupes["some_component"] = "GROUPA";

    Modeler modeler(inMemoryLoader, {}, TableFormat::CSV);
    modeler.run();
    BOOST_CHECK(modeler.subProbSolution() != nullptr);
}

BOOST_AUTO_TEST_CASE(read_scenario_indexes_skips_malformed_rows)
{
    namespace fs = std::filesystem;
    const auto csvPath = fs::temp_directory_path() / "antares-modeler-e2e-malformed-csv.csv";
    {
        std::ofstream out(csvPath);
        out << "block,component,output,absolute_time_index,block_time_index,scenario_index,"
               "value,basis_status\n";
        out << "0,c,o,0,0,1,0.0,BASIC\n";                    // valid -> 1
        out << "0,c,o,0,0\n";                                // fewer than 5 delimiters -> skipped
        out << "0,c,o,0,0,abc,0.0,BASIC\n";                  // non-numeric -> skipped
        out << "0,c,o,0,0,1abc,0.0,BASIC\n";                 // trailing garbage -> skipped
        out << "0,c,o,0,0,18446744073709551616,0.0,BASIC\n"; // > ULLONG_MAX -> skipped
        out << "0,c,o,0,0,4294967296,0.0,BASIC\n";           // > UINT_MAX -> skipped
        out << "0,c,o,0,0,2\n";                              // no trailing comma -> 2
    }

    const auto indices = readScenarioIndexes(csvPath);
    std::error_code ec;
    fs::remove(csvPath, ec);

    BOOST_REQUIRE_EQUAL(indices.size(), 2u);
    BOOST_CHECK_EQUAL(indices[0], 1u);
    BOOST_CHECK_EQUAL(indices[1], 2u);
}
