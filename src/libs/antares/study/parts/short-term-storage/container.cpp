// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

// clang-format off
#include "antares/study/parts/short-term-storage/makeGroupsOfHoursFromString.h"
// clang-format on
#include "antares/study/parts/short-term-storage/container.h"

#include <algorithm>
#include <numeric>
#include <string>

#include <antares/study/parts/reserves/makeGroupsOfSymmetriesFromString.h>
#include <antares/utils/utils.h>
#include "antares/study/parts/reserves/reservesParticipationsLoader.h"
#include "antares/study/parts/short-term-storage/container.h"
#include "antares/study/parts/short-term-storage/makeGroupsOfHoursFromString.h"
#include "antares/study/study.h"

#define SEP Yuni::IO::Separator

namespace fs = std::filesystem;

namespace Antares::Data::ShortTermStorage
{
bool STStorageInput::validate(StudyVersion studyVersion) const
{
    return std::ranges::all_of(storagesByIndex,
                               [&studyVersion](auto& cluster)
                               { return cluster.validate(studyVersion); });
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
        cluster.properties.clusterGlobalIndex = storagesByIndex.size();
        storagesByIndex.push_back(cluster);
    }

    std::ranges::sort(storagesByIndex,
                      [](const auto& a, const auto& b)
                      { return a.properties.name < b.properties.name; });

    return true;
}

static std::vector<SingleAdditionalConstraint> toConstraints(
  const std::vector<std::set<int>>& groups)
{
    std::vector<SingleAdditionalConstraint> to_return;
    unsigned int counter = 0;
    for (const auto& group: groups)
    {
        to_return.push_back({.hours = group, .localIndex = counter});
        counter++;
    }
    return to_return;
}

static std::vector<SingleAdditionalConstraint> makeConstraints(std::string& hoursField)
{
    auto groupsOfHours = makeGroupsOfHours(hoursField);
    return toConstraints(groupsOfHours);
}

static bool readRHS(const fs::path& rhsPath, TimeSeries& rhsSeries)
{
    const bool ret = loadFile(rhsPath, rhsSeries);
    if (ret)
    {
        fillIfEmpty(rhsSeries, 0.0);
    }
    return ret;
}

static bool loadAdditionalConstraintsProperties(AdditionalConstraints* additionalConstraints,
                                                const IniFile::Section* section)
{
    for (auto* property = section->firstProperty; property; property = property->next)
    {
        const std::string key = property->key;
        const auto value = property->value;

        if (key == "enabled")
        {
            value.to<bool>(additionalConstraints->enabled);
        }
        else if (key == "variable")
        {
            value.to<std::string>(additionalConstraints->variable);
        }
        else if (key == "operator")
        {
            value.to<std::string>(additionalConstraints->operatorType);
        }
        else if (key == "hours")
        {
            try
            {
                std::string hoursField = value.c_str();
                additionalConstraints->constraints = makeConstraints(hoursField);
            }
            catch (const std::exception& e)
            {
                logs.error() << "Constraint " << additionalConstraints->name << " : " << e.what()
                             << '\n';
                return false;
            }
        }
        else
        {
            logs.error() << "Constraint " << additionalConstraints->name << " : "
                         << "has a wrong key : " << key;
            return false;
        }
    }
    return true;
}

bool STStorageInput::loadAdditionalConstraintsFromIni(const fs::path& parentPath)
{
    for (auto& sts: storagesByIndex)
    {
        auto data_path = parentPath / sts.id;
        IniFile ini;
        const auto pathIni = data_path / "additional-constraints.ini";
        if (!ini.open(pathIni, false))
        {
            logs.info() << "There is no: " << pathIni;
            continue;
        }

        for (auto* section = ini.firstSection; section; section = section->next)
        {
            auto additionalConstraints = std::make_shared<AdditionalConstraints>();
            additionalConstraints->name = section->name.c_str();
            additionalConstraints->id = transformNameIntoID(section->name.c_str());
            additionalConstraints->cluster_id = sts.id;

            if (!loadAdditionalConstraintsProperties(additionalConstraints.get(), section))
            {
                return false;
            }

            if (!additionalConstraints->enabled)
            {
                logs.info() << "Additional constraints disabled for ST "
                            << additionalConstraints->cluster_id;
                continue;
            }

            if (auto [ok, error_msg] = ShortTermStorage::validate(*additionalConstraints); !ok)
            {
                logs.error() << "Invalid constraint in section: " << section->name;
                logs.error() << error_msg;
                return false;
            }

            logs.info() << "Loaded ST additional constraint " << additionalConstraints->cluster_id
                        << "/" << additionalConstraints->name;
            sts.additionalConstraints.push_back(additionalConstraints);
        }
    }
    return true;
}

bool STStorageInput::loadAdditionalConstraintsRHS(const fs::path& parentPath)
{
    for (auto& sts: storagesByIndex)
    {
        auto data_path = parentPath / sts.id;
        for (auto& additionalConstraints: sts.additionalConstraints)
        {
            const auto rhsPath = data_path / ("rhs_" + additionalConstraints->id + ".txt");
            if (!readRHS(rhsPath, additionalConstraints->rhs()))
            {
                logs.error() << "Error while reading rhs file: " << rhsPath;
                return false;
            }
        }
    }
    return true;
}

bool STStorageInput::loadAdditionalConstraints(const fs::path& parentPath)
{
    return loadAdditionalConstraintsFromIni(parentPath) && loadAdditionalConstraintsRHS(parentPath);
}

bool STStorageInput::loadSeriesFromFolder(const fs::path& folder, StudyVersion studyVersion) const
{
    if (folder.empty())
    {
        return false;
    }

    bool ret = true;

    for (auto& sts: storagesByIndex)
    {
        fs::path seriesFolder = folder / sts.id;
        ret = sts.loadSeries(seriesFolder, studyVersion) && ret;
    }

    return ret;
}

bool STStorageInput::loadReserveParticipations(Area& area, const std::filesystem::path& file)
{
    STStorageReserveLoader loader;
    return loader.load(area, file);
}

void STStorageInput::resizeTimeseriesNumbers(unsigned int nbYears)
{
    for (auto& sts: storagesByIndex)
    {
        sts.series->inflowsTSNumbers.reset(nbYears);
        for (auto& ct: sts.additionalConstraints)
        {
            ct->timeseriesNumbers.reset(nbYears);
        }
    }
}

std::size_t STStorageInput::cumulativeConstraintCount() const
{
    return std::accumulate(
      storagesByIndex.begin(),
      storagesByIndex.end(),
      0,
      [](size_t outer_constraint_count, const auto& sts)
      {
          return outer_constraint_count
                 + std::accumulate(sts.additionalConstraints.begin(),
                                   sts.additionalConstraints.end(),
                                   0,
                                   [](size_t inner_constraint_count,
                                      const auto& additionalConstraints) {
                                       return inner_constraint_count
                                              + additionalConstraints->enabledConstraintsCount();
                                   });
      });
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

std::pair<std::string, ReserveID> STStorageInput::reserveParticipationClusterAt(
  const Area* area,
  unsigned int index) const
{
    unsigned int globalReserveParticipationIdx = 0;

    for (const auto& reserveID:
         area->allCapacityReservations.value().areaCapacityReservations | std::views::keys)
    {
        for (auto& cluster: storagesByIndex)
        {
            if (cluster.reserveParticipationContainer
                && cluster.reserveParticipationContainer.value().isParticipatingInReserve(
                  reserveID))
            {
                if (globalReserveParticipationIdx == index)
                {
                    return {cluster.id, reserveID};
                }
                globalReserveParticipationIdx++;
            }
        }
    }

    throw std::out_of_range("This cluster reserve participation index has not been found in all "
                            "the reserve participations");
}

std::pair<std::string, ReserveID> STStorageInput::reserveParticipationGroupAt(
  const Area* area,
  unsigned int index) const
{
    unsigned int column = 0;
    for (const auto& reserveID:
         area->allCapacityReservations.value().areaCapacityReservations | std::views::keys)
    {
        if (area->allCapacityReservations->reserveGroupPartSTS.contains(reserveID))
        {
            for (auto group: area->allCapacityReservations->reserveGroupPartSTS.at(reserveID))
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

STStorageCluster* STStorageInput::findInAll(const std::string& name)
{
    auto it = std::find_if(storagesByIndex.begin(),
                           storagesByIndex.end(),
                           [&name](STStorageCluster& cluster) { return cluster.id == name; });
    if (it != storagesByIndex.end())
    {
        return &(*it);
    }
    else
    {
        return nullptr;
    }
}

size_t STStorageInput::getClusterIdx(STStorageCluster& cluster) const
{
    auto it = std::find_if(storagesByIndex.begin(),
                           storagesByIndex.end(),
                           [&cluster](const STStorageCluster& elem) { return &elem == &cluster; });
    if (it != storagesByIndex.end())
    {
        return std::distance(storagesByIndex.begin(), it);
    }
    else
    {
        throw std::out_of_range("This Short Term Storage is not in the list");
    }
}

uint STStorageInput::reserveParticipationsCount() const
{
    return std::accumulate(
      storagesByIndex.begin(),
      storagesByIndex.end(),
      0,
      [](int total, const STStorageCluster& cluster)
      {
          return cluster.reserveParticipationContainer
                   ? total
                       + cluster.reserveParticipationContainer.value().reserveParticipationsCount()
                   : total;
      });
}
} // namespace Antares::Data::ShortTermStorage
