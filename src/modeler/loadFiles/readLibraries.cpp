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

#include <yaml-cpp/yaml.h>

#include <antares/io/file.h>
#include <antares/io/inputs/model-converter/modelConverter.h>
#include <antares/io/inputs/yml-model/parser.h>
#include <antares/logs/logs.h>
#include "antares/io/inputs/yml-optim-config/OptimConfig.h"
#include "antares/solver/modeler/loadFiles/loadFiles.h"

using namespace Antares::ModelerStudy;
using namespace Antares::IO::Inputs;
using namespace Antares::IO::Inputs::YmlOptimConfig;
namespace fs = std::filesystem;

// gp : declared here because can't be declared in loadFiles.h (which refuses to load OptimConfig.h)
// gp : TODO : this will have to be improved.
namespace Antares::Solver::LoadFiles
{
OptimConfig loadOptimConfig(const std::filesystem::path& studyPath);
}

namespace Antares::Solver::LoadFiles
{

static YmlModel::Library loadSingleLibrary(const fs::path& filePath)
{
    std::string libraryStr;
    try
    {
        libraryStr = IO::readFile(filePath);
    }
    catch (const std::runtime_error& e)
    {
        logs.error() << "Error while trying to read this library file: " << filePath;
        throw ErrorLoadingYaml(e.what());
    }

    YmlModel::Parser parser;
    YmlModel::Library libraryObj;

    try
    {
        return parser.parse(libraryStr);
    }
    catch (const YAML::Exception& e)
    {
        handleYamlError(e, filePath.string());
        throw ErrorLoadingYaml(e.what());
    }
}

std::vector<SystemModel::Library> convertIntoSystemLibs(const std::vector<YmlModel::Library>& libs)
{
    std::vector<SystemModel::Library> libraries;
    for (const auto& lib: libs)
    {
        try
        {
            libraries.push_back(ModelConverter::convert(lib));
        }
        catch (const std::runtime_error& e)
        {
            logs.error() << "Error while converting library : " << lib.id << ": " << e.what();
            throw ErrorLoadingYaml(e.what());
        }

        logs.info() << "Library loaded: " << libraries.back().Id();
    }
    return libraries;
}

std::vector<SystemModel::Library> loadLibraries(const fs::path& studyPath,
                                                const OptimConfig& optimConfig)
{
    std::vector<YmlModel::Library> yml_libs;
    const fs::path directoryPath = studyPath / "input" / "model-libraries";
    for (const auto& entry: fs::directory_iterator(directoryPath))
    {
        if (entry.path().extension() != ".yml")
        {
            logs.info() << entry.path()
                        << " ignored, only files having the `.yml` extension are loaded";
            continue;
        }

        yml_libs.push_back(loadSingleLibrary(entry.path()));
    }

    return convertIntoSystemLibs(yml_libs);
}

std::vector<SystemModel::Library> loadLibraries(const fs::path& studyPath)
{
    // First we extract the optim config from its own yaml file.
    auto optimConfig = loadOptimConfig(studyPath);

    // Then we load libraries, updating them with the optim config.
    return loadLibraries(studyPath, optimConfig);
}
} // namespace Antares::Solver::LoadFiles
