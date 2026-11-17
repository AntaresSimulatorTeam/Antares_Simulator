// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
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
        auto lib = YmlModel::Parser::parse(libraryStr);
        lib.filename = filePath.string();
        return lib;
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

Solver::ScenarioScope convertScenarioScope(const OptimConfig& ymlOptimConfig)
{
    if (!ymlOptimConfig.scenario_scope.has_value())
    {
        return {};
    }
    const auto& ymlScope = ymlOptimConfig.scenario_scope.value();
    Solver::ScenarioScope scope;
    scope.include = ymlScope.include;
    scope.exclude = ymlScope.exclude;
    return scope;
}
} // namespace

namespace
{
SystemModel::HeuristicType convertHeuristicType(YmlOptimConfig::HeuristicType type)
{
    return type == YmlOptimConfig::HeuristicType::Fast ? SystemModel::HeuristicType::Fast
                                                       : SystemModel::HeuristicType::Accurate;
}

SystemModel::HeuristicElement convertHeuristicElement(YmlOptimConfig::HeuristicElement element)
{
    using In = YmlOptimConfig::HeuristicElement;
    using Out = SystemModel::HeuristicElement;
    switch (element)
    {
    case In::NumUnitsOnOpt:
        return Out::NumUnitsOnOpt;
    case In::NumUnitsMax:
        return Out::NumUnitsMax;
    case In::MinUpDuration:
        return Out::MinUpDuration;
    case In::MinDownDuration:
        return Out::MinDownDuration;
    case In::GenerationPower:
        return Out::GenerationPower;
    case In::ClusterMaxGeneration:
        return Out::ClusterMaxGeneration;
    case In::MinPowerPerUnit:
        return Out::MinPowerPerUnit;
    case In::MaxPowerPerUnit:
        return Out::MaxPowerPerUnit;
    case In::MinimumNumUnitsOn:
        return Out::MinimumNumUnitsOn;
    case In::MinimumGenerationPower:
        return Out::MinimumGenerationPower;
    }
    throw Error::InvalidArgumentError("Unknown heuristic element");
}

SystemModel::HeuristicInputSource convertHeuristicInputSource(
  YmlOptimConfig::HeuristicInputType type)
{
    switch (type)
    {
    case YmlOptimConfig::HeuristicInputType::VariableSolution:
        return SystemModel::HeuristicInputSource::VariableSolution;
    case YmlOptimConfig::HeuristicInputType::VariableLowerBound:
        return SystemModel::HeuristicInputSource::VariableLowerBound;
    case YmlOptimConfig::HeuristicInputType::VariableUpperBound:
        return SystemModel::HeuristicInputSource::VariableUpperBound;
    case YmlOptimConfig::HeuristicInputType::Parameter:
        return SystemModel::HeuristicInputSource::Parameter;
    }
    throw Error::InvalidArgumentError("Unknown heuristic input type");
}

SystemModel::HeuristicOutputTarget convertHeuristicOutputTarget(
  YmlOptimConfig::HeuristicOutputType type)
{
    return type == YmlOptimConfig::HeuristicOutputType::VariableLowerBound
             ? SystemModel::HeuristicOutputTarget::VariableLowerBound
             : SystemModel::HeuristicOutputTarget::VariableUpperBound;
}

const SystemModel::Model& findSystemModel(const std::string& fullModelId,
                                          const std::vector<SystemModel::Library>& libraries)
{
    const auto [libId, modelId] = splitModelId(fullModelId);
    for (const auto& lib: libraries)
    {
        if (lib.Id() != libId)
        {
            continue;
        }
        if (auto it = lib.Models().find(modelId); it != lib.Models().end())
        {
            return it->second;
        }
    }
    throw Error::InvalidArgumentError("No model '" + fullModelId + "' found.");
}

// Checks that the model element bound to a heuristic element exists and has the expected time
// dependency
void checkHeuristicElementBinding(const SystemModel::Model& model,
                                  const std::string& errorPrefix,
                                  SystemModel::HeuristicElement element,
                                  const std::string& id,
                                  bool isParameter)
{
    const auto prefix = errorPrefix + "'" + SystemModel::toString(element) + "': ";
    bool timeDependent = false;
    if (isParameter)
    {
        const auto it = model.Parameters().find(id);
        if (it == model.Parameters().end())
        {
            throw Error::InvalidArgumentError(prefix + "no parameter '" + id + "' in the model");
        }
        timeDependent = it->second.isTimeDependent();
    }
    else
    {
        const auto it = std::ranges::find(model.Variables(), id, &SystemModel::Variable::Id);
        if (it == model.Variables().end())
        {
            throw Error::InvalidArgumentError(prefix + "no variable '" + id + "' in the model");
        }
        timeDependent = it->isTimeDependent();
    }

    switch (SystemModel::expectedTimeDependency(element))
    {
    case SystemModel::HeuristicElementTimeDependency::Constant:
        // Model parameters are time-dependent by default: the value given by each component is
        // checked when the system is loaded (see checkIntegerStrategies)
        if (timeDependent && !isParameter)
        {
            throw Error::InvalidArgumentError(prefix + "'" + id + "' must not be time-dependent");
        }
        break;
    case SystemModel::HeuristicElementTimeDependency::TimeDependent:
        if (!timeDependent)
        {
            throw Error::InvalidArgumentError(prefix + "'" + id + "' must be time-dependent");
        }
        break;
    case SystemModel::HeuristicElementTimeDependency::Any:
        break;
    }
}

SystemModel::HeuristicDefinition convertHeuristic(const YmlOptimConfig::Heuristic& ymlHeuristic,
                                                  const SystemModel::Model& model,
                                                  const std::string& fullModelId)
{
    SystemModel::HeuristicDefinition heuristic{.type = convertHeuristicType(ymlHeuristic.type)};
    const auto errorPrefix = fmt::format("optim-config.yml: model '{}', heuristic '{}': ",
                                         fullModelId,
                                         ymlHeuristic.id);

    const auto expectedInputs = SystemModel::mandatoryInputs(heuristic.type);
    for (const auto& ymlInput: ymlHeuristic.inputs)
    {
        const auto element = convertHeuristicElement(ymlInput.heuristic_element);
        if (std::ranges::find(expectedInputs, element) == expectedInputs.end())
        {
            throw Error::InvalidArgumentError(errorPrefix + "'" + SystemModel::toString(element)
                                              + "' is not an input of this heuristic");
        }
        if (heuristic.input(element))
        {
            throw Error::InvalidArgumentError(errorPrefix + "input '"
                                              + SystemModel::toString(element)
                                              + "' is defined several times");
        }
        const auto source = convertHeuristicInputSource(ymlInput.type);
        checkHeuristicElementBinding(model,
                                     errorPrefix,
                                     element,
                                     ymlInput.id,
                                     source == SystemModel::HeuristicInputSource::Parameter);
        heuristic.inputs.push_back({.element = element, .id = ymlInput.id, .source = source});
    }

    const auto expectedOutputs = SystemModel::mandatoryOutputs(heuristic.type);
    for (const auto& ymlOutput: ymlHeuristic.outputs)
    {
        const auto element = convertHeuristicElement(ymlOutput.heuristic_element);
        if (std::ranges::find(expectedOutputs, element) == expectedOutputs.end())
        {
            throw Error::InvalidArgumentError(errorPrefix + "'" + SystemModel::toString(element)
                                              + "' is not an output of this heuristic");
        }
        if (heuristic.output(element))
        {
            throw Error::InvalidArgumentError(errorPrefix + "output '"
                                              + SystemModel::toString(element)
                                              + "' is defined several times");
        }
        checkHeuristicElementBinding(model, errorPrefix, element, ymlOutput.id, false);
        heuristic.outputs.push_back({.element = element,
                                     .id = ymlOutput.id,
                                     .target = convertHeuristicOutputTarget(ymlOutput.type)});
    }

    for (const auto element: expectedInputs)
    {
        if (!heuristic.input(element))
        {
            throw Error::InvalidArgumentError(errorPrefix + "missing input '"
                                              + SystemModel::toString(element) + "'");
        }
    }
    for (const auto element: expectedOutputs)
    {
        if (!heuristic.output(element))
        {
            throw Error::InvalidArgumentError(errorPrefix + "missing output '"
                                              + SystemModel::toString(element) + "'");
        }
    }
    return heuristic;
}
} // namespace

HeuristicsByModel convertHeuristics(const OptimConfig& ymlOptimConfig,
                                    const std::vector<SystemModel::Library>& libraries)
{
    HeuristicsByModel heuristicsByModel;
    for (const auto& optimConfigModel: ymlOptimConfig.models)
    {
        if (optimConfigModel.heuristics.empty())
        {
            continue;
        }
        const auto& model = findSystemModel(optimConfigModel.id, libraries);
        auto& heuristics = heuristicsByModel[optimConfigModel.id];
        for (const auto& ymlHeuristic: optimConfigModel.heuristics)
        {
            auto heuristic = convertHeuristic(ymlHeuristic, model, optimConfigModel.id);
            if (std::ranges::find(heuristics,
                                  heuristic.type,
                                  &SystemModel::HeuristicDefinition::type)
                != heuristics.end())
            {
                throw Error::InvalidArgumentError(
                  fmt::format("optim-config.yml: model '{}': heuristic '{}' is defined several "
                              "times",
                              optimConfigModel.id,
                              ymlHeuristic.id));
            }
            heuristics.push_back(std::move(heuristic));
        }
    }
    return heuristicsByModel;
}

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

std::optional<LoadedLibraries> loadLibraries(const fs::path& studyPath)
{
    auto ymlLibraries = loadLibrariesFromYaml(studyPath);
    if (ymlLibraries.has_value())
    {
        const auto ymlOptimConfig = loadOptimConfigFromYaml(studyPath);
        updateLibrariesWithOptimConfig(ymlLibraries.value(), ymlOptimConfig);
        LoadedLibraries loadedLibraries;
        loadedLibraries.libraries = convertIntoSystemLibs(ymlLibraries.value());
        loadedLibraries.resolutionMode = convertResolutionMode(ymlOptimConfig.resolution_mode);
        loadedLibraries.scenarioScope = convertScenarioScope(ymlOptimConfig);
        loadedLibraries.heuristicsByModel = convertHeuristics(ymlOptimConfig,
                                                              loadedLibraries.libraries);
        return loadedLibraries;
    }
    return {};
}
} // namespace Antares::Solver::LoadFiles
