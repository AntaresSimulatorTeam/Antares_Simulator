// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <string>
#include <yaml-cpp/yaml.h>

#include <boost/algorithm/string.hpp>

#include <antares/io/file.h>
#include <antares/io/inputs/model-converter/modelConverter.h>
#include <antares/io/inputs/yml-model/parser.h>
#include <antares/logs/logs.h>
#include "antares/io/inputs/yml-optim-config/OptimConfig.h"
#include "antares/solver/modeler/loadFiles/loadFiles.h"
#include "antares/solver/modeler/loadFiles/readOptimConfig.h"

using namespace Antares::ModelerStudy;
using namespace Antares::IO::Inputs;
namespace fs = std::filesystem;

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

std::vector<YmlModel::Library> loadLibrariesFromYaml(const fs::path& studyPath)
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
    return yml_libs;
}

static std::pair<std::string, std::string> splitModelId(const std::string& modelId)
{
    std::vector<std::string> result;
    boost::split(result, modelId, boost::is_any_of("."));
    return {result[0], result[1]}; // We assume the format is always libraryId.modelId
}

YmlModel::Model& fetchModelInLibrairies(const YmlOptimConfig::Model& optimConfigModel,
                                        std::vector<YmlModel::Library>& ymlLibs)
{
    const auto [libId, modelId] = splitModelId(optimConfigModel.id);

    auto lib = std::ranges::find_if(ymlLibs, [&libId](const auto& l) { return l.id == libId; });
    if (lib == ymlLibs.end())
    {
        throw std::runtime_error("No library '" + libId + "' found.");
    }

    auto modelIt = std::ranges::find_if(lib->models,
                                        [&modelId](const auto& m) { return m.id == modelId; });
    if (modelIt == lib->models.end())
    {
        throw std::runtime_error("No model '" + libId + "." + modelId + "' found.");
    }

    return *modelIt;
}

template<class U, class V>
void update(std::vector<U>& out, const std::vector<V>& yml_in, const std::string& elementType)
{
    for (const auto& ymlOptCfg: yml_in)
    {
        auto predicate = [&ymlOptCfg](const auto& v) { return v.id == ymlOptCfg.id; };
        auto element = std::ranges::find_if(out, predicate);
        if (element == out.end())
        {
            throw std::runtime_error("No " + elementType + " '" + ymlOptCfg.id + "' found.");
        }
        element->location = ymlOptCfg.location;
    }
}

void updateSystemModel(YmlModel::Model& model, const YmlOptimConfig::Model& optimConfigModel)
{
    update(model.variables, optimConfigModel.variables, "variable");
    update(model.constraints, optimConfigModel.constraints, "constraint");
    update(model.objectives, optimConfigModel.objectives, "objective");
}

void updateLibrariesWithOptimConfig(std::vector<YmlModel::Library>& ymlLibs,
                                    const YmlOptimConfig::OptimConfig& ymlOptimConfig)
{
    for (const auto& optimConfigModel: ymlOptimConfig)
    {
        auto& model = fetchModelInLibrairies(optimConfigModel, ymlLibs);
        updateSystemModel(model, optimConfigModel);
    }
}

std::vector<SystemModel::Library> loadLibraries(const fs::path& studyPath)
{
    auto ymlLibraries = loadLibrariesFromYaml(studyPath);
    const auto ymlOptimConfig = loadOptimConfigFromYaml(studyPath);
    updateLibrariesWithOptimConfig(ymlLibraries, ymlOptimConfig);
    return convertIntoSystemLibs(ymlLibraries);
}
} // namespace Antares::Solver::LoadFiles
