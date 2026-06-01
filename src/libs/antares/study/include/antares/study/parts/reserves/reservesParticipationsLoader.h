/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#pragma once

#include <boost/algorithm/string.hpp>

#include <antares/inifile/inifile.h>
#include <antares/study/area/area.h>

namespace Antares::Data
{
template<typename Derived>
class ReserveParticipationLoader
{
public:
    bool load(Area& area, const std::filesystem::path& file)
    {
        IniFile ini;
        if (!ini.open(file, false))
        {
            return false;
        }

        ini.each(
          [&](const IniFile::Section& section)
          {
              if (section.name != "symmetries")
              {
                  auto [participation, clusterName] = Derived::readCapacityReservationSection(
                    section);
                  std::string reserveId = transformNameIntoID(section.name);
                  Derived::validateCapacityInputs(area.name, participation, clusterName, reserveId);
                  Derived::addCapacityReservation(area, participation, clusterName, reserveId);
              }
          });

        ini.each(
          [&](const IniFile::Section& section)
          {
              if (section.name == "symmetries")
              {
                  Derived::readSymmetrySection(area, section);
              }
          });

        return true;
    }
};

void static errorIfNegativeValue(const std::string& propertyName,
                                 double value,
                                 const std::string& areaName,
                                 const std::optional<std::string>& clusterName,
                                 const std::string& resserveID)
{
    if (value < 0)
    {
        logs.error() << "in area " << areaName
                     << (clusterName.has_value() ? ", cluster: " + clusterName.value() : "")
                     << ", reservation capacity in reserve: " << resserveID << ", " << propertyName
                     << " can not be negative";
    }
}

class HydroReserveLoader;

template<typename Derived, typename ParticipationT>
class ReserveLoaderMixin: public ReserveParticipationLoader<Derived>
{
public:
    using participation_type = ParticipationT;

    static std::pair<ParticipationT, std::optional<std::string>> readCapacityReservationSection(
      const IniFile::Section& section)
    {
        participation_type rp{};
        std::optional<std::string> clusterName;

        Derived::readProperties(section, clusterName, rp);
        return {rp, clusterName};
    }

    static void validateCapacityInputs(const std::string& areaName,
                                       const participation_type& rp,
                                       const std::optional<std::string>& clusterName,
                                       const std::string& reserveID)
    {
        if ((!clusterName || clusterName.value().empty())
            && !std::is_same<Derived, HydroReserveLoader>::value)
        {
            logs.error()
              << areaName
              << " : Please provide a cluster name when declaring a capacity reservation";
        }
        errorIfNegativeValue("participation-cost",
                             rp.participationCost,
                             areaName,
                             clusterName,
                             reserveID);
        Derived::validateSpecificInputs(areaName, rp, clusterName.value_or(""), reserveID);
    }

    static void addCapacityReservation(Area& area,
                                       participation_type& rp,
                                       const std::optional<std::string>& clusterName,
                                       const std::string& reserveID)
    {
        const auto* reserve = area.allCapacityReservations.value().getReserveByID(reserveID);
        auto* cluster = Derived::findCluster(area, clusterName.value_or(""));

        if (!reserve || !cluster)
        {
            Derived::reportMissing(area, reserveID, reserve, cluster);
            return;
        }

        rp.capacityReservation = reserve;

        auto& container = Derived::getContainer(cluster);
        if (container && container->isParticipatingInReserve(reserveID))
        {
            Derived::duplicateParticipation(area.name, clusterName.value_or(""), reserveID);
        }
        else if (!container)
        {
            container.emplace();
        }

        container->addReserveParticipation(reserveID, rp);
        addGroupToResIndex(area, reserveID, cluster);
    }

    static void readSymmetrySection(Area& area, const IniFile::Section& section)
    {
        for (auto* p = section.firstProperty; p; p = p->next)
        {
            std::string clusterName;
            TransformNameIntoID(p->key, clusterName);
            auto symGroups = Symmetries::makeGroupsOfSymmetries(p->value);
            for (auto& symElems: symGroups)
            {
                std::set<std::string> transformed;
                for (const auto& sym: symElems)
                {
                    transformed.insert(transformNameIntoID(sym));
                }
                symElems = std::move(transformed);
            }

            auto* cluster = Derived::findCluster(area, clusterName);
            if (!cluster)
            {
                Derived::reportInvalidSymmetry(area, clusterName);
                continue;
            }
            auto& reserveContainer = Derived::getContainer(cluster);
            if (!reserveContainer.has_value())
            {
                Derived::reportLackOfReserveParticipation(area, clusterName);
            }
            else
            {
                for (const auto& sym: symGroups)
                {
                    reserveContainer->addReserveParticipationSymmetry(sym);
                }
            }
        }
    }

private:
    template<class ClusterT>
    static void addGroupToResIndex(Area& area, const std::string& reserveID, ClusterT* cluster)
    {
        if constexpr (std::is_same_v<ClusterT, ShortTermStorage::STStorageCluster>)
        {
            area.allCapacityReservations.value().reserveGroupPartSTS[reserveID].insert(
              cluster->getGroup());
        }
        else if constexpr (std::is_same_v<ClusterT, ThermalCluster>)
        {
            area.allCapacityReservations.value().reserveGroupPartThermal[reserveID].insert(
              cluster->getGroup());
        }
    }
};

class ThermalReserveLoader
    : public ReserveLoaderMixin<ThermalReserveLoader, ThermalClusterReserveParticipation>
{
public:
    // lire les propriétés spécifiques au thermal
    static void readProperties(const IniFile::Section& section,
                               std::optional<std::string>& clusterName,
                               ThermalClusterReserveParticipation& rp)
    {
        section.each(
          [&](const IniFile::Property& p)
          {
              if (p.key == "cluster-name")
              {
                  clusterName.emplace();
                  TransformNameIntoID(p.value, clusterName.value());
              }
              else if (p.key == "participation-cost")
              {
                  p.value.to(rp.participationCost);
              }
              else if (p.key == "max-power")
              {
                  p.value.to(rp.maxPower);
              }
              else if (p.key == "max-power-off")
              {
                  p.value.to(rp.maxPowerOff);
              }
              else if (p.key == "participation-cost-off")
              {
                  p.value.to(rp.participationCostOff);
              }
              else
              {
                  logs.warning() << "invalid thermal reserve property " << p.key;
              }
          });
    }

    static void validateSpecificInputs(const std::string& areaName,
                                       const ThermalClusterReserveParticipation& rp,
                                       const std::string& clusterName,
                                       const std::string& reserveID)
    {
        errorIfNegativeValue("max-power", rp.maxPower, areaName, clusterName, reserveID);
        errorIfNegativeValue("max-power-off", rp.maxPowerOff, areaName, clusterName, reserveID);
        errorIfNegativeValue("participation-cost-off",
                             rp.participationCostOff,
                             areaName,
                             clusterName,
                             reserveID);
    }

    static auto* findCluster(const Area& area, const std::string& name)
    {
        return area.thermal.list.findInAll(name);
    }

    static auto& getContainer(auto* cluster)
    {
        return cluster->reserveParticipationContainer;
    }

    static void reportMissing(const Area& area,
                              const std::string& resserveID,
                              bool reserveOK,
                              bool clusterOK)
    {
        if (!reserveOK)
        {
            logs.error() << area.name << " : missing reserve " << resserveID
                         << " when loading thermal reserve participations";
        }
        if (!clusterOK)
        {
            logs.error() << area.name
                         << " : missing cluster name when loading thermal reserve participations";
        }
    }

    static void duplicateParticipation(const std::string& areaName,
                                       const std::string& clusterName,
                                       const std::string& resserveID)
    {
        logs.error() << areaName << ", cluster " << clusterName
                     << " : duplicate participation to reserve " << resserveID;
    }

    static void reportInvalidSymmetry(const Area& area, const std::string& clusterName)
    {
        logs.error() << "Thermal cluster " << clusterName << " does not exist in area "
                     << area.name;
    }

    static void reportLackOfReserveParticipation(const Area& area, const std::string& clusterName)
    {
        logs.error() << "Area " << area.name << ", " << clusterName
                     << " : trying to add symmetries without any reserve participation";
    }
};

class STStorageReserveLoader
    : public ReserveLoaderMixin<STStorageReserveLoader, StorageClusterReserveParticipation>
{
public:
    static void readProperties(const IniFile::Section& section,
                               std::optional<std::string>& clusterName,
                               StorageClusterReserveParticipation& rp)
    {
        section.each(
          [&](const IniFile::Property& p)
          {
              if (p.key == "cluster-name")
              {
                  clusterName.emplace();
                  TransformNameIntoID(p.value, clusterName.value());
              }
              else if (p.key == "participation-cost")
              {
                  p.value.to(rp.participationCost);
              }
              else if (p.key == "max-release")
              {
                  p.value.to(rp.maxRelease);
              }
              else if (p.key == "max-store")
              {
                  p.value.to(rp.maxStore);
              }
              else
              {
                  logs.warning() << "invalid STS reserve property " << p.key;
              }
          });
    }

    static void validateSpecificInputs(const std::string& areaName,
                                       const StorageClusterReserveParticipation& rp,
                                       const std::string& clusterName,
                                       const std::string& reserveID)
    {
        errorIfNegativeValue("max-release", rp.maxRelease, areaName, clusterName, reserveID);
        errorIfNegativeValue("max-store", rp.maxStore, areaName, clusterName, reserveID);
    }

    static void duplicateParticipation(const std::string& areaName,
                                       const std::string& clusterName,
                                       const std::string& reserveID)
    {
        logs.error() << areaName << ", cluster " << clusterName
                     << " : duplicate participation to reserve " << reserveID;
    }

    static auto* findCluster(Area& area, const std::string& name)
    {
        return area.shortTermStorage.findInAll(name);
    }

    static auto& getContainer(auto* cluster)
    {
        return cluster->reserveParticipationContainer;
    }

    static void reportMissing(const Area& area,
                              const std::string& reserveID,
                              bool reserveOK,
                              bool clusterOK)
    {
        if (!reserveOK)
        {
            logs.error() << area.name << ": missing reserve " << reserveID
                         << " when loading STS reserve participation";
        }
        if (!clusterOK)
        {
            logs.error() << area.name
                         << " : missing STStorage cluster when loading STS reserve participation";
        }
    }

    static void reportInvalidSymmetry(Area& area, const std::string& clusterName)
    {
        logs.error() << "ShortTerm Storage cluster " << clusterName << " does not exist in area "
                     << area.name;
    }

    static void reportLackOfReserveParticipation(const Area& area, const std::string& clusterName)
    {
        logs.error() << "Area " << area.name << ", " << clusterName
                     << " : trying to add symmetries without any reserve participation";
    }
};

class HydroReserveLoader
    : public ReserveLoaderMixin<HydroReserveLoader, StorageClusterReserveParticipation>
{
public:
    static void readProperties(const IniFile::Section& section,
                               std::optional<std::string>& /*hydro: ignore clusterName*/,
                               StorageClusterReserveParticipation& rp)
    {
        section.each(
          [&](const IniFile::Property& p)
          {
              if (p.key == "participation-cost")
              {
                  p.value.to(rp.participationCost);
              }
              else if (p.key == "max-release")
              {
                  p.value.to(rp.maxRelease);
              }
              else if (p.key == "max-store")
              {
                  p.value.to(rp.maxStore);
              }
              else
              {
                  logs.warning() << "invalid hydro reserve property " << p.key;
              }
          });
    }

    static void validateSpecificInputs(const std::string& areaName,
                                       const StorageClusterReserveParticipation& rp,
                                       const std::string& clusterName,
                                       const std::string& reserveID)
    {
        errorIfNegativeValue("max-release", rp.maxRelease, areaName, clusterName, reserveID);
        errorIfNegativeValue("max-store", rp.maxStore, areaName, clusterName, reserveID);
    }

    static void duplicateParticipation(const std::string& areaName,
                                       const std::string&,
                                       const std::string& reserveID)
    {
        logs.error() << areaName << ", hydro: duplicate participation to reserve " << reserveID;
    }

    static auto* findCluster(Area& area, const std::string& clusterName)
    {
        if (clusterName != "hydro" && clusterName != "lt" && !clusterName.empty())
        {
            logs.error() << area.name << " : invalid cluster name for hydro symmetry "
                         << clusterName << " please use 'hydro' or 'lt'";
        }
        return &area.hydro; // unique cluster
    }

    static auto& getContainer(auto* hydro)
    {
        return hydro->reserveParticipationContainer;
    }

    static void reportMissing(const Area& area, const std::string& resserveID, bool reserveOK, bool)
    {
        if (!reserveOK)
        {
            logs.error() << area.name << " : missing reserve " << resserveID
                         << " when loading hydro reserve participations";
        }
    }

    static void reportLackOfReserveParticipation(const Area& area,
                                                 [[maybe_unused]] const std::string& clusterName)
    {
        logs.error() << "Area " << area.name
                     << ", hydro : trying to add symmetries without any reserve participation";
    }

    static void reportInvalidSymmetry(const Area& area, const std::string& clusterName)
    {
        logs.error() << area.name << " : invalid hydro symmetry cluster " << clusterName;
    }
};

} // namespace Antares::Data
