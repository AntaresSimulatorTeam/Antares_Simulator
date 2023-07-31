//
// Created by marechaljas on 17/03/23.
//

#pragma once

#include <memory>
#include "ITimeSeriesNumbersWriter.h"
#include "i_writer.h"
#include "antares/study/binding_constraint/BindingConstraintsRepository.h"

namespace Antares::Solver::Simulation {
class BindingConstraintsTimeSeriesNumbersWriter: public ITimeSeriesNumbersWriter {

public:
    explicit BindingConstraintsTimeSeriesNumbersWriter(std::shared_ptr<Antares::Solver::IResultWriter> resultWriter);
    BindingConstraintsTimeSeriesNumbersWriter() = default;
    void write(const Data::BindingConstraintGroupRepository &bindingConstraintGroupRepository) override;

private:
    std::shared_ptr<IResultWriter> writer_;
};
} // Simulation
