//
// Created by marechaljas on 17/03/23.
//

#pragma once
#include "antares/study/binding_constraint/BindingConstraint.h"

namespace Antares::Solver::Simulation {
class ITimeSeriesWriter {

public:
    virtual void write(const Antares::Data::BindingConstraintsList& list) = 0;
};
}