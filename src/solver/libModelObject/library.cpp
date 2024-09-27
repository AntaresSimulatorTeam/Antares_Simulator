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

#include "antares/solver/libObjectModel/library.h"

namespace Antares::Solver::ObjectModel
{

LibraryBuilder& LibraryBuilder::withId(const std::string& id)
{
    library_.id_ = id;
    return *this;
}

LibraryBuilder& LibraryBuilder::withDescription(const std::string& description)
{
    library_.description_ = description;
    return *this;
}

LibraryBuilder& LibraryBuilder::withPortType(const std::map<std::string, PortType>& portTypes)
{
    library_.portTypes_ = portTypes;
    return *this;
}

LibraryBuilder& LibraryBuilder::withModel(const std::map<std::string, Model>& models)
{
    library_.models_ = models;
    return *this;
}

Library LibraryBuilder::build()
{
    return library_;
}
} // namespace Antares::Solver::ObjectModel
