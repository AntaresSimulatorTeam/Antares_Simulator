#define WIN32_LEAN_AND_MEAN

#include <filesystem>
#include <fstream>

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/loadFiles/loadFiles.h>
#include "antares/study/system-model/optimConfig.h"

using namespace Antares::Solver::LoadFiles;
using namespace Antares::Modeler::Config;

struct CreateInputFileFixture
{
    CreateInputFileFixture();
    void createLibraryFile(const std::string& yaml_content);
    void createOptimConfigFile(const std::string& yaml_content);
    ~CreateInputFileFixture();

    std::filesystem::path studyFolder;

private:
    void createYamlFile(const std::string& filename, const std::string& yaml_content);
    std::filesystem::path inputFolder_;
    std::filesystem::path librariesFolder_;
    std::filesystem::path creationFolder_;
};

CreateInputFileFixture::CreateInputFileFixture()
{
    studyFolder = std::filesystem::temp_directory_path();

    inputFolder_ = studyFolder / "input";
    std::filesystem::create_directory(inputFolder_);

    librariesFolder_ = inputFolder_ / "model-libraries";
    std::filesystem::create_directory(librariesFolder_);

}

void CreateInputFileFixture::createLibraryFile(const std::string& yaml_content)
{
    creationFolder_ = librariesFolder_;
    createYamlFile("my-library.yml", yaml_content);
}

void CreateInputFileFixture::createOptimConfigFile(const std::string& yaml_content)
{
    creationFolder_ = inputFolder_;
    createYamlFile("optim-config.yml", yaml_content);
}

void CreateInputFileFixture::createYamlFile(const std::string& filename,
                                          const std::string& yaml_content)
{
    std::filesystem::path yamlPath = creationFolder_ / filename;
    std::ofstream outStream;
    outStream.open(yamlPath, std::ofstream::trunc | std::ofstream::out);
    outStream << yaml_content;
    outStream.flush();
    outStream.close();
}

CreateInputFileFixture ::~CreateInputFileFixture()
{
    std::filesystem::remove_all(librariesFolder_);
    std::filesystem::remove_all(inputFolder_);
}

BOOST_FIXTURE_TEST_CASE(dummy_test, CreateInputFileFixture)
{
    // Arrange part
    std::string yamlContent = R"(library:
  id: my-lib
  description: blah-blah

  models:
    - id: some-model
      variables:
        - id: x
          lower-bound: 0
          upper-bound: 1
          variable-type: continuous
        - id: y
          lower-bound: 0
          upper-bound: 1
          variable-type: continuous
        - id: z
          lower-bound: 0
          upper-bound: 1
          variable-type: continuous)";

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
    const auto& modelVariables = libraries[0].Models()["some-model"].Variables();

    BOOST_CHECK_EQUAL(modelVariables[0].Id(), "x");
    BOOST_CHECK(modelVariables[0].location() == Location::MASTER);

    BOOST_CHECK_EQUAL(modelVariables[1].Id(), "y");
    BOOST_CHECK(modelVariables[1].location() == Location::MASTER_AND_SUBPROBLEMS);

    BOOST_CHECK_EQUAL(modelVariables[2].Id(), "z");
    BOOST_CHECK(modelVariables[2].location() == Location::SUBPROBLEMS);
    
    BOOST_CHECK(true);
}
