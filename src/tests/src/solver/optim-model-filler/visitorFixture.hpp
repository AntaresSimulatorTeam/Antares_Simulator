#pragma once

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h>
#include "antares/exception/InvalidArgumentError.hpp"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"

#include "UtilMocks.h"

using namespace Antares::Optimisation;
using namespace Antares::Expressions;
using namespace Antares::ModelerStudy;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;

inline ScenarioGroupRepository createScenario()
{
    ScenarioGroupRepository scenarioGroupRepository;
    auto scenarioPtr = std::make_unique<LinearProblemDataImpl::Scenario>("SCENARIO_GROUP");
    scenarioPtr->setTimeSerieNumber(0, 1);
    scenarioGroupRepository.addScenario("SCENARIO_GROUP", std::move(scenarioPtr));
    scenarioPtr = std::make_unique<LinearProblemDataImpl::Scenario>("GROUP");
    scenarioPtr->setTimeSerieNumber(0, 1);
    scenarioGroupRepository.addScenario("GROUP", std::move(scenarioPtr));
    return scenarioGroupRepository;
}

struct MockLinearProblemData: LinearProblemApi::ILinearProblemData
{
    [[nodiscard]] double getData([[maybe_unused]] const std::string& dataSetId,
                                 [[maybe_unused]] unsigned scenario,
                                 unsigned hour) const override
    {
        return hour; // for test
    }

    [[nodiscard]] std::span<const double> getData(const std::string& dataSetId,
                                                  unsigned timeSeriesNumber,
                                                  unsigned firstHour,
                                                  unsigned lastHour) const override
    {
        return {v_.begin(), lastHour - firstHour + 1};
    }

    const std::vector<double> v_{0., 1., 2., 3., 4., 5.}; // should be enough
};

template<class Visitor>
struct VisitorFixture: Registry<Node>
{
    MockLinearProblem linearProblem;
    MockLinearProblemData data;
    LinearProblemApi::EmptyScenario empty_scenario;
    ScenarioGroupRepository scenarioGroupRepository;
    SystemModel::Model m;

    OptimEntityContainer optimContainer;
    std::vector<SystemModel::Component> components;
    LinearProblemApi::FillContext ctx{0, 0, 0, 0, 0};

    VisitorFixture():
        linearProblem(false),
        scenarioGroupRepository(createScenario()),
        optimContainer(linearProblem, &data, &scenarioGroupRepository),
        components(1, setupComponent())
    {
        optimContainer.addFromSystemComponents(components);
        auto& optimComponent = optimContainer.getOptimComponent(0);
        optimComponent.index = 0;
        optimComponent.modelVariableGlobalIndices = {0, 1, 2};
        {
            optimContainer.addStartColumn();
            linearProblem.addNumVariable(0, 1, "var1");
            optimContainer.addStartColumn();
            linearProblem.addNumVariable(0, 1, "var2");
            optimContainer.addStartColumn();
            linearProblem.addNumVariable(0, 1, "var3");
        }
    }

    Visitor visitor()
    {
        return Visitor(optimContainer, ctx, components[0]);
    }

private:
    SystemModel::Component setupComponent()
    {
        return setComponentParameterValues(
          {{"param_3", SystemModel::ParameterType::CONSTANT, "3."},
           {"param_m5", SystemModel::ParameterType::CONSTANT, "-5."},
           {"param_ts", SystemModel::ParameterType::TIMESERIE, "0_1_2"},
           {"param1", SystemModel::ParameterType::CONSTANT, "-2."},
           {"param2", SystemModel::ParameterType::CONSTANT, "8."}});
    }

    SystemModel::Component setComponentParameterValues(
      const std::vector<
        std::tuple<std::string, Antares::ModelerStudy::SystemModel::ParameterType, std::string>>&
        values)
    {
        std::map<std::string, Antares::ModelerStudy::SystemModel::ParameterTypeAndValue> map;
        std::vector<SystemModel::Parameter> parameters;
        for (auto value: values)
        {
            map[std::get<0>(value)] = Antares::ModelerStudy::SystemModel::ParameterTypeAndValue{
              .id = std::get<0>(value),
              .type = std::get<1>(value),
              .value = std::get<2>(value)};
            SystemModel::Parameter parameter{std::get<0>(value),
                                             SystemModel::TimeDependent::YES,
                                             SystemModel::ScenarioDependent::YES};
            parameters.push_back(parameter);
        }
        SystemModel::ModelBuilder modelBuilder;
        m = modelBuilder.withId("model").withParameters(std::move(parameters)).build();
        SystemModel::ComponentBuilder componentBuilder;
        return componentBuilder.withId("compo")
          .withModel(&m)
          .withScenarioGroupId("group")
          .withParameterValues(map)
          .build();
    }
};
