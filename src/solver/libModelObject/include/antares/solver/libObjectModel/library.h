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
#pragma once

#include <map>

#include "model.h"
#include "portType.h"

namespace Antares::Solver::ObjectModel
{

/// A library is a collection of models
class Library
{
public:
    Library() = default;
    ~Library() = default;

    const std::string& id() const
    {
        return id_;
    }

    const std::string& description() const
    {
        return description_;
    }

    const std::map<std::string, PortType>& portTypes() const
    {
        return portTypes_;
    }

    const std::map<std::string, Model>& models() const
    {
        return models_;
    }

private:
    friend class LibraryBuilder;

    std::string id_;
    std::string description_;

    std::map<std::string, PortType> portTypes_;
    std::map<std::string, Model> models_;
};

/**
 * @brief Builder for the Library class
 * Follow builder pattern:
 * builder.Library().withId("id").withDescription("description").withPortType(portList).withModel(modelList).build();
 */
class LibraryBuilder
{
public:
    LibraryBuilder& withId(const std::string& id);
    LibraryBuilder& withDescription(const std::string& description);
    LibraryBuilder& withPortType(std::vector<PortType>& portTypes);
    LibraryBuilder& withModel(std::vector<Model>& models);

    Library build();

private:
    Library library_;
};

} // namespace Antares::Solver::ObjectModel
