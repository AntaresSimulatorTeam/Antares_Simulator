//
// Created by marechaljas on 17/03/23.
//

#pragma once

#include <memory>
#include "ITimeSeriesWriter.h"
#include "i_writer.h"
#include "antares/study/constraint/constraint.h"

namespace Antares::Solver::Simulation {
class TimeSeriesWriter: public ITimeSeriesWriter {

public:
    TimeSeriesWriter(std::shared_ptr<Antares::Solver::IResultWriter> sharedPtr);

    void write(const Data::BindConstList &list) override;

private:
    std::shared_ptr<IResultWriter> writer_;
};
} // Simulation
