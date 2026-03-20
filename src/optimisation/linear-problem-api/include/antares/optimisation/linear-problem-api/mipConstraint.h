// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "mipVariable.h"
#include "vector"

namespace Antares::Optimisation::LinearProblemApi
{

class IMipConstraint: public IHasBounds, public IHasName, public IHasStatus
{
public:
    virtual void setCoefficient(IMipVariable* var, double coefficient) = 0;

    virtual double getCoefficient(const IMipVariable* var) const = 0;
    [[nodiscard]] virtual std::vector<std::pair<int, double>> getCoefficients() const = 0;

    [[nodiscard]] virtual double dual() const = 0;
};

} // namespace Antares::Optimisation::LinearProblemApi
