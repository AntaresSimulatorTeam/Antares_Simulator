// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "antares/study/binding_constraint/BindingConstraintGroupRepository.h"

namespace Antares::Solver::Simulation
{
class ITimeSeriesNumbersWriter
{
public:
    virtual ~ITimeSeriesNumbersWriter() = default;
    virtual void write(const Data::BindingConstraintGroupRepository& list) = 0;
};
} // namespace Antares::Solver::Simulation
