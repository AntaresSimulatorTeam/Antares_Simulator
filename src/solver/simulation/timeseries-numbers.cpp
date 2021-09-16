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

#include <algorithm>
#include <vector>
#include <map>
#include <string>

#include <yuni/yuni.h>
#include "timeseries-numbers.h"
#include <antares/study.h>
#include <antares/study/scenario-builder/sets.h>
#include "../aleatoire/alea_fonctions.h"

using namespace Yuni;
using namespace Antares::Data;
using namespace std;

#define TS_INDEX(T) Data::TimeSeriesBitPatternIntoIndex<T>::value

const map<TimeSeries, int> ts_to_tsIndex = { 
    {timeSeriesLoad, 0},
    {timeSeriesHydro, 1}, 
    {timeSeriesWind, 2},
    {timeSeriesThermal, 3},
    {timeSeriesSolar, 4},
    {timeSeriesRenewable, 5},
    {timeSeriesCount, 6}
};

const map<TimeSeries, string> ts_to_tsTitle = {
    {timeSeriesLoad, "load"},
    {timeSeriesHydro, "hydro"},
    {timeSeriesWind, "wind"},
    {timeSeriesThermal, "thermal"},
    {timeSeriesSolar, "solar"},
    {timeSeriesRenewable, "renewable clusters"}
};

#define BUILD_LOG_ENTRY(T, TEXT)     \
    do                               \
    {                                \
        if (isTSintermodal[TS_INDEX(T)]) \
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


class TimeSeriesCounter
{
public:
    TimeSeriesCounter(Data::Study& study) : study_(study) {}
    virtual std::vector<uint> getAreaTimeSeriesNumber(const Data::Area& area) = 0;
    virtual uint getGeneratedTimeSeriesNumber() = 0;
protected:
    Data::Study& study_;
};

class loadTimeSeriesCounter : public TimeSeriesCounter
{
public:
    loadTimeSeriesCounter(Data::Study& study) : TimeSeriesCounter(study) {}
    std::vector<uint> getAreaTimeSeriesNumber(const Data::Area& area)
    {
        std::vector<uint> to_return = { area.load.series->series.width };
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber() { return study_.parameters.nbTimeSeriesLoad; }
};

class hydroTimeSeriesCounter : public TimeSeriesCounter
{
public:
    hydroTimeSeriesCounter(Data::Study& study) : TimeSeriesCounter(study) {}
    std::vector<uint> getAreaTimeSeriesNumber(const Data::Area& area)
    {
        std::vector<uint> to_return = { area.hydro.series->count };
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber() { return study_.parameters.nbTimeSeriesHydro; }
};

class windTimeSeriesCounter : public TimeSeriesCounter
{
public:
    windTimeSeriesCounter(Data::Study& study) : TimeSeriesCounter(study) {}
    std::vector<uint> getAreaTimeSeriesNumber(const Data::Area& area)
    {
        std::vector<uint> to_return = { area.wind.series->series.width };
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber() { return study_.parameters.nbTimeSeriesWind; }
};

class solarTimeSeriesCounter : public TimeSeriesCounter
{
public:
    solarTimeSeriesCounter(Data::Study& study) : TimeSeriesCounter(study) {}
    std::vector<uint> getAreaTimeSeriesNumber(const Data::Area& area)
    {
        std::vector<uint> to_return = { area.solar.series->series.width };
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber() { return study_.parameters.nbTimeSeriesSolar; }
};

class thermalTimeSeriesCounter : public TimeSeriesCounter
{
public:
    thermalTimeSeriesCounter(Data::Study& study) : TimeSeriesCounter(study) {}
    std::vector<uint> getAreaTimeSeriesNumber(const Data::Area& area)
    {
        std::vector<uint> to_return;
        uint clusterCount = area.thermal.clusterCount();
        for (uint i = 0; i != clusterCount; ++i)
        {
            auto& cluster = *(area.thermal.clusters[i]);
            to_return.push_back(cluster.series->series.width);
        }
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber() { return study_.parameters.nbTimeSeriesThermal; }
};

class renewableTimeSeriesCounter : public TimeSeriesCounter
{
public:
    renewableTimeSeriesCounter(Data::Study& study) : TimeSeriesCounter(study) {}
    std::vector<uint> getAreaTimeSeriesNumber(const Data::Area& area)
    {
        std::vector<uint> to_return;
        uint clusterCount = area.renewable.clusterCount();
        for (uint i = 0; i != clusterCount; ++i)
        {
            auto& cluster = *(area.renewable.clusters[i]);
            to_return.push_back(cluster.series->series.width);
        }
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber() { return 1; }
};

class InterModalConsistencyChecker
{
public:
    InterModalConsistencyChecker(const TimeSeries ts, const bool* isTSintramodal, const bool* isTSgenerated, TimeSeriesCounter* tsCounter, Data::Study & study)
        : tsCounter_(tsCounter), study_(study), nbTimeseries_(0)
    {
        int indexTS = ts_to_tsIndex.at(ts);
        isTSintramodal_ = isTSintramodal[indexTS];
        isTSgenerated_ = isTSgenerated[indexTS];
        tsTitle_ = ts_to_tsTitle.at(ts);
    }
    ~InterModalConsistencyChecker() = default;
    bool check();
    uint getTimeSeriesNumber() { return nbTimeseries_; }

private:
    bool checkTSconsistency();

private:
    bool isTSintramodal_;
    bool isTSgenerated_;
    TimeSeriesCounter* tsCounter_;
    Data::Study& study_;
    uint nbTimeseries_;
    string tsTitle_;
};

bool InterModalConsistencyChecker::check()
{
    if (isTSintramodal_ && not isTSgenerated_)
    {
        if (not checkTSconsistency())
            return false;
    }
    else
        nbTimeseries_ = tsCounter_->getGeneratedTimeSeriesNumber();

    return true;
}

bool InterModalConsistencyChecker::checkTSconsistency()
{
    logs.info() << "Checking intra-modal correlation: " << tsTitle_;
    std::vector<uint> listNumberTS;
    for (auto i = study_.areas.begin(); i != study_.areas.end(); ++i)
    {
        const Data::Area& area = *(i->second);
        vector<uint> areaNumberTSList = tsCounter_->getAreaTimeSeriesNumber(area);
        listNumberTS.insert(listNumberTS.end(), areaNumberTSList.begin(), areaNumberTSList.end());
    }

    if (std::adjacent_find(listNumberTS.begin(), listNumberTS.end(), std::not_equal_to<uint>()) != listNumberTS.end())
    {
        logs.error() << "Intra-modal correlation: " << tsTitle_ << "'s numbers of time-series are not equal for all areas";
        return false;
    }
    nbTimeseries_ = listNumberTS[0];
    return true;
}

bool checkIntraModalConsistency(uint* nbTimeseriesByMode,
                                const bool* isTSintramodal,
                                const bool* isTSgenerated,
                                Data::Study& study)
{
    // Load ...
    int indexTS = TS_INDEX(Data::timeSeriesLoad);
    loadTimeSeriesCounter loadTScounter(study);
    InterModalConsistencyChecker loadIntraModalchecker(timeSeriesLoad, isTSintramodal, isTSgenerated, &loadTScounter, study);
    if (not loadIntraModalchecker.check())
        return false;
    nbTimeseriesByMode[indexTS] = loadIntraModalchecker.getTimeSeriesNumber();

    // Hydro ...
    indexTS = TS_INDEX(Data::timeSeriesHydro);
    hydroTimeSeriesCounter hydroTScounter(study);
    InterModalConsistencyChecker hydroIntraModalchecker(timeSeriesHydro, isTSintramodal, isTSgenerated, &hydroTScounter, study);
    if (not hydroIntraModalchecker.check())
        return false;
    nbTimeseriesByMode[indexTS] = hydroIntraModalchecker.getTimeSeriesNumber();

    // Wind ...
    indexTS = TS_INDEX(Data::timeSeriesWind);
    windTimeSeriesCounter windTScounter(study);
    InterModalConsistencyChecker windIntraModalchecker(timeSeriesWind, isTSintramodal, isTSgenerated, &windTScounter, study);
    if (not windIntraModalchecker.check())
        return false;
    nbTimeseriesByMode[indexTS] = windIntraModalchecker.getTimeSeriesNumber();

    // Solar ...
    indexTS = TS_INDEX(Data::timeSeriesSolar);
    solarTimeSeriesCounter solarTScounter(study);
    InterModalConsistencyChecker solarIntraModalchecker(timeSeriesSolar, isTSintramodal, isTSgenerated, &solarTScounter, study);
    if (not solarIntraModalchecker.check())
        return false;
    nbTimeseriesByMode[indexTS] = solarIntraModalchecker.getTimeSeriesNumber();

    // Thermal ...
    indexTS = TS_INDEX(Data::timeSeriesThermal);
    thermalTimeSeriesCounter thermalTScounter(study);
    InterModalConsistencyChecker thermalIntraModalchecker(timeSeriesThermal, isTSintramodal, isTSgenerated, &thermalTScounter, study);
    if (not thermalIntraModalchecker.check())
        return false;
    nbTimeseriesByMode[indexTS] = thermalIntraModalchecker.getTimeSeriesNumber();

    // Renewable clusters ...
    indexTS = TS_INDEX(Data::timeSeriesRenewable);
    renewableTimeSeriesCounter renewableTScounter(study);
    InterModalConsistencyChecker renewableIntraModalchecker(timeSeriesRenewable, isTSintramodal, isTSgenerated, &renewableTScounter, study);
    if (not renewableIntraModalchecker.check())
        return false;
    nbTimeseriesByMode[indexTS] = renewableIntraModalchecker.getTimeSeriesNumber();

    return true;
}

bool checkInterModalConsistencyForArea( Data::Area& area,
                                        const bool* isTSintermodal,
                                        const bool* isTSgenerated,
                                        Data::Study& study)
{
    // 1. Making a list of TS numbers :
    //    In this list, we put the numbers of TS of every "inter-modal" mode over the current area.
    // 2. All elements of this list must be identical
    
    // The list containing the numbers of TS of every "inter-modal" mode over the current area
    std::vector<uint> listNumberTsOverArea;

    auto& parameters = study.parameters;

    // Load : Add load's number of TS in area ...
    int indexTS = TS_INDEX(Data::timeSeriesLoad);
    if (isTSintermodal[indexTS])
    {
        uint nbTimeSeries = isTSgenerated[indexTS] ? parameters.nbTimeSeriesLoad : area.load.series->series.width;
        listNumberTsOverArea.push_back(nbTimeSeries);
    }

    // Solar : Add solar's number of TS in area ...
    indexTS = TS_INDEX(Data::timeSeriesSolar);
    if (isTSintermodal[indexTS])
    {
        uint nbTimeSeries = isTSgenerated[indexTS] ? parameters.nbTimeSeriesSolar : area.solar.series->series.width;
        listNumberTsOverArea.push_back(nbTimeSeries);
    }

    // Wind : Add wind's number of TS in area ...
    indexTS = TS_INDEX(Data::timeSeriesWind);
    if (isTSintermodal[indexTS])
    {
        uint nbTimeSeries = isTSgenerated[indexTS] ? parameters.nbTimeSeriesWind : area.wind.series->series.width;
        listNumberTsOverArea.push_back(nbTimeSeries);
    }

    // Hydro : Add hydro's number of TS in area ...
    indexTS = TS_INDEX(Data::timeSeriesHydro);
    if (isTSintermodal[indexTS])
    {
        uint nbTimeSeries = isTSgenerated[indexTS] ? parameters.nbTimeSeriesHydro : area.hydro.series->count;
        listNumberTsOverArea.push_back(nbTimeSeries);
    }

    // Thermal : Add thermal's number of TS of each cluster in area ...
    indexTS = TS_INDEX(Data::timeSeriesThermal);
    if (isTSintermodal[indexTS])
    {
        const uint clusterCount = (uint)area.thermal.clusterCount();
        for (unsigned int j = 0; j != clusterCount; ++j)
        {
            auto& cluster = *(area.thermal.clusters[j]);
            uint nbTimeSeries = isTSgenerated[indexTS] ? parameters.nbTimeSeriesThermal : cluster.series->series.width;
            listNumberTsOverArea.push_back(nbTimeSeries);
        }
    }

    // Renewable clusters : Add renewable's number of TS of each cluster in area ...
    indexTS = TS_INDEX(Data::timeSeriesRenewable);
    if (isTSintermodal[indexTS])
    {
        const uint clusterCount = (uint)area.renewable.clusterCount();
        for (unsigned int j = 0; j != clusterCount; ++j)
        {
            auto& cluster = *(area.renewable.clusters[j]);
            uint nbTimeSeries = cluster.series->series.width;
            listNumberTsOverArea.push_back(nbTimeSeries);
        }
    }

    // Now check if all elements of list of TS numbers are identical
    if (std::adjacent_find(listNumberTsOverArea.begin(), listNumberTsOverArea.end(), std::not_equal_to<uint>()) != listNumberTsOverArea.end())
    {
        logs.error() << "Inter-modal correlation: time-series numbers of inter-modal modes in area '" << area.name << "'"
            << " are not identical";

        return false;
    }

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

    const bool isTSintermodal[Data::timeSeriesCount]
      = {0 != (Data::timeSeriesLoad & parameters.interModal),
         0 != (Data::timeSeriesHydro & parameters.interModal),
         0 != ((Data::timeSeriesWind & parameters.interModal) && (parameters.renewableGeneration() == Data::rgAggregated)),
         0 != (Data::timeSeriesThermal & parameters.interModal),
         0 != ((Data::timeSeriesSolar & parameters.interModal) && (parameters.renewableGeneration() == Data::rgAggregated)),
         0 != ((Data::timeSeriesRenewable & parameters.interModal) && (parameters.renewableGeneration() == Data::rgClusters))};

    if (std::any_of(std::begin(isTSintermodal), std::end(isTSintermodal), [](bool x) { return x; }))
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

        auto end = study.areas.end();
        for (auto i = study.areas.begin(); i != end; ++i)
        {
            auto& area = *(i->second);
            
            if (not checkInterModalConsistencyForArea(area, isTSintermodal, isTSgenerated, study))
                return false;

            Matrix<uint32>* tsNumbers = nullptr;
            if (isTSintermodal[TS_INDEX(Data::timeSeriesLoad)])
                tsNumbers = &(area.load.series->timeseriesNumbers);
            else
            {
                if (isTSintermodal[TS_INDEX(Data::timeSeriesSolar)])
                    tsNumbers = &(area.solar.series->timeseriesNumbers);
                else if (isTSintermodal[TS_INDEX(Data::timeSeriesWind)])
                    tsNumbers = &(area.wind.series->timeseriesNumbers);
                else if (isTSintermodal[TS_INDEX(Data::timeSeriesHydro)])
                    tsNumbers = &(area.hydro.series->timeseriesNumbers);
                else if (isTSintermodal[TS_INDEX(Data::timeSeriesThermal)]
                         && area.thermal.clusterCount() > 0)
                    tsNumbers = &(area.thermal.clusters[0]->series->timeseriesNumbers);
                else if (isTSintermodal[TS_INDEX(Data::timeSeriesRenewable)]
                         && area.renewable.clusterCount() > 0)
                    tsNumbers = &(area.renewable.clusters[0]->series->timeseriesNumbers);
            }
            assert(tsNumbers);

            for (unsigned int y = 0; y < years; ++y)
            {
                const unsigned int draw = tsNumbers->entry[0][y];
                assert(draw < 100000);

                assert(y < area.load.series->timeseriesNumbers.height);
                if (isTSintermodal[TS_INDEX(Data::timeSeriesLoad)])
                    area.load.series->timeseriesNumbers.entry[0][y] = draw;

                assert(y < area.solar.series->timeseriesNumbers.height);
                if (isTSintermodal[TS_INDEX(Data::timeSeriesSolar)])
                    area.solar.series->timeseriesNumbers.entry[0][y] = draw;

                assert(y < area.wind.series->timeseriesNumbers.height);
                if (isTSintermodal[TS_INDEX(Data::timeSeriesWind)])
                    area.wind.series->timeseriesNumbers.entry[0][y] = draw;

                assert(y < area.hydro.series->timeseriesNumbers.height);
                if (isTSintermodal[TS_INDEX(Data::timeSeriesHydro)])
                    area.hydro.series->timeseriesNumbers.entry[0][y] = draw;

                if (isTSintermodal[TS_INDEX(Data::timeSeriesThermal)])
                {
                    unsigned int clusterCount = area.thermal.clusterCount();
                    for (unsigned int i = 0; i != clusterCount; ++i)
                    {
                        auto& cluster = *(area.thermal.clusters[i]);
                        assert(y < cluster.series->timeseriesNumbers.height);
                        cluster.series->timeseriesNumbers.entry[0][y] = draw;
                    }
                }
                if (isTSintermodal[TS_INDEX(Data::timeSeriesRenewable)])
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
