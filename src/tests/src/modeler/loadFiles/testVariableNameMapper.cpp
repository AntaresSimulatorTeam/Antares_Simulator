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
        studyFolder = fs::temp_directory_path()
                      / fs::path("variable-names-test-" + std::to_string(unique));
        inputFolder = studyFolder / "input";
        fs::create_directories(inputFolder);
    }

    ~VariableNamesFixture()
    {
        fs::remove_all(studyFolder);
    }

    void writeVariableNamesFile(const std::string& content)
    {
        std::ofstream out(inputFolder / "variable-names.yml",
                          std::ofstream::trunc | std::ofstream::out);
        out << content;
    }

    fs::path studyFolder;
    fs::path inputFolder;
};

BOOST_FIXTURE_TEST_SUITE(test_variable_name_mapper, VariableNamesFixture)

BOOST_AUTO_TEST_CASE(loads_and_maps_known_names)
{
    writeVariableNamesFile(R"(UnsuppliedEnergy: unsupplied_energy
Spillage: spillaged_energy)");

    const VariableNameMapper mapper(studyFolder);

    BOOST_CHECK_EQUAL(mapper.mapOutput("UnsuppliedEnergy"), "unsupplied_energy");
    BOOST_CHECK_EQUAL(mapper.mapOutput("Spillage"), "spillaged_energy");
}

BOOST_AUTO_TEST_CASE(unknown_name_is_returned_unchanged)
{
    writeVariableNamesFile("UnsuppliedEnergy: unsupplied_energy");
    const VariableNameMapper mapper(studyFolder);
    BOOST_CHECK_EQUAL(mapper.mapOutput("HydProd"), "HydProd");
}

BOOST_AUTO_TEST_CASE(mapping_is_case_sensitive)
{
    writeVariableNamesFile("UnsuppliedEnergy: unsupplied_energy");
    const VariableNameMapper mapper(studyFolder);
    BOOST_CHECK_EQUAL(mapper.mapOutput("unsuppliedenergy"), "unsuppliedenergy");
    BOOST_CHECK_EQUAL(mapper.mapOutput("UNSUPPLIEDENERGY"), "UNSUPPLIEDENERGY");
}

BOOST_AUTO_TEST_CASE(missing_file_leaves_every_name_unchanged)
{
    const VariableNameMapper mapper(studyFolder);
    BOOST_CHECK_EQUAL(mapper.mapOutput("UnsuppliedEnergy"), "UnsuppliedEnergy");
    BOOST_CHECK_EQUAL(mapper.mapOutput(""), "");
}

BOOST_AUTO_TEST_CASE(empty_file_leaves_every_name_unchanged)
{
    writeVariableNamesFile("");
    const VariableNameMapper mapper(studyFolder);
    BOOST_CHECK_EQUAL(mapper.mapOutput("UnsuppliedEnergy"), "UnsuppliedEnergy");
}

BOOST_AUTO_TEST_CASE(default_constructed_mapper_leaves_every_name_unchanged)
{
    const VariableNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput("UnsuppliedEnergy"), "UnsuppliedEnergy");
    BOOST_CHECK_EQUAL(mapper.mapOutput(""), "");
}

BOOST_AUTO_TEST_SUITE_END()
