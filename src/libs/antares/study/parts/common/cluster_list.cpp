// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/common/cluster_list.h"

#include <algorithm>
#include <numeric>

#include <boost/algorithm/string/case_conv.hpp>

#include <antares/study/parts/reserves/makeGroupsOfSymmetriesFromString.h>
#include <antares/utils/utils.h>
#include "antares/study/parts/reserves/reservesParticipationsLoader.h"
#include "antares/study/study.h"

namespace fs = std::filesystem;
using namespace Yuni;

namespace Antares::Data
{
using namespace Antares;

template<class ClusterT>
inline bool ClusterList<ClusterT>::empty() const
{
    return allClusters_.empty();
}

template<class ClusterT>
std::shared_ptr<ClusterT> ClusterList<ClusterT>::enabledClusterAt(unsigned int index) const
{
    // No operator [] was found for std::view (returned by each_enabled()).
    // The current function is there to replace it.
    return *(std::views::drop(each_enabled(), index).begin());
}

template<class ClusterT>
std::pair<std::string, ReserveID> ClusterList<ClusterT>::reserveParticipationClusterAt(
  const Area* area,
  unsigned int index) const
{
    int globalReserveParticipationIdx = 0;

    for (const auto& reserveID:
         area->allCapacityReservations.value().areaCapacityReservations | std::views::keys)
    {
        for (auto& cluster: allClusters_)
        {
            if (cluster->reserveParticipationContainer
                && cluster->reserveParticipationContainer.value().isParticipatingInReserve(
                  reserveID))
            {
                if (globalReserveParticipationIdx == index)
                {
                    return {cluster->name(), reserveID};
                }
                globalReserveParticipationIdx++;
            }
        }
    }

    throw std::out_of_range("This cluster reserve participation index has not been found in all "
                            "the reserve participations");
}

template<class ClusterT>
std::pair<std::string, ReserveID> ClusterList<ClusterT>::reserveParticipationGroupAt(
  const Area* area,
  unsigned int index) const
{
    int column = 0;
    for (const auto& reserveID:
         area->allCapacityReservations.value().areaCapacityReservations | std::views::keys)
    {
        if (area->allCapacityReservations->reserveGroupPartThermal.contains(reserveID))
        {
            for (auto group: area->allCapacityReservations->reserveGroupPartThermal.at(reserveID))
            {
                if (column == index)
                {
                    return {group, reserveID};
                }
                column++;
            }
        }
    }
    throw std::out_of_range("This group reserve participation index has not been found in all the "
                            "reserve participations");
}

template<class ClusterT>
ClusterT* ClusterList<ClusterT>::findInAll(std::string_view id) const
{
    for (auto& cluster: all())
    {
        if (cluster->id() == id)
        {
            return cluster.get();
        }
    }
    return nullptr;
}

template<class ClusterT>
std::vector<std::shared_ptr<ClusterT>> ClusterList<ClusterT>::all() const
{
    return allClusters_;
}

template<class ClusterT>
bool ClusterList<ClusterT>::exists(const std::string& id) const
{
    return std::ranges::any_of(allClusters_, [&id](const auto& c) { return c->id() == id; });
}

template<class ClusterT>
void ClusterList<ClusterT>::clearAll()
{
    allClusters_.clear();
}

template<class ClusterT>
void ClusterList<ClusterT>::resizeAllTimeseriesNumbers(uint n) const
{
    for (auto& c: allClusters_)
    {
        c->series.timeseriesNumbers.reset(n);
    }
}

template<class ClusterT>
void ClusterList<ClusterT>::storeTimeseriesNumbers(Solver::IResultWriter& writer) const
{
    std::string ts_content;
    fs::path basePath = fs::path("ts-numbers") / typeID();

    for (auto& cluster: each_enabled())
    {
        fs::path path = basePath / cluster->parentArea->id.c_str()
                        / std::string(cluster->id() + ".txt");

        ts_content.clear(); // We must clear ts_content here, since saveToBuffer does not do it.
        cluster->series.timeseriesNumbers.saveToBuffer(ts_content);
        writer.addEntryFromBuffer(path, ts_content);
    }
}

template<class ClusterT>
bool ClusterList<ClusterT>::alreadyInAllClusters(std::string clusterId)
{
    return std::ranges::any_of(allClusters_,
                               [&clusterId](const auto& c) { return c->id() == clusterId; });
}

template<class ClusterT>
void ClusterList<ClusterT>::addToCompleteList(std::shared_ptr<ClusterT> cluster)
{
    allClusters_.push_back(cluster);
}

template<class ClusterT>
void ClusterList<ClusterT>::sortCompleteList()
{
    std::ranges::sort(allClusters_, [](const auto a, const auto b) { return a->id() < b->id(); });
}

template<class ClusterT>
unsigned int ClusterList<ClusterT>::enabledCount() const
{
    return std::ranges::count_if(allClusters_, &ClusterT::isEnabled);
}

template<class ClusterT>
unsigned int ClusterList<ClusterT>::allClustersCount() const
{
    return allClusters_.size();
}

template<class ClusterT>
void ClusterList<ClusterT>::buildIndexes()
{
    sortCompleteList();

    // First, we give an index to every cluster, enabled / must-run or not.
    // We do that to :
    //  - Stick to what was done before and not change the results
    //  - Avoids seg faults, for instance when storing thermal noises (solver.hxx).
    //    Indeed : otherwise disabled clusters have an infinite index
    unsigned int index = 0;
    for (auto& c: allClusters_)
    {
        c->areaWideIndex = index++;
    }

    index = 0;
    for (auto& c: each_enabled())
    {
        c->enabledIndex = index++;
    }
}

template<class ClusterT>
bool ClusterList<ClusterT>::saveDataSeriesToFolder(const AnyString& folder) const
{
    return std::ranges::all_of(allClusters_,
                               [&folder](const auto c)
                               { return c->saveDataSeriesToFolder(folder); });
}

template<class ClusterT>
bool ClusterList<ClusterT>::loadDataSeriesFromFolder(Study& s, const std::filesystem::path& folder)
{
    return std::ranges::all_of(allClusters_,
                               [&s, &folder](auto c)
                               { return c->loadDataSeriesFromFolder(s, folder); });
}

template<class ClusterT>
bool ClusterList<ClusterT>::loadReserveParticipations(Area& area, const std::filesystem::path& file)
{
    ThermalReserveLoader loader;
    return loader.load(area, file);
}

template<class ClusterT>
void ClusterList<ClusterT>::retrieveTotalCapacityAndUnitCount(double& total, uint& unitCount) const
{
    total = 0.;
    unitCount = 0;

    for (const auto& c: all())
    {
        unitCount += c->unitCount;
        total += c->unitCount * c->nominalCapacity;
    }
}

// Force template instantiation
template class ClusterList<ThermalCluster>;
template class ClusterList<RenewableCluster>;

} // namespace Antares::Data
