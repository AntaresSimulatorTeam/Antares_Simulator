// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE testE2EModeler
#include <chrono>
#include <fmt/format.h>
#include <iostream>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/ILoader.h>
#include <antares/solver/modeler/Modeler.h>
#include <antares/study/system-model-base/variabilityType.h>
#include "antares/expressions/nodes/GreaterThanOrEqualNode.h"
#include "antares/optimisation/linear-problem-api/mipSolution.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"
#include "antares/solver/modeler/IWriter.h"

#include "inmemory-modeler.h"

using namespace Antares::Expressions;
using namespace Antares::Solver;
using namespace Antares::Optimisation;
using PTV = ParameterTypeAndValue;
using VV = VariabilityType;

class ConstantDataSeries: public LinearProblemApi::ILinearProblemData
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

class DefaultScenario final: public LinearProblemApi::IScenario
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
        fixture.createModelWithOneFloatVar("some_model",
                                           parameterIds,
                                           "x",
                                           lower_bound,
                                           fixture.literal(10),
                                           {{"ct1", ct_node}},
                                           objective,
                                           timeDependent);

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
        setComponents(fixture.components); // Component model may not be the system model
        SystemBuilder builder;
        auto system = builder.withId("dummy-system").withComponents(std::move(components)).build();
        ModelerData md;
        md.libraries = {library};
        md.system = std::make_unique<System>(std::move(system));
        md.dataSeries = std::move(data);
        md.scenarioGroupRepository = std::move(scenarioGroupRepository);

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
        auto scenario = std::make_unique<LinearProblemDataImpl::Scenario>(str);
        scenario->setTimeSerieNumber(year, timeSeriesNumber);
        scenarioGroupRepository.addScenario(str, std::move(scenario));
    }

    Models models;
    std::vector<Component> components;
    Test::Modeler::LinearProblemBuildingFixture fixture;
    std::unique_ptr<LinearProblemApi::ILinearProblemData>
      data = std::make_unique<ConstantDataSeries>(0.);
    Nodes::Node* lower_bound = fixture.literal(0.0);
    bool timeDependent{false};
    std::map<std::string, PTV> parameters{};
    std::vector<std::string> parameterIds{};
    ScenarioGroupRepository scenarioGroupRepository{};
    std::unordered_map<std::string, std::string> groupes;
    std::pair<unsigned int, unsigned int> timeSteps{0, 0};
};

struct Solution
{
    double objectiveValue{0.0};
};

class InMemoryWriter final: public IWriter
{
public:
    mutable Solution solution_{};

    void init(const std::string&) override
    {
        // No initialization needed for in-memory writer
    }

    const std::filesystem::path& outputPath() const override
    {
        static std::filesystem::path dummy;
        return dummy;
    }

    void writeSimulationTable(const LinearProblemApi::ILinearProblem& /*linearProblem*/,
                              const LinearProblemApi::IMipSolution& solution,
                              const ModelerData& /*modelerData*/,
                              const OptimEntityContainer& /*variableContainer*/,
                              const LinearProblemApi::FillContext& /*fillContext*/,
                              const std::string& /*simulationTableName*/) const override
    {
        solution_.objectiveValue = solution.getObjectiveValue();
    }
};

BOOST_AUTO_TEST_CASE(Minimal_system_minimize_to_0)
{
    InMemoryLoader inMemoryLoader;
    InMemoryWriter inMemoryWriter;

    Modeler modeler(inMemoryLoader, inMemoryWriter);
    modeler.run();
    BOOST_CHECK_EQUAL(inMemoryWriter.solution_.objectiveValue, 0);
}

BOOST_AUTO_TEST_CASE(system_with_one_constant_serie_value_10)
{
    InMemoryLoader inMemoryLoader;
    inMemoryLoader.timeDependent = true;
    inMemoryLoader.setLowerBoundToParameter("paramA");
    inMemoryLoader.addParameter("paramA");

    inMemoryLoader.data = std::make_unique<ConstantDataSeries>(5);

    InMemoryWriter inMemoryWriter;

    Modeler modeler(inMemoryLoader, inMemoryWriter);
    modeler.run();
    BOOST_CHECK_EQUAL(inMemoryWriter.solution_.objectiveValue, 5);
}

struct TSDimensions
{
    int nRows{1};
    int nCols{1};
};

LinearProblemDataImpl::TimeSeriesSet constantTimeSeriesSets(const std::string& id,
                                                            std::span<double> values,
                                                            unsigned int nRows = 1)
{
    LinearProblemDataImpl::TimeSeriesSet timeSeriesSet(id, nRows);
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

LinearProblemDataImpl::TimeSeriesSet constantTimeSeriesSet(const std::string& id,
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

    LinearProblemDataImpl::DataSeriesRepository data_series_repository;
    std::vector<double> values = {2, 3, 4};
    data_series_repository.addDataSeries(std::make_unique<LinearProblemDataImpl::TimeSeriesSet>(
      constantTimeSeriesSets("GROUPA", values, 1)));
    inMemoryLoader.data = std::make_unique<LinearProblemDataImpl::LinearProblemData>(
      std::move(data_series_repository));

    InMemoryWriter inMemoryWriter;

    Modeler modeler(inMemoryLoader, inMemoryWriter);
    modeler.run();
    BOOST_CHECK_EQUAL(inMemoryWriter.solution_.objectiveValue, 2);
}

BOOST_AUTO_TEST_CASE(system_with_three_time_series_use_second_one_all_3)
{
    InMemoryLoader inMemoryLoader;
    inMemoryLoader.timeDependent = true;
    inMemoryLoader.setLowerBoundToParameter("paramA");
    inMemoryLoader.addParameter("paramA");

    LinearProblemDataImpl::DataSeriesRepository data_series_repository;
    std::vector<double> values = {2, 3, 4};
    data_series_repository.addDataSeries(std::make_unique<LinearProblemDataImpl::TimeSeriesSet>(
      constantTimeSeriesSets("GROUPA", values, 1)));
    inMemoryLoader.data = std::make_unique<LinearProblemDataImpl::LinearProblemData>(
      std::move(data_series_repository));

    inMemoryLoader.addScenario("GROUPA", 0, 2); // Year 0, timeseriesNumber 1
    inMemoryLoader.groupes["some_component"] = "GROUPA";

    InMemoryWriter inMemoryWriter;

    Modeler modeler(inMemoryLoader, inMemoryWriter);
    modeler.run();
    BOOST_CHECK_EQUAL(inMemoryWriter.solution_.objectiveValue, 3);
}

class ScalingWriter final: public IWriter
{
public:
    ScalingWriter() = default;

    void init(const std::string&) override
    {
    }

    const std::filesystem::path& outputPath() const override
    {
        static std::filesystem::path dummy;
        return dummy;
    }

    void writeSimulationTable(const LinearProblemApi::ILinearProblem& linearProblem,
                              const LinearProblemApi::IMipSolution& solution,
                              const ModelerData& modelerData,
                              const OptimEntityContainer& variableContainer,
                              const LinearProblemApi::FillContext& fillContext,
                              const std::string& simulationTableName) const override
    {
        (void)linearProblem;
        (void)solution;
        (void)modelerData;
        (void)variableContainer;
        (void)fillContext;
        (void)simulationTableName;
    }
};

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
    ScalingWriter writer;

    auto start_total = std::chrono::high_resolution_clock::now();
    Modeler modeler(loader, writer);
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
