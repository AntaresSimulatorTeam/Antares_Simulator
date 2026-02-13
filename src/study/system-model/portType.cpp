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
    return ac.injection.empty() && ac.spillage_bound.empty() && ac.unsupplied_energy_bound.empty();
}

bool operator==(const std::optional<AreaConnection>& a, const std::optional<AreaConnection>& b)
{
    if (a.has_value() != b.has_value())
    {
        return false;
    }

    if (a.has_value())
    {
        return a->injection == b->injection && a->spillage_bound == b->spillage_bound
               && a->unsupplied_energy_bound == b->unsupplied_energy_bound;
    }

    return true; // both are std::nullopt
}

std::string getConnectionField(const std::vector<PortField>& fields,
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
    return "";
}

// -------------------
// PortType methods
// -------------------
PortType::PortType(const std::string& id,
                   std::vector<PortField>&& portFields,
                   const AreaConnection& areaConnection,
                   const std::string& thermalCapacityConnectionField):
    id_(id),
    fields_(std::move(portFields))
{
    if (!isEmpty(areaConnection))
    {
        areaConnection_.emplace();
        areaConnection_->injection = getConnectionField(fields_,
                                                        id,
                                                        areaConnection.injection,
                                                        "area-connection");
        areaConnection_->spillage_bound = getConnectionField(fields_,
                                                             id,
                                                             areaConnection.spillage_bound,
                                                             "area-connection");

        areaConnection_->unsupplied_energy_bound = getConnectionField(
          fields_,
          id,
          areaConnection.unsupplied_energy_bound,
          "area-connection");
    }
    auto thermalConnectionField = getConnectionField(fields_,
                                                     id,
                                                     thermalCapacityConnectionField,
                                                     "thermal capacity");
    thermalCapacityConnectionFieldId_ = thermalConnectionField == ""
                                          ? std::nullopt
                                          : std::optional<std::string>(
                                              thermalCapacityConnectionField);
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
           && thermalCapacityConnectionFieldId_ == other.thermalCapacityConnectionFieldId_;
}

const std::optional<std::string>& PortType::ThermalCapacityConnectionFieldId() const
{
    return thermalCapacityConnectionFieldId_;
}
} // namespace Antares::ModelerStudy::SystemModel
