// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <yaml-cpp/yaml.h>

#include <antares/io/file.h>
#include <antares/io/inputs/yml-system/converter.h>
#include <antares/io/inputs/yml-system/parser.h>
#include <antares/logs/logs.h>
#include "antares/solver/modeler/loadFiles/loadFiles.h"

namespace fs = std::filesystem;

namespace Antares::Solver::LoadFiles
{
using namespace IO::Inputs;

ModelerStudy::SystemModel::System loadSystem(
  const fs::path& studyPath,
  const std::vector<ModelerStudy::SystemModel::Library>& libraries)
{
    std::string filename = "system.yml";
    std::string systemStr;
    try
    {
        systemStr = IO::readFile(studyPath / "input" / filename);
    }
    catch (const std::runtime_error& e)
    {
        logs.error() << "Error while trying to read file system.yml";
        throw ErrorLoadingYaml(e.what());
    }

    YmlSystem::Parser parser;
    YmlSystem::System systemObj;
    try
    {
        systemObj = parser.parse(systemStr, (fs::path("input") / filename).string());
    }
    catch (const YAML::Exception& e)
    {
        handleYamlError(e, filename);
        throw ErrorLoadingYaml(e.what());
    }

    try
    {
        return SystemConverter::convert(systemObj, libraries);
    }
    catch (const std::runtime_error& e)
    {
        logs.error() << "Error while converting the system yaml to components";
        throw ErrorLoadingYaml(e.what());
    }
}

} // namespace Antares::Solver::LoadFiles
