// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

// clang-format off
#include "antares/study/parts/short-term-storage/makeGroupsOfHoursFromString.h"
// clang-format on
#include "antares/study/parts/short-term-storage/container.h"

#include <algorithm>
#include <numeric>
#include <string>

#include <yuni/io/file.h>

#include <antares/logs/logs.h>
#include <antares/utils/utils.h>

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
} // namespace Antares::Data::ShortTermStorage
