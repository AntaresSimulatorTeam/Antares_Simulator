/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/simulation/timeseries-numbers.h"

#include <algorithm> // std::adjacent_find
#include <array>
#include <filesystem>
#include <functional> // std::not_equal_to
#include <iterator>   // std::back_inserter
#include <map>
#include <string>
#include <utility>

#include "antares/solver/simulation/BindingConstraintsTimeSeriesNumbersWriter.h"
#include "antares/solver/simulation/ITimeSeriesNumbersWriter.h"
#include "antares/study/fwd.h"

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

const map<TimeSeriesType, string> ts_to_tsTitle = {{timeSeriesLoad, "load"},
                                                   {timeSeriesHydro, "hydro"},
                                                   {timeSeriesWind, "wind"},
                                                   {timeSeriesThermal, "thermal"},
                                                   {timeSeriesSolar, "solar"},
                                                   {timeSeriesRenewable, "renewable clusters"},
                                                   {timeSeriesTransmissionCapacities,
                                                    "transmission capacities"}};

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
            {
                interModalTsMsg.append(", ");
            }
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
    {
        logs.warning() << "The derated mode is enabled. The custom building mode will be ignored";
    }

    return true;
}

class AreaNumberOfTSretriever
{
public:
    virtual ~AreaNumberOfTSretriever() = default;
    virtual std::vector<uint> getAreaTimeSeriesNumber(const Area& area) = 0;
};

class LoadAreaNumberOfTSretriever: public AreaNumberOfTSretriever
{
public:
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area) override
    {
        return {area.load.series.timeSeries.width};
    }
};

class HydroAreaNumberOfTSretriever: public AreaNumberOfTSretriever
{
public:
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area) override
    {
        return {area.hydro.series->TScount()};
    }
};

class WindAreaNumberOfTSretriever: public AreaNumberOfTSretriever
{
public:
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area) override
    {
        return {area.wind.series.timeSeries.width};
    }
};

class SolarAreaNumberOfTSretriever: public AreaNumberOfTSretriever
{
public:
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area) override
    {
        return {area.solar.series.timeSeries.width};
    }
};

class ThermalAreaNumberOfTSretriever: public AreaNumberOfTSretriever
{
public:
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area) override
    {
        std::vector<uint> to_return;
        for (auto& cluster: area.thermal.list.each_enabled())
        {
            to_return.push_back(cluster->series.timeSeries.width);
        }
        return to_return;
    }
};

class RenewClustersAreaNumberOfTSretriever: public AreaNumberOfTSretriever
{
public:
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area) override
    {
        std::vector<uint> to_return;
        for (const auto& cluster: area.renewable.list.each_enabled())
        {
            to_return.push_back(cluster->series.timeSeries.width);
        }
        return to_return;
    }
};

class AreaLinksTransCapaNumberOfTSretriever: public AreaNumberOfTSretriever
{
public:
    std::vector<uint> getAreaTimeSeriesNumber(const Area& area) override
    {
        std::vector<uint> to_return;

        for (auto it = area.links.begin(); it != area.links.end(); ++it)
        {
            const auto& link = *(it->second);
            to_return.push_back(link.directCapacities.timeSeries.width);
        }
        return to_return;
    }
};

class IntraModalConsistencyChecker
{
public:
    IntraModalConsistencyChecker(const TimeSeriesType ts,
                                 AreaNumberOfTSretriever* tsCounter,
                                 Study& study):
        tsCounter_(tsCounter),
        study_(study)
    {
        tsTitle_ = ts_to_tsTitle.at(ts);
    }

    ~IntraModalConsistencyChecker() = default;
    bool checkTSconsistency();

    uint getTimeSeriesNumber()
    {
        return nbTimeseries_;
    }

private:
    AreaNumberOfTSretriever* tsCounter_;
    Study& study_;
    uint nbTimeseries_ = 0;
    string tsTitle_;
};

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
    // At this point, all elements are identical or 1
    nbTimeseries_ = *(std::max_element(listNumberTS.begin(), listNumberTS.end()));

    return true;
}

bool checkIntraModalConsistency(array<uint, timeSeriesCount>& nbTimeseriesByMode,
                                const array<bool, timeSeriesCount>& isTSintramodal,
                                Study& study)
{
    // Initialization of a map associating a time-series to an object that retrieves
    // the number of time series inside an area
    using mapTStoRetriever = map<TimeSeriesType, shared_ptr<AreaNumberOfTSretriever>>;
    mapTStoRetriever ts_to_numberOfTSretrievers;
    ts_to_numberOfTSretrievers[timeSeriesLoad] = make_shared<LoadAreaNumberOfTSretriever>();
    ts_to_numberOfTSretrievers[timeSeriesHydro] = make_shared<HydroAreaNumberOfTSretriever>();
    ts_to_numberOfTSretrievers[timeSeriesWind] = make_shared<WindAreaNumberOfTSretriever>();
    ts_to_numberOfTSretrievers[timeSeriesSolar] = make_shared<SolarAreaNumberOfTSretriever>();
    ts_to_numberOfTSretrievers[timeSeriesThermal] = make_shared<ThermalAreaNumberOfTSretriever>();
    ts_to_numberOfTSretrievers[timeSeriesRenewable] = make_shared<
      RenewClustersAreaNumberOfTSretriever>();
    ts_to_numberOfTSretrievers[timeSeriesTransmissionCapacities] = make_shared<
      AreaLinksTransCapaNumberOfTSretriever>();

    // Loop over TS kind and check intra-modal consistency
    mapTStoRetriever::iterator it = ts_to_numberOfTSretrievers.begin();
    for (; it != ts_to_numberOfTSretrievers.end(); ++it)
    {
        const TimeSeriesType tsKind = it->first;
        AreaNumberOfTSretriever* tsRetriever = (it->second).get();
        int indexTS = ts_to_tsIndex.at(it->first);
        if (isTSintramodal[indexTS])
        {
            IntraModalConsistencyChecker intraModalchecker(tsKind, tsRetriever, study);
            if (!intraModalchecker.checkTSconsistency())
            {
                return false;
            }
            nbTimeseriesByMode[indexTS] = intraModalchecker.getTimeSeriesNumber();
        }
    }

    return true;
}

bool checkInterModalConsistencyForArea(const Area& area,
                                       const array<bool, timeSeriesCount>& isTSintermodal)
{
    // 1. Making a list of TS numbers :
    //    In this list, we put the numbers of TS of every "inter-modal" mode over the current area.
    // 2. All elements of this list must be identical

    // The list containing the numbers of TS of every "inter-modal" mode over the current area
    std::vector<uint> listNumberTsOverArea;

    // Load : Add load's number of TS in area ...
    int indexTS = ts_to_tsIndex.at(timeSeriesLoad);
    if (isTSintermodal[indexTS])
    {
        listNumberTsOverArea.push_back(area.load.series.timeSeries.width);
    }

    // Solar : Add solar's number of TS in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesSolar);
    if (isTSintermodal[indexTS])
    {
        listNumberTsOverArea.push_back(area.solar.series.timeSeries.width);
    }

    // Wind : Add wind's number of TS in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesWind);
    if (isTSintermodal[indexTS])
    {
        listNumberTsOverArea.push_back(area.wind.series.timeSeries.width);
    }

    // Hydro : Add hydro's number of TS in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesHydro);
    if (isTSintermodal[indexTS])
    {
        listNumberTsOverArea.push_back(area.hydro.series->TScount());
    }

    // Thermal : Add thermal's number of TS of each cluster in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesThermal);
    if (isTSintermodal[indexTS])
    {
        for (auto& cluster: area.thermal.list.each_enabled())
        {
            listNumberTsOverArea.push_back(cluster->series.timeSeries.width);
        }
    }

    // Renewable clusters : Add renewable's number of TS of each cluster in area ...
    indexTS = ts_to_tsIndex.at(timeSeriesRenewable);
    if (isTSintermodal[indexTS])
    {
        for (const auto& cluster: area.renewable.list.each_enabled())
        {
            listNumberTsOverArea.push_back(cluster->series.timeSeries.width);
        }
    }

    // Now check if all elements of list of TS numbers are identical or equal to 1
    if (!TimeSeriesNumbers::checkAllElementsIdenticalOrOne(listNumberTsOverArea))
    {
        logs.error()
          << "Inter-modal correlation: time-series numbers of inter-modal modes in area '"
          << area.name << "'" << " are not identical";

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
    areas.each(
      [year, &isTSintramodal, &intramodal_draws](Area& area)
      {
          // -------------
          // Load ...
          // -------------
          assert(year < area.load.series.timeseriesNumbers.height());
          int indexTS = ts_to_tsIndex.at(timeSeriesLoad);

          if (isTSintramodal[indexTS] && area.load.series.timeSeries.width > 1)
          {
              area.load.series.timeseriesNumbers[year] = intramodal_draws[indexTS];
          }

          // -------------
          // Solar ...
          // -------------
          assert(year < area.solar.series.timeseriesNumbers.height());
          indexTS = ts_to_tsIndex.at(timeSeriesSolar);

          if (isTSintramodal[indexTS] && area.solar.series.timeSeries.width > 1)
          {
              area.solar.series.timeseriesNumbers[year] = intramodal_draws[indexTS];
          }

          // -------------
          // Wind ...
          // -------------
          assert(year < area.wind.series.timeseriesNumbers.height());
          indexTS = ts_to_tsIndex.at(timeSeriesWind);

          if (isTSintramodal[indexTS] && area.wind.series.timeSeries.width > 1)
          {
              area.wind.series.timeseriesNumbers[year] = intramodal_draws[indexTS];
          }

          // -------------
          // Hydro ...
          // -------------
          assert(year < area.hydro.series->timeseriesNumbers.height());
          indexTS = ts_to_tsIndex.at(timeSeriesHydro);

          if (isTSintramodal[indexTS] && area.hydro.series->TScount() > 1)
          {
              area.hydro.series->timeseriesNumbers[year] = intramodal_draws[indexTS];
          }

          // -------------
          // Thermal ...
          // -------------
          indexTS = ts_to_tsIndex.at(timeSeriesThermal);

          if (isTSintramodal[indexTS])
          {
              for (auto& cluster: area.thermal.list.each_enabled())
              {
                  cluster->series.timeseriesNumbers[year] = intramodal_draws[indexTS];
              }
          }

          // --------------------------
          // Renewable clusters ...
          // --------------------------
          indexTS = ts_to_tsIndex.at(timeSeriesRenewable);

          if (isTSintramodal[indexTS])
          {
              for (auto& cluster: area.renewable.list.each_enabled())
              {
                  cluster->series.timeseriesNumbers[year] = intramodal_draws[indexTS];
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
                  if (link.directCapacities.timeSeries.width > 1)
                  {
                      link.timeseriesNumbers[year] = intramodal_draws[indexTS];
                  }
              }
          }
      });
}

void drawAndStoreTSnumbersForNOTintraModal(const array<bool, timeSeriesCount>& isTSintramodal,
                                           uint year,
                                           Study& study)
{
    study.areas.each(
      [&study, &isTSintramodal, year](Area& area)
      {
          // -------------
          // Load ...
          // -------------
          int indexTS = ts_to_tsIndex.at(timeSeriesLoad);

          if (!isTSintramodal[indexTS])
          {
              area.load.series.timeseriesNumbers[year] = (uint32_t)(floor(
                study.runtime.random[seedTimeseriesNumbers].next()
                * area.load.series.timeSeries.width));
          }

          // -------------
          // Solar ...
          // -------------
          indexTS = ts_to_tsIndex.at(timeSeriesSolar);

          if (!isTSintramodal[indexTS])
          {
              area.solar.series.timeseriesNumbers[year] = (uint32_t)(floor(
                study.runtime.random[seedTimeseriesNumbers].next()
                * area.solar.series.timeSeries.width));
          }

          // -------------
          // Wind ...
          // -------------
          indexTS = ts_to_tsIndex.at(timeSeriesWind);

          if (!isTSintramodal[indexTS])
          {
              area.wind.series.timeseriesNumbers[year] = (uint32_t)(floor(
                study.runtime.random[seedTimeseriesNumbers].next()
                * area.wind.series.timeSeries.width));
          }

          // -------------
          // Hydro ...
          // -------------
          indexTS = ts_to_tsIndex.at(timeSeriesHydro);

          if (!isTSintramodal[indexTS])
          {
              area.hydro.series->timeseriesNumbers[year] = (uint32_t)(floor(
                study.runtime.random[seedTimeseriesNumbers].next() * area.hydro.series->TScount()));
          }

          // -------------
          // Thermal ...
          // -------------
          indexTS = ts_to_tsIndex.at(timeSeriesThermal);

          for (auto& cluster: area.thermal.list.all())
          {
              if (!cluster->enabled)
              {
                  study.runtime.random[seedTimeseriesNumbers].next();
              }
              else
              {
                  if (!isTSintramodal[indexTS])
                  {
                      cluster->series.timeseriesNumbers[year] = (uint32_t)(floor(
                        study.runtime.random[seedTimeseriesNumbers].next()
                        * cluster->series.timeSeries.width));
                  }
              }
          }

          // --------------------------
          // Renewable clusters ...
          // --------------------------
          indexTS = ts_to_tsIndex.at(timeSeriesRenewable);

          for (auto& cluster: area.renewable.list.each_enabled())
          {
              if (!isTSintramodal[indexTS])
              {
                  // There is no TS generation for renewable clusters
                  uint nbTimeSeries = cluster->series.timeSeries.width;
                  cluster->series.timeseriesNumbers[year] = (uint32_t)(floor(
                    study.runtime.random[seedTimeseriesNumbers].next() * nbTimeSeries));
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
                  if (nbTimeSeries > 1)
                  {
                      link.timeseriesNumbers[year] = (uint32_t)(floor(
                        study.runtime.random[seedTimeseriesNumbers].next() * nbTimeSeries));
                  }
              }
          }
      });
    // Binding constraints
    for (auto& group: study.bindingConstraintsGroups)
    {
        const auto nbTimeSeries = group->numberOfTimeseries();
        auto& groupTsNumber = group->timeseriesNumbers[year];
        if (nbTimeSeries > 1)
        {
            groupTsNumber = (uint32_t)(floor(study.runtime.random[seedTimeseriesNumbers].next()
                                             * nbTimeSeries));
        }
    }
}

Data::TimeSeriesNumbers* getFirstTSnumberInterModalMatrixFoundInArea(
  Area& area,
  const array<bool, timeSeriesCount>& isTSintermodal)
{
    Data::TimeSeriesNumbers* tsNumbersMtx = nullptr;
    if (isTSintermodal[ts_to_tsIndex.at(timeSeriesLoad)])
    {
        tsNumbersMtx = &(area.load.series.timeseriesNumbers);
    }
    else
    {
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesSolar)])
        {
            tsNumbersMtx = &(area.solar.series.timeseriesNumbers);
        }
        else if (isTSintermodal[ts_to_tsIndex.at(timeSeriesWind)])
        {
            tsNumbersMtx = &(area.wind.series.timeseriesNumbers);
        }
        else if (isTSintermodal[ts_to_tsIndex.at(timeSeriesHydro)])
        {
            tsNumbersMtx = &(area.hydro.series->timeseriesNumbers);
        }
        else if (isTSintermodal[ts_to_tsIndex.at(timeSeriesThermal)]
                 && !area.thermal.list.each_enabled().empty())
        {
            std::shared_ptr<ThermalCluster> cluster = *(area.thermal.list.each_enabled().begin());
            tsNumbersMtx = &(cluster->series.timeseriesNumbers);
        }
        else if (isTSintermodal[ts_to_tsIndex.at(timeSeriesRenewable)]
                 && area.renewable.list.enabledCount() > 0)
        {
            std::shared_ptr<RenewableCluster> cluster = *(
              area.renewable.list.each_enabled().begin());
            tsNumbersMtx = &(cluster->series.timeseriesNumbers);
        }
    }
    assert(tsNumbersMtx);

    return tsNumbersMtx;
}

static void applyMatrixDrawsToInterModalModesInArea(
  Data::TimeSeriesNumbers& tsNumbersMtx,
  Area& area,
  const array<bool, timeSeriesCount>& isTSintermodal,
  const uint years)
{
    for (uint year = 0; year < years; ++year)
    {
        const uint32_t draw = tsNumbersMtx[year];
        assert(draw < 100000);

        assert(year < area.load.series.timeseriesNumbers.height());
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesLoad)])
        {
            area.load.series.timeseriesNumbers[year] = draw;
        }

        assert(year < area.solar.series.timeseriesNumbers.height());
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesSolar)])
        {
            area.solar.series.timeseriesNumbers[year] = draw;
        }

        assert(year < area.wind.series.timeseriesNumbers.height());
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesWind)])
        {
            area.wind.series.timeseriesNumbers[year] = draw;
        }

        assert(year < area.hydro.series->timeseriesNumbers.height());
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesHydro)])
        {
            area.hydro.series->timeseriesNumbers[year] = draw;
        }

        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesThermal)])
        {
            for (auto& cluster: area.thermal.list.each_enabled())
            {
                assert(year < cluster->series.timeseriesNumbers.height());
                cluster->series.timeseriesNumbers[year] = draw;
            }
        }
        if (isTSintermodal[ts_to_tsIndex.at(timeSeriesRenewable)])
        {
            for (const auto& cluster: area.renewable.list.each_enabled())
            {
                assert(year < cluster->series.timeseriesNumbers.height());
                cluster->series.timeseriesNumbers[year] = draw;
            }
        }
    }
}

bool TimeSeriesNumbers::checkAllElementsIdenticalOrOne(std::vector<uint> w)
{
    auto first_one = std::remove(w.begin(), w.end(), 1); // Reject all 1 to the end
    return std::adjacent_find(w.begin(), first_one, std::not_equal_to<uint>()) == first_one;
}

using Checks = std::vector<std::pair<const Antares::Data::TimeSeriesNumbers*, std::string>>;

static Checks buildChecksFromStudy(const AreaList& areas)
{
    Checks toCheck;

    // LINKS
    for (const auto& [_, area]: areas)
    {
        const std::string areaID = area->id.to<std::string>();
        for (const auto& [_, link]: area->links)
        {
            const std::string areaID2 = link->with->id.to<std::string>();
            toCheck.push_back({&link->timeseriesNumbers, "link " + areaID + " / " + areaID2});
        }
    }

    // HYDRO
    for (const auto& [_, area]: areas)
    {
        const std::string areaID = area->id.to<std::string>();
        toCheck.push_back({&area->hydro.series->timeseriesNumbers, "hydro " + areaID});
    }

    return toCheck;
}

static bool performChecks(const Checks& toCheck)
{
    bool ret = true;
    for (const auto& [tsNumber, context]: toCheck)
    {
        const auto errorMessageMaybe = tsNumber->checkSeriesNumberOfColumnsConsistency();
        if (errorMessageMaybe.has_value())
        {
            logs.error() << "Inconsistent number of columns for " << context << " ("
                         << errorMessageMaybe.value() << ")";
            ret = false;
        }
    }
    if (!ret)
    {
        logs.error() << "Please check that all series have the same number of columns or 1 column, "
                        "or a combination of 1 column and the same number of columns";
    }

    return ret;
}

bool TimeSeriesNumbers::CheckNumberOfColumns(const AreaList& areas)
{
    Checks toCheck = buildChecksFromStudy(areas);
    return performChecks(toCheck);
}

bool TimeSeriesNumbers::Generate(Study& study)
{
    logs.info() << "Preparing time-series numbers...";

    auto& parameters = study.parameters;

    if (parameters.derated)
    {
        return GenerateDeratedMode(study);
    }

    const uint years = 1 + study.runtime.rangeLimits.year[rangeEnd];

    const array<bool, timeSeriesCount> isTSintramodal = {
      (bool)(timeSeriesLoad & parameters.intraModal),
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

    if (!checkIntraModalConsistency(nbTimeseriesByMode, isTSintramodal, study))
    {
        return false;
    }

    for (uint year = 0; year < years; ++year)
    {
        // Intra-modal TS : draw and store TS numbers
        drawTSnumbersForIntraModal(intramodal_draws,
                                   isTSintramodal,
                                   nbTimeseriesByMode,
                                   study.runtime.random);
        storeTSnumbersForIntraModal(intramodal_draws, isTSintramodal, year, study.areas);

        // NOT intra-modal TS : draw and store TS numbers
        drawAndStoreTSnumbersForNOTintraModal(isTSintramodal, year, study);
    }

    // ===============
    // Inter-modal
    // ===============

    const array<bool, timeSeriesCount> isTSintermodal = {
      (bool)(timeSeriesLoad & parameters.interModal),
      (bool)(timeSeriesHydro & parameters.interModal),
      (bool)(timeSeriesWind & parameters.interModal)
        && parameters.renewableGeneration.isAggregated(),
      (bool)(timeSeriesThermal & parameters.interModal),
      (bool)(timeSeriesSolar & parameters.interModal)
        && parameters.renewableGeneration.isAggregated(),
      (bool)(timeSeriesRenewable & parameters.interModal)
        && parameters.renewableGeneration.isClusters(),
      false // links transmission capacities time series cannot be inter-modal
    };

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
            if (not checkInterModalConsistencyForArea(area, isTSintermodal))
            {
                return false;
            }

            Data::TimeSeriesNumbers* tsNumbersMtx = getFirstTSnumberInterModalMatrixFoundInArea(
              area,
              isTSintermodal);

            applyMatrixDrawsToInterModalModesInArea(*tsNumbersMtx, area, isTSintermodal, years);
        }
    }
    return true;
}

void TimeSeriesNumbers::StoreTimeSeriesNumbersIntoOuput(Data::Study& study,
                                                        IResultWriter& resultWriter)
{
    using namespace Antares::Data;

    if (study.parameters.storeTimeseriesNumbers)
    {
        study.storeTimeSeriesNumbers<TimeSeriesType::timeSeriesLoad>(resultWriter);
        study.storeTimeSeriesNumbers<TimeSeriesType::timeSeriesSolar>(resultWriter);
        study.storeTimeSeriesNumbers<TimeSeriesType::timeSeriesHydro>(resultWriter);
        study.storeTimeSeriesNumbers<TimeSeriesType::timeSeriesWind>(resultWriter);
        study.storeTimeSeriesNumbers<TimeSeriesType::timeSeriesThermal>(resultWriter);
        study.storeTimeSeriesNumbers<TimeSeriesType::timeSeriesRenewable>(resultWriter);
        study.storeTimeSeriesNumbers<TimeSeriesType::timeSeriesTransmissionCapacities>(
          resultWriter);

        Simulation::BindingConstraintsTimeSeriesNumbersWriter ts_writer(resultWriter);
        ts_writer.write(study.bindingConstraintsGroups);
    }
}

} // namespace Solver
} // namespace Antares
