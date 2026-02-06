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

void checkNonEmptyFieldExist(const std::string& field,
                             const std::vector<PortField>& portFields,
                             const std::string& portTypeId)
{
    if (field.empty())
    {
        return;
    }

    if (std::ranges::all_of(portFields,
                            [&](const auto& portField) { return portField.Id() != field; }))
    {
        std::string err_msg = "In PortType " + portTypeId + ", area connexion field '" + field
                              + "' undefined.";
        throw std::invalid_argument(err_msg);
    }
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

// -------------------
// PortType methods
// -------------------
PortType::PortType(const std::string& id,
                   std::vector<PortField>&& portFields,
                   const AreaConnection& areaConnection):
    id_(id),
    fields_(std::move(portFields))
{
    if (!isEmpty(areaConnection))
    {
        checkNonEmptyFieldExist(areaConnection.injection, fields_, id_);
        checkNonEmptyFieldExist(areaConnection.spillage_bound, fields_, id_);
        checkNonEmptyFieldExist(areaConnection.unsupplied_energy_bound, fields_, id_);
        areaConnection_ = areaConnection;
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

bool PortType::operator==(const PortType& other) const
{
    return id_ == other.id_ && fields_ == other.fields_ && areaConnection_ == other.areaConnection_;
}
} // namespace Antares::ModelerStudy::SystemModel
