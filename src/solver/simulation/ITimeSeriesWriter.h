//
// Created by marechaljas on 17/03/23.
//

#pragma once
#include "antares/study/constraint/constraint.h"

namespace Antares::Solver::Simulation {
class ITimeSeriesWriter {

public:
    virtual void write(const Antares::Data::BindConstList& list) = 0;
};
}