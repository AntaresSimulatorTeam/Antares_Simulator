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
#include <antares/logs/logs.h>
#include <antares/modeler/optimConfig/optimConfig.h>
#include "antares/io/inputs/yml-optim-config/converter.h"
#include "antares/io/inputs/yml-optim-config/parser.h"
#include "antares/solver/modeler/loadFiles/loadFiles.h"

namespace fs = std::filesystem;

namespace Antares::Solver::LoadFiles
{

std::unique_ptr<Modeler::Config::OptimConfig> loadOptimConfig(const fs::path& studyPath)
{
    std::string filename = "optim-config.yml";
    std::string optimConfigStr;
    try
    {
        optimConfigStr = IO::readFile(studyPath / "input" / filename);
    }
    catch (const std::runtime_error& e)
    {
        logs.error() << "Error while trying to read file " << filename;
        throw ErrorLoadingYaml(e.what());
    }

    IO::Inputs::YmlOptimConfig::Parser parser;
    IO::Inputs::YmlOptimConfig::OptimConfig optimConfigObj;
    try
    {
        optimConfigObj = parser.parse(optimConfigStr);
    }
    catch (const YAML::Exception& e)
    {
        handleYamlError(e, filename);
        throw ErrorLoadingYaml(e.what());
    }

    try
    {
        return std::make_unique<Modeler::Config::OptimConfig>(
          IO::Inputs::YmlOptimConfig::OptimConfigConverter::convert(optimConfigObj));
    }
    catch (const std::runtime_error& e)
    {
        logs.error() << "Error while converting optim config yaml: " << e.what();
        throw ErrorLoadingYaml(e.what());
    }
}

} // namespace Antares::Solver::LoadFiles
