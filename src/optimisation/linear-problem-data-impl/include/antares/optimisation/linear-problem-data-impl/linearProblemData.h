/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

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

    void addDataSeries(std::unique_ptr<IDataSeries> dataSeries);

private:
    DataSeriesRepository dataSeriesRepository_;
};

} // namespace Antares::Optimisation::LinearProblemDataImpl
