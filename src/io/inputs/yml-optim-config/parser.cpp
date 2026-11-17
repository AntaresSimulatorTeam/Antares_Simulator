// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-optim-config/parser.h"

#include <fmt/format.h>

#include <antares/io/inputs/InputError.h>

#include "antares/io/inputs/yml-optim-config/decoders.h"
#include "antares/io/inputs/yml-utils/checkFields.h"

namespace Antares::IO::Inputs::YmlOptimConfig
{
namespace
{
// Keep this schema in sync with field reads in decoders.cpp.
const std::vector<YmlUtils::Field> locationFields = {{"id", true}, {"location", true}};

const std::vector<YmlUtils::Field> heuristicFields = {
  {"id", true},
  // the model element is named "id" or "value" (both spellings are used in the specification)
  {"inputs", false, {{"heuristic-element", true}, {"id"}, {"value"}, {"type"}}},
  {"outputs", false, {{"heuristic-element", true}, {"id", true}, {"type", true}}}};

const std::vector<YmlUtils::Field> optimConfigFields = {
  {"models",
   false,
   {{"id", true},
    // model-decomposition or heuristics is mandatory: checked by the Model decoder
    {"model-decomposition",
     false,
     {{"variables", false, locationFields},
      {"constraints", false, locationFields},
      {"objective-contributions", false, locationFields}},
     YmlUtils::Shape::Map},
    {"out-of-bounds-processing",
     false,
     {{"constraints", false, {{"id", true}, {"mode"}}}},
     YmlUtils::Shape::Map},
    {"heuristics", false, heuristicFields}}},
  {"resolution-mode"},
  {"scenario-scope", false, {{"include"}, {"exclude"}}}};

// A model needs model-decomposition or heuristics (or both): the schema above cannot express it
std::string checkModelSections(const YAML::Node& root)
{
    std::string errors;
    if (!root.IsMap())
    {
        return errors;
    }
    const auto models = root["models"];
    if (!models.IsDefined() || !models.IsSequence())
    {
        return errors;
    }
    for (const auto& model: models)
    {
        if (model.IsMap() && !model["model-decomposition"] && !model["heuristics"])
        {
            errors += fmt::format("Line {}: in 'optim-config/models':\n"
                                  "- Missing field: model-decomposition (or heuristics)\n",
                                  model.Mark().line + 1);
        }
    }
    return errors;
}
} // namespace

OptimConfig Parser::parse(const std::string& content)
{
    YAML::Node root = YAML::Load(content);
    const auto modelSectionErrors = checkModelSections(root);
    try
    {
        YmlUtils::checkFields(root, optimConfigFields, "optim-config");
    }
    catch (const InputError& e)
    {
        if (modelSectionErrors.empty())
        {
            throw;
        }
        throw InputError(std::string(e.what()) + "\n" + modelSectionErrors);
    }
    if (!modelSectionErrors.empty())
    {
        throw InputError(modelSectionErrors);
    }
    return root.as<OptimConfig>();
}
} // namespace Antares::IO::Inputs::YmlOptimConfig
