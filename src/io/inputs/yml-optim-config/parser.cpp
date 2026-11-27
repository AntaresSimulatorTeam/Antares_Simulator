// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-optim-config/parser.h"

#include "antares/io/inputs/yml-optim-config/decoders.h"
#include "antares/io/inputs/yml-utils/checkFields.h"

using namespace Antares::IO::Inputs::YmlUtils;

namespace Antares::IO::Inputs::YmlOptimConfig
{
namespace
{
// Keep this schema in sync with field reads in decoders.cpp.
const std::vector<ExpectedField> locationFields = {{"id", true}, {"location", true}};

const std::vector<ExpectedField> optimConfigFields = {
  {"models",
   false,
   {{"id", true},
    {"model-decomposition",
     true,
     {{"variables", false, locationFields},
      {"constraints", false, locationFields},
      {"objective-contributions", false, locationFields}},
     Shape::Map},
    {"out-of-bounds-processing",
     false,
     {{"constraints", false, {{"id", true}, {"mode"}}}},
     Shape::Map}}},
  {"resolution-mode"},
  {"scenario-scope", false, {{"include"}, {"exclude"}}}};
} // namespace

OptimConfig Parser::parse(const std::string& content)
{
    YAML::Node root = YAML::Load(content);
    checkFields(root, optimConfigFields, "optim-config");
    return root.as<OptimConfig>();
}
} // namespace Antares::IO::Inputs::YmlOptimConfig
