/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <yuni/yuni.h>
#include "timeseries-numbers.h"
#include <antares/study.h>
#include <antares/study/scenario-builder/sets.h>
#include "../aleatoire/alea_fonctions.h"
#include <algorithm>
#include <vector>

using namespace Yuni;

#define TS_INDEX(T) Data::TimeSeriesBitPatternIntoIndex<T>::value

#define DRAW_A_RANDOM_NUMBER(T, X, PreproSize)                                              \
                                                                                            \
    ((!isTSintramodal[TS_INDEX(T)])                                                         \
                                                                                            \
       ? ((uint32)(floor((double)(study.runtime->random[Data::seedTimeseriesNumbers].next() \
                                  * (isTSgenerated[TS_INDEX(T)] ? PreproSize : X.width))))) \
                                                                                            \
       : draw_intramodal[TS_INDEX(T)])

#define CORRELATION_CHECK_INTERMODAL_SINGLE_AREA(T, PREPRO_WIDTH, MTX_WIDTH)                      \
    do                                                                                            \
    {                                                                                             \
        const unsigned int tsindx = TS_INDEX(T);                                                  \
        if (intermodal[tsindx])                                                                   \
        {                                                                                         \
            if (1 != (w = (isTSgenerated[tsindx] ? PREPRO_WIDTH : MTX_WIDTH)))                    \
            {                                                                                     \
                if (r[tsindx] != 1 && r[tsindx] != w)                                             \
                {                                                                                 \
                    logs.error() << "Inter-modal correlation: Constraint violation: The number "  \
                                    "of time-series for '"                                        \
                                 << area.name << "' does not match (found " << w << ", expected " \
                                 << r[tsindx] << ')';                                             \
                    return false;                                                                 \
                }                                                                                 \
                r[tsindx] = w;                                                                    \
            }                                                                                     \
        }                                                                                         \
    } while (0)

#define BUILD_LOG_ENTRY(T, TEXT)     \
    do                               \
    {                                \
        if (intermodal[TS_INDEX(T)]) \
        {                            \
            if (first)               \
            {                        \
                e += ", ";           \
                first = false;       \
            }                        \
            e += TEXT;               \
        }                            \
    } while (0)

namespace Antares
{
namespace Solver
{
static bool GenerateDeratedMode(Data::Study& study)
{
    logs.info() << "  :: using the `derated` mode";
    if (study.parameters.useCustomScenario)
        logs.warning() << "The derated mode is enabled. The custom building mode will be ignored";

    study.areas.each([&](Data::Area& area) {
        area.load.series->timeseriesNumbers.zero();
        area.solar.series->timeseriesNumbers.zero();
        area.wind.series->timeseriesNumbers.zero();
        area.hydro.series->timeseriesNumbers.zero();

        for (unsigned int i = 0; i != area.thermal.clusterCount(); ++i)
        {
            auto& cluster = *(area.thermal.clusters[i]);
            cluster.series->timeseriesNumbers.zero();
        }
    });

    return true;
}

bool checkIntraModalConsistencyForLoad(uint& nbTSload, Data::Study& study)
{
    logs.info() << "Checking intra-modal correlation: Load";
    std::vector<uint> listNumberGenTS;
    for (auto i = study.areas.begin(); i != study.areas.end(); ++i)
    {
        const Data::Area& area = *(i->second);
        listNumberGenTS.push_back(area.load.series->series.width);
    }

    if (std::adjacent_find(
          listNumberGenTS.begin(), listNumberGenTS.end(), std::not_equal_to<uint>())
        != listNumberGenTS.end())
    {
        logs.error()
          << "Intra-modal correlation: load numbers of time-series are not equal for all areas";
        return false;
    }
    nbTSload = listNumberGenTS[0];
    return true;
}

bool checkIntraModalConsistencyForHydro(uint& nbTShydro, Data::Study& study)
{
    logs.info() << "Checking intra-modal correlation: Hydro";
    std::vector<uint> listNumberGenTS;
    for (auto i = study.areas.begin(); i != study.areas.end(); ++i)
    {
        const Data::Area& area = *(i->second);
        listNumberGenTS.push_back(area.hydro.series->count);
    }

    if (std::adjacent_find(
          listNumberGenTS.begin(), listNumberGenTS.end(), std::not_equal_to<uint>())
        != listNumberGenTS.end())
    {
        logs.error()
          << "Intra-modal correlation: hydro numbers of time-series are not equal for all areas";
        return false;
    }
    nbTShydro = listNumberGenTS[0];
    return true;
}

bool checkIntraModalConsistencyForWind(uint& nbTSwind, Data::Study& study)
{
    logs.info() << "Checking intra-modal correlation: Wind";
    std::vector<uint> listNumberGenTS;
    for (auto i = study.areas.begin(); i != study.areas.end(); ++i)
    {
        const Data::Area& area = *(i->second);
        listNumberGenTS.push_back(area.wind.series->series.width);
    }

    if (std::adjacent_find(
          listNumberGenTS.begin(), listNumberGenTS.end(), std::not_equal_to<uint>())
        != listNumberGenTS.end())
    {
        logs.error()
          << "Intra-modal correlation: wind numbers of time-series are not equal for all areas";
        return false;
    }
    nbTSwind = listNumberGenTS[0];
    return true;
}

bool checkIntraModalConsistencyForSolar(uint& nbTSsolar, Data::Study& study)
{
    logs.info() << "Checking intra-modal correlation: Solar";
    std::vector<uint> listNumberGenTS;
    for (auto i = study.areas.begin(); i != study.areas.end(); ++i)
    {
        const Data::Area& area = *(i->second);
        listNumberGenTS.push_back(area.wind.series->series.width);
    }

    if (std::adjacent_find(
          listNumberGenTS.begin(), listNumberGenTS.end(), std::not_equal_to<uint>())
        != listNumberGenTS.end())
    {
        logs.error()
          << "Intra-modal correlation: solar numbers of time-series are not equal for all areas";
        return false;
    }
    nbTSsolar = listNumberGenTS[0];
    return true;
}

bool checkIntraModalConsistencyForThermal(uint& nbTSthermal, Data::Study& study)
{
    logs.info() << "Checking intra-modal correlation: Thermal";
    std::vector<uint> listNumberGenTS;
    for (auto i = study.areas.begin(); i != study.areas.end(); ++i)
    {
        const Data::Area& area = *(i->second);
        unsigned int clusterCount = area.thermal.clusterCount();
        for (unsigned int i = 0; i != clusterCount; ++i)
        {
            auto& cluster = *(area.thermal.clusters[i]);
            listNumberGenTS.push_back(cluster.series->series.width);
        }
    }

    if (std::adjacent_find(
          listNumberGenTS.begin(), listNumberGenTS.end(), std::not_equal_to<uint>())
        != listNumberGenTS.end())
    {
        logs.error()
          << "Intra-modal correlation: thermal numbers of time-series are not equal for all areas";
        return false;
    }
    nbTSthermal = listNumberGenTS[0];
    return true;
}

bool checkIntraModalConsistencyForRenewableClusters(uint& nbTSrenewables, Data::Study& study)
{
    logs.info() << "Checking intra-modal correlation: Renewable clusters";
    std::vector<uint> listNumberGenTS;
    for (auto i = study.areas.begin(); i != study.areas.end(); ++i)
    {
        const Data::Area& area = *(i->second);
        unsigned int clusterCount = area.renewable.clusterCount();
        for (unsigned int i = 0; i != clusterCount; ++i)
        {
            auto& cluster = *(area.renewable.clusters[i]);
            listNumberGenTS.push_back(cluster.series->series.width);
        }
    }

    if (std::adjacent_find(
          listNumberGenTS.begin(), listNumberGenTS.end(), std::not_equal_to<uint>())
        != listNumberGenTS.end())
    {
        logs.error() << "Intra-modal correlation: renwable cluster numbers of time-series are not "
                        "equal for all areas";
        return false;
    }
    nbTSrenewables = listNumberGenTS[0];
    return true;
}

bool checkIntraModalConsistency(uint* nbTimeseriesByMode,
                                const bool* isTSintramodal,
                                const bool* isTSgenerated,
                                Data::Study& study)
{
    auto& parameters = study.parameters;

    // Load ...
    int indexTS = TS_INDEX(Data::timeSeriesLoad);
    bool isIntraModal = isTSintramodal[indexTS];
    bool isGenerated = isTSgenerated[indexTS];

    if (isIntraModal && not isGenerated)
    {
        if (not checkIntraModalConsistencyForLoad(nbTimeseriesByMode[indexTS], study))
            return false;
    }
    else
        nbTimeseriesByMode[indexTS] = parameters.nbTimeSeriesLoad;

    // Hydro ...
    indexTS = TS_INDEX(Data::timeSeriesHydro);
    isIntraModal = isTSintramodal[indexTS];
    isGenerated = isTSgenerated[indexTS];

    if (isIntraModal && not isGenerated)
    {
        if (not checkIntraModalConsistencyForHydro(nbTimeseriesByMode[indexTS], study))
            return false;
    }
    else
        nbTimeseriesByMode[indexTS] = parameters.nbTimeSeriesHydro;

    // Wind ...
    indexTS = TS_INDEX(Data::timeSeriesWind);
    isIntraModal = isTSintramodal[indexTS];
    isGenerated = isTSgenerated[indexTS];

    if (isIntraModal && not isGenerated)
    {
        if (not checkIntraModalConsistencyForWind(nbTimeseriesByMode[indexTS], study))
            return false;
    }
    else
        nbTimeseriesByMode[indexTS] = parameters.nbTimeSeriesWind;

    // Solar ...
    indexTS = TS_INDEX(Data::timeSeriesSolar);
    isIntraModal = isTSintramodal[indexTS];
    isGenerated = isTSgenerated[indexTS];

    if (isIntraModal && not isGenerated)
    {
        if (not checkIntraModalConsistencyForSolar(nbTimeseriesByMode[indexTS], study))
            return false;
    }
    else
        nbTimeseriesByMode[indexTS] = parameters.nbTimeSeriesSolar;

    // Thermal ...
    indexTS = TS_INDEX(Data::timeSeriesThermal);
    isIntraModal = isTSintramodal[indexTS];
    isGenerated = isTSgenerated[indexTS];

    if (isIntraModal && not isGenerated)
    {
        if (not checkIntraModalConsistencyForThermal(nbTimeseriesByMode[indexTS], study))
            return false;
    }
    else
        nbTimeseriesByMode[indexTS] = parameters.nbTimeSeriesThermal;

    // Renewable clusters ...
    indexTS = TS_INDEX(Data::timeSeriesRenewable);
    isIntraModal = isTSintramodal[indexTS];
    isGenerated = isTSgenerated[indexTS];

    if (isIntraModal && not isGenerated)
    {
        if (not checkIntraModalConsistencyForRenewableClusters(nbTimeseriesByMode[indexTS], study))
            return false;
    }
    else
        nbTimeseriesByMode[indexTS] = 1;

    return true;
}

bool TimeSeriesNumbers::Generate(Data::Study& study)
{
    logs.info() << "Preparing time-series numbers...";

    auto& parameters = study.parameters;

    if (parameters.derated)
        return GenerateDeratedMode(study);

    const unsigned int years = 1 + study.runtime->rangeLimits.year[Data::rangeEnd];

    const bool isTSintramodal[Data::timeSeriesCount]
      = {0 != (Data::timeSeriesLoad & parameters.intraModal),
         0 != (Data::timeSeriesHydro & parameters.intraModal),
         0
           != ((Data::timeSeriesWind & parameters.intraModal)
               && (parameters.renewableGeneration() == Data::rgAggregated)),
         0 != (Data::timeSeriesThermal & parameters.intraModal),
         0
           != ((Data::timeSeriesSolar & parameters.intraModal)
               && (parameters.renewableGeneration() == Data::rgAggregated)),
         0
           != ((Data::timeSeriesRenewable & parameters.intraModal)
               && (parameters.renewableGeneration() == Data::rgClusters))};

    unsigned int nbTimeseriesByMode[Data::timeSeriesCount];

    uint32 draw_intramodal[Data::timeSeriesCount] = {0, 0, 0, 0, 0, 0};

    const bool isTSgenerated[Data::timeSeriesCount]
      = {0 != (Data::timeSeriesLoad & parameters.timeSeriesToRefresh),
         0 != (Data::timeSeriesHydro & parameters.timeSeriesToRefresh),
         0 != (Data::timeSeriesWind & parameters.timeSeriesToRefresh),
         0 != (Data::timeSeriesThermal & parameters.timeSeriesToRefresh),
         0 != (Data::timeSeriesSolar & parameters.timeSeriesToRefresh),
         false}; // TS generation is always disabled for renewables

    if (not checkIntraModalConsistency(nbTimeseriesByMode, isTSintramodal, isTSgenerated, study))
        return false;

    for (unsigned int y = 0; y < years; ++y)
    {
        // Draw TS numbers for intra-modal TS
        for (unsigned int tsKind = 0; tsKind < Data::timeSeriesCount; ++tsKind)
        {
            if (isTSintramodal[tsKind])
            {
                draw_intramodal[tsKind]
                  = (uint32)(floor(study.runtime->random[Data::seedTimeseriesNumbers].next()
                                   * nbTimeseriesByMode[tsKind]));
            }
        }

        // Draw TS numbers for non intra-modal TS
        study.areas.each([&](Data::Area& area) {
            // -------------
            // Load ...
            // -------------
            assert(y < area.load.series->timeseriesNumbers.height);
            int indexTS = TS_INDEX(Data::timeSeriesLoad);

            if (not isTSintramodal[indexTS])
            {
                uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                           : area.load.series->series.width;
                area.load.series->timeseriesNumbers[0][y] = (uint32)(
                  floor(study.runtime->random[Data::seedTimeseriesNumbers].next() * nbTimeSeries));
            }
            else
                area.load.series->timeseriesNumbers[0][y] = draw_intramodal[indexTS];

            // -------------
            // Solar ...
            // -------------
            assert(y < area.solar.series->timeseriesNumbers.height);
            indexTS = TS_INDEX(Data::timeSeriesSolar);

            if (not isTSintramodal[indexTS])
            {
                uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                           : area.solar.series->series.width;
                area.solar.series->timeseriesNumbers[0][y]
                  = (uint32)(floor(study.runtime->random[Data::seedTimeseriesNumbers].next() * nbTimeSeries));
            }
            else area.solar.series->timeseriesNumbers[0][y] = draw_intramodal[indexTS];

            // -------------
            // Wind ...
            // -------------
            assert(y < area.wind.series->timeseriesNumbers.height);
            indexTS = TS_INDEX(Data::timeSeriesWind);

            if (not isTSintramodal[indexTS])
            {
                uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                           : area.wind.series->series.width;
                area.wind.series->timeseriesNumbers[0][y]
                  = (uint32)(floor(study.runtime->random[Data::seedTimeseriesNumbers].next()
                                   * nbTimeSeries));
            }
            else
                area.wind.series->timeseriesNumbers[0][y] = draw_intramodal[indexTS];

            // -------------
            // Hydro ...
            // -------------
            assert(y < area.hydro.series->timeseriesNumbers.height);
            indexTS = TS_INDEX(Data::timeSeriesHydro);

            if (not isTSintramodal[indexTS])
            {
                uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                           : area.hydro.series->ror.width;
                area.hydro.series->timeseriesNumbers[0][y]
                  = (uint32)(floor(study.runtime->random[Data::seedTimeseriesNumbers].next() * nbTimeSeries));
            }
            else
                area.hydro.series->timeseriesNumbers[0][y] = draw_intramodal[indexTS];

            // -------------
            // Thermal ...
            // -------------
            indexTS = TS_INDEX(Data::timeSeriesThermal);

            auto end_th_clusters = area.thermal.list.mapping.end();
            for (auto i = area.thermal.list.mapping.begin(); i != end_th_clusters; ++i)
            {
                Data::ThermalClusterList::SharedPtr cluster = i->second;
                if (not cluster->enabled)
                    study.runtime->random[Data::seedTimeseriesNumbers].next();
                else
                {
                    if (not isTSintramodal[indexTS])
                    {
                        uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                                   : cluster->series->series.width;
                        cluster->series->timeseriesNumbers.entry[0][y]
                          = (uint32)(floor(study.runtime->random[Data::seedTimeseriesNumbers].next()
                                           * nbTimeSeries));
                    }
                    else
                        cluster->series->timeseriesNumbers.entry[0][y] = draw_intramodal[indexTS];
                }
            }

            // --------------------------
            // Renewable clusters ...
            // --------------------------
            indexTS = TS_INDEX(Data::timeSeriesRenewable);

            auto end_rn_clusters = area.renewable.list.cluster.end();
            for (auto j = area.renewable.list.cluster.begin(); j != end_rn_clusters; ++j)
            {
                Data::RenewableClusterList::SharedPtr cluster = j->second;
                if (not cluster->enabled)
                    study.runtime->random[Data::seedTimeseriesNumbers].next();
                else
                {
                    if (not isTSintramodal[indexTS])
                    {
                        uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                                   : cluster->series->series.width;
                        cluster->series->timeseriesNumbers.entry[0][y]
                          = (uint32)(floor(study.runtime->random[Data::seedTimeseriesNumbers].next()
                                           * nbTimeSeries));
                    }
                    else
                        cluster->series->timeseriesNumbers.entry[0][y] = draw_intramodal[indexTS];
                }
            }
        });
    }

    // ===============
    // Inter-modal
    // ===============

    const bool intermodal[Data::timeSeriesCount]
      = {0 != (Data::timeSeriesLoad & parameters.interModal),
         0 != (Data::timeSeriesHydro & parameters.interModal),
         0
           != ((Data::timeSeriesWind & parameters.interModal)
               && (parameters.renewableGeneration() == Data::rgAggregated)),
         0 != (Data::timeSeriesThermal & parameters.interModal),
         0
           != ((Data::timeSeriesSolar & parameters.interModal)
               && (parameters.renewableGeneration() == Data::rgAggregated)),
         0
           != ((Data::timeSeriesRenewable & parameters.interModal)
               && (parameters.renewableGeneration() == Data::rgClusters))};

    if (std::any_of(std::begin(intermodal), std::end(intermodal), [](bool x) { return x; }))
    {
        {
            CString<248, false> e = "Checking inter-modal correlation... (";
            bool first = true;
            BUILD_LOG_ENTRY(Data::timeSeriesLoad, "load");
            BUILD_LOG_ENTRY(Data::timeSeriesSolar, "solar");
            BUILD_LOG_ENTRY(Data::timeSeriesWind, "wind");
            BUILD_LOG_ENTRY(Data::timeSeriesHydro, "hydro");
            BUILD_LOG_ENTRY(Data::timeSeriesThermal, "thermal");
            BUILD_LOG_ENTRY(Data::timeSeriesRenewable, "renewables");
            logs.info() << e << ')';
        }

        unsigned int w;

        auto end = study.areas.end();
        for (auto i = study.areas.begin(); i != end; ++i)
        {
            auto& area = *(i->second);
            unsigned int r[Data::timeSeriesCount] = {1, 1, 1, 1, 1, 1};

            CORRELATION_CHECK_INTERMODAL_SINGLE_AREA(
              Data::timeSeriesLoad, parameters.nbTimeSeriesLoad, area.load.series->series.width);
            CORRELATION_CHECK_INTERMODAL_SINGLE_AREA(
              Data::timeSeriesSolar, parameters.nbTimeSeriesSolar, area.solar.series->series.width);
            CORRELATION_CHECK_INTERMODAL_SINGLE_AREA(
              Data::timeSeriesWind, parameters.nbTimeSeriesWind, area.wind.series->series.width);
            CORRELATION_CHECK_INTERMODAL_SINGLE_AREA(
              Data::timeSeriesHydro, parameters.nbTimeSeriesHydro, area.hydro.series->count);

            if (intermodal[TS_INDEX(Data::timeSeriesThermal)])
            {
                const unsigned int clusterCount = area.thermal.clusterCount();
                for (unsigned int j = 0; j != clusterCount; ++j)
                {
                    auto& cluster = *(area.thermal.clusters[j]);
                    CORRELATION_CHECK_INTERMODAL_SINGLE_AREA(Data::timeSeriesThermal,
                                                             parameters.nbTimeSeriesThermal,
                                                             cluster.series->series.width);
                }
            }

            if (intermodal[TS_INDEX(Data::timeSeriesRenewable)])
            {
                const unsigned int clusterCount = area.renewable.clusterCount();
                for (unsigned int j = 0; j != clusterCount; ++j)
                {
                    auto& cluster = *(area.renewable.clusters[j]);
                    CORRELATION_CHECK_INTERMODAL_SINGLE_AREA(
                      Data::timeSeriesRenewable, 1, cluster.series->series.width);
                }
            }

            unsigned int q = 1;
            for (unsigned int j = 0; j != Data::timeSeriesCount; ++j)
            {
                if (r[j] != 1)
                {
                    if (q != 1 && q != r[j])
                    {
                        logs.error() << "Inter-modal correlation: Constraint violation: The number "
                                        "of time-series for '"
                                     << area.name << "' does not match (found " << r[j]
                                     << ", expected " << q << ')';
                        return false;
                    }
                    q = r[j];
                }
            }

            Matrix<uint32>* tsNumbers = nullptr;
            if (intermodal[TS_INDEX(Data::timeSeriesLoad)])
                tsNumbers = &(area.load.series->timeseriesNumbers);
            else
            {
                if (intermodal[TS_INDEX(Data::timeSeriesSolar)])
                    tsNumbers = &(area.solar.series->timeseriesNumbers);
                else if (intermodal[TS_INDEX(Data::timeSeriesWind)])
                    tsNumbers = &(area.wind.series->timeseriesNumbers);
                else if (intermodal[TS_INDEX(Data::timeSeriesHydro)])
                    tsNumbers = &(area.hydro.series->timeseriesNumbers);
                else if (intermodal[TS_INDEX(Data::timeSeriesThermal)]
                         && area.thermal.clusterCount() > 0)
                    tsNumbers = &(area.thermal.clusters[0]->series->timeseriesNumbers);
                else if (intermodal[TS_INDEX(Data::timeSeriesRenewable)]
                         && area.renewable.clusterCount() > 0)
                    tsNumbers = &(area.renewable.clusters[0]->series->timeseriesNumbers);
            }
            assert(tsNumbers);

            for (unsigned int y = 0; y < years; ++y)
            {
                const unsigned int draw = tsNumbers->entry[0][y];
                assert(draw < 100000);

                assert(y < area.load.series->timeseriesNumbers.height);
                if (intermodal[TS_INDEX(Data::timeSeriesLoad)])
                    area.load.series->timeseriesNumbers.entry[0][y] = draw;

                assert(y < area.solar.series->timeseriesNumbers.height);
                if (intermodal[TS_INDEX(Data::timeSeriesSolar)])
                    area.solar.series->timeseriesNumbers.entry[0][y] = draw;

                assert(y < area.wind.series->timeseriesNumbers.height);
                if (intermodal[TS_INDEX(Data::timeSeriesWind)])
                    area.wind.series->timeseriesNumbers.entry[0][y] = draw;

                assert(y < area.hydro.series->timeseriesNumbers.height);
                if (intermodal[TS_INDEX(Data::timeSeriesHydro)])
                    area.hydro.series->timeseriesNumbers.entry[0][y] = draw;

                if (intermodal[TS_INDEX(Data::timeSeriesThermal)])
                {
                    unsigned int clusterCount = area.thermal.clusterCount();
                    for (unsigned int i = 0; i != clusterCount; ++i)
                    {
                        auto& cluster = *(area.thermal.clusters[i]);
                        assert(y < cluster.series->timeseriesNumbers.height);
                        cluster.series->timeseriesNumbers.entry[0][y] = draw;
                    }
                }
                if (intermodal[TS_INDEX(Data::timeSeriesRenewable)])
                {
                    unsigned int clusterCount = area.renewable.clusterCount();
                    for (unsigned int i = 0; i != clusterCount; ++i)
                    {
                        auto& cluster = *(area.renewable.clusters[i]);
                        assert(y < cluster.series->timeseriesNumbers.height);
                        cluster.series->timeseriesNumbers.entry[0][y] = draw;
                    }
                }
            }
        }
    }

    return true;
}

void TimeSeriesNumbers::StoreTimeseriesIntoOuput(Data::Study& study)
{
    using namespace Antares::Data;

    if (study.parameters.storeTimeseriesNumbers)
    {
        study.storeTimeSeriesNumbers<timeSeriesLoad>();
        study.storeTimeSeriesNumbers<timeSeriesSolar>();
        study.storeTimeSeriesNumbers<timeSeriesHydro>();
        study.storeTimeSeriesNumbers<timeSeriesWind>();
        study.storeTimeSeriesNumbers<timeSeriesThermal>();
        study.storeTimeSeriesNumbers<timeSeriesRenewable>();
    }
}

} // namespace Solver
} // namespace Antares
