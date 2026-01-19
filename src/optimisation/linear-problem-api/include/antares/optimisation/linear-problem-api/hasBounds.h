// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares::Optimisation::LinearProblemApi
{

/// Used to handle bounds for IMipVariable and IMipConstraint
class IHasBounds
{
public:
    virtual ~IHasBounds() = default;

    virtual void setLb(double lb) = 0;
    virtual void setUb(double ub) = 0;

    virtual void setBounds(double lb, double ub) = 0;

    virtual double getLb() const = 0;
    virtual double getUb() const = 0;
};

} // namespace Antares::Optimisation::LinearProblemApi
