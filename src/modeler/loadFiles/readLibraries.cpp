// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <fmt/format.h>
#include <string>
#include <yaml-cpp/yaml.h>

#include <boost/algorithm/string.hpp>

#include <antares/io/file.h>
#include <antares/io/inputs/model-converter/modelConverter.h>
#include <antares/io/inputs/yml-model/parser.h>
#include <antares/logs/logs.h>
#include "antares/exception/InvalidArgumentError.hpp"
#include "antares/exception/LoadingError.hpp"
#include "antares/io/inputs/yml-optim-config/OptimConfig.h"
#include "antares/solver/modeler/loadFiles/loadFiles.h"
#include "antares/solver/modeler/loadFiles/readOptimConfig.h"

using namespace Antares::ModelerStudy;
using namespace Antares::IO::Inputs;
namespace fs = std::filesystem;

namespace Antares::Solver::LoadFiles
{
YmlModel::Library loadSingleLibrary(const fs::path& filePath)
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

    try
    {
        return YmlModel::Parser::parse(libraryStr);
    }
    catch (const YAML::Exception& e)
    {
        auto&& err = markYamlError(e, filePath.string());
        throw Error::LoadingError(err);
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

std::optional<std::vector<YmlModel::Library>> loadLibrariesFromYaml(const fs::path& studyPath)
{
    std::vector<YmlModel::Library> yml_libs;
    const fs::path directoryPath = studyPath / "input" / "model-libraries";
    if (!fs::exists(directoryPath))
    {
        logs.warning() << "Model library missing";
        return {};
    }

    warnOnYamlFiles(directoryPath);

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

std::pair<std::string, std::string> splitModelId(const std::string& modelId)
{
    std::vector<std::string> result;
    boost::split(result, modelId, boost::is_any_of("."));
    return {result[0], result[1]}; // We assume the format is always libraryId.modelId
}

YmlModel::Model& fetchModelInLibrairies(const Model& optimConfigModel,
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

void updateSystemModel(YmlModel::Model& model, const Model& optimConfigModel)
{
    update(model.variables, optimConfigModel.variables, "variable");
    update(model.constraints, optimConfigModel.constraints, "constraint");
    update(model.objectives, optimConfigModel.objectives, "objective");
}

namespace
{
std::string normalizeOutOfBoundsProcessingMode(const std::string& mode)
{
    if (mode.empty() || mode == "cyclic")
    {
        return "cyclic";
    }
    if (mode == "drop")
    {
        return "drop";
    }
    throw Error::InvalidArgumentError(
      fmt::format("Invalid out-of-bounds processing mode in optim-config.yaml: {}", mode));
}

void updateConstraintOutOfBoundsProcessing(YmlModel::Model& model, const Model& optimConfigModel)
{
    for (const auto& ymlOptCfg: optimConfigModel.constraints_out_of_bounds_processing)
    {
        auto predicate = [&ymlOptCfg](const auto& v) { return v.id == ymlOptCfg.id; };
        auto element = std::ranges::find_if(model.constraints, predicate);
        if (element != model.constraints.end())
        {
            element->out_of_bounds_processing_mode = normalizeOutOfBoundsProcessingMode(
              ymlOptCfg.mode);
            continue;
        }

        auto bindingConstraint = std::ranges::find_if(model.binding_constraints, predicate);
        if (bindingConstraint != model.binding_constraints.end())
        {
            bindingConstraint->out_of_bounds_processing_mode = normalizeOutOfBoundsProcessingMode(
              ymlOptCfg.mode);
            continue;
        }

        throw Error::InvalidArgumentError("No constraint '" + ymlOptCfg.id + "' found.");
    }
}

Solver::ResolutionMode convertResolutionMode(std::string ymlMode)
{
    if (ymlMode == "benders-decomposition")
    {
        return Solver::ResolutionMode::BENDERS_DECOMPOSITION;
    }
    if (ymlMode == "sequential-subproblems" || ymlMode.empty())
    {
        return Solver::ResolutionMode::SEQUENTIAL_SUBPROBLEMS;
    }
    throw Error::InvalidArgumentError(
      fmt::format("Invalid resolution mode in optim-config.yaml: {}", ymlMode));
}
} // namespace

void updateLibrariesWithOptimConfig(std::vector<YmlModel::Library>& ymlLibs,
                                    const OptimConfig& ymlOptimConfig)
{
    for (const auto& optimConfigModel: ymlOptimConfig.models)
    {
        auto& model = fetchModelInLibrairies(optimConfigModel, ymlLibs);
        updateSystemModel(model, optimConfigModel);
        updateConstraintOutOfBoundsProcessing(model, optimConfigModel);
    }
}

std::optional<std::pair<std::vector<SystemModel::Library>, ResolutionMode>> loadLibraries(
  const fs::path& studyPath)
{
    auto ymlLibraries = loadLibrariesFromYaml(studyPath);
    if (ymlLibraries.has_value())
    {
        const auto ymlOptimConfig = loadOptimConfigFromYaml(studyPath);
        updateLibrariesWithOptimConfig(ymlLibraries.value(), ymlOptimConfig);
        return std::make_optional<std::pair<std::vector<SystemModel::Library>, ResolutionMode>>(
          convertIntoSystemLibs(ymlLibraries.value()),
          convertResolutionMode(ymlOptimConfig.resolution_mode));
    }
    return {};
}
} // namespace Antares::Solver::LoadFiles
