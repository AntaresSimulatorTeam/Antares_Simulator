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

#pragma once

#include <filesystem>
#include <vector>
#include <yaml-cpp/yaml.h>

#include <antares/solver/modeler/parameters/modelerParameters.h>
#include <antares/study/system-model/library.h>
#include <antares/study/system-model/system.h>

namespace Antares::Solver::LoadFiles
{

ModelerParameters loadParameters(const std::filesystem::path& studyPath);

std::vector<Study::SystemModel::Library> loadLibraries(const std::filesystem::path& studyPath);

Study::SystemModel::System loadSystem(const std::filesystem::path& studyPath,
                                      const std::vector<Study::SystemModel::Library>& libraries);

void handleYamlError(const YAML::Exception& e, const std::string& context);

/// Generic error class for all loading errors to catch in the main
class ErrorLoadingYaml: public std::runtime_error
{
public:
    explicit ErrorLoadingYaml(const std::string& s):
        runtime_error(s)
    {
    }
};

} // namespace Antares::Solver::LoadFiles
