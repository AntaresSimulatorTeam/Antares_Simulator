// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <chrono>
#include <filesystem>
#include <fstream>

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/VariableNameMapper.h>

using Antares::Solver::VariableNameMapper;

namespace fs = std::filesystem;

struct VariableNamesFixture
{
    VariableNamesFixture()
    {
        const auto unique = std::chrono::steady_clock::now().time_since_epoch().count();
        const auto root = fs::temp_directory_path()
                          / fs::path("variable-names-test-" + std::to_string(unique));
        studyFolder = root / "study";
        inputFolder = studyFolder / "input";
        defaultFile = root / "default-variable-names.yml";
        fs::create_directories(inputFolder);
    }

    ~VariableNamesFixture()
    {
        fs::remove_all(studyFolder.parent_path());
    }

    void writeFile(const fs::path& path, const std::string& content)
    {
        std::ofstream out(path, std::ofstream::trunc | std::ofstream::out);
        out << content;
    }

    void writeStudyFile(const std::string& content)
    {
        writeFile(inputFolder / "variable-names.yml", content);
    }

    void writeDefaultFile(const std::string& content)
    {
        writeFile(defaultFile, content);
    }

    fs::path studyFolder;
    fs::path inputFolder;
    fs::path defaultFile;
};

BOOST_FIXTURE_TEST_SUITE(test_variable_name_mapper, VariableNamesFixture)

BOOST_AUTO_TEST_CASE(loads_and_maps_known_names_from_study_file)
{
    writeStudyFile(R"(UnsuppliedEnergy: unsupplied_energy
Spillage: spillaged_energy)");

    const VariableNameMapper mapper(studyFolder, defaultFile);

    BOOST_CHECK_EQUAL(mapper.mapOutput("UnsuppliedEnergy"), "unsupplied_energy");
    BOOST_CHECK_EQUAL(mapper.mapOutput("Spillage"), "spillaged_energy");
}

BOOST_AUTO_TEST_CASE(unknown_name_is_returned_unchanged)
{
    writeStudyFile("UnsuppliedEnergy: unsupplied_energy");
    const VariableNameMapper mapper(studyFolder, defaultFile);
    BOOST_CHECK_EQUAL(mapper.mapOutput("HydProd"), "HydProd");
}

BOOST_AUTO_TEST_CASE(mapping_is_case_sensitive)
{
    writeStudyFile("UnsuppliedEnergy: unsupplied_energy");
    const VariableNameMapper mapper(studyFolder, defaultFile);
    BOOST_CHECK_EQUAL(mapper.mapOutput("unsuppliedenergy"), "unsuppliedenergy");
    BOOST_CHECK_EQUAL(mapper.mapOutput("UNSUPPLIEDENERGY"), "UNSUPPLIEDENERGY");
}

BOOST_AUTO_TEST_CASE(default_file_is_used_when_study_file_is_absent)
{
    writeDefaultFile("UnsuppliedEnergy: default_mapping");
    const VariableNameMapper mapper(studyFolder, defaultFile);
    BOOST_CHECK_EQUAL(mapper.mapOutput("UnsuppliedEnergy"), "default_mapping");
}

BOOST_AUTO_TEST_CASE(study_file_overrides_default_file)
{
    writeDefaultFile("UnsuppliedEnergy: default_mapping");
    writeStudyFile("UnsuppliedEnergy: study_mapping");
    const VariableNameMapper mapper(studyFolder, defaultFile);
    BOOST_CHECK_EQUAL(mapper.mapOutput("UnsuppliedEnergy"), "study_mapping");
}

BOOST_AUTO_TEST_CASE(throws_when_neither_study_nor_default_file_exists)
{
    BOOST_CHECK_THROW(VariableNameMapper(studyFolder, defaultFile), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(empty_study_file_leaves_every_name_unchanged)
{
    writeStudyFile("");
    const VariableNameMapper mapper(studyFolder, defaultFile);
    BOOST_CHECK_EQUAL(mapper.mapOutput("UnsuppliedEnergy"), "UnsuppliedEnergy");
}

BOOST_AUTO_TEST_CASE(default_constructed_mapper_leaves_every_name_unchanged)
{
    const VariableNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput("UnsuppliedEnergy"), "UnsuppliedEnergy");
    BOOST_CHECK_EQUAL(mapper.mapOutput(""), "");
}

BOOST_AUTO_TEST_SUITE_END()
