#define WIN32_LEAN_AND_MEAN

#include <filesystem>
#include <fstream>
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/loadFiles/loadFiles.h>
#include "antares/study/system-model/optimConfig.h"

using namespace Antares::Solver::LoadFiles;
using namespace Antares::Solver::Config;

struct CreateInputFileFixture
{
    CreateInputFileFixture();
    void createLibraryFile(const std::string& yaml_content);
    void createOptimConfigFile(const std::string& yaml_content);
    ~CreateInputFileFixture();

    std::filesystem::path studyFolder;

private:
    void createYamlFile(const std::filesystem::path& filepath, const std::string& yaml_content);

    std::filesystem::path inputFolder_;
    std::filesystem::path librariesFolder_;
};

CreateInputFileFixture::CreateInputFileFixture()
{
    studyFolder = std::filesystem::temp_directory_path();

    inputFolder_ = studyFolder / "input";
    std::filesystem::create_directory(inputFolder_);

    librariesFolder_ = studyFolder / "input" / "model-libraries";
    std::filesystem::create_directory(librariesFolder_);
}

void CreateInputFileFixture::createLibraryFile(const std::string& yaml_content)
{
    createYamlFile(librariesFolder_ / "my-library.yml", yaml_content);
}

void CreateInputFileFixture::createOptimConfigFile(const std::string& yaml_content)
{
    createYamlFile(inputFolder_ / "optim-config.yml", yaml_content);
}

void CreateInputFileFixture::createYamlFile(const std::filesystem::path& filepath,
                                            const std::string& yaml_content)
{
    std::ofstream outStream;
    outStream.open(filepath, std::ofstream::trunc | std::ofstream::out);
    outStream << yaml_content;
    outStream.flush();
}

CreateInputFileFixture ::~CreateInputFileFixture()
{
    std::filesystem::remove_all(librariesFolder_);
    std::filesystem::remove_all(inputFolder_);
}

BOOST_FIXTURE_TEST_CASE(load_optim_config_with_variable_decomposition, CreateInputFileFixture)
{
    // Arrange part
    std::string yamlContent = R"(library:
  id: my-lib
  description: blah-blah
  models:
    - id: some-model
      variables:
        - id: x
        - id: y
        - id: z)";

    createLibraryFile(yamlContent);

    yamlContent = R"(models:
      - id: my-lib.some-model
        model-decomposition:
          variables:
            - id: x
              location: master
            - id: y
              location: master-and-subproblems
            - id: z
              location: subproblems)";

    createOptimConfigFile(yamlContent);

    // Act part
    auto libraries = loadLibraries(studyFolder);

    // Assert part
    const auto& modelVariables = libraries[0].Models().at("some-model").Variables();

    BOOST_CHECK_EQUAL(modelVariables[0].Id(), "x");
    BOOST_CHECK(modelVariables[0].location() == Location::MASTER);

    BOOST_CHECK_EQUAL(modelVariables[1].Id(), "y");
    BOOST_CHECK(modelVariables[1].location() == Location::MASTER_AND_SUBPROBLEMS);

    BOOST_CHECK_EQUAL(modelVariables[2].Id(), "z");
    BOOST_CHECK(modelVariables[2].location() == Location::SUBPROBLEMS);
}

BOOST_FIXTURE_TEST_CASE(load_optim_config_with_constraint_decomposition, CreateInputFileFixture)
{
    // Arrange part
    std::string yamlContent = R"(library:
  id: my-lib
  description: blah-blah
  models:
    - id: some-model
      variables:
        - id: x
      constraints:
        - id: c1
          expression: x = 0
        - id: c2
          expression: x = 0
        - id: c3
          expression: x = 0)";

    createLibraryFile(yamlContent);

    yamlContent = R"(models:
      - id: my-lib.some-model
        model-decomposition:
          constraints:
            - id: c1
              location: master
            - id: c2
              location: master-and-subproblems
            - id: c3
              location: subproblems)";

    createOptimConfigFile(yamlContent);

    // Act part
    auto libraries = loadLibraries(studyFolder);

    // Assert part
    const auto& modelConstraints = libraries[0].Models().at("some-model").Constraints();

    BOOST_CHECK_EQUAL(modelConstraints[0].Id(), "c1");
    BOOST_CHECK(modelConstraints[0].location() == Location::MASTER);

    BOOST_CHECK_EQUAL(modelConstraints[1].Id(), "c2");
    BOOST_CHECK(modelConstraints[1].location() == Location::MASTER_AND_SUBPROBLEMS);

    BOOST_CHECK_EQUAL(modelConstraints[2].Id(), "c3");
    BOOST_CHECK(modelConstraints[2].location() == Location::SUBPROBLEMS);
}

BOOST_FIXTURE_TEST_CASE(load_optim_config_with_objective_decomposition, CreateInputFileFixture)
{
    // Arrange part
    std::string yamlContent = R"(library:
  id: my-lib
  description: blah-blah
  models:
    - id: some-model
      variables:
        - id: x
      objective-contributions:
        - id: obj_1
          expression: x
        - id: obj_2
          expression: x
        - id: obj_3
          expression: x)";

    createLibraryFile(yamlContent);

    yamlContent = R"(models:
      - id: my-lib.some-model
        model-decomposition:
          objective-contributions:
            - id: obj_1
              location: master
            - id: obj_2
              location: master-and-subproblems
            - id: obj_3
              location: subproblems)";

    createOptimConfigFile(yamlContent);

    // Act part
    auto libraries = loadLibraries(studyFolder);

    // Assert part
    const auto& modelObjectives = libraries[0].Models().at("some-model").Objectives();

    BOOST_CHECK_EQUAL(modelObjectives[0].Id(), "obj_1");
    BOOST_CHECK(modelObjectives[0].location() == Location::MASTER);

    BOOST_CHECK_EQUAL(modelObjectives[1].Id(), "obj_2");
    BOOST_CHECK(modelObjectives[1].location() == Location::MASTER_AND_SUBPROBLEMS);

    BOOST_CHECK_EQUAL(modelObjectives[2].Id(), "obj_3");
    BOOST_CHECK(modelObjectives[2].location() == Location::SUBPROBLEMS);
}

BOOST_FIXTURE_TEST_CASE(no_library_but_reference_an_objective_in_optim_config___exception_raised,
                        CreateInputFileFixture)
{
    std::string yamlContent = R"(models:
      - id: my-lib.some-model
        model-decomposition: [])";

    createOptimConfigFile(yamlContent);

    BOOST_CHECK_EXCEPTION(loadLibraries(studyFolder),
                          std::runtime_error,
                          checkMessage("No library 'my-lib' found."));
}

BOOST_FIXTURE_TEST_CASE(model_does_not_exist_in_library___exception_raised, CreateInputFileFixture)
{
    std::string yamlContent = R"(library:
  id: my-lib
  description: blah-blah
  models: [])";

    createLibraryFile(yamlContent);

    yamlContent = R"(models:
      - id: my-lib.some-model
        model-decomposition:
          objective-contributions: [])";

    createOptimConfigFile(yamlContent);

    BOOST_CHECK_EXCEPTION(loadLibraries(studyFolder),
                          std::runtime_error,
                          checkMessage("No model 'my-lib.some-model' found."));
}

BOOST_FIXTURE_TEST_CASE(objective_does_not_exist_in_model___exception_raised,
                        CreateInputFileFixture)
{
    std::string yamlContent = R"(library:
  id: my-lib
  description: blah-blah
  models:
    - id: some-model
      variables:
        - id: x
      objective-contributions:
        - id: my_obj
          expression: x)";

    createLibraryFile(yamlContent);

    yamlContent = R"(models:
      - id: my-lib.some-model
        model-decomposition:
          objective-contributions:
            - id: some_obj
              location: subproblems)";

    createOptimConfigFile(yamlContent);

    BOOST_CHECK_EXCEPTION(loadLibraries(studyFolder),
                          std::runtime_error,
                          checkMessage("No objective 'some_obj' found."));
}

BOOST_FIXTURE_TEST_CASE(variable_does_not_exist_in_model___exception_raised, CreateInputFileFixture)
{
    std::string yamlContent = R"(library:
  id: my-lib
  description: blah-blah
  models:
    - id: some-model
      variables:
        - id: x
        - id: y
        - id: z)";

    createLibraryFile(yamlContent);

    yamlContent = R"(models:
      - id: my-lib.some-model
        model-decomposition:
          variables:
            - id: v
              location: master)";

    createOptimConfigFile(yamlContent);

    BOOST_CHECK_EXCEPTION(loadLibraries(studyFolder),
                          std::runtime_error,
                          checkMessage("No variable 'v' found."));
}

BOOST_FIXTURE_TEST_CASE(constraint_does_not_exist_in_model___exception_raised,
                        CreateInputFileFixture)
{
    // Arrange part
    std::string yamlContent = R"(library:
  id: my-lib
  description: blah-blah
  models:
    - id: some-model
      variables:
        - id: x
      constraints:
        - id: my_constraint
          expression: x)";

    createLibraryFile(yamlContent);

    yamlContent = R"(models:
      - id: my-lib.some-model
        model-decomposition:
          constraints:
            - id: some_constraint
              location: master)";

    createOptimConfigFile(yamlContent);

    BOOST_CHECK_EXCEPTION(loadLibraries(studyFolder),
                          std::runtime_error,
                          checkMessage("No constraint 'some_constraint' found."));
}

BOOST_AUTO_TEST_CASE(locationToString)
{
    BOOST_CHECK_EQUAL(LocationToStr(Location::MASTER), "master");
    BOOST_CHECK_EQUAL(LocationToStr(Location::MASTER_AND_SUBPROBLEMS), "master-and-subproblems");
    BOOST_CHECK_EQUAL(LocationToStr(Location::SUBPROBLEMS), "subproblems");
}

BOOST_AUTO_TEST_CASE(locationsCompatibleForFillers)
{
    BOOST_CHECK(AreLocationsCompatibleForFillers(Location::MASTER, Location::MASTER));
    BOOST_CHECK(AreLocationsCompatibleForFillers(Location::SUBPROBLEMS, Location::SUBPROBLEMS));
    BOOST_CHECK(!AreLocationsCompatibleForFillers(Location::SUBPROBLEMS, Location::MASTER));
    BOOST_CHECK(!AreLocationsCompatibleForFillers(Location::MASTER, Location::SUBPROBLEMS));
    BOOST_CHECK(
      AreLocationsCompatibleForFillers(Location::MASTER_AND_SUBPROBLEMS, Location::MASTER));
    BOOST_CHECK(
      AreLocationsCompatibleForFillers(Location::MASTER_AND_SUBPROBLEMS, Location::SUBPROBLEMS));
    BOOST_CHECK(AreLocationsCompatibleForFillers(Location::MASTER_AND_SUBPROBLEMS,
                                                 Location::MASTER_AND_SUBPROBLEMS));
}

BOOST_AUTO_TEST_CASE(locationsCompatibleForExpressions)
{
    BOOST_CHECK(AreLocationsCompatibleForExpressions(Location::MASTER, Location::MASTER));
    BOOST_CHECK(AreLocationsCompatibleForExpressions(Location::SUBPROBLEMS, Location::SUBPROBLEMS));
    BOOST_CHECK(AreLocationsCompatibleForExpressions(Location::MASTER_AND_SUBPROBLEMS,
                                                     Location::MASTER_AND_SUBPROBLEMS));
    BOOST_CHECK(
      !AreLocationsCompatibleForExpressions(Location::MASTER, Location::MASTER_AND_SUBPROBLEMS));
    BOOST_CHECK(!AreLocationsCompatibleForExpressions(Location::SUBPROBLEMS,
                                                      Location::MASTER_AND_SUBPROBLEMS));
    BOOST_CHECK(!AreLocationsCompatibleForExpressions(Location::SUBPROBLEMS, Location::MASTER));
    BOOST_CHECK(!AreLocationsCompatibleForExpressions(Location::MASTER, Location::SUBPROBLEMS));
}
