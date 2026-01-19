// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "hasBounds.h"
#include "hasName.h"
#include "hasStatus.h"

namespace Antares::Optimisation::LinearProblemApi
{

class IMipVariable: public IHasBounds, public IHasName, public IHasStatus
{
public:
    virtual bool isInteger() const = 0;
    virtual double solutionValue() const = 0;
    virtual double reducedCost() const = 0;
};

} // namespace Antares::Optimisation::LinearProblemApi
