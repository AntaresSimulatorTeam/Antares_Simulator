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

        for (uint i = 0; i != area.thermal.clusterCount(); ++i)
        {
            auto& cluster = *(area.thermal.clusters[i]);
            cluster.series->timeseriesNumbers.zero();
        }
    });

    return true;
}


class areaTSNumbersListRetriever
{
public:
    areaTSNumbersListRetriever(Data::Study& study) : study_(study) {}
    virtual std::vector<uint> getAreaTimeSeriesNumber(const Data::Area& area) = 0;
    virtual uint getGeneratedTimeSeriesNumber() = 0;
protected:
    Data::Study& study_;
};

class areaLoadTSNumbersListRetriever : public areaTSNumbersListRetriever
{
public:
    areaLoadTSNumbersListRetriever(Data::Study& study) : areaTSNumbersListRetriever(study) {}
    std::vector<uint> getAreaTimeSeriesNumber(const Data::Area& area)
    {
        std::vector<uint> to_return = { area.load.series->series.width };
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber() { return study_.parameters.nbTimeSeriesLoad; }
};

class areaHydroTSNumbersListRetriever : public areaTSNumbersListRetriever
{
public:
    areaHydroTSNumbersListRetriever(Data::Study& study) : areaTSNumbersListRetriever(study) {}
    std::vector<uint> getAreaTimeSeriesNumber(const Data::Area& area)
    {
        std::vector<uint> to_return = { area.hydro.series->count };
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber() { return study_.parameters.nbTimeSeriesHydro; }
};

class areaWindTSNumbersListRetriever : public areaTSNumbersListRetriever
{
public:
    areaWindTSNumbersListRetriever(Data::Study& study) : areaTSNumbersListRetriever(study) {}
    std::vector<uint> getAreaTimeSeriesNumber(const Data::Area& area)
    {
        std::vector<uint> to_return = { area.wind.series->series.width };
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber() { return study_.parameters.nbTimeSeriesWind; }
};

class areaSolarTSNumbersListRetriever : public areaTSNumbersListRetriever
{
public:
    areaSolarTSNumbersListRetriever(Data::Study& study) : areaTSNumbersListRetriever(study) {}
    std::vector<uint> getAreaTimeSeriesNumber(const Data::Area& area)
    {
        std::vector<uint> to_return = { area.solar.series->series.width };
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber() { return study_.parameters.nbTimeSeriesSolar; }
};

class areaThermalTSNumbersListRetriever : public areaTSNumbersListRetriever
{
public:
    areaThermalTSNumbersListRetriever(Data::Study& study) : areaTSNumbersListRetriever(study) {}
    std::vector<uint> getAreaTimeSeriesNumber(const Data::Area& area)
    {
        std::vector<uint> to_return;
        uint clusterCount = (uint) area.thermal.clusterCount();
        for (uint i = 0; i != clusterCount; ++i)
        {
            auto& cluster = *(area.thermal.clusters[i]);
            to_return.push_back(cluster.series->series.width);
        }
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber() { return study_.parameters.nbTimeSeriesThermal; }
};

class areaRenewableTSNumbersListRetriever : public areaTSNumbersListRetriever
{
public:
    areaRenewableTSNumbersListRetriever(Data::Study& study) : areaTSNumbersListRetriever(study) {}
    std::vector<uint> getAreaTimeSeriesNumber(const Data::Area& area)
    {
        std::vector<uint> to_return;
        uint clusterCount = (uint) area.renewable.clusterCount();
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
    InterModalConsistencyChecker(const TimeSeries ts, const bool* isTSintramodal, const bool* isTSgenerated, areaTSNumbersListRetriever* tsCounter, Data::Study & study)
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
    areaTSNumbersListRetriever* tsCounter_;
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
    areaLoadTSNumbersListRetriever loadTSretriever(study);
    InterModalConsistencyChecker loadIntraModalchecker(timeSeriesLoad, isTSintramodal, isTSgenerated, &loadTSretriever, study);
    if (not loadIntraModalchecker.check())
        return false;
    nbTimeseriesByMode[indexTS] = loadIntraModalchecker.getTimeSeriesNumber();

    // Hydro ...
    indexTS = TS_INDEX(Data::timeSeriesHydro);
    areaHydroTSNumbersListRetriever hydroTSretriever(study);
    InterModalConsistencyChecker hydroIntraModalchecker(timeSeriesHydro, isTSintramodal, isTSgenerated, &hydroTSretriever, study);
    if (not hydroIntraModalchecker.check())
        return false;
    nbTimeseriesByMode[indexTS] = hydroIntraModalchecker.getTimeSeriesNumber();

    // Wind ...
    indexTS = TS_INDEX(Data::timeSeriesWind);
    areaWindTSNumbersListRetriever windTSretriever(study);
    InterModalConsistencyChecker windIntraModalchecker(timeSeriesWind, isTSintramodal, isTSgenerated, &windTSretriever, study);
    if (not windIntraModalchecker.check())
        return false;
    nbTimeseriesByMode[indexTS] = windIntraModalchecker.getTimeSeriesNumber();

    // Solar ...
    indexTS = TS_INDEX(Data::timeSeriesSolar);
    areaSolarTSNumbersListRetriever solarTSretriever(study);
    InterModalConsistencyChecker solarIntraModalchecker(timeSeriesSolar, isTSintramodal, isTSgenerated, &solarTSretriever, study);
    if (not solarIntraModalchecker.check())
        return false;
    nbTimeseriesByMode[indexTS] = solarIntraModalchecker.getTimeSeriesNumber();

    // Thermal ...
    indexTS = TS_INDEX(Data::timeSeriesThermal);
    areaThermalTSNumbersListRetriever thermalTSretriever(study);
    InterModalConsistencyChecker thermalIntraModalchecker(timeSeriesThermal, isTSintramodal, isTSgenerated, &thermalTSretriever, study);
    if (not thermalIntraModalchecker.check())
        return false;
    nbTimeseriesByMode[indexTS] = thermalIntraModalchecker.getTimeSeriesNumber();

    // Renewable clusters ...
    indexTS = TS_INDEX(Data::timeSeriesRenewable);
    areaRenewableTSNumbersListRetriever renewableTSretriever(study);
    InterModalConsistencyChecker renewableIntraModalchecker(timeSeriesRenewable, isTSintramodal, isTSgenerated, &renewableTSretriever, study);
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
        for (uint j = 0; j != clusterCount; ++j)
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
        for (uint j = 0; j != clusterCount; ++j)
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

void drawTSnumbersForIntraModal(    uint32* intramodal_draws,
                                    const bool* isTSintramodal, 
                                    uint* nbTimeseriesByMode, 
                                    Data::Study& study)
{
    for (uint tsKind = 0; tsKind < Data::timeSeriesCount; ++tsKind)
    {
        if (isTSintramodal[tsKind])
        {
            intramodal_draws[tsKind]
                = (uint32)(floor(study.runtime->random[Data::seedTimeseriesNumbers].next()
                    * nbTimeseriesByMode[tsKind]));
        }
    }
}

void storeTSnumbersForIntraModal(   const bool* isTSintramodal, 
                                    uint32* intramodal_draws, 
                                    uint year, 
                                    Data::Study& study)
{
    study.areas.each([&](Data::Area& area) {
        // -------------
        // Load ...
        // -------------
        assert(year < area.load.series->timeseriesNumbers.height);
        int indexTS = TS_INDEX(Data::timeSeriesLoad);

        if (isTSintramodal[indexTS])       
            area.load.series->timeseriesNumbers[0][year] = intramodal_draws[indexTS];

        // -------------
        // Solar ...
        // -------------
        assert(year < area.solar.series->timeseriesNumbers.height);
        indexTS = TS_INDEX(Data::timeSeriesSolar);

        if (isTSintramodal[indexTS])
            area.solar.series->timeseriesNumbers[0][year] = intramodal_draws[indexTS];

        // -------------
        // Wind ...
        // -------------
        assert(year < area.wind.series->timeseriesNumbers.height);
        indexTS = TS_INDEX(Data::timeSeriesWind);

        if (isTSintramodal[indexTS])
            area.wind.series->timeseriesNumbers[0][year] = intramodal_draws[indexTS];

        // -------------
        // Hydro ...
        // -------------
        assert(year < area.hydro.series->timeseriesNumbers.height);
        indexTS = TS_INDEX(Data::timeSeriesHydro);

        if (isTSintramodal[indexTS])
            area.hydro.series->timeseriesNumbers[0][year] = intramodal_draws[indexTS];

        // -------------
        // Thermal ...
        // -------------
        indexTS = TS_INDEX(Data::timeSeriesThermal);

        if (isTSintramodal[indexTS])
        {
            auto end_th_clusters = area.thermal.list.mapping.end();
            for (auto i = area.thermal.list.mapping.begin(); i != end_th_clusters; ++i)
            {
                Data::ThermalClusterList::SharedPtr cluster = i->second;
                if (cluster->enabled)
                    cluster->series->timeseriesNumbers.entry[0][year] = intramodal_draws[indexTS];
            }
        }

        // --------------------------
        // Renewable clusters ...
        // --------------------------
        indexTS = TS_INDEX(Data::timeSeriesRenewable);

        if (isTSintramodal[indexTS])
        {
            auto end_rn_clusters = area.renewable.list.cluster.end();
            for (auto j = area.renewable.list.cluster.begin(); j != end_rn_clusters; ++j)
            {
                Data::RenewableClusterList::SharedPtr cluster = j->second;
                if (cluster->enabled)
                    cluster->series->timeseriesNumbers.entry[0][year] = intramodal_draws[indexTS];
            }
        }
    });
}

void drawAndStoreTSnumbersForNOTintraModal( const bool* isTSintramodal, 
                                            const bool* isTSgenerated, 
                                            uint* nbTimeseriesByMode, 
                                            uint year, 
                                            Data::Study& study)
{
    study.areas.each([&](Data::Area& area) {
        // -------------
        // Load ...
        // -------------
        int indexTS = TS_INDEX(Data::timeSeriesLoad);

        if (not isTSintramodal[indexTS])
        {
            uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                       : area.load.series->series.width;
            area.load.series->timeseriesNumbers[0][year] = (uint32)(
                floor(study.runtime->random[Data::seedTimeseriesNumbers].next() * nbTimeSeries));
        }

        // -------------
        // Solar ...
        // -------------
        indexTS = TS_INDEX(Data::timeSeriesSolar);

        if (not isTSintramodal[indexTS])
        {
            uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                       : area.solar.series->series.width;
            area.solar.series->timeseriesNumbers[0][year]
                = (uint32)(floor(study.runtime->random[Data::seedTimeseriesNumbers].next() * nbTimeSeries));
        }

        // -------------
        // Wind ...
        // -------------
        indexTS = TS_INDEX(Data::timeSeriesWind);

        if (not isTSintramodal[indexTS])
        {
            uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                       : area.wind.series->series.width;
            area.wind.series->timeseriesNumbers[0][year]
                = (uint32)(floor(study.runtime->random[Data::seedTimeseriesNumbers].next()
                    * nbTimeSeries));
        }

        // -------------
        // Hydro ...
        // -------------
        indexTS = TS_INDEX(Data::timeSeriesHydro);

        if (not isTSintramodal[indexTS])
        {
            uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                       : area.hydro.series->ror.width;
            area.hydro.series->timeseriesNumbers[0][year]
                = (uint32)(floor(study.runtime->random[Data::seedTimeseriesNumbers].next() * nbTimeSeries));
        }

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
                    cluster->series->timeseriesNumbers.entry[0][year]
                        = (uint32)(floor(study.runtime->random[Data::seedTimeseriesNumbers].next()
                            * nbTimeSeries));
                }
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
                    cluster->series->timeseriesNumbers.entry[0][year]
                        = (uint32)(floor(study.runtime->random[Data::seedTimeseriesNumbers].next()
                            * nbTimeSeries));
                }
            }
        }
    });
}

bool TimeSeriesNumbers::Generate(Data::Study& study)
{
    logs.info() << "Preparing time-series numbers...";

    auto& parameters = study.parameters;

    if (parameters.derated)
        return GenerateDeratedMode(study);

    const uint years = 1 + study.runtime->rangeLimits.year[Data::rangeEnd];

    const bool isTSintramodal[Data::timeSeriesCount]
      = {0 != (Data::timeSeriesLoad & parameters.intraModal),
         0 != (Data::timeSeriesHydro & parameters.intraModal),
         0 != ((Data::timeSeriesWind & parameters.intraModal)
               && (parameters.renewableGeneration() == Data::rgAggregated)),
         0 != (Data::timeSeriesThermal & parameters.intraModal),
         0 != ((Data::timeSeriesSolar & parameters.intraModal)
               && (parameters.renewableGeneration() == Data::rgAggregated)),
         0 != ((Data::timeSeriesRenewable & parameters.intraModal)
               && (parameters.renewableGeneration() == Data::rgClusters))};

    uint nbTimeseriesByMode[Data::timeSeriesCount];

    uint32 intramodal_draws[Data::timeSeriesCount] = {0, 0, 0, 0, 0, 0};

    const bool isTSgenerated[Data::timeSeriesCount]
      = {0 != (Data::timeSeriesLoad & parameters.timeSeriesToRefresh),
         0 != (Data::timeSeriesHydro & parameters.timeSeriesToRefresh),
         0 != (Data::timeSeriesWind & parameters.timeSeriesToRefresh),
         0 != (Data::timeSeriesThermal & parameters.timeSeriesToRefresh),
         0 != (Data::timeSeriesSolar & parameters.timeSeriesToRefresh),
         false}; // TS generation is always disabled for renewables

    if (not checkIntraModalConsistency(nbTimeseriesByMode, isTSintramodal, isTSgenerated, study))
        return false;

    for (uint year = 0; year < years; ++year)
    {
        // Intra-modal : draw and store TS numbres
        drawTSnumbersForIntraModal(intramodal_draws, isTSintramodal, nbTimeseriesByMode, study);
        storeTSnumbersForIntraModal(isTSintramodal, intramodal_draws, year, study);

        // Draw and store TS numbers for NOT intra-modal TS
        drawAndStoreTSnumbersForNOTintraModal(isTSintramodal, isTSgenerated, nbTimeseriesByMode, year, study);
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

            for (uint year = 0; year < years; ++year)
            {
                const uint draw = tsNumbers->entry[0][year];
                assert(draw < 100000);

                assert(year < area.load.series->timeseriesNumbers.height);
                if (isTSintermodal[TS_INDEX(Data::timeSeriesLoad)])
                    area.load.series->timeseriesNumbers.entry[0][year] = draw;

                assert(year < area.solar.series->timeseriesNumbers.height);
                if (isTSintermodal[TS_INDEX(Data::timeSeriesSolar)])
                    area.solar.series->timeseriesNumbers.entry[0][year] = draw;

                assert(year < area.wind.series->timeseriesNumbers.height);
                if (isTSintermodal[TS_INDEX(Data::timeSeriesWind)])
                    area.wind.series->timeseriesNumbers.entry[0][year] = draw;

                assert(year < area.hydro.series->timeseriesNumbers.height);
                if (isTSintermodal[TS_INDEX(Data::timeSeriesHydro)])
                    area.hydro.series->timeseriesNumbers.entry[0][year] = draw;

                if (isTSintermodal[TS_INDEX(Data::timeSeriesThermal)])
                {
                    uint clusterCount = (uint) area.thermal.clusterCount();
                    for (uint i = 0; i != clusterCount; ++i)
                    {
                        auto& cluster = *(area.thermal.clusters[i]);
                        assert(year < cluster.series->timeseriesNumbers.height);
                        cluster.series->timeseriesNumbers.entry[0][year] = draw;
                    }
                }
                if (isTSintermodal[TS_INDEX(Data::timeSeriesRenewable)])
                {
                    uint clusterCount = (uint) area.renewable.clusterCount();
                    for (uint i = 0; i != clusterCount; ++i)
                    {
                        auto& cluster = *(area.renewable.clusters[i]);
                        assert(year < cluster.series->timeseriesNumbers.height);
                        cluster.series->timeseriesNumbers.entry[0][year] = draw;
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
