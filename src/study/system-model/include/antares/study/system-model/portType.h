// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>
#include <string>
#include <vector>

#include "portField.h"

namespace Antares::ModelerStudy::SystemModel
{
struct AreaConnection
{
    std::string injection;
    std::string to_area_bound;
    std::string from_area_bound;
};

class PortType final
{
public:
    PortType(const std::string& id,
             std::vector<PortField>&& fields,
             const AreaConnection& areaConnection = {});

    const std::string& Id() const;
    const std::vector<PortField>& Fields() const;
    const std::optional<AreaConnection>& areaConnection() const;
    bool operator==(const PortType& other) const;

private:
    std::string id_;
    std::vector<PortField> fields_;
    std::optional<AreaConnection> areaConnection_;
};

} // namespace Antares::ModelerStudy::SystemModel
