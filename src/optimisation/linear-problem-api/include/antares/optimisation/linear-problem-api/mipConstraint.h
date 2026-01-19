// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "mipVariable.h"

namespace Antares::Optimisation::LinearProblemApi
{

class IMipConstraint: public IHasBounds, public IHasName, public IHasStatus
{
public:
    virtual void setCoefficient(IMipVariable* var, double coefficient) = 0;

    virtual double getCoefficient(const LinearProblemApi::IMipVariable* var) const = 0;

    virtual double dual() const = 0;
};

} // namespace Antares::Optimisation::LinearProblemApi
