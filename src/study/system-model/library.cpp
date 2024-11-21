/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "antares/study/system-model/library.h"

#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

namespace Antares::Study::SystemModel
{

/**
 * \brief Sets the ID of the library.
 *
 * \param id The ID to set.
 * \return Reference to the LibraryBuilder object.
 */
LibraryBuilder& LibraryBuilder::withId(const std::string& id)
{
    library_.id_ = id;
    return *this;
}

/**
 * \brief Sets the description of the library.
 *
 * \param description The description to set.
 * \return Reference to the LibraryBuilder object.
 */
LibraryBuilder& LibraryBuilder::withDescription(const std::string& description)
{
    library_.description_ = description;
    return *this;
}

/**
 * \brief Sets the port types of the library.
 *
 * \param portTypes A vector of PortType objects to set.
 * \return Reference to the LibraryBuilder object.
 *
 * inputs it not garanteed to be valid after the call
 */
LibraryBuilder& LibraryBuilder::withPortTypes(std::vector<PortType>&& portTypes)
{
    std::transform(portTypes.begin(),
                   portTypes.end(),
                   std::inserter(library_.portTypes_, library_.portTypes_.end()),
                   [](/*Non const to prevent copy*/ PortType& portType)
                   { return std::make_pair(portType.Id(), std::move(portType)); });
    return *this;
}

/**
 * \brief Sets the models of the library.
 *
 * \param models A vector of Model objects to set.
 * \return Reference to the LibraryBuilder object.
 *
 * inputs it not garanteed to be valid after the call
 */
LibraryBuilder& LibraryBuilder::withModels(std::vector<Model>&& models)
{
    std::transform(models.begin(),
                   models.end(),
                   std::inserter(library_.models_, library_.models_.end()),
                   [](/*Non const to prevent copy*/ Model& model)
                   { return std::make_pair(model.Id(), std::move(model)); });
    return *this;
}

/**
 * \brief Returns the Library object.
 *
 * \return The constructed Library object.
 */
Library LibraryBuilder::build()
{
    return library_;
}
} // namespace Antares::Study::SystemModel
