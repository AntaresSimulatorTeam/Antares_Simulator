// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/system-model/portType.h"

#include <algorithm>
#include <stdexcept>

namespace Antares::ModelerStudy::SystemModel
{
// -----------------
// Help functions
// -----------------
bool isEmpty(const AreaConnection& ac)
{
    return ac.inject_to_balance.empty() && ac.spillage_bound.empty()
           && ac.unsupplied_energy_bound.empty() && ac.price.empty();
}

bool operator==(const std::optional<AreaConnection>& a, const std::optional<AreaConnection>& b)
{
    if (a.has_value() != b.has_value())
    {
        return false;
    }

    if (a.has_value())
    {
        return a->inject_to_balance == b->inject_to_balance
               && a->spillage_bound == b->spillage_bound
               && a->unsupplied_energy_bound == b->unsupplied_energy_bound && a->price == b->price;
    }

    return true; // both are std::nullopt
}

std::optional<std::string> getConnectionField(const std::vector<PortField>& fields,
                                              const std::string& portTypeId,
                                              const std::string& connectionFieldId,
                                              const std::string& nameOfTheConnection)
{
    if (!connectionFieldId.empty())
    {
        if (!std::ranges::any_of(fields,
                                 [&connectionFieldId](const auto& field)
                                 { return field.Id() == connectionFieldId; }))
        {
            const auto msg = fmt::format(
              "Field '{}' selected for {} connections was not defined in PortType '{}'.",
              connectionFieldId,
              nameOfTheConnection,
              portTypeId);
            throw std::invalid_argument(msg);
        }
        return connectionFieldId;
    }
    return std::nullopt;
}

std::optional<LinkCapacityConnectionFields> getLinkConnectionFields(
  const std::vector<PortField>& fields,
  const std::string& portTypeId,
  const std::string& directConnectionFieldId,
  const std::string& indirectConnectionFieldId)
{
    if (directConnectionFieldId.empty() && indirectConnectionFieldId.empty())
    {
        return std::nullopt;
    }
    const auto directField = getConnectionField(fields,
                                                portTypeId,
                                                directConnectionFieldId,
                                                "link capacity (direct)");
    const auto indirectField = getConnectionField(fields,
                                                  portTypeId,
                                                  indirectConnectionFieldId,
                                                  "link capacity (indirect)");
    if (!directField.has_value() || !indirectField.has_value())
    {
        throw std::invalid_argument(
          fmt::format("PortType '{}': both direct and indirect fields are required for "
                      "link capacity connections.",
                      portTypeId));
    }
    return LinkCapacityConnectionFields{directField.value(), indirectField.value()};
}

// -------------------
// PortType methods
// -------------------
PortType::PortType(const std::string& id,
                   std::vector<PortField>&& portFields,
                   const AreaConnection& areaConnection,
                   const std::string& thermalCapacityConnectionField,
                   const std::string& linkCapacityConnectionDirectField,
                   const std::string& linkCapacityConnectionIndirectField):
    id_(id),
    fields_(std::move(portFields))
{
    if (!isEmpty(areaConnection))
    {
        areaConnection_ = areaConnection;
    }
    thermalCapacityConnectionFieldId_ = getConnectionField(fields_,
                                                           id,
                                                           thermalCapacityConnectionField,
                                                           "thermal capacity");
    linkCapacityConnectionFieldIds_ = getLinkConnectionFields(fields_,
                                                              id,
                                                              linkCapacityConnectionDirectField,
                                                              linkCapacityConnectionIndirectField);
}

const std::string& PortType::Id() const
{
    return id_;
}

const std::vector<PortField>& PortType::Fields() const
{
    return fields_;
}

const std::optional<AreaConnection>& PortType::areaConnection() const
{
    return areaConnection_;
}

bool PortType::operator==(const PortType& other) const
{
    return id_ == other.id_ && fields_ == other.fields_ && areaConnection_ == other.areaConnection_
           && thermalCapacityConnectionFieldId_ == other.thermalCapacityConnectionFieldId_
           && linkCapacityConnectionFieldIds_ == other.linkCapacityConnectionFieldIds_;
}

const std::optional<std::string>& PortType::ThermalCapacityConnectionFieldId() const
{
    return thermalCapacityConnectionFieldId_;
}

const std::optional<LinkCapacityConnectionFields>& PortType::LinkCapacityConnectionFieldIds() const
{
    return linkCapacityConnectionFieldIds_;
}
} // namespace Antares::ModelerStudy::SystemModel
