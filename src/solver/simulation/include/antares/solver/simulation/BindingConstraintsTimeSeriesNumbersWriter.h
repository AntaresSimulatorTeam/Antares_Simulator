// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/writer/i_writer.h>
#include "antares/study/binding_constraint/BindingConstraintsRepository.h"

#include "ITimeSeriesNumbersWriter.h"

namespace Antares::Solver::Simulation
{
class BindingConstraintsTimeSeriesNumbersWriter final: public ITimeSeriesNumbersWriter
{
public:
    explicit BindingConstraintsTimeSeriesNumbersWriter(IResultWriter& resultWriter);
    BindingConstraintsTimeSeriesNumbersWriter() = delete;
    void write(
      const Data::BindingConstraintGroupRepository& bindingConstraintGroupRepository) override;

private:
    IResultWriter& writer_;
};
} // namespace Antares::Solver::Simulation
