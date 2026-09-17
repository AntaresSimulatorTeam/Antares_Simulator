// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/InactiveComponentsAnalyzerBuilder.h"

#include <algorithm>

#include <antares/study/area/constants.h>
#include <antares/study/study.h>
#include <antares/utils/utils.h>

namespace Antares::Optimization
{

namespace
{
bool columnIsAllZero(const Matrix<>& matrix, unsigned column)
{
    const auto* values = matrix[column];
    return std::all_of(values, values + matrix.height, Utils::isZero);
}

// A component can have several chronicles (columns) in its matrix, only some
// of which are ever drawn by the scenario builder / TS-number generator for
// this study's Monte-Carlo years (`ts.timeseriesNumbers`, filled once, before
// the simulation loop, in TimeSeriesNumbers::Generate). A chronicle that
// exists in the file but is never selected for any year should not keep the
// component "active": only the selected chronicles matter.
bool selectedColumnsAreAllZero(const Data::TimeSeries& ts)
{
    const uint32_t nbYears = ts.timeseriesNumbers.height();
    if (nbYears == 0)
    {
        return ts.timeSeries.containsOnlyZero();
    }
    for (uint32_t year = 0; year < nbYears; ++year)
    {
        const double* values = ts.getColumn(year);
        for (uint32_t h = 0; h < ts.timeSeries.height; ++h)
        {
            if (!Utils::isZero(values[h]))
            {
                return false;
            }
        }
    }
    return true;
}
} // namespace

std::shared_ptr<const InactiveComponentsAnalyzer> BuildInactiveComponentsAnalyzer(
  const Data::Study& study)
{
    auto analyzer = std::make_shared<InactiveComponentsAnalyzer>();

    uint32_t interco = 0;
    for (uint32_t pays = 0; pays < study.areas.size(); ++pays)
    {
        const auto& area = *(study.areas[pays]);

        analyzer->setLoadAllZero(pays, selectedColumnsAreAllZero(area.load.series));
        analyzer->setRorAllZero(pays, selectedColumnsAreAllZero(area.hydro.series->ror));
        analyzer->setSolarAllZero(pays, selectedColumnsAreAllZero(area.solar.series));
        analyzer->setWindAllZero(pays, selectedColumnsAreAllZero(area.wind.series));
        analyzer->setHydroInflowAllZero(pays,
                                        selectedColumnsAreAllZero(area.hydro.series->storage));
        for (unsigned column = 0; column < Data::fhhMax; ++column)
        {
            analyzer->setMiscGenColumnAllZero(pays, column, columnIsAllZero(area.miscGen, column));
        }

        // Links are numbered by walking areas then, per area, their `links`
        // map, matching StudyRuntimeInfosInitializeAreaLinks's nested order
        // (both alphabetical), so `interco` lines up with PROBLEME_HEBDO's.
        for (const auto& [linkName, link]: area.links)
        {
            const bool bothDirectionsAllZero = selectedColumnsAreAllZero(link->directCapacities)
                                               && selectedColumnsAreAllZero(
                                                 link->indirectCapacities);
            analyzer->setLinkAllZero(interco, bothDirectionsAllZero);
            ++interco;
        }
    }

    return analyzer;
}

} // namespace Antares::Optimization
