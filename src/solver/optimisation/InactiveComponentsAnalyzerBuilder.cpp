// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/InactiveComponentsAnalyzerBuilder.h"

#include <antares/study/area/constants.h>
#include <antares/study/study.h>
#include <antares/utils/utils.h>

namespace Antares::Optimization
{

namespace
{
bool columnIsAllZero(const Matrix<>& matrix, unsigned column)
{
    const auto& values = matrix[column];
    for (uint y = 0; y < matrix.height; ++y)
    {
        if (!Utils::isZero(values[y]))
        {
            return false;
        }
    }
    return true;
}
} // namespace

std::shared_ptr<const InactiveComponentsAnalyzer> BuildInactiveComponentsAnalyzer(
  const Data::Study& study)
{
    auto analyzer = std::make_shared<InactiveComponentsAnalyzer>();

    for (uint32_t pays = 0; pays < study.areas.size(); ++pays)
    {
        const auto& area = *(study.areas[pays]);

        analyzer->setLoadAllZero(pays, area.load.series.timeSeries.containsOnlyZero());
        analyzer->setRorAllZero(pays, area.hydro.series->ror.timeSeries.containsOnlyZero());
        analyzer->setSolarAllZero(pays, area.solar.series.timeSeries.containsOnlyZero());
        analyzer->setWindAllZero(pays, area.wind.series.timeSeries.containsOnlyZero());
        for (unsigned column = 0; column < Data::fhhMax; ++column)
        {
            analyzer->setMiscGenColumnAllZero(pays, column, columnIsAllZero(area.miscGen, column));
        }
    }

    return analyzer;
}

} // namespace Antares::Optimization
