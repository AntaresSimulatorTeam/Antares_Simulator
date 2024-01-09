/*
** Copyright 2007-2023 RTE
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

#include <algorithm>  // std::adjacent_find
#include <iterator>   // std::back_inserter
#include <functional> // std::not_equal_to
#include <map>
#include <string>
#include <array>
#include <filesystem>
#include <utility>

#include "antares/study/fwd.h"
#include "timeseries-numbers.h"
#include "ITimeSeriesNumbersWriter.h"
#include "BindingConstraintsTimeSeriesNumbersWriter.h"

using namespace Yuni;
using namespace Antares::Data;
using namespace std;

const map<TimeSeriesType, int> ts_to_tsIndex = {{timeSeriesLoad, 0},
                                            {timeSeriesHydro, 1},
                                            {timeSeriesWind, 2},
                                            {timeSeriesThermal, 3},
                                            {timeSeriesSolar, 4},
                                            {timeSeriesRenewable, 5},
                                            {timeSeriesTransmissionCapacities, 6}};

const map<TimeSeriesType, string> ts_to_tsTitle
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

    map<TimeSeriesType, int>::const_iterator it = ts_to_tsIndex.begin();
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
        area.load.series.timeseriesNumbers.zero();
        area.solar.series.timeseriesNumbers.zero();
        area.wind.series.timeseriesNumbers.zero();
        area.hydro.series->timeseriesNumbers.zero();

        for (auto cluster : area.thermal.list.each_enabled())
        {
            cluster->series.timeseriesNumbers.zero();
        }

        for (const auto& cluster : area.renewable.list)
            cluster->series.timeseriesNumbers.zero();
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
    virtual ~loadAreaNumberOfTSretriever() = default;
    loadAreaNumberOfTSretriever(Study& study) : areaNumberOfTSretriever(study)
    {
    }
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area)
    {
        std::vector<uint> to_return = {area.load.series.timeSeries.width};
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
    virtual ~hydroAreaNumberOfTSretriever() = default;
    hydroAreaNumberOfTSretriever(Study& study) : areaNumberOfTSretriever(study)
    {
    }
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area)
    {
        std::vector<uint> to_return = {area.hydro.series->TScount()};
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
    virtual ~windAreaNumberOfTSretriever() = default;
    windAreaNumberOfTSretriever(Study& study) : areaNumberOfTSretriever(study)
    {
    }
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area)
    {
        std::vector<uint> to_return = {area.wind.series.timeSeries.width};
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
    virtual ~solarAreaNumberOfTSretriever() = default;
    solarAreaNumberOfTSretriever(Study& study) : areaNumberOfTSretriever(study)
    {
    }
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area)
    {
        std::vector<uint> to_return = {area.solar.series.timeSeries.width};
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
    virtual ~thermalAreaNumberOfTSretriever() = default;
    thermalAreaNumberOfTSretriever(Study& study) : areaNumberOfTSretriever(study)
    {
    }
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area)
    {
        std::vector<uint> to_return;
        for (auto cluster : area.thermal.list.each_enabled())
        {
            to_return.push_back(cluster->series.timeSeries.width);
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
    virtual ~renewClustersAreaNumberOfTSretriever() = default;
    renewClustersAreaNumberOfTSretriever(Study& study) : areaNumberOfTSretriever(study)
    {
    }
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area)
    {
        std::vector<uint> to_return;
        for (const auto& cluster : area.renewable.list)
        {
            to_return.push_back(cluster->series.timeSeries.width);
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
            to_return.push_back(link.directCapacities.timeSeries.width);
            to_return.push_back(link.indirectCapacities.timeSeries.width);
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
    IntraModalConsistencyChecker(const TimeSeriesType ts,
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

    if (!TimeSeriesNumbers::checkAllElementsIdenticalOrOne(listNumberTS))
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
    using mapTStoRetriever = map<TimeSeriesType, shared_ptr<areaNumberOfTSretriever>>;
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
        const TimeSeriesType tsKind = it->first;
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
          = isTSgenerated[indexTS] ? parameters.nbTimeSeriesLoad : area.load.series.timeSeries.width;
        listNumberTsOverArea.push_back(nbTimeSeries);
    }

    // Solar : Add solar's number of TS in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesSolar);
    if (isTSintermodal[indexTS])
    {
        uint nbTimeSeries
          = isTSgenerated[indexTS] ? parameters.nbTimeSeriesSolar : area.solar.series.timeSeries.width;
        listNumberTsOverArea.push_back(nbTimeSeries);
    }

    // Wind : Add wind's number of TS in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesWind);
    if (isTSintermodal[indexTS])
    {
        uint nbTimeSeries
          = isTSgenerated[indexTS] ? parameters.nbTimeSeriesWind : area.wind.series.timeSeries.width;
        listNumberTsOverArea.push_back(nbTimeSeries);
    }

    // Hydro : Add hydro's number of TS in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesHydro);
    if (isTSintermodal[indexTS])
    {
        uint nbTimeSeries
          = isTSgenerated[indexTS] ? parameters.nbTimeSeriesHydro : area.hydro.series->TScount();
        listNumberTsOverArea.push_back(nbTimeSeries);
    }

    // Thermal : Add thermal's number of TS of each cluster in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesThermal);
    if (isTSintermodal[indexTS])
    {
        for (auto cluster : area.thermal.list.each_enabled())
        {
            uint nbTimeSeries = isTSgenerated[indexTS] ? parameters.nbTimeSeriesThermal
                                                       : cluster->series.timeSeries.width;
            listNumberTsOverArea.push_back(nbTimeSeries);
        }
    }

    // Renewable clusters : Add renewable's number of TS of each cluster in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesRenewable);
    if (isTSintermodal[indexTS])
    {
        for (const auto& cluster : area.renewable.list)
        {
            uint nbTimeSeries = cluster->series.timeSeries.width;
            listNumberTsOverArea.push_back(nbTimeSeries);
        }
    }

    // Now check if all elements of list of TS numbers are identical or equal to 1
    if (!TimeSeriesNumbers::checkAllElementsIdenticalOrOne(listNumberTsOverArea))
    {
        logs.error()
          << "Inter-modal correlation: time-series numbers of inter-modal modes in area '"
          << area.name << "'"
          << " are not identical";

        return false;
    }

    return true;
}

void drawTSnumbersForIntraModal(array<uint32_t, timeSeriesCount>& intramodal_draws,
                                const array<bool, timeSeriesCount>& isTSintramodal,
                                array<uint, timeSeriesCount>& nbTimeseriesByMode,
                                MersenneTwister* mersenneTwisterTable)
{
    for (unsigned tsKind = 0; tsKind < timeSeriesCount; ++tsKind)
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
                intramodal_draws[tsKind] = (uint32_t)(floor(
                  mersenneTwisterTable[seedTimeseriesNumbers].next() * nbTimeseriesByMode[tsKind]));
            }
        }
    }
}

void storeTSnumbersForIntraModal(const array<uint32_t, timeSeriesCount>& intramodal_draws,
                                 const array<bool, timeSeriesCount>& isTSintramodal,
                                 uint year,
                                 AreaList& areas)
{
    areas.each([&](Area& area) {
        // -------------
        // Load ...
        // -------------
        assert(year < area.load.series.timeseriesNumbers.height);
        int indexTS = ts_to_tsIndex.at(timeSeriesLoad);

        if (isTSintramodal[indexTS])
            area.load.series.timeseriesNumbers[0][year] = intramodal_draws[indexTS];

        // -------------
        // Solar ...
        // -------------
        assert(year < area.solar.series.timeseriesNumbers.height);
        indexTS = ts_to_tsIndex.at(timeSeriesSolar);

        if (isTSintramodal[indexTS])
            area.solar.series.timeseriesNumbers[0][year] = intramodal_draws[indexTS];

        // -------------
        // Wind ...
        // -------------
        assert(year < area.wind.series.timeseriesNumbers.height);
        indexTS = ts_to_tsIndex.at(timeSeriesWind);

        if (isTSintramodal[indexTS])
            area.wind.series.timeseriesNumbers[0][year] = intramodal_draws[indexTS];

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
            for (auto cluster : area.thermal.list.each_enabled())
            {
                cluster->series.timeseriesNumbers[0][year] = intramodal_draws[indexTS];
            }
        }

        // --------------------------
        // Renewable clusters ...
        // --------------------------
        indexTS = ts_to_tsIndex.at(timeSeriesRenewable);

        if (isTSintramodal[indexTS])
        {
            for (auto& cluster : area.renewable.list)
            {
                if (cluster->enabled)
                    cluster->series.timeseriesNumbers[0][year] = intramodal_draws[indexTS];
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
                                                       : area.load.series.timeSeries.width;
            area.load.series.timeseriesNumbers[0][year]
              = (uint32_t)(floor(study.runtime->random[seedTimeseriesNumbers].next() * nbTimeSeries));
        }

        // -------------
        // Solar ...
        // -------------
        indexTS = ts_to_tsIndex.at(timeSeriesSolar);

        if (!isTSintramodal[indexTS])
        {
            uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                       : area.solar.series.timeSeries.width;
            area.solar.series.timeseriesNumbers[0][year]
              = (uint32_t)(floor(study.runtime->random[seedTimeseriesNumbers].next() * nbTimeSeries));
        }

        // -------------
        // Wind ...
        // -------------
        indexTS = ts_to_tsIndex.at(timeSeriesWind);

        if (!isTSintramodal[indexTS])
        {
            uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                       : area.wind.series.timeSeries.width;
            area.wind.series.timeseriesNumbers[0][year]
              = (uint32_t)(floor(study.runtime->random[seedTimeseriesNumbers].next() * nbTimeSeries));
        }

        // -------------
        // Hydro ...
        // -------------
        indexTS = ts_to_tsIndex.at(timeSeriesHydro);

        if (!isTSintramodal[indexTS])
        {
            uint nbTimeSeries
              = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS] : area.hydro.series->ror.timeSeries.width;
            area.hydro.series->timeseriesNumbers[0][year]
              = (uint32_t)(floor(study.runtime->random[seedTimeseriesNumbers].next() * nbTimeSeries));
        }

        // -------------
        // Thermal ...
        // -------------
        indexTS = ts_to_tsIndex.at(timeSeriesThermal);

        for (auto cluster : area.thermal.list.all())
        {
            if (not cluster->enabled)
                study.runtime->random[seedTimeseriesNumbers].next();
            else
            {
                if (!isTSintramodal[indexTS])
                {
                    uint nbTimeSeries = isTSgenerated[indexTS] ? nbTimeseriesByMode[indexTS]
                                                               : cluster->series.timeSeries.width;
                    cluster->series.timeseriesNumbers[0][year] = (uint32_t)(
                      floor(study.runtime->random[seedTimeseriesNumbers].next() * nbTimeSeries));
                }
            }
        }

        // --------------------------
        // Renewable clusters ...
        // --------------------------
        indexTS = ts_to_tsIndex.at(timeSeriesRenewable);

        for (auto& cluster : area.renewable.list)
        {
            if (not cluster->enabled)
                study.runtime->random[seedTimeseriesNumbers].next();
            else
            {
                if (!isTSintramodal[indexTS])
                {
                    // There is no TS generation for renewable clusters
                    uint nbTimeSeries = cluster->series.timeSeries.width;
                    cluster->series.timeseriesNumbers[0][year] = (uint32_t)(
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
                const uint nbTimeSeries = link.directCapacities.timeSeries.width;
                if (nbTimeSeries == 1)
                {
                    // Random generator (mersenne-twister) must not be called here
                    // in order to avoid a shift in the random generator results
                    // that would cause a change of Antares results
                    link.timeseriesNumbers[0][year] = 0;
                }
                else
                {
                    link.timeseriesNumbers[0][year] = (uint32_t)(
                      floor(study.runtime->random[seedTimeseriesNumbers].next() * nbTimeSeries));
                }
            }
        }
    });
    // Binding constraints
    for (auto& group: study.bindingConstraintsGroups)
    {
        const auto nbTimeSeries = group->numberOfTimeseries();
        auto& value = group->timeseriesNumbers[0][year];
        if (nbTimeSeries == 1)
        {
            value = 0;
        }
        else
        {
            value  = (uint32_t)(floor(study.runtime->random[seedTimeseriesNumbers].next() * nbTimeSeries));
        }
    }
}

Matrix<uint32_t>* getFirstTSnumberInterModalMatrixFoundInArea(
  Area& area,
  const array<bool, timeSeriesCount>& isTSintermodal)
{
    Matrix<uint32_t>* tsNumbersMtx = nullptr;
    if (isTSintermodal[ts_to_tsIndex.at(timeSeriesLoad)])
        tsNumbersMtx = &(area.load.series.timeseriesNumbers);
    else
    {
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesSolar)])
            tsNumbersMtx = &(area.solar.series.timeseriesNumbers);
        else if (isTSintermodal[ts_to_tsIndex.at(timeSeriesWind)])
            tsNumbersMtx = &(area.wind.series.timeseriesNumbers);
        else if (isTSintermodal[ts_to_tsIndex.at(timeSeriesHydro)])
            tsNumbersMtx = &(area.hydro.series->timeseriesNumbers);
        else if (isTSintermodal[ts_to_tsIndex.at(timeSeriesThermal)] &&
            not area.thermal.list.each_enabled().empty())
        {
            std::shared_ptr<ThermalCluster> cluster = *(area.thermal.list.each_enabled().begin());
            tsNumbersMtx = &(cluster->series.timeseriesNumbers);
        }
        else if (isTSintermodal[ts_to_tsIndex.at(timeSeriesRenewable)]
                 && area.renewable.list.size() > 0)
            tsNumbersMtx = &(area.renewable.list[0]->series.timeseriesNumbers);
    }
    assert(tsNumbersMtx);

    return tsNumbersMtx;
}

void applyMatrixDrawsToInterModalModesInArea(Matrix<uint32_t>* tsNumbersMtx,
                                             Area& area,
                                             const array<bool, timeSeriesCount>& isTSintermodal,
                                             const uint years)
{
    for (uint year = 0; year < years; ++year)
    {
        const uint draw = tsNumbersMtx->entry[0][year];
        assert(draw < 100000);

        assert(year < area.load.series.timeseriesNumbers.height);
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesLoad)])
            area.load.series.timeseriesNumbers[0][year] = draw;

        assert(year < area.solar.series.timeseriesNumbers.height);
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesSolar)])
            area.solar.series.timeseriesNumbers[0][year] = draw;

        assert(year < area.wind.series.timeseriesNumbers.height);
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesWind)])
            area.wind.series.timeseriesNumbers[0][year] = draw;

        assert(year < area.hydro.series->timeseriesNumbers.height);
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesHydro)])
            area.hydro.series->timeseriesNumbers[0][year] = draw;

        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesThermal)])
        {
            for (auto cluster : area.thermal.list.each_enabled())
            {
                assert(year < cluster->series.timeseriesNumbers.height);
                cluster->series.timeseriesNumbers[0][year] = draw;
            }
        }
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesRenewable)])
        {
            for (const auto& cluster : area.renewable.list)
            {
                assert(year < cluster->series.timeseriesNumbers.height);
                cluster->series.timeseriesNumbers[0][year] = draw;
            }
        }
    }
}

// Set tsNumbers to 1 for all years if only one TS is present
static void fixTSNumbersSingleAreaSingleMode(Matrix<uint32_t>& tsNumbers, uint width, uint years)
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
                area.load.series.timeseriesNumbers, area.load.series.timeSeries.width, years);
        // Solar
        fixTSNumbersSingleAreaSingleMode(
                area.solar.series.timeseriesNumbers, area.solar.series.timeSeries.width, years);
        // Wind
        fixTSNumbersSingleAreaSingleMode(
                area.wind.series.timeseriesNumbers, area.wind.series.timeSeries.width, years);
        // Hydro
        fixTSNumbersSingleAreaSingleMode(
          area.hydro.series->timeseriesNumbers, area.hydro.series->TScount(), years);

        // Thermal
        for (auto cluster : area.thermal.list.each_enabled())
        {
            fixTSNumbersSingleAreaSingleMode(cluster->series.timeseriesNumbers,
                                             cluster->series.timeSeries.width,
                                             years);
        }

        // Renewables
        for (const auto& cluster : area.renewable.list)
            fixTSNumbersSingleAreaSingleMode(cluster->series.timeseriesNumbers,
                    cluster->series.timeSeries.width,
                    years);

        // NTC
        std::for_each(area.links.cbegin(),
                      area.links.cend(),
                      [&years](const std::pair<Data::AreaName, Data::AreaLink*>& it) {
                          auto link = it.second;
                          fixTSNumbersSingleAreaSingleMode(
                            link->timeseriesNumbers, link->directCapacities.timeSeries.width, years);
                      });
    });
    study.bindingConstraintsGroups.fixTSNumbersWhenWidthIsOne();
}

bool TimeSeriesNumbers::checkAllElementsIdenticalOrOne(const std::vector<uint>& w)
{
    std::vector<uint> removedOnes;
    // Remove all 1
    std::remove_copy(w.begin(), w.end(), std::back_inserter(removedOnes), 1);
    // Try to find adjacent elements that are pairwise different
    auto result
      = std::adjacent_find(removedOnes.begin(), removedOnes.end(), std::not_equal_to<uint>());
    // Return "no such pair exists"
    return result == removedOnes.end();
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

    array<uint32_t, timeSeriesCount> intramodal_draws;
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

            Matrix<uint32_t>* tsNumbersMtx
              = getFirstTSnumberInterModalMatrixFoundInArea(area, isTSintermodal);

            applyMatrixDrawsToInterModalModesInArea(tsNumbersMtx, area, isTSintermodal, years);
        }
    }
    return true;
}

void TimeSeriesNumbers::StoreTimeSeriesNumbersIntoOuput(Data::Study& study, IResultWriter& resultWriter)
{
    using namespace Antares::Data;

    if (study.parameters.storeTimeseriesNumbers)
    {
        fixTSNumbersWhenWidthIsOne(study);
        study.storeTimeSeriesNumbers<TimeSeriesType::timeSeriesLoad>(resultWriter);
        study.storeTimeSeriesNumbers<TimeSeriesType::timeSeriesSolar>(resultWriter);
        study.storeTimeSeriesNumbers<TimeSeriesType::timeSeriesHydro>(resultWriter);
        study.storeTimeSeriesNumbers<TimeSeriesType::timeSeriesWind>(resultWriter);
        study.storeTimeSeriesNumbers<TimeSeriesType::timeSeriesThermal>(resultWriter);
        study.storeTimeSeriesNumbers<TimeSeriesType::timeSeriesRenewable>(resultWriter);
        study.storeTimeSeriesNumbers<TimeSeriesType::timeSeriesTransmissionCapacities>(resultWriter);

        Simulation::BindingConstraintsTimeSeriesNumbersWriter ts_writer(resultWriter);
        ts_writer.write(study.bindingConstraintsGroups);
    }
}

} // namespace Solver
} // namespace Antares
