// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/modeler/loadFiles/readOptimConfig.h"

#include <yaml-cpp/yaml.h>

#include <antares/io/file.h>
#include <antares/logs/logs.h>
#include "antares/io/inputs/yml-optim-config/parser.h"
#include "antares/solver/modeler/loadFiles/loadFiles.h"

namespace fs = std::filesystem;
using namespace Antares::IO::Inputs;

namespace Antares::Solver::LoadFiles
{

static std::string readOptimConfigFile(const fs::path& configPath)
{
    try
    {
        return IO::readFile(configPath);
    }
    catch (const std::runtime_error& e)
    {
        logs.error() << "Error while trying to read file " << configPath;
        throw ErrorLoadingYaml(e.what());
    }
}

static YmlOptimConfig::OptimConfig parseOptimConfig(const std::string& content,
                                                    const fs::path& configPath)
{
    YmlOptimConfig::Parser parser;
    try
    {
        return parser.parse(content);
    }
    catch (const YAML::Exception& e)
    {
        handleYamlError(e, configPath.string());
        throw ErrorLoadingYaml(e.what());
    }
}

YmlOptimConfig::OptimConfig loadOptimConfigFromYaml(const fs::path& studyPath)
{
    const fs::path configPath = studyPath / "input" / "optim-config.yml";
    if (!std::filesystem::exists(configPath))
    {
        logs.info() << "Optim config file not found at " << configPath;
        return {};
    }

    std::string content = readOptimConfigFile(configPath);
    return parseOptimConfig(content, configPath);
}

} // namespace Antares::Solver::LoadFiles
