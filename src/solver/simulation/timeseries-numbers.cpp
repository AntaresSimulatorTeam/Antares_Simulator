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
#include <array>

#include "timeseries-numbers.h"

using namespace Yuni;
using namespace Antares::Data;
using namespace std;

const map<TimeSeries, int> ts_to_tsIndex = {{timeSeriesLoad, 0},
                                            {timeSeriesHydro, 1},
                                            {timeSeriesWind, 2},
                                            {timeSeriesThermal, 3},
                                            {timeSeriesSolar, 4},
                                            {timeSeriesRenewable, 5},
                                            {timeSeriesTransmissionCapacities, 6}};

const map<TimeSeries, string> ts_to_tsTitle
  = {{timeSeriesLoad, "load"},
     {timeSeriesHydro, "hydro"},
     {timeSeriesWind, "wind"},
     {timeSeriesThermal, "thermal"},
     {timeSeriesSolar, "solar"},
     {timeSeriesRenewable, "renewable clusters"},
     {timeSeriesTransmissionCapacities, "transmission capacities"}};

void addInterModalTimeSeriesToMessage(const array<bool, timeSeriesCount>& isTSintermodal,
                                      std::string& interModalTsMsg)
{
    bool isFirstLogged = true;

    map<TimeSeries, int>::const_iterator it = ts_to_tsIndex.begin();
    for (; it != ts_to_tsIndex.end(); ++it)
    {
        if (isTSintermodal[it->second])
        {
            if (!isFirstLogged)
                interModalTsMsg.append(", ");
            interModalTsMsg.append(ts_to_tsTitle.at(it->first));
            isFirstLogged = false;
        }
    }
}

namespace Antares
{
namespace Solver
{
static bool GenerateDeratedMode(Study& study)
{
    logs.info() << "  :: using the `derated` mode";
    if (study.parameters.useCustomScenario)
        logs.warning() << "The derated mode is enabled. The custom building mode will be ignored";

    study.areas.each([&](Area& area) {
        area.load.series->timeseriesNumbers.zero();
        area.solar.series->timeseriesNumbers.zero();
        area.wind.series->timeseriesNumbers.zero();
        area.hydro.series->timeseriesNumbers.zero();

        for (uint i = 0; i != area.thermal.clusterCount(); ++i)
        {
            auto& cluster = *(area.thermal.clusters[i]);
            cluster.series->timeseriesNumbers.zero();
        }

        for (uint i = 0; i != area.renewable.clusterCount(); ++i)
        {
            auto& cluster = *(area.renewable.clusters[i]);
            cluster.series->timeseriesNumbers.zero();
        }
    });

    return true;
}

class areaNumberOfTSretriever
{
public:
    areaNumberOfTSretriever(Study& study) : study_(study)
    {
    }
    virtual std::vector<uint> getAreaTimeSeriesNumber(const Area& area) = 0;
    virtual uint getGeneratedTimeSeriesNumber() = 0;

protected:
    Study& study_;
};

class loadAreaNumberOfTSretriever : public areaNumberOfTSretriever
{
public:
    loadAreaNumberOfTSretriever(Study& study) : areaNumberOfTSretriever(study)
    {
    }
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area)
    {
        std::vector<uint> to_return = {area.load.series->series.width};
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber()
    {
        return study_.parameters.nbTimeSeriesLoad;
    }
};

class hydroAreaNumberOfTSretriever : public areaNumberOfTSretriever
{
public:
    hydroAreaNumberOfTSretriever(Study& study) : areaNumberOfTSretriever(study)
    {
    }
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area)
    {
        std::vector<uint> to_return = {area.hydro.series->count};
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber()
    {
        return study_.parameters.nbTimeSeriesHydro;
    }
};

class windAreaNumberOfTSretriever : public areaNumberOfTSretriever
{
public:
    windAreaNumberOfTSretriever(Study& study) : areaNumberOfTSretriever(study)
    {
    }
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area)
    {
        std::vector<uint> to_return = {area.wind.series->series.width};
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber()
    {
        return study_.parameters.nbTimeSeriesWind;
    }
};

class solarAreaNumberOfTSretriever : public areaNumberOfTSretriever
{
public:
    solarAreaNumberOfTSretriever(Study& study) : areaNumberOfTSretriever(study)
    {
    }
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area)
    {
        std::vector<uint> to_return = {area.solar.series->series.width};
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber()
    {
        return study_.parameters.nbTimeSeriesSolar;
    }
};

class thermalAreaNumberOfTSretriever : public areaNumberOfTSretriever
{
public:
    thermalAreaNumberOfTSretriever(Study& study) : areaNumberOfTSretriever(study)
    {
    }
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area)
    {
        std::vector<uint> to_return;
        uint clusterCount = (uint)area.thermal.clusterCount();
        for (uint i = 0; i != clusterCount; ++i)
        {
            auto& cluster = *(area.thermal.clusters[i]);
            to_return.push_back(cluster.series->series.width);
        }
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber()
    {
        return study_.parameters.nbTimeSeriesThermal;
    }
};

class renewClustersAreaNumberOfTSretriever : public areaNumberOfTSretriever
{
public:
    renewClustersAreaNumberOfTSretriever(Study& study) : areaNumberOfTSretriever(study)
    {
    }
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area)
    {
        std::vector<uint> to_return;
        uint clusterCount = (uint)area.renewable.clusterCount();
        for (uint i = 0; i != clusterCount; ++i)
        {
            auto& cluster = *(area.renewable.clusters[i]);
            to_return.push_back(cluster.series->series.width);
        }
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber()
    {
        return 1;
    }
};

class areaLinksTransCapaNumberOfTSretriever : public areaNumberOfTSretriever
{
public:
    using areaNumberOfTSretriever::areaNumberOfTSretriever;
    virtual ~areaLinksTransCapaNumberOfTSretriever() = default;
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area) override
    {
        std::vector<uint> to_return;

        for (auto it = area.links.begin(); it != area.links.end(); ++it)
        {
            const auto& link = *(it->second);
            to_return.push_back(link.directCapacities.width);
            to_return.push_back(link.indirectCapacities.width);
        }
        return to_return;
    }
    uint getGeneratedTimeSeriesNumber() override
    {
        return 1;
    }
};

class IntraModalConsistencyChecker
{
public:
    IntraModalConsistencyChecker(const TimeSeries ts,
                                 const array<bool, timeSeriesCount>& isTSintramodal,
                                 const array<bool, timeSeriesCount>& isTSgenerated,
                                 areaNumberOfTSretriever* tsCounter,
                                 Study& study) :
     tsCounter_(tsCounter), study_(study), nbTimeseries_(0)
    {
        int indexTS = ts_to_tsIndex.at(ts);
        isTSintramodal_ = isTSintramodal[indexTS];
        isTSgenerated_ = isTSgenerated[indexTS];
        tsTitle_ = ts_to_tsTitle.at(ts);
    }
    ~IntraModalConsistencyChecker() = default;
    bool check();
    uint getTimeSeriesNumber()
    {
        return nbTimeseries_;
    }

private:
    bool checkTSconsistency();

private:
    bool isTSintramodal_;
    bool isTSgenerated_;
    areaNumberOfTSretriever* tsCounter_;
    Study& study_;
    uint nbTimeseries_;
    string tsTitle_;
};

bool IntraModalConsistencyChecker::check()
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

bool IntraModalConsistencyChecker::checkTSconsistency()
{
    logs.info() << "Checking intra-modal correlation: " << tsTitle_;
    std::vector<uint> listNumberTS;
    for (auto i = study_.areas.begin(); i != study_.areas.end(); ++i)
    {
        const Area& area = *(i->second);
        vector<uint> areaNumberTSList = tsCounter_->getAreaTimeSeriesNumber(area);
        listNumberTS.insert(listNumberTS.end(), areaNumberTSList.begin(), areaNumberTSList.end());
    }

    if (std::adjacent_find(listNumberTS.begin(),
                           listNumberTS.end(),
                           Antares::Solver::TimeSeriesNumbers::compareWidth)
        != listNumberTS.end())
    {
        logs.error() << "Intra-modal correlation: " << tsTitle_
                     << "'s numbers of time-series are not equal for all areas";
        return false;
    }

    auto result = find_if(listNumberTS.begin(), listNumberTS.end(), [](uint x) { return x != 1; });
    if (result == listNumberTS.end())
    { // All elements are equal to 1 or list is empty
        nbTimeseries_ = 1;
    }
    else
    {
        nbTimeseries_ = *result;
    }
    return true;
}

bool checkIntraModalConsistency(array<uint, timeSeriesCount>& nbTimeseriesByMode,
                                const array<bool, timeSeriesCount>& isTSintramodal,
                                const array<bool, timeSeriesCount>& isTSgenerated,
                                Study& study)
{
    // Initialization of a map associating a time-series to an object that retrieves
    // the number of time series inside an area
    using mapTStoRetriever = map<TimeSeries, shared_ptr<areaNumberOfTSretriever>>;
    mapTStoRetriever ts_to_numberOfTSretrievers;
    ts_to_numberOfTSretrievers[timeSeriesLoad] = make_shared<loadAreaNumberOfTSretriever>(study);
    ts_to_numberOfTSretrievers[timeSeriesHydro] = make_shared<hydroAreaNumberOfTSretriever>(study);
    ts_to_numberOfTSretrievers[timeSeriesWind] = make_shared<windAreaNumberOfTSretriever>(study);
    ts_to_numberOfTSretrievers[timeSeriesSolar] = make_shared<solarAreaNumberOfTSretriever>(study);
    ts_to_numberOfTSretrievers[timeSeriesThermal]
      = make_shared<thermalAreaNumberOfTSretriever>(study);
    ts_to_numberOfTSretrievers[timeSeriesRenewable]
      = make_shared<renewClustersAreaNumberOfTSretriever>(study);
    ts_to_numberOfTSretrievers[timeSeriesTransmissionCapacities]
      = make_shared<areaLinksTransCapaNumberOfTSretriever>(study);

    // Loop over TS kind and check intra-modal consistency
    mapTStoRetriever::iterator it = ts_to_numberOfTSretrievers.begin();
    for (; it != ts_to_numberOfTSretrievers.end(); ++it)
    {
        const TimeSeries tsKind = it->first;
        areaNumberOfTSretriever* tsRetriever = (it->second).get();
        int indexTS = ts_to_tsIndex.at(it->first);
        IntraModalConsistencyChecker intraModalchecker(
          tsKind, isTSintramodal, isTSgenerated, tsRetriever, study);
        if (!intraModalchecker.check())
            return false;
        nbTimeseriesByMode[indexTS] = intraModalchecker.getTimeSeriesNumber();
    }

    return true;
}

bool checkInterModalConsistencyForArea(Area& area,
                                       const array<bool, timeSeriesCount>& isTSintermodal,
                                       const array<bool, timeSeriesCount>& isTSgenerated,
                                       Study& study)
{
    // 1. Making a list of TS numbers :
    //    In this list, we put the numbers of TS of every "inter-modal" mode over the current area.
    // 2. All elements of this list must be identical

    // The list containing the numbers of TS of every "inter-modal" mode over the current area
    std::vector<uint> listNumberTsOverArea;

    auto& parameters = study.parameters;

    // Load : Add load's number of TS in area ...
    int indexTS = ts_to_tsIndex.at(timeSeriesLoad);
    if (isTSintermodal[indexTS])
    {
        uint nbTimeSeries
          = isTSgenerated[indexTS] ? parameters.nbTimeSeriesLoad : area.load.series->series.width;
        listNumberTsOverArea.push_back(nbTimeSeries);
    }

    // Solar : Add solar's number of TS in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesSolar);
    if (isTSintermodal[indexTS])
    {
        uint nbTimeSeries
          = isTSgenerated[indexTS] ? parameters.nbTimeSeriesSolar : area.solar.series->series.width;
        listNumberTsOverArea.push_back(nbTimeSeries);
    }

    // Wind : Add wind's number of TS in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesWind);
    if (isTSintermodal[indexTS])
    {
        uint nbTimeSeries
          = isTSgenerated[indexTS] ? parameters.nbTimeSeriesWind : area.wind.series->series.width;
        listNumberTsOverArea.push_back(nbTimeSeries);
    }

    // Hydro : Add hydro's number of TS in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesHydro);
    if (isTSintermodal[indexTS])
    {
        uint nbTimeSeries
          = isTSgenerated[indexTS] ? parameters.nbTimeSeriesHydro : area.hydro.series->count;
        listNumberTsOverArea.push_back(nbTimeSeries);
    }

    // Thermal : Add thermal's number of TS of each cluster in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesThermal);
    if (isTSintermodal[indexTS])
    {
        const uint clusterCount = (uint)area.thermal.clusterCount();
        for (uint j = 0; j != clusterCount; ++j)
        {
            auto& cluster = *(area.thermal.clusters[j]);
            uint nbTimeSeries = isTSgenerated[indexTS] ? parameters.nbTimeSeriesThermal
                                                       : cluster.series->series.width;
            listNumberTsOverArea.push_back(nbTimeSeries);
        }
    }

    // Renewable clusters : Add renewable's number of TS of each cluster in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesRenewable);
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

    // Now check if all elements of list of TS numbers are identical or equal to 1
    if (std::adjacent_find(listNumberTsOverArea.begin(),
                           listNumberTsOverArea.end(),
                           Antares::Solver::TimeSeriesNumbers::compareWidth)
        != listNumberTsOverArea.end())
    {
        logs.error()
          << "Inter-modal correlation: time-series numbers of inter-modal modes in area '"
          << area.name << "'"
          << " are not identical";

        return false;
    }

    return true;
}

void drawTSnumbersForIntraModal(array<uint32, timeSeriesCount>& intramodal_draws,
                                const array<bool, timeSeriesCount>& isTSintramodal,
                                array<uint, timeSeriesCount>& nbTimeseriesByMode,
                                MersenneTwister* mersenneTwisterTable)
{
    for (int tsKind = 0; tsKind < timeSeriesCount; ++tsKind)
    {
        if (isTSintramodal[tsKind])
        {
            if (((1 << tsKind) == timeSeriesTransmissionCapacities)
                && nbTimeseriesByMode[tsKind] == 1)
            {
                // Random generator (mersenne-twister) must not be called here
                // in order to avoid a shift in the random generator results
                // that would cause a change of Antares results
                intramodal_draws[tsKind] = 0;
            }
            else
            {
                intramodal_draws[tsKind] = (uint32)(floor(
                  mersenneTwisterTable[seedTimeseriesNumbers].next() * nbTimeseriesByMode[tsKind]));
            }
        }
    }
}

void storeTSnumbersForIntraModal(const array<uint32, timeSeriesCount>& intramodal_draws,
                                 const array<bool, timeSeriesCount>& isTSintramodal,
                                 uint year,
                                 AreaList& areas)
{
    areas.each([&](Area& area) {
        // -------------
        // Load ...
        // -------------
        assert(year < area.load.series->timeseriesNumbers.height);
        int indexTS = ts_to_tsIndex.at(timeSeriesLoad);

        if (isTSintramodal[indexTS])
            area.load.series->timeseriesNumbers[0][year] = intramodal_draws[indexTS];

        // -------------
        // Solar ...
        // -------------
        assert(year < area.solar.series->timeseriesNumbers.height);
        indexTS = ts_to_tsIndex.at(timeSeriesSolar);

        if (isTSintramodal[indexTS])
            area.solar.series->timeseriesNumbers[0][year] = intramodal_draws[indexTS];

        // -------------
        // Wind ...
        // -------------
        assert(year < area.wind.series->timeseriesNumbers.height);
        indexTS = ts_to_tsIndex.at(timeSeriesWind);

        if (isTSintramodal[indexTS])
            area.wind.series->timeseriesNumbers[0][year] = intramodal_draws[indexTS];

        // -------------
        // Hydro ...
        // -------------
        assert(year < area.hydro.series->timeseriesNumbers.height);
        indexTS = ts_to_tsIndex.at(timeSeriesHydro);

        if (isTSintramodal[indexTS])
            area.hydro.series->timeseriesNumbers[0][year] = intramodal_draws[indexTS];

        // -------------
        // Thermal ...
        // -------------
        indexTS = ts_to_tsIndex.at(timeSeriesThermal);

        if (isTSintramodal[indexTS])
        {
            auto end_th_clusters = area.thermal.list.mapping.end();
            for (auto i = area.thermal.list.mapping.begin(); i != end_th_clusters; ++i)
            {
                ThermalClusterList::SharedPtr cluster = i->second;
                if (cluster->enabled)
                    cluster->series->timeseriesNumbers[0][year] = intramodal_draws[indexTS];
            }
        }

        // --------------------------
        // Renewable clusters ...
        // --------------------------
        indexTS = ts_to_tsIndex.at(timeSeriesRenewable);

        if (isTSintramodal[indexTS])
        {
            auto end_rn_clusters = area.renewable.list.cluster.end();
            for (auto j = area.renewable.list.cluster.begin(); j != end_rn_clusters; ++j)
            {
                RenewableClusterList::SharedPtr cluster = j->second;
                if (cluster->enabled)
                    cluster->series->timeseriesNumbers[0][year] = intramodal_draws[indexTS];
            }
        }

        // -------------------------------
        // Transmission capacities ...
        // -------------------------------
        indexTS = ts_to_tsIndex.at(timeSeriesTransmissionCapacities);

        if (isTSintramodal[indexTS])
        {
            for (auto it = area.links.begin(); it != area.links.end(); ++it)
            {
                auto& link = *(it->second);
                link.timeseriesNumbers[0][year] = intramodal_draws[indexTS];
            }
        }
    });
}

void drawAndStoreTSnumbersForNOTintraModal(const array<bool, timeSeriesCount>& isTSintramodal,
                                           const array<bool, timeSeriesCount>& isTSgenerated,
                                           array<uint, timeSeriesCount>& nbTimeseriesByMode,
                                           uint year,
                                           Study& study)
{
    study.areas.each([&](Area& area) {
        // -------------
        // Load ...
        // -------------
        int indexTS = ts_to_tsIndex.at(timeSeriesLoad);

        if (!isTSintramodal[indexTS])
        {
            uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                       : area.load.series->series.width;
            area.load.series->timeseriesNumbers[0][year]
              = (uint32)(floor(study.runtime->random[seedTimeseriesNumbers].next() * nbTimeSeries));
        }

        // -------------
        // Solar ...
        // -------------
        indexTS = ts_to_tsIndex.at(timeSeriesSolar);

        if (!isTSintramodal[indexTS])
        {
            uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                       : area.solar.series->series.width;
            area.solar.series->timeseriesNumbers[0][year]
              = (uint32)(floor(study.runtime->random[seedTimeseriesNumbers].next() * nbTimeSeries));
        }

        // -------------
        // Wind ...
        // -------------
        indexTS = ts_to_tsIndex.at(timeSeriesWind);

        if (!isTSintramodal[indexTS])
        {
            uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                       : area.wind.series->series.width;
            area.wind.series->timeseriesNumbers[0][year]
              = (uint32)(floor(study.runtime->random[seedTimeseriesNumbers].next() * nbTimeSeries));
        }

        // -------------
        // Hydro ...
        // -------------
        indexTS = ts_to_tsIndex.at(timeSeriesHydro);

        if (!isTSintramodal[indexTS])
        {
            uint nbTimeSeries
              = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS] : area.hydro.series->ror.width;
            area.hydro.series->timeseriesNumbers[0][year]
              = (uint32)(floor(study.runtime->random[seedTimeseriesNumbers].next() * nbTimeSeries));
        }

        // -------------
        // Thermal ...
        // -------------
        indexTS = ts_to_tsIndex.at(timeSeriesThermal);

        auto end_th_clusters = area.thermal.list.mapping.end();
        for (auto i = area.thermal.list.mapping.begin(); i != end_th_clusters; ++i)
        {
            ThermalClusterList::SharedPtr cluster = i->second;
            if (not cluster->enabled)
                study.runtime->random[seedTimeseriesNumbers].next();
            else
            {
                if (!isTSintramodal[indexTS])
                {
                    uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                               : cluster->series->series.width;
                    cluster->series->timeseriesNumbers[0][year] = (uint32)(
                      floor(study.runtime->random[seedTimeseriesNumbers].next() * nbTimeSeries));
                }
            }
        }

        // --------------------------
        // Renewable clusters ...
        // --------------------------
        indexTS = ts_to_tsIndex.at(timeSeriesRenewable);

        auto end_rn_clusters = area.renewable.list.cluster.end();
        for (auto j = area.renewable.list.cluster.begin(); j != end_rn_clusters; ++j)
        {
            RenewableClusterList::SharedPtr cluster = j->second;
            if (not cluster->enabled)
                study.runtime->random[seedTimeseriesNumbers].next();
            else
            {
                if (!isTSintramodal[indexTS])
                {
                    // There is no TS generation for renewable clusters
                    uint nbTimeSeries = cluster->series->series.width;
                    cluster->series->timeseriesNumbers[0][year] = (uint32)(
                      floor(study.runtime->random[seedTimeseriesNumbers].next() * nbTimeSeries));
                }
            }
        }

        // -------------------------------
        // Transmission capacities ...
        // -------------------------------
        indexTS = ts_to_tsIndex.at(timeSeriesTransmissionCapacities);

        if (!isTSintramodal[indexTS])
        {
            for (auto it = area.links.begin(); it != area.links.end(); ++it)
            {
                auto& link = *(it->second);
                const uint nbTimeSeries = link.directCapacities.width;
                if (nbTimeSeries == 1)
                {
                    // Random generator (mersenne-twister) must not be called here
                    // in order to avoid a shift in the random generator results
                    // that would cause a change of Antares results
                    link.timeseriesNumbers[0][year] = 0;
                }
                else
                {
                    link.timeseriesNumbers[0][year] = (uint32)(
                      floor(study.runtime->random[seedTimeseriesNumbers].next() * nbTimeSeries));
                }
            }
        }
    });
}

Matrix<uint32>* getFirstTSnumberInterModalMatrixFoundInArea(
  Area& area,
  const array<bool, timeSeriesCount>& isTSintermodal)
{
    Matrix<uint32>* tsNumbersMtx = nullptr;
    if (isTSintermodal[ts_to_tsIndex.at(timeSeriesLoad)])
        tsNumbersMtx = &(area.load.series->timeseriesNumbers);
    else
    {
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesSolar)])
            tsNumbersMtx = &(area.solar.series->timeseriesNumbers);
        else if (isTSintermodal[ts_to_tsIndex.at(timeSeriesWind)])
            tsNumbersMtx = &(area.wind.series->timeseriesNumbers);
        else if (isTSintermodal[ts_to_tsIndex.at(timeSeriesHydro)])
            tsNumbersMtx = &(area.hydro.series->timeseriesNumbers);
        else if (isTSintermodal[ts_to_tsIndex.at(timeSeriesThermal)]
                 && area.thermal.clusterCount() > 0)
            tsNumbersMtx = &(area.thermal.clusters[0]->series->timeseriesNumbers);
        else if (isTSintermodal[ts_to_tsIndex.at(timeSeriesRenewable)]
                 && area.renewable.clusterCount() > 0)
            tsNumbersMtx = &(area.renewable.clusters[0]->series->timeseriesNumbers);
    }
    assert(tsNumbersMtx);

    return tsNumbersMtx;
}

void applyMatrixDrawsToInterModalModesInArea(Matrix<uint32>* tsNumbersMtx,
                                             Area& area,
                                             const array<bool, timeSeriesCount>& isTSintermodal,
                                             const uint years)
{
    for (uint year = 0; year < years; ++year)
    {
        const uint draw = tsNumbersMtx->entry[0][year];
        assert(draw < 100000);

        assert(year < area.load.series->timeseriesNumbers.height);
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesLoad)])
            area.load.series->timeseriesNumbers[0][year] = draw;

        assert(year < area.solar.series->timeseriesNumbers.height);
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesSolar)])
            area.solar.series->timeseriesNumbers[0][year] = draw;

        assert(year < area.wind.series->timeseriesNumbers.height);
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesWind)])
            area.wind.series->timeseriesNumbers[0][year] = draw;

        assert(year < area.hydro.series->timeseriesNumbers.height);
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesHydro)])
            area.hydro.series->timeseriesNumbers[0][year] = draw;

        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesThermal)])
        {
            uint clusterCount = (uint)area.thermal.clusterCount();
            for (uint i = 0; i != clusterCount; ++i)
            {
                auto& cluster = *(area.thermal.clusters[i]);
                assert(year < cluster.series->timeseriesNumbers.height);
                cluster.series->timeseriesNumbers[0][year] = draw;
            }
        }
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesRenewable)])
        {
            uint clusterCount = (uint)area.renewable.clusterCount();
            for (uint i = 0; i != clusterCount; ++i)
            {
                auto& cluster = *(area.renewable.clusters[i]);
                assert(year < cluster.series->timeseriesNumbers.height);
                cluster.series->timeseriesNumbers[0][year] = draw;
            }
        }
    }
}

// Set tsNumbers to 1 for all years if only one TS is present
static void fixTSNumbersSingleAreaSingleMode(Matrix<uint32>& tsNumbers, uint width, uint years)
{
    if (width == 1)
    {
        for (uint year = 0; year < years; year++)
        {
            tsNumbers[0][year] = 0;
        }
    }
}

static void fixTSNumbersWhenWidthIsOne(Study& study)
{
    const uint years = 1 + study.runtime->rangeLimits.year[rangeEnd];

    study.areas.each([&years](Area& area) {
        // Load
        fixTSNumbersSingleAreaSingleMode(
          area.load.series->timeseriesNumbers, area.load.series->series.width, years);
        // Solar
        fixTSNumbersSingleAreaSingleMode(
          area.solar.series->timeseriesNumbers, area.solar.series->series.width, years);
        // Wind
        fixTSNumbersSingleAreaSingleMode(
          area.wind.series->timeseriesNumbers, area.wind.series->series.width, years);
        // Hydro
        fixTSNumbersSingleAreaSingleMode(
          area.hydro.series->timeseriesNumbers, area.hydro.series->count, years);

        // Thermal
        std::for_each(area.thermal.clusters.cbegin(),
                      area.thermal.clusters.cend(),
                      [&years](const Data::ThermalCluster* cluster) {
                          fixTSNumbersSingleAreaSingleMode(cluster->series->timeseriesNumbers,
                                                           cluster->series->series.width,
                                                           years);
                      });

        // Renewables
        std::for_each(area.renewable.clusters.cbegin(),
                      area.renewable.clusters.cend(),
                      [&years](const Data::RenewableCluster* cluster)

                      {
                          fixTSNumbersSingleAreaSingleMode(cluster->series->timeseriesNumbers,
                                                           cluster->series->series.width,
                                                           years);
                      });

        // NTC
        std::for_each(area.links.cbegin(),
                      area.links.cend(),
                      [&years](const std::pair<Data::AreaName, Data::AreaLink*>& it) {
                          auto link = it.second;
                          fixTSNumbersSingleAreaSingleMode(
                            link->timeseriesNumbers, link->directCapacities.width, years);
                      });
    });
}

bool TimeSeriesNumbers::compareWidth(uint a, uint b)
{
    if (a == 1 || b == 1)
        return false; // ignore 1 values
    return a != b;
}

bool TimeSeriesNumbers::Generate(Study& study)
{
    logs.info() << "Preparing time-series numbers...";

    auto& parameters = study.parameters;

    if (parameters.derated)
        return GenerateDeratedMode(study);

    const uint years = 1 + study.runtime->rangeLimits.year[rangeEnd];

    const array<bool, timeSeriesCount> isTSintramodal
      = {(bool)(timeSeriesLoad & parameters.intraModal),
         (bool)(timeSeriesHydro & parameters.intraModal),
         (bool)(timeSeriesWind & parameters.intraModal)
           && parameters.renewableGeneration.isAggregated(),
         (bool)(timeSeriesThermal & parameters.intraModal),
         (bool)(timeSeriesSolar & parameters.intraModal)
           && parameters.renewableGeneration.isAggregated(),
         (bool)(timeSeriesRenewable & parameters.intraModal)
           && parameters.renewableGeneration.isClusters(),
         (bool)(timeSeriesTransmissionCapacities & parameters.intraModal)};

    array<uint, timeSeriesCount> nbTimeseriesByMode;

    array<uint32, timeSeriesCount> intramodal_draws;
    std::fill(intramodal_draws.begin(), intramodal_draws.end(), 0);

    const array<bool, timeSeriesCount> isTSgenerated
      = {(bool)(timeSeriesLoad & parameters.timeSeriesToRefresh),
         (bool)(timeSeriesHydro & parameters.timeSeriesToRefresh),
         (bool)(timeSeriesWind & parameters.timeSeriesToRefresh),
         (bool)(timeSeriesThermal & parameters.timeSeriesToRefresh),
         (bool)(timeSeriesSolar & parameters.timeSeriesToRefresh),
         false,  // TS generation is always disabled for renewables
         false}; // TS generation is always disabled for links transmission capacities

    if (not checkIntraModalConsistency(nbTimeseriesByMode, isTSintramodal, isTSgenerated, study))
        return false;

    for (uint year = 0; year < years; ++year)
    {
        // Intra-modal TS : draw and store TS numbres
        drawTSnumbersForIntraModal(
          intramodal_draws, isTSintramodal, nbTimeseriesByMode, study.runtime->random);
        storeTSnumbersForIntraModal(intramodal_draws, isTSintramodal, year, study.areas);

        // NOT intra-modal TS : draw and store TS numbers
        drawAndStoreTSnumbersForNOTintraModal(
          isTSintramodal, isTSgenerated, nbTimeseriesByMode, year, study);
    }

    // ===============
    // Inter-modal
    // ===============

    const array<bool, timeSeriesCount> isTSintermodal
      = {(bool)(timeSeriesLoad & parameters.interModal),
         (bool)(timeSeriesHydro & parameters.interModal),
         (bool)(timeSeriesWind & parameters.interModal)
           && parameters.renewableGeneration.isAggregated(),
         (bool)(timeSeriesThermal & parameters.interModal),
         (bool)(timeSeriesSolar & parameters.interModal)
           && parameters.renewableGeneration.isAggregated(),
         (bool)(timeSeriesRenewable & parameters.interModal)
           && parameters.renewableGeneration.isClusters(),
         false}; // links transmission capacities time series cannot be inter-modal

    if (std::any_of(std::begin(isTSintermodal), std::end(isTSintermodal), [](bool x) { return x; }))
    {
        // Logging inter-modal time-series
        std::string interModalTsMsg = "Checking inter-modal correlation... (";
        addInterModalTimeSeriesToMessage(isTSintermodal, interModalTsMsg);
        logs.info() << interModalTsMsg << ')';

        auto end = study.areas.end();
        for (auto i = study.areas.begin(); i != end; ++i)
        {
            auto& area = *(i->second);
            if (not checkInterModalConsistencyForArea(area, isTSintermodal, isTSgenerated, study))
                return false;

            Matrix<uint32>* tsNumbersMtx
              = getFirstTSnumberInterModalMatrixFoundInArea(area, isTSintermodal);

            applyMatrixDrawsToInterModalModesInArea(tsNumbersMtx, area, isTSintermodal, years);
        }
    }
    return true;
}

void TimeSeriesNumbers::StoreTimeseriesIntoOuput(Study& study)
{
    using namespace Antares::Data;

    if (study.parameters.storeTimeseriesNumbers)
    {
        fixTSNumbersWhenWidthIsOne(study);
        study.storeTimeSeriesNumbers<timeSeriesLoad>();
        study.storeTimeSeriesNumbers<timeSeriesSolar>();
        study.storeTimeSeriesNumbers<timeSeriesHydro>();
        study.storeTimeSeriesNumbers<timeSeriesWind>();
        study.storeTimeSeriesNumbers<timeSeriesThermal>();
        study.storeTimeSeriesNumbers<timeSeriesRenewable>();
        study.storeTimeSeriesNumbers<timeSeriesTransmissionCapacities>();
    }
}

} // namespace Solver
} // namespace Antares
