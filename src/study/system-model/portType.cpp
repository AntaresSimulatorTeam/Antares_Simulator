#include "antares/study/system-model/portType.h"

#include <algorithm>
#include <stdexcept>

namespace Antares::ModelerStudy::SystemModel
{
PortType::PortType(const std::string& id,
                   std::vector<PortField>&& fields,
                   const std::string& areaConnectionFieldId):
    id_(id),
    fields_(std::move(fields))
{
    if (!areaConnectionFieldId.empty())
    {
        if (!std::ranges::any_of(fields_,
                                 [areaConnectionFieldId](const auto& field)
                                 { return field.Id() == areaConnectionFieldId; }))
        {
            throw std::invalid_argument(
              "Field \"" + areaConnectionFieldId
              + "\" selected for area connections was not defined in PortType \"" + id_ + "\".");
        }
        areaConnection_ = {areaConnectionFieldId, "", ""};
    }
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

bool operator==(const std::optional<AreaConnection>& a, const std::optional<AreaConnection>& b)
{
    if (!a.has_value() && !b.has_value())
    {
        return true;
    }

    if (!(a.has_value() || b.has_value()))
    {
        return false;
    }

    return a->injection == b->injection && a->to_area_bound == b->to_area_bound
           && a->from_area_bound == b->from_area_bound;
}

bool PortType::operator==(const PortType& other) const
{
    return id_ == other.id_ && fields_ == other.fields_ && areaConnection_ == other.areaConnection_;
}
} // namespace Antares::ModelerStudy::SystemModel
