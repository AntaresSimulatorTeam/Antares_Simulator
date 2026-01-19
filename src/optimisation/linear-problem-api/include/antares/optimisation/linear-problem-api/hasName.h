// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

namespace Antares::Optimisation::LinearProblemApi
{

/// Inherited by IMipVariable and IMipConstraint
class IHasName
{
public:
    virtual ~IHasName() = default;
    virtual const std::string& getName() const = 0;
};

} // namespace Antares::Optimisation::LinearProblemApi
