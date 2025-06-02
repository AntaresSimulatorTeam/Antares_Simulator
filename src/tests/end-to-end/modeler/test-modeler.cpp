// Copyright 2007-2025, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// SPDX-License-Identifier: MPL-2.0
// This file is part of Antares-Simulator,
// Adequacy and Performance assessment for interconnected energy networks.
//
// Antares_Simulator is free software: you can redistribute it and/or modify
// it under the terms of the Mozilla Public Licence 2.0 as published by
// the Mozilla Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Antares_Simulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// Mozilla Public Licence 2.0 for more details.
//
// You should have received a copy of the Mozilla Public Licence 2.0
// along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.

#define BOOST_TEST_MODULE testE2EModeler
#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/ILoader.h>
#include <antares/solver/modeler/Modeler.h>
#include "antares/expressions/nodes/GreaterThanOrEqualNode.h"
#include "antares/solver/modeler/IWriter.h"

#include "inmemory-modeler.h"

class EmptyDataSeries: public Antares::Optimisation::LinearProblemApi::ILinearProblemData
{
class ConstantDataSeries : public Antares::Optimisation::LinearProblemApi::ILinearProblemData {
public:
    explicit ConstantDataSeries(double value) : value_(value) {}

    double getData([[maybe_unused]] const std::string& dataSetId,
                   [[maybe_unused]] unsigned timeSeriesNumber,
                   [[maybe_unused]] unsigned hour) const override
    {
        return 0.;
                   [[maybe_unused]] const std::string &scenarioGroup,
                   [[maybe_unused]] unsigned year,
                   [[maybe_unused]] unsigned hour) override {
        return value_;
    }
private:
    double value_{0.};
};

class EmptyDataSeries : public ConstantDataSeries {
public:
    EmptyDataSeries(): ConstantDataSeries(0.0) {}
};
EmptyDataSeries emptyDataSeries;

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

using Models = std::unordered_map<std::string, Antares::ModelerStudy::SystemModel::Model>;

template<class Fixture>
class InMemoryLoader: public Antares::Solver::ILoader
{
public:
    Antares::Solver::ModelerParameters loadParameters() override
    {
        return {.solver = "sirius",
                .solverLogs = false,
                .solverParameters = "DUMMY",
                .noOutput = true,
                .firstTimeStep = 0,
                .lastTimeStep = 0};
    }

    Antares::Modeler::Data loadAll() override
    {
        auto var_node = fixture.variable("var1");
        auto zero = fixture.literal(0);
        auto ct_node = fixture.nodes.template create<
          Antares::Expressions::Nodes::GreaterThanOrEqualNode>(var_node, zero);
        fixture.createModelWithOneFloatVar("some_model",
                                           {"a"},
                                           "x",
                                           lower_bound,
                                           fixture.literal(10),
                                           { },
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
        fixture.createComponent("some_model", "some_component", {Test::Modeler::build_context_parameter_with("a", "a", Antares::Expressions::Visitors::ParameterType::TIMESERIE)});
        setComponents(fixture.components); // Component model may not be the system model
        Antares::ModelerStudy::SystemModel::SystemBuilder builder;
        auto system = builder.withId("dummy-system").withComponents(std::move(components)).build();
        return {.libraries = {library},
                .system = std::make_unique<Antares::ModelerStudy::SystemModel::System>(
                  std::move(system)),
            .dataSeries = std::move(data)
        };
    }

    void setComponents(const std::span<Antares::ModelerStudy::SystemModel::Component>& vector)
    {
        for (const auto& component: vector)
        {
            components.emplace(component.Id(), copyComponent(component));
        }
    }

    void setModels(Models&& map)
    {
        models = std::move(map);
    }

    void setLowerBoundToParameter(const std::string& parameterId) {
        lower_bound = fixture.parameter(parameterId, Antares::Expressions::Visitors::TimeIndex::VARYING_IN_TIME_ONLY);
    }

    Models models;
    std::unordered_map<std::string, Antares::ModelerStudy::SystemModel::Component> components;
    Fixture fixture;
    std::unique_ptr<Antares::Optimisation::LinearProblemApi::ILinearProblemData> data = std::make_unique<EmptyDataSeries>();
    Antares::Expressions::Nodes::Node *lower_bound = fixture.literal(0.0);
    bool timeDependent{false};
};

struct Solution {
    double objectiveValue{0.0};
};

class StubWriter : public Antares::Solver::IWriter {
public:
    Solution solution_{};

    void init(bool) override
    {
        // No initialization needed for in-memory writer
    }

    void writeSolution(
        const Antares::Optimisation::LinearProblemApi::IMipSolution &
        solution) override {
        solution_.objectiveValue = solution.getObjectiveValue();
        // No output to write for in-memory writer
    }

    void writeProblem(
      [[maybe_unused]] const Antares::Optimisation::LinearProblemMpsolverImpl::OrtoolsLinearProblem&
        problem) override {};
        problem.WriteLP("dummy.lp");
    };
};

BOOST_AUTO_TEST_CASE(minimal_system) {
    InMemoryLoader<Test::Modeler::LinearProblemBuildingFixture> inMemoryLoader;
    StubWriter inMemoryWriter;

    const Antares::Solver::Modeler modeler(inMemoryLoader, inMemoryWriter);
    modeler.solve();
    BOOST_CHECK_EQUAL(inMemoryWriter.solution_.objectiveValue, 0);
}

BOOST_AUTO_TEST_CASE(system_with_one_constant_serie_value_10) {
    InMemoryLoader<Test::Modeler::LinearProblemBuildingFixture> inMemoryLoader;
    inMemoryLoader.timeDependent = true;
    inMemoryLoader.setLowerBoundToParameter("a");


    inMemoryLoader.data = std::make_unique<ConstantDataSeries>(5);

    InMemoryWriter inMemoryWriter;

    const Antares::Solver::Modeler modeler(inMemoryLoader, inMemoryWriter);
    modeler.solve();
    BOOST_CHECK_EQUAL(inMemoryWriter.solution_.objectiveValue, 5);
}