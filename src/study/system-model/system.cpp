// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>

#include <antares/study/system-model/system.h>

namespace Antares::ModelerStudy::SystemModel
{
System::System(const std::string_view id, std::vector<Component>&& components):
    id_(id),
    components_(std::move(components))
{
    // Check that mandatory attributes are not empty
    if (id.empty())
    {
        throw std::invalid_argument("A system can't have an empty id");
    }
    if (components_.empty())
    {
        throw std::invalid_argument("A system must contain at least one component");
    }
}

/**
 * \brief Sets the ID of the system.
 *
 * \param id The ID to set.
 * \return Reference to the SystemBuilder object.
 */
SystemBuilder& SystemBuilder::withId(std::string_view id)
{
    id_ = id;
    return *this;
}

/**
 * \brief Sets the components of the system.
 *
 * \param components A vector of components to set.
 * \return Reference to the SystemBuilder object.
 */
SystemBuilder& SystemBuilder::withComponents(std::vector<Component>&& components)
{
    components_ = std::move(components);
    return *this;
}

/**
 * \brief Builds and returns the System object.
 *
 * \return The constructed System object.
 */
System SystemBuilder::build()
{
    return System(id_, std::move(components_));
}
} // namespace Antares::ModelerStudy::SystemModel
