// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 28/06/23.
//

#pragma once

#include <memory>

#include "BindingConstraint.h"

namespace Antares::Data
{

class BindingConstraintGroup final
{
public:
    explicit BindingConstraintGroup(std::string name);

    [[nodiscard]] std::string name()
    {
        return name_;
    }

    void add(const std::shared_ptr<BindingConstraint>& constraint);
    [[nodiscard]] const BindingConstraint::Set& constraints() const;
    [[nodiscard]] unsigned numberOfTimeseries() const;

    // Public data members
    TimeSeriesNumbers timeseriesNumbers;

private:
    BindingConstraint::Set constraints_;
    std::string name_;
};

} // namespace Antares::Data
