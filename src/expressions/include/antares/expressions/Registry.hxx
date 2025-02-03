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

#include <concepts>
#include <memory>
#include <mutex>
#include <vector>

namespace Antares::Expressions
{
//  Template class to manage the memory allocation and registry for a base class
template<class Base>
class Registry
{
public:
    Registry() = default;
    Registry(Registry<Base>&&) = default;
    Registry<Base>& operator=(Registry<Base>&&) = default;

    //  Method to create a new derived class object and add it to the registry
    template<class Derived, class... Args>
    requires std::derived_from<Derived, Base>
    Derived* create(Args&&... args)
    {
        auto created = std::make_unique<Derived>(std::forward<Args>(args)...);
        registry_.push_back(std::move(created));
        return dynamic_cast<Derived*>(
          registry_.back().get()); //  Return the pointer to the newly created object
    }

private:
    std::vector<std::unique_ptr<Base>>
      registry_; //  Registry to manage dynamically allocated objects
};
} // namespace Antares::Expressions
