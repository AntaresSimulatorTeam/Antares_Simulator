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
public:
    double getData([[maybe_unused]] const std::string& dataSetId,
                   [[maybe_unused]] unsigned timeSeriesNumber,
                   [[maybe_unused]] unsigned hour) const override
    {
        return 0.;
    }
};

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
                                           {},
                                           "var1",
                                           fixture.literal(-5),
                                           fixture.literal(10),
                                           {{"ct1", ct_node}});

        Antares::ModelerStudy::SystemModel::LibraryBuilder library_builder;
        auto&& library = library_builder.withId("dummy-library")
                           .withDescription("")
                           // We should set models in the library
                           // However doing so will move the model, reseting the fixture.models
                           // Also invaliding the component reference to the model
                           //.withModelsMap(std::move(fixture.models))
                           .build();
        fixture.createComponent("some_model", "some_component");
        setComponents(fixture.components); // Component model may not be the system model
        Antares::ModelerStudy::SystemModel::SystemBuilder builder;
        auto system = builder.withId("dummy-system").withComponents(std::move(components)).build();
        Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepository;
        return {.libraries = {library},
                .system = std::make_unique<Antares::ModelerStudy::SystemModel::System>(
                  std::move(system)),
                .dataSeries = std::make_unique<EmptyDataSeries>(),
                .scenario_group_repository = std::move(scenarioGroupRepository)};
    }

    void setComponents(const std::span<Antares::ModelerStudy::SystemModel::Component>& vector)
    {
        for (const auto& component: vector)
        {
            components.emplace(component.Id(), copyComponent(component));
        }
    }

    std::unordered_map<std::string, Antares::ModelerStudy::SystemModel::Component> components;
    Fixture fixture;
};

class StubWriter: public Antares::Solver::IWriter
{
public:
    void init(bool) override
    {
        // No initialization needed for in-memory writer
    }

    void writeSolution(
      [[maybe_unused]] const Antares::Optimisation::LinearProblemMpsolverImpl::OrtoolsMipSolution&
        solution) override
    {
        // No output to write for in-memory writer
    }

    void writeProblem(
      [[maybe_unused]] const Antares::Optimisation::LinearProblemMpsolverImpl::OrtoolsLinearProblem&
        problem) override {};
};

BOOST_AUTO_TEST_CASE(dummy)
{
    InMemoryLoader<Test::Modeler::LinearProblemBuildingFixture> inMemoryLoader;
    StubWriter inMemoryWriter;

    const Antares::Solver::Modeler modeler(inMemoryLoader, inMemoryWriter);
    modeler.solve();
}
