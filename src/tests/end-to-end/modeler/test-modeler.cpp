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
#include <antares/solver/modeler/Modeler.h>
#include <antares/solver/modeler/ILoader.h>

#include "antares/solver/modeler/IWriter.h"

class EmptyDataSeries : public Antares::Optimisation::LinearProblemApi::ILinearProblemData {
public:
    double getData(const std::string &dataSetId, const std::string &scenarioGroup, unsigned year,
                   unsigned hour) override { return 0.; };
};

class InMemoryLoader : public Antares::Solver::ILoader {
public:
    Antares::Solver::ModelerParameters loadParameters() override {
        return {
            .solver = "sirius", .solverLogs = false, .solverParameters = "DUMMY", .noOutput = true, .firstTimeStep = 0,
            .lastTimeStep = 0
        };
    }

    Antares::Modeler::Data loadAll() override {
        Antares::ModelerStudy::SystemModel::SystemBuilder builder;
        Antares::ModelerStudy::SystemModel::ComponentBuilder componentBuilder;
        auto component = componentBuilder
        .withId("dummy")
        .withModel(&model)
        .withScenarioGroupId("dummy-scenario-group")
        .build();
        std::unordered_map<std::string, Antares::ModelerStudy::SystemModel::Component> components{{"dummy", component}};
        auto system = builder
                .withId("dummy-system")
                .withComponents(std::move(components))
                .build();
        return {
            .libraries = {}, .system = std::make_unique<Antares::ModelerStudy::SystemModel::System>(std::move(system)),
            .dataSeries = std::make_unique<EmptyDataSeries>()
        };
    };
private:
        Antares::ModelerStudy::SystemModel::Model model;
};

class InMemoryWriter : public Antares::Solver::IWriter {
public:
    void init() override {
        // No initialization needed for in-memory writer
    }

    void writeSolution(
        [[maybe_unused]] const Antares::Optimisation::LinearProblemMpsolverImpl::OrtoolsMipSolution &
        solution) override {
        // No output to write for in-memory writer
    }

    void writeProblem(
        [[maybe_unused]] const Antares::Optimisation::LinearProblemMpsolverImpl::OrtoolsLinearProblem &
        problem) override {
    };
};

BOOST_AUTO_TEST_CASE(dummy) {
    InMemoryLoader inMemoryLoader;
    InMemoryWriter inMemoryWriter;
    Antares::Solver::Modeler modeler(inMemoryLoader, inMemoryWriter);
    modeler.solve();
}
