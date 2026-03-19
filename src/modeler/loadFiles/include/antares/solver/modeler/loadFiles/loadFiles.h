// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <vector>
#include <yaml-cpp/yaml.h>

#include <antares/solver/modeler/ModelerData.h>
#include <antares/solver/modeler/parameters/modelerParameters.h>
#include <antares/study/system-model/library.h>
#include <antares/study/system-model/system.h>

namespace Antares::Solver::LoadFiles
{

/// Load the libraries, system and dataseries
std::optional<ModelerData> loadAll(const std::filesystem::path& studyPath);

ModelerParameters loadParameters(const std::filesystem::path& studyPath);

std::optional<std::pair<std::vector<ModelerStudy::SystemModel::Library>, ResolutionMode>>
loadLibraries(const std::filesystem::path& studyPath);

ModelerStudy::SystemModel::System loadSystem(
  const std::filesystem::path& studyPath,
  const std::vector<ModelerStudy::SystemModel::Library>& libraries);

std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblemData> loadDataSeries(
  const std::filesystem::path& studyPath);

Optimisation::ScenarioGroupRepository loadScenarioGroupRepository(
  const std::filesystem::path& studyPath);

void handleYamlError(const YAML::Exception& e, const std::string& filename);

/// Generic error class for all loading errors to catch in the main
class ErrorLoadingYaml final: public std::runtime_error
{
public:
    explicit ErrorLoadingYaml(const std::string& s):
        runtime_error(s)
    {
    }
};

} // namespace Antares::Solver::LoadFiles
