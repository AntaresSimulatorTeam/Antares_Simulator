#pragma once

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h>
#include "antares/exception/InvalidArgumentError.hpp"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"

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
        return v_;
    }

    const std::vector<double> v_{0., 1., 2.};
};

template<class Visitor>
struct VisitorFixture: Registry<Node>
{
    LinearProblemMpsolverImpl::OrtoolsLinearProblem linearProblem; // TODO use mock
    MockLinearProblemData data;
    LinearProblemApi::EmptyScenario empty_scenario;
    ScenarioGroupRepository scenarioGroupRepository;
    SystemModel::Model m;
    SystemModel::ComponentBuilder componentBuilder;
    OptimEntityContainer optimContainer;
    SystemModel::Component component = componentBuilder.withId("compo")
                                         .withModel(&m)
                                         .withScenarioGroupId("group")
                                         .build();
    LinearProblemApi::FillContext ctx{0, 0, 0, 0, 0};

    VisitorFixture():
        linearProblem(false, "sirius"),
        scenarioGroupRepository(createScenario()),
        optimContainer(linearProblem, &data, &scenarioGroupRepository)
    {
        optimContainer.addFromSystemComponent(component);
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
        return Visitor(optimContainer, ctx, component);
    }

    void setComponentParameterValues(
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
        component = componentBuilder.withId("compo")
                      .withModel(&m)
                      .withScenarioGroupId("group")
                      .withParameterValues(map)
                      .build();
    }
};
