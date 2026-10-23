// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-model/parser.h"

#include "antares/io/inputs/yml-model/Library.h"
#include "antares/io/inputs/yml-model/decoders.h"
#include "antares/io/inputs/yml-utils/checkFields.h"

namespace Antares::IO::Inputs::YmlModel
{
namespace
{
// Keep this schema in sync with field reads in decoders.cpp.
const std::vector<YmlUtils::Field> expressionFields = {{"id", true},
                                                       {"expression", true},
                                                       {"location"}};

const std::vector<YmlUtils::Field> libraryFields = {
  {"id", true},
  {"description"},
  {"port-types",
   false,
   {{"id", true},
    {"description"},
    {"fields", false, {{"id", true}}},
    {"area-connection",
     false,
     {{"injection-to-balance", true},
      {"spillage-bound", true},
      {"unsupplied-energy-bound", true},
      {"price"}}},
    {"thermal-capacity-connection", false, {{"capacity-field", true}}}}},
  {"models",
   false,
   {{"id", true},
    {"description"},
    {"parameters", false, {{"id", true}, {"time-dependent"}, {"scenario-dependent"}}},
    {"variables",
     false,
     {{"id", true},
      {"lower-bound"},
      {"upper-bound"},
      {"variable-type"},
      {"time-dependent"},
      {"scenario-dependent"},
      {"location"}}},
    {"ports", false, {{"id", true}, {"type", true}}},
    {"port-field-definitions", false, {{"port", true}, {"field", true}, {"definition", true}}},
    {"constraints", false, expressionFields},
    {"binding-constraints", false, expressionFields},
    {"objective-contributions", false, expressionFields},
    {"extra-outputs", false, {{"id", true}, {"expression", true}}}}}};
} // anonymous namespace

Library Parser::parse(const std::string& content)
{
    YAML::Node root = YAML::Load(content);
    YmlUtils::checkFields(root, {{"library", true, libraryFields}});
    return root["library"].as<Library>();
}
} // namespace Antares::IO::Inputs::YmlModel
