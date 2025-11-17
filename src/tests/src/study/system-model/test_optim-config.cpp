/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#define WIN32_LEAN_AND_MEAN

#include <filesystem>
#include <fstream>
#include <string>

#include "antares/solver/modeler/loadFiles/loadFiles.h"
#include "antares/study/system-model/model.h"

// If we don't turn clang-format off here, some antlr4 header does not compile :
// it collides with a #include <windows.h> somewhere in Yuni
// clang-format off
#include <unit_test_utils.h>
// clang-format on

#include <boost/test/unit_test.hpp>

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Solver::LoadFiles;
using namespace Antares::Modeler::Config;

BOOST_AUTO_TEST_SUITE(optimConfigYaml)

struct DecompositionFixture
{
    DecompositionFixture() = default;

    void buildModel()
    {
        ModelBuilder model_builder;
        model_builder.withId("model")
          .withVariables(std::move(variables))
          .withConstraints(std::move(constraints))
          .withObjectives(std::move(objectives));
        model_ = model_builder.build();
    }

    void buildLibraryWithModel()
    {
        std::vector<Model> models;
        models.emplace_back(std::move(model_));
        LibraryBuilder library_builder;
        Library lib = library_builder.withId("library").withModels(std::move(models)).build();
        libraries = {lib};
    }

    void createOptimConfigFile(const std::string& yaml_content)
    {
        studyFolder = std::filesystem::temp_directory_path();
        inputFolder_ = studyFolder / "input";
        std::filesystem::create_directory(inputFolder_);
        std::filesystem::path yamlPath = inputFolder_ / "optim-config.yml";

        std::ofstream outStream;
        outStream.open(yamlPath, std::ofstream::trunc | std::ofstream::out);
        outStream << yaml_content;
        outStream.flush();
        outStream.close();
    }

    ~DecompositionFixture()
    {
        std::filesystem::remove_all(inputFolder_);
    }

    std::filesystem::path studyFolder;
    std::vector<Library> libraries;
    std::vector<Variable> variables;
    std::vector<Constraint> constraints;
    std::vector<Objective> objectives;

private:
    std::filesystem::path inputFolder_;
    Model model_;
};

BOOST_FIXTURE_TEST_CASE(variable_decomposition, DecompositionFixture)
{
    // Arrange part : build a model, then a library, and create an optim config file
    variables.push_back({"x", {}, {}, ValueType::FLOAT, {}, {}});
    variables.push_back({"y", {}, {}, ValueType::FLOAT, {}, {}});
    variables.push_back({"z", {}, {}, ValueType::FLOAT, {}, {}});

    buildModel();
    buildLibraryWithModel();

    std::string yamlContent = R"(models:
      - id: library.model
        model-decomposition:
          variables:
            - id: x
              location: master
            - id: y
              location: master-and-subproblems
            - id: z
              location: subproblems)";

    createOptimConfigFile(yamlContent);

    // Act part : load the optim config
    loadOptimConfig(studyFolder, libraries);

    // Assert part
    const auto& modelVariables = libraries[0].Models()["model"].Variables();

    BOOST_CHECK_EQUAL(modelVariables[0].Id(), "x");
    BOOST_CHECK(modelVariables[0].location() == Location::MASTER);

    BOOST_CHECK_EQUAL(modelVariables[1].Id(), "y");
    BOOST_CHECK(modelVariables[1].location() == Location::MASTER_AND_SUBPROBLEMS);

    BOOST_CHECK_EQUAL(modelVariables[2].Id(), "z");
    BOOST_CHECK(modelVariables[2].location() == Location::SUBPROBLEMS);
}

BOOST_FIXTURE_TEST_CASE(constraint_decomposition, DecompositionFixture)
{
    // Model with constraints
    constraints.push_back({"c1", {}});
    constraints.push_back({"c2", {}});
    constraints.push_back({"c3", {}});
    buildModel();
    buildLibraryWithModel();

    std::string yamlContent = R"(models:
      - id: library.model
        model-decomposition:
          constraints:
            - id: c1
              location: master
            - id: c2
              location: master-and-subproblems
            - id: c3
              location: subproblems)";

    createOptimConfigFile(yamlContent);

    loadOptimConfig(studyFolder, libraries);

    const auto& modelConstraints = libraries[0].Models()["model"].Constraints();

    BOOST_CHECK_EQUAL(modelConstraints[0].Id(), "c1");
    BOOST_CHECK(modelConstraints[0].location() == Location::MASTER);

    BOOST_CHECK_EQUAL(modelConstraints[1].Id(), "c2");
    BOOST_CHECK(modelConstraints[1].location() == Location::MASTER_AND_SUBPROBLEMS);

    BOOST_CHECK_EQUAL(modelConstraints[2].Id(), "c3");
    BOOST_CHECK(modelConstraints[2].location() == Location::SUBPROBLEMS);
}

BOOST_FIXTURE_TEST_CASE(objective_decomposition, DecompositionFixture)
{
    objectives.push_back({"o1", {}});
    objectives.push_back({"o2", {}});
    objectives.push_back({"o3", {}});
    buildModel();
    buildLibraryWithModel();

    std::string yamlContent = R"(models:
      - id: library.model
        model-decomposition:
          objective-contributions:
            - id: o1
              location: master
            - id: o2
              location: master-and-subproblems
            - id: o3
              location: subproblems)";

    createOptimConfigFile(yamlContent);

    loadOptimConfig(studyFolder, libraries);
    const auto& modelObjectives = libraries[0].Models()["model"].Objectives();

    BOOST_CHECK_EQUAL(modelObjectives[0].Id(), "o1");
    BOOST_CHECK(modelObjectives[0].location() == Location::MASTER);

    BOOST_CHECK_EQUAL(modelObjectives[1].Id(), "o2");
    BOOST_CHECK(modelObjectives[1].location() == Location::MASTER_AND_SUBPROBLEMS);

    BOOST_CHECK_EQUAL(modelObjectives[2].Id(), "o3");
    BOOST_CHECK(modelObjectives[2].location() == Location::SUBPROBLEMS);
}

BOOST_FIXTURE_TEST_CASE(objective_does_not_exist_in_model___exception_raised, DecompositionFixture)
{
    buildModel();
    buildLibraryWithModel();

    std::string yamlContent = R"(models:
      - id: library.model
        model-decomposition:
          objective-contributions:
            - id: o2
              location: subproblems)";

    createOptimConfigFile(yamlContent);

    BOOST_CHECK_EXCEPTION(loadOptimConfig(studyFolder, libraries),
                          ErrorLoadingYaml,
                          checkMessage("No objective found with this name: o2"));
}

BOOST_FIXTURE_TEST_CASE(variable_does_not_exist_in_model___exception_raised, DecompositionFixture)
{
    buildModel();
    buildLibraryWithModel();

    std::string yamlContent = R"(models:
      - id: library.model
        model-decomposition:
          variables:
            - id: y
              location: master)";
    createOptimConfigFile(yamlContent);

    BOOST_CHECK_EXCEPTION(loadOptimConfig(studyFolder, libraries),
                          ErrorLoadingYaml,
                          checkMessage("No variable found with this name: y"));
}

BOOST_FIXTURE_TEST_CASE(constraint_does_not_exist_in_model___exception_raised, DecompositionFixture)
{
    buildModel();
    buildLibraryWithModel();

    std::string yamlContent = R"(models:
      - id: library.model
        model-decomposition:
          constraints:
            - id: c2
              location: master-and-subproblems)";

    createOptimConfigFile(yamlContent);

    BOOST_CHECK_EXCEPTION(loadOptimConfig(studyFolder, libraries),
                          ErrorLoadingYaml,
                          checkMessage("No constraint found with this name: c2"));
}

BOOST_AUTO_TEST_SUITE_END()
