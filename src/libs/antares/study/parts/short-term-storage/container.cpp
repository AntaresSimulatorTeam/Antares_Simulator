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

#include "antares/study/parts/short-term-storage/container.h"

#include <algorithm>
#include <filesystem>
#include <string>
#include <numeric>

#include "antares/study/parts/short-term-storage/container.h"
#include "antares/study/study.h"

#define SEP Yuni::IO::Separator

namespace fs = std::filesystem;

namespace Antares::Data::ShortTermStorage
{
bool STStorageInput::validate() const
{
    return std::ranges::all_of(storagesByIndex, [](auto& cluster) { return cluster.validate(); });
}

bool STStorageInput::createSTStorageClustersFromIniFile(const fs::path& path)
{
    const fs::path pathIni = path / "list.ini";
    IniFile ini;
    if (!ini.open(pathIni))
    {
        return false;
    }

    if (!ini.firstSection)
    {
        return true;
    }

    logs.debug() << "  :: loading `" << pathIni << "`";

    for (auto* section = ini.firstSection; section; section = section->next)
    {
        STStorageCluster cluster;
        if (!cluster.loadFromSection(*section))
        {
            return false;
        }

        storagesByIndex.push_back(cluster);
    }

    std::ranges::sort(storagesByIndex,
                      [](const auto& a, const auto& b)
                      { return a.properties.name < b.properties.name; });

    return true;
}

bool STStorageInput::loadSeriesFromFolder(const std::string& folder) const
{
    if (folder.empty())
    {
        return false;
    }

    bool ret = true;

    for (auto& cluster: storagesByIndex)
    {
        const std::string buffer(folder + SEP + cluster.id);
        ret = cluster.loadSeries(buffer) && ret;
    }

    return ret;
}

bool STStorageInput::loadReserveParticipations(Area& area, const AnyString& file)
{
    IniFile ini;
    if (!ini.open(file, false))
        return false;
    ini.each(
      [&](const IniFile::Section& section)
      {
          std::string tmpClusterName;
          float tmpMaxTurbining = 0;
          float tmpMaxPumping = 0;
          float tmpParticipationCost = 0;
          for (auto* p = section.firstProperty; p; p = p->next)
          {
              auto tmp = p->key;
              tmp.toLower();

              if (tmp == "cluster-name")
              {
                  TransformNameIntoID(p->value, tmpClusterName);
              }
              else if (tmp == "max-turbining")
              {
                  if (!p->value.to<float>(tmpMaxTurbining))
                  {
                      logs.warning() << area.name << ": invalid max turbining power for reserve "
                                     << section.name;
                  }
              }
              else if (tmp == "max-pumping")
              {
                  if (!p->value.to<float>(tmpMaxPumping))
                  {
                      logs.warning()
                        << area.name << ": invalid max pumping power for reserve " << section.name;
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
          auto cluster = getClusterByName(tmpClusterName);
          if (reserve && cluster)
          {
              STStorageClusterReserveParticipation tmpReserveParticipation{
                reserve.value(), tmpMaxTurbining, tmpMaxPumping, tmpParticipationCost};
              cluster.value().get().addReserveParticipation(section.name, tmpReserveParticipation);
          }
          else
          {
              if (!reserve)
                  logs.warning() << area.name << ": does not contains this reserve "
                                 << section.name;
              if (!cluster)
                  logs.warning() << area.name << ": does not contains this cluster "
                                 << tmpClusterName;
          }
      });
    return true;
}

bool STStorageInput::saveToFolder(const std::string& folder) const
{
    // create empty list.ini if there's no sts in this area
    Yuni::IO::Directory::Create(folder);
    const std::string pathIni(folder + SEP + "list.ini");
    IniFile ini;

    logs.debug() << "saving file " << pathIni;
    std::ranges::for_each(storagesByIndex,
                          [&ini](auto& storage) { return storage.saveProperties(ini); });

    return ini.save(pathIni);
}

bool STStorageInput::saveDataSeriesToFolder(const std::string& folder) const
{
    Yuni::IO::Directory::Create(folder);
    return std::ranges::all_of(storagesByIndex,
                               [&folder](auto& storage)
                               { return storage.saveSeries(folder + SEP + storage.id); });
}

std::size_t STStorageInput::count() const
{
    return std::ranges::count_if(storagesByIndex,
                                 [](const STStorageCluster& st) { return st.properties.enabled; });
}

uint STStorageInput::removeDisabledClusters()
{
    return std::erase_if(storagesByIndex, [](const auto& c) { return !c.enabled(); });
}

std::pair<Data::ClusterName, Data::ReserveName> STStorageInput::reserveParticipationClusterAt(
  const Area* area,
  unsigned int index) const
{
    int globalReserveParticipationIdx = 0;

    for (auto const& [reserveUpName, _] : area->allCapacityReservations.areaCapacityReservationsUp)
    {
        for (auto& cluster : storagesByIndex)
        {
            if (cluster.clusterReservesParticipations.find(reserveUpName)
                != cluster.clusterReservesParticipations.end())
            {
                if (globalReserveParticipationIdx == index)
                {
                    return {cluster.id, reserveUpName};
                }
                globalReserveParticipationIdx++;
            }
        }
    }

    for (auto const& [reserveDownName, _] :
         area->allCapacityReservations.areaCapacityReservationsDown)
    {
        for (auto& cluster : storagesByIndex)
        {
            if (cluster.clusterReservesParticipations.find(reserveDownName)
                != cluster.clusterReservesParticipations.end())
            {
                if (globalReserveParticipationIdx == index)
                {
                    return {cluster.id, reserveDownName};
                }
                globalReserveParticipationIdx++;
            }
        }
    }

    throw std::out_of_range("This cluster reserve participation index has not been found in all "
                            "the reserve participations");
}

std::pair<Data::ShortTermStorage::Group, Data::ReserveName>
  STStorageInput::reserveParticipationGroupAt(const Area* area, unsigned int index) const
{
    int column = 0;
    for (auto [reserveName, _] : area->allCapacityReservations.areaCapacityReservationsUp)
    {
        for (int indexGroup = 0; indexGroup < Data::groupMax; indexGroup++)
        {
            if (column == index)
                return {static_cast<Data::ShortTermStorage::Group>(indexGroup), reserveName};
            column++;
        }
    }
    for (auto [reserveName, _] : area->allCapacityReservations.areaCapacityReservationsDown)
    {
        for (int indexGroup = 0; indexGroup < Data::groupMax; indexGroup++)
        {
            if (column == index)
                return {static_cast<Data::ShortTermStorage::Group>(indexGroup), reserveName};
            column++;
        }
    }
    throw std::out_of_range("This group reserve participation index has not been found in all the "
                            "reserve participations");
}

std::optional<std::reference_wrapper<STStorageCluster>> STStorageInput::getClusterByName(
  const std::string& name)
{
    auto it = std::find_if(storagesByIndex.begin(),
                           storagesByIndex.end(),
                           [&name](STStorageCluster& cluster) { return cluster.id == name; });
    if (it != storagesByIndex.end())
        return std::ref(*it);
    else
        return std::nullopt;
}

size_t STStorageInput::getClusterIdx(STStorageCluster& cluster)
{
    auto it = std::find_if(storagesByIndex.begin(),
                           storagesByIndex.end(),
                           [&cluster](STStorageCluster& elem) { return &elem == &cluster; });
    if (it != storagesByIndex.end())
    {
        return std::distance(storagesByIndex.begin(), it);
    }
    else
    {
        throw std::out_of_range("This Short Term Storage is not in the list");
    }
}

uint STStorageInput::reserveParticipationsCount()
{
    return std::accumulate(storagesByIndex.begin(),
                           storagesByIndex.end(),
                           0,
                           [](int total, STStorageCluster& cluster)
                           { return total + cluster.reserveParticipationsCount(); });
}

} // namespace Antares::Data::ShortTermStorage
