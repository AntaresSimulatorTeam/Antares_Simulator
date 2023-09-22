//
// Created by marechaljas on 17/03/23.
//

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