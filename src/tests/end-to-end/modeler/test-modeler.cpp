// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE testE2EModeler
#include <fmt/format.h>

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/ILoader.h>
#include <antares/solver/modeler/Modeler.h>
#include <antares/study/system-model/variabilityType.h>
#include "antares/expressions/nodes/GreaterThanOrEqualNode.h"
#include "antares/optimisation/linear-problem-api/mipSolution.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"
#include "antares/solver/modeler/IWriter.h"

#include "inmemory-modeler.h"

using namespace Antares::Expressions;
using PTV = Antares::ModelerStudy::SystemModel::ParameterTypeAndValue;
using VV = Antares::Optimisation::VariabilityType;

class ConstantDataSeries: public Antares::Optimisation::LinearProblemApi::ILinearProblemData
{
public:
    explicit ConstantDataSeries(double value):
        value_(value)
    {
    }

    double getData([[maybe_unused]] const std::string& dataSetId,
                   [[maybe_unused]] unsigned year,
                   [[maybe_unused]] unsigned hour) const override
    {
        return value_;
    }

    [[nodiscard]] std::span<const double> getData(const std::string& dataSetId,
                                                  unsigned timeSeriesNumber,
                                                  unsigned firstHour,
                                                  unsigned lastHour) const override

    {
        return vector_;
    }

private:
    double value_{0.};
    std::vector<double> vector_{value_};
};

ConstantDataSeries emptyDataSeries(0.);

Antares::ModelerStudy::SystemModel::Component copyComponent(
  const Antares::ModelerStudy::SystemModel::Component& c)
{
    Antares::ModelerStudy::SystemModel::ComponentBuilder builder;
    return builder.withId(c.Id())
      .withModel(c.getModel())
      .withScenarioGroupId(c.getScenarioGroupId())
      .withParameterValues(c.getParameterValues())
      .build();
}

class DefaultScenario final: public Antares::Optimisation::LinearProblemApi::IScenario
{
public:
    using IScenario::IScenario;

    [[nodiscard]] TimeSeriesNumber getData(Year) const override
    {
        return 1; // Default rank for empty groupId
    }
};

using Models = std::unordered_map<std::string, Antares::ModelerStudy::SystemModel::Model>;

class InMemoryLoader final: public Antares::Solver::ILoader
{
public:
    Antares::Solver::ModelerParameters loadParameters() override
    {
        return {.solver = "sirius",
                .solverLogs = false,
                .solverParameters = "DUMMY",
                .noOutput = true,
                .firstTimeStep = timeSteps.first,
                .lastTimeStep = timeSteps.second};
    }

    Antares::Modeler::Data loadAll() override
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

        Antares::ModelerStudy::SystemModel::LibraryBuilder library_builder;
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
        Antares::ModelerStudy::SystemModel::SystemBuilder builder;
        auto system = builder.withId("dummy-system").withComponents(std::move(components)).build();
        return {.libraries = {library},
                .system = std::make_unique<Antares::ModelerStudy::SystemModel::System>(
                  std::move(system)),
                .dataSeries = std::move(data),
                .scenarioGroupRepository = std::move(scenarioGroupRepository)};
    }

    void setComponents(const std::vector<Antares::ModelerStudy::SystemModel::Component>& compos)
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
        auto scenario = std::make_unique<Antares::Optimisation::LinearProblemDataImpl::Scenario>(
          str);
        scenario->setTimeSerieNumber(year, timeSeriesNumber);
        scenarioGroupRepository.addScenario(str, std::move(scenario));
    }

    Models models;
    std::vector<Antares::ModelerStudy::SystemModel::Component> components;
    Test::Modeler::LinearProblemBuildingFixture fixture;
    std::unique_ptr<Antares::Optimisation::LinearProblemApi::ILinearProblemData>
      data = std::make_unique<ConstantDataSeries>(0.);
    Nodes::Node* lower_bound = fixture.literal(0.0);
    bool timeDependent{false};
    std::map<std::string, PTV> parameters{};
    std::vector<std::string> parameterIds{};
    Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepository{};
    std::unordered_map<std::string, std::string> groupes;
    std::pair<unsigned int, unsigned int> timeSteps{0, 0};
};

struct Solution
{
    double objectiveValue{0.0};
};

class InMemoryWriter final: public Antares::Solver::IWriter
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

    void writeSimulationTable(
      const Antares::Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
      const Antares::Optimisation::LinearProblemApi::IMipSolution& solution,
      const Antares::Modeler::Data& modelerData,
      const Antares::Optimisation::OptimEntityContainer& variableContainer,
      const Antares::Optimisation::LinearProblemApi::FillContext& fillContext) const override
    {
        solution_.objectiveValue = solution.getObjectiveValue();
    }
};

BOOST_AUTO_TEST_CASE(Minimal_system_minimize_to_0)
{
    InMemoryLoader inMemoryLoader;
    InMemoryWriter inMemoryWriter;

    const Antares::Solver::Modeler modeler(inMemoryLoader, inMemoryWriter);
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

    const Antares::Solver::Modeler modeler(inMemoryLoader, inMemoryWriter);
    modeler.run();
    BOOST_CHECK_EQUAL(inMemoryWriter.solution_.objectiveValue, 5);
}

struct TSDimensions
{
    int nRows{1};
    int nCols{1};
};

Antares::Optimisation::LinearProblemDataImpl::TimeSeriesSet
constantTimeSeriesSets(const std::string& id, std::span<double> values, unsigned int nRows = 1)
{
    Antares::Optimisation::LinearProblemDataImpl::TimeSeriesSet timeSeriesSet(id, nRows);
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

Antares::Optimisation::LinearProblemDataImpl::TimeSeriesSet
constantTimeSeriesSet(const std::string& id, double value = 0., TSDimensions dims = {1, 1})
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

    Antares::Optimisation::LinearProblemDataImpl::DataSeriesRepository data_series_repository;
    std::vector<double> values = {2, 3, 4};
    data_series_repository.addDataSeries(
      std::make_unique<Antares::Optimisation::LinearProblemDataImpl::TimeSeriesSet>(
        constantTimeSeriesSets("GROUPA", values, 1)));
    inMemoryLoader.data = std::make_unique<
      Antares::Optimisation::LinearProblemDataImpl::LinearProblemData>(
      std::move(data_series_repository));

    InMemoryWriter inMemoryWriter;

    const Antares::Solver::Modeler modeler(inMemoryLoader, inMemoryWriter);
    modeler.run();
    BOOST_CHECK_EQUAL(inMemoryWriter.solution_.objectiveValue, 2);
}

BOOST_AUTO_TEST_CASE(system_with_three_time_series_use_second_one_all_3)
{
    InMemoryLoader inMemoryLoader;
    inMemoryLoader.timeDependent = true;
    inMemoryLoader.setLowerBoundToParameter("paramA");
    inMemoryLoader.addParameter("paramA");

    Antares::Optimisation::LinearProblemDataImpl::DataSeriesRepository data_series_repository;
    std::vector<double> values = {2, 3, 4};
    data_series_repository.addDataSeries(
      std::make_unique<Antares::Optimisation::LinearProblemDataImpl::TimeSeriesSet>(
        constantTimeSeriesSets("GROUPA", values, 1)));
    inMemoryLoader.data = std::make_unique<
      Antares::Optimisation::LinearProblemDataImpl::LinearProblemData>(
      std::move(data_series_repository));

    inMemoryLoader.addScenario("GROUPA", 0, 2); // Year 0, timeseriesNumber 1
    inMemoryLoader.groupes["some_component"] = "GROUPA";

    InMemoryWriter inMemoryWriter;

    const Antares::Solver::Modeler modeler(inMemoryLoader, inMemoryWriter);
    modeler.run();
    BOOST_CHECK_EQUAL(inMemoryWriter.solution_.objectiveValue, 3);
}
