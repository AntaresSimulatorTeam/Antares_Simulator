//
// Created by marechaljas on 17/03/23.
//

#pragma once
#include "antares/study/binding_constraint/BindingConstraintsList.h"

namespace Antares::Solver::Simulation {
class ITimeSeriesNumbersWriter {

public:
    virtual ~ITimeSeriesNumbersWriter() = default;
    virtual void write(const Data::BindingConstraintsList& list) = 0;
};
}