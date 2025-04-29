/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <algorithm>

#include <antares/study/system-model/system.h>

namespace Antares::ModelerStudy::SystemModel
{
System::System(const std::string_view id, std::unordered_map<std::string, Component>&& components):
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
SystemBuilder& SystemBuilder::withComponents(
  std::unordered_map<std::string, Component>&& components)
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
