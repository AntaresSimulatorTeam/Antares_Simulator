// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-optim-config/parser.h"

#include "antares/io/inputs/yml-optim-config/decoders.h"
#include "antares/io/inputs/yml-utils/checkFields.h"

namespace Antares::IO::Inputs::YmlOptimConfig
{
namespace
{
const std::vector<YmlUtils::Field> locationFields = {{"id", true}, {"location", true}};

const std::vector<YmlUtils::Field> optimConfigFields = {
  {"models",
   false,
   {{"id", true},
    {"model-decomposition",
     false,
     {{"variables", false, locationFields},
      {"constraints", false, locationFields},
      {"objective-contributions", false, locationFields}}},
    {"out-of-bounds-processing", false, {{"constraints", false, {{"id", true}, {"mode"}}}}}}},
  {"resolution-mode"},
  {"scenario-scope", false, {{"include"}, {"exclude"}}}};
} // namespace

OptimConfig Parser::parse(const std::string& content)
{
    YAML::Node root = YAML::Load(content);
    YmlUtils::checkFields(root, optimConfigFields, "optim-config");
    return root.as<OptimConfig>();
}
} // namespace Antares::IO::Inputs::YmlOptimConfig
