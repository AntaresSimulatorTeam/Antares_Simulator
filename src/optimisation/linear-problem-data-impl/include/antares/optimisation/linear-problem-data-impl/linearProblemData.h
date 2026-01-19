// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>
#include <string>

#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"
#include "antares/optimisation/linear-problem-api/IScenario.h"
#include "antares/optimisation/linear-problem-data-impl/dataSeriesRepo.h"

namespace Antares::Optimisation::LinearProblemDataImpl
{

class LinearProblemData: public LinearProblemApi::ILinearProblemData
{
public:
    LinearProblemData() = default;

    explicit LinearProblemData(DataSeriesRepository data_series_repository):
        dataSeriesRepository_(std::move(data_series_repository))
    {
    }

    [[nodiscard]] double getData(const std::string& dataSetId,
                                 LinearProblemApi::IScenario::TimeSeriesNumber timeSeriesNumber,
                                 unsigned hour) const override;
    [[nodiscard]] std::span<const double> getData(const std::string& dataSetId,
                                                  unsigned timeSeriesNumber,
                                                  unsigned firstHour,
                                                  unsigned lastHour) const override;

    void addDataSeries(std::unique_ptr<IDataSeries> dataSeries);

private:
    DataSeriesRepository dataSeriesRepository_;
};

} // namespace Antares::Optimisation::LinearProblemDataImpl
