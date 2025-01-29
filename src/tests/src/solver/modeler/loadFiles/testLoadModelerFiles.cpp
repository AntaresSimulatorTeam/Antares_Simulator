/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#include <fstream>
#define BOOST_TEST_MODULE load modeler files

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/loadFiles/loadFiles.h>

#include "files-system.h"

BOOST_AUTO_TEST_SUITE(test_modeler_files_loading)

namespace fs = std::filesystem;

struct FixtureLoadFile
{
    fs::path studyPath;
    fs::path inputPath;
    fs::path libraryDirPath;

    FixtureLoadFile()
    {
        studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
        inputPath = createFolder(studyPath.string(), "input");
        libraryDirPath = createFolder(inputPath.string(), "model-libraries");
    }

    ~FixtureLoadFile()
    {
        fs::remove_all(studyPath);
    }
};

BOOST_AUTO_TEST_CASE(files_not_existing)
{
    fs::path studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::vector<Antares::Study::SystemModel::Library> libraries;

    BOOST_CHECK_THROW(Antares::Solver::LoadFiles::loadLibraries(studyPath), std::runtime_error);
    BOOST_CHECK_THROW(Antares::Solver::LoadFiles::loadSystem(studyPath, libraries),
                      std::runtime_error);
}

BOOST_FIXTURE_TEST_CASE(read_one_lib_treile, FixtureLoadFile)
{
    std::ofstream libStream(libraryDirPath / "simple.yml");
    libStream << R"(
        library:
            id: lib_id
            description: lib_description
            port-types: []
            models: []
    )";
    libStream.close();

    auto libraries = Antares::Solver::LoadFiles::loadLibraries(studyPath);
    BOOST_CHECK_EQUAL(libraries[0].Id(), "lib_id");
}

BOOST_FIXTURE_TEST_CASE(dont_read_bad_extension, FixtureLoadFile)
{
    createFile(libraryDirPath.string(), "abc.txt");
    auto libraries = Antares::Solver::LoadFiles::loadLibraries(studyPath);
    BOOST_CHECK(libraries.empty());
}

BOOST_FIXTURE_TEST_CASE(incorrect_library, FixtureLoadFile)
{
    std::ofstream libStream(libraryDirPath / "simple.yml");
    libStream << R"(
        library:
            port-types: []
            models: []
    )";
    libStream.close();

    BOOST_CHECK_THROW(Antares::Solver::LoadFiles::loadLibraries(studyPath), std::runtime_error);
}

BOOST_FIXTURE_TEST_CASE(incorrect_library2, FixtureLoadFile)
{
    std::ofstream libStream(libraryDirPath / "simple.yml");
    libStream << R"(
        library:
            id: std
            port-types: []
                - id: generator
                  description: A basic generator model

    )";
    libStream.close();
    BOOST_CHECK_THROW(Antares::Solver::LoadFiles::loadLibraries(studyPath), std::runtime_error);
}

BOOST_FIXTURE_TEST_CASE(read_several_lib_file, FixtureLoadFile)
{
    std::ofstream libStream(libraryDirPath / "simple.yml");
    libStream << R"(
        library:
            id: lib_id
            description: lib_description
            port-types: []
            models: []
    )";
    libStream.close();

    std::ofstream libStream2(libraryDirPath / "2.yml");
    libStream2 << R"(
        library:
            id: lib_id2
            description: lib_description
            port-types: []
            models: []
    )";
    libStream2.close();

    std::ofstream libStream3(libraryDirPath / "3.yml");
    libStream3 << R"(
        library:
            id: lib_id3
            description: lib_description
            port-types: []
            models: []
    )";
    libStream3.close();

    auto libraries = Antares::Solver::LoadFiles::loadLibraries(studyPath);

    auto checkLibIdInVector = [&libraries](const std::string& libId)
    {
        return std::ranges::find_if(libraries, [&libId](const auto& l) { return l.Id() == libId; })
               != libraries.end();
    };

    BOOST_CHECK(checkLibIdInVector("lib_id"));
    BOOST_CHECK(checkLibIdInVector("lib_id2"));
    BOOST_CHECK(checkLibIdInVector("lib_id3"));

    BOOST_CHECK(!checkLibIdInVector("id not in vector"));
}

BOOST_FIXTURE_TEST_CASE(read_system_file, FixtureLoadFile)
{
    std::ofstream libStream(libraryDirPath / "simple.yml");
    libStream << R"(
        library:
            id: std
            description: lib_description
            port-types: []
            models:
                - id: generator
                  description: A basic generator model

    )";
    libStream.close();

    std::ofstream systemStream(inputPath / "system.yml");
    systemStream << R"(
        system:
            id: base_system
            description: two components
            components:
                - id: N
                  model: std.generator
                  scenario-group: group-234
    )";
    systemStream.close();

    auto libraries = Antares::Solver::LoadFiles::loadLibraries(studyPath);
    auto system = Antares::Solver::LoadFiles::loadSystem(studyPath, libraries);
}

BOOST_FIXTURE_TEST_CASE(read_invalid_system_file, FixtureLoadFile)
{
    std::ofstream libStream(libraryDirPath / "simple.yml");
    libStream << R"(
        library:
            id: std
            description: lib_description
            port-types: []
            models:
                - id: generator
                  description: A basic generator model

    )";
    libStream.close();

    std::ofstream systemStream(inputPath / "system.yml");
    systemStream << R"(
        system:
    )";
    systemStream.close();

    auto libraries = Antares::Solver::LoadFiles::loadLibraries(studyPath);
    BOOST_CHECK_THROW(Antares::Solver::LoadFiles::loadSystem(studyPath, libraries),
                      std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
