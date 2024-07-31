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
#include "antares/study/parts/common/cluster_list.h"

#include <algorithm>
#include <numeric>

#include <boost/algorithm/string/case_conv.hpp>

#include <antares/utils/utils.h>
#include "antares/study/study.h"

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
std::pair<Data::ClusterName, Data::ReserveName>
  ClusterList<ClusterT>::reserveParticipationClusterAt(const Area* area, unsigned int index) const
{
    int globalReserveParticipationIdx = 0;

    for (auto const& [reserveUpName, _] : area->allCapacityReservations.areaCapacityReservationsUp)
    {
        for (auto& cluster : allClusters_)
        {
            if (cluster->isParticipatingInReserve(reserveUpName))
            {
                if (globalReserveParticipationIdx == index)
                {
                    return {cluster->name(), reserveUpName};
                }
                globalReserveParticipationIdx++;
            }
        }
    }

    for (auto const& [reserveDownName, _] :
         area->allCapacityReservations.areaCapacityReservationsDown)
    {
        for (auto& cluster : allClusters_)
        {
            if (cluster->isParticipatingInReserve(reserveDownName))
            {
                if (globalReserveParticipationIdx == index)
                {
                    return {cluster->name(), reserveDownName};
                }
                globalReserveParticipationIdx++;
            }
        }
    }

    throw std::out_of_range("This cluster reserve participation index has not been found in all "
                            "the reserve participations");
}

template<class ClusterT>
std::pair<Data::ThermalDispatchableGroup, Data::ReserveName>
  ClusterList<ClusterT>::reserveParticipationGroupAt(const Area* area, unsigned int index) const
{
    int column = 0;
    for (auto [reserveName, _] : area->allCapacityReservations.areaCapacityReservationsUp)
    {
        for (int indexGroup = 0; indexGroup < Data::groupMax; indexGroup++)
        {
            if (column == index)
                return {static_cast<Data::ThermalDispatchableGroup>(indexGroup), reserveName};
            column++;
        }
    }
    for (auto [reserveName, _] : area->allCapacityReservations.areaCapacityReservationsDown)
    {
        for (int indexGroup = 0; indexGroup < Data::groupMax; indexGroup++)
        {
            if (column == index)
                return {static_cast<Data::ThermalDispatchableGroup>(indexGroup), reserveName};
            column++;
        }
    }
    throw std::out_of_range("This group reserve participation index has not been found in all the "
                            "reserve participations");
}

template<class ClusterT>
std::pair<Data::ThermalUnsuppliedSpilled, Data::ReserveName>
ClusterList<ClusterT>::reserveParticipationUnsuppliedSpilledAt(const Area* area, unsigned int index) const
{
    int column = 0;
    for (auto [reserveName, _] : area->allCapacityReservations.areaCapacityReservationsUp)
    {
        for (int indexUnsuppliedSpilled = 0; indexUnsuppliedSpilled < Data::unsuppliedSpilledMax; indexUnsuppliedSpilled++)
        {
            if (column == index)
                return { static_cast<Data::ThermalUnsuppliedSpilled>(indexUnsuppliedSpilled), reserveName };
            column++;
        }
    }
    for (auto [reserveName, _] : area->allCapacityReservations.areaCapacityReservationsDown)
    {
        for (int indexUnsuppliedSpilled = 0; indexUnsuppliedSpilled < Data::unsuppliedSpilledMax; indexUnsuppliedSpilled++)
        {
            if (column == index)
                return { static_cast<Data::ThermalUnsuppliedSpilled>(indexUnsuppliedSpilled), reserveName };
            column++;
        }
    }
    throw std::out_of_range("This reserve status index has not been found in all the "
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
bool ClusterList<ClusterT>::exists(const Data::ClusterName& id) const
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

#define SEP IO::Separator

template<class ClusterT>
void ClusterList<ClusterT>::storeTimeseriesNumbers(Solver::IResultWriter& writer) const
{
    Clob path;
    std::string ts_content;

    for (auto& cluster: each_enabled())
    {
        path.clear() << "ts-numbers" << SEP << typeID() << SEP << cluster->parentArea->id << SEP
                     << cluster->id() << ".txt";
        ts_content.clear(); // We must clear ts_content here, since saveToBuffer does not do it.
        cluster->series.timeseriesNumbers.saveToBuffer(ts_content);
        writer.addEntryFromBuffer(path.c_str(), ts_content);
    }
}

template<class ClusterT>
std::optional<std::shared_ptr<Cluster>> ClusterList<ClusterT>::getClusterByName(
  std::string clusterName)
{
    auto it = std::find_if(allClusters_.begin(),
                           allClusters_.end(),
                           [&](const auto& cluster) { return cluster->id() == clusterName; });

    return (it != allClusters_.end()) ? std::optional<std::shared_ptr<ClusterT>>(*it)
                                      : std::nullopt;
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
    if (alreadyInAllClusters(cluster->id()))
    {
        return;
    }
    allClusters_.push_back(cluster);
    sortCompleteList();
    rebuildIndexes();
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
void ClusterList<ClusterT>::rebuildIndexes()
{
    // First, we give an index to every cluster, enabled / must-run or not.
    // We do that to :
    //  - Stick to what was done before and not change the results
    //  - Avoids seg faults, for instance when storing thermal noises (solver.hxx).
    //    Indeed : otherwise disabled clusters have an infinite index
    unsigned int index = 0;
    for (auto& c: allClusters_)
    {
        c->areaWideIndex = index;
        index++;
    }

    index = 0;
    for (auto& c: each_enabled())
    {
        c->areaWideIndex = index;
        index++;
    }
}

template<class ClusterT>
bool ClusterList<ClusterT>::rename(Data::ClusterName idToFind, Data::ClusterName newName)
{
    if (idToFind.empty() or newName.empty())
    {
        return false;
    }

    // Internal:
    // It is vital to make copy of these strings. We can not make assumption that these
    // CString are not from the same buffer (name, id) than ours.
    // It may have an undefined behavior.
    // Consequently, the parameters `idToFind` and `newName` shall not be `const &`.

    // Making sure that the id is lowercase
    boost::to_lower(idToFind);

    // The new ID
    Data::ClusterName newID = Antares::transformNameIntoID(newName);

    // Looking for the renewable clusters in the list
    auto* cluster_ptr = this->findInAll(idToFind);
    if (!cluster_ptr)
    {
        return true;
    }

    if (idToFind == newID)
    {
        cluster_ptr->setName(newName);
        return true;
    }

    // The name is the same. Aborting nicely.
    if (cluster_ptr->name() == newName)
    {
        return true;
    }

    // Already exist
    if (this->exists(newID))
    {
        return false;
    }

    cluster_ptr->setName(newName);

    // Invalidate matrices attached to the area
    // It is a bit excessive (all matrices not only those related to the renewable clusters)
    // will be rewritten but currently it is the less error-prone.
    if (cluster_ptr->parentArea)
    {
        (cluster_ptr->parentArea)->invalidateJIT = true;
    }

    return true;
}

template<class ClusterT>
bool ClusterList<ClusterT>::forceReload(bool reload) const
{
    return std::ranges::all_of(allClusters_,
                               [&reload](const auto& c) { return c->forceReload(reload); });
}

template<class ClusterT>
void ClusterList<ClusterT>::markAsModified() const
{
    for (const auto& c: allClusters_)
    {
        c->markAsModified();
    }
}

template<class ClusterT>
bool ClusterList<ClusterT>::remove(const Data::ClusterName& id)
{
    auto nbDeletion = std::erase_if(allClusters_,
                                    [&id](const SharedPtr& c) { return c->id() == id; });

    // Invalidating the parent area
    forceReload();

    return nbDeletion > 0;
}

template<class ClusterT>
bool ClusterList<ClusterT>::saveDataSeriesToFolder(const AnyString& folder) const
{
    return std::ranges::all_of(allClusters_,
                               [&folder](const auto c)
                               { return c->saveDataSeriesToFolder(folder); });
}

template<class ClusterT>
bool ClusterList<ClusterT>::loadDataSeriesFromFolder(Study& s, const AnyString& folder)
{
    return std::ranges::all_of(allClusters_,
                               [&s, &folder](auto c)
                               { return c->loadDataSeriesFromFolder(s, folder); });
}

template<class ClusterT>
bool ClusterList<ClusterT>::loadReserveParticipations(Area& area, const AnyString& file)
{
    IniFile ini;
    if (!ini.open(file, false))
        return false;
    ini.each(
      [&](const IniFile::Section& section)
      {
          std::string tmpClusterName;
          float tmpMaxPower = 0;
          float tmpParticipationCost = 0;
          for (auto* p = section.firstProperty; p; p = p->next)
          {
              CString<30, false> tmp;
              tmp = p->key;
              tmp.toLower();

              if (tmp == "cluster-name")
              {
                  TransformNameIntoID(p->value, tmpClusterName);
              }
              else if (tmp == "max-power")
              {
                  if (!p->value.to<float>(tmpMaxPower))
                  {
                      logs.warning()
                        << area.name << ": invalid max power for reserve " << section.name;
                  }
              }
              else if (tmp == "participation-cost")
              {
                  if (!p->value.to<float>(tmpParticipationCost))
                  {
                      logs.warning()
                        << area.name << ": invalid participation cost for reserve " << section.name;
                  }
              }
          }
          auto reserve = area.allCapacityReservations.getReserveByName(section.name);
          auto cluster = area.thermal.list.getClusterByName(tmpClusterName);
          if (reserve && cluster)
          {
              ClusterReserveParticipation tmpReserveParticipation{
                reserve.value(), tmpMaxPower, tmpParticipationCost};
              cluster.value().get()->addReserveParticipation(section.name, tmpReserveParticipation);
          }
          else
          {
              if (!reserve)
                logs.warning() << area.name << ": does not contains this reserve " << section.name;
              if (!cluster)
                  logs.warning() << area.name << ": does not contains this cluster "
                                 << tmpClusterName;
          }
      });
    return true;
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
