// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <algorithm>
#include <fmt/format.h>
#include <optional>
#include <string>
#include <vector>

#include "portField.h"

namespace Antares::ModelerStudy::SystemModel
{

struct AreaConnection
{
    std::string inject_to_balance;
    std::string spillage_bound;
    std::string unsupplied_energy_bound;
};

class PortType final
{
public:
    PortType(const std::string& id,
             std::vector<PortField>&& fields,
             const AreaConnection& areaConnection = {},
             const std::string& thermalCapacityConnectionField = "");

    const std::optional<std::string>& ThermalCapacityConnectionFieldId() const;

    const std::string& Id() const;
    const std::vector<PortField>& Fields() const;
    const std::optional<AreaConnection>& areaConnection() const;
    bool operator==(const PortType& other) const;

private:
    std::string id_;
    std::vector<PortField> fields_;
    std::optional<std::string> thermalCapacityConnectionFieldId_;
    std::optional<AreaConnection> areaConnection_;
};

} // namespace Antares::ModelerStudy::SystemModel
