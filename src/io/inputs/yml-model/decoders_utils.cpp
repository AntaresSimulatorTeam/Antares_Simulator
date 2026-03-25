// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#include "antares/io/inputs/yml-model/decoders.h"


// Implement convert<Library>::decode in this translation unit to ensure all
// other convert<> specializations (e.g., Model) are available and avoid
// premature template instantiation in other TUs.
namespace YAML
{
bool convert<Antares::IO::Inputs::YmlModel::Library>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlModel::Library& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.description = node["description"].as<std::string>("");
    rhs.port_types = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::PortType>>(
      node["port-types"]);
    rhs.models = node["models"].as<std::vector<Antares::IO::Inputs::YmlModel::Model>>();
    return true;
}
} // namespace YAML
