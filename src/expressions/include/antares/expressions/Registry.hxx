// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
