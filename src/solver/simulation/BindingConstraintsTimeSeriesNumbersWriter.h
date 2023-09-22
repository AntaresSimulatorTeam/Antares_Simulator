//
// Created by marechaljas on 17/03/23.
//

#pragma once

#include <memory>
#include "ITimeSeriesNumbersWriter.h"
#include <antares/writer/i_writer.h>
#include "antares/study/binding_constraint/BindingConstraintsRepository.h"

namespace Antares::Solver::Simulation
{
class BindingConstraintsTimeSeriesNumbersWriter : public ITimeSeriesNumbersWriter
{
public:
    explicit BindingConstraintsTimeSeriesNumbersWriter(IResultWriter& resultWriter);
    BindingConstraintsTimeSeriesNumbersWriter() = default;
    void write(
      const Data::BindingConstraintGroupRepository& bindingConstraintGroupRepository) override;

private:
    IResultWriter& writer_;
};
} // namespace Antares::Solver::Simulation
