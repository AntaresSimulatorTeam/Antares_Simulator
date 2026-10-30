// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-system/parser.h"

#include "antares/io/inputs/yml-system/decoders.h"
#include "antares/io/inputs/yml-system/system.h"
#include "antares/io/inputs/yml-utils/checkFields.h"

namespace Antares::IO::Inputs::YmlSystem
{
namespace
{
// Keep this schema in sync with field reads in decoders.cpp.
const std::vector<YmlUtils::Field> systemFields = {
  {"id"},
  {"description"},
  {"model-libraries"},
  {"components",
   false,
   {{"id", true},
    {"model", true},
    {"scenario-group"},
    {"parameters",
     false,
     {{"id", true}, {"value", true}, {"time-dependent"}, {"scenario-dependent"}}},
    {"properties", false, {{"id", true}, {"value", true}}}}},
  {"connections",
   false,
   {{"component1", true}, {"port1", true}, {"component2", true}, {"port2", true}}},
  {"area-connections", false, {{"component", true}, {"port", true}, {"area", true}}},
  {"thermal-capacity-connections",
   false,
   {{"component", true},
    {"port", true},
    {"thermal-component", true, {{"area", true}, {"cluster-id", true}}}}}};
} // namespace

System Parser::parse(const std::string& content, const std::string& pathFromRoot)
{
    YAML::Node root = YAML::Load(content);
    YmlUtils::checkFields(root, {{"system", true, systemFields}}, pathFromRoot);
    return root["system"].as<System>();
}

} // namespace Antares::IO::Inputs::YmlSystem
