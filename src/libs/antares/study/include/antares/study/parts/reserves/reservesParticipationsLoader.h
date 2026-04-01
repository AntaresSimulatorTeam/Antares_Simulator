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
#include <antares/study/parts/reserves/makeGroupsOfSymmetriesFromString.h>

namespace Antares::Data
{
template<typename Derived /*, typename ParticipationT*/>
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
                  auto& derivedObj = derived();
                  auto [participation, clusterName] = derivedObj.readCapacityReservationSection(
                    section);
                  derivedObj.validateCapacityInputs(area.name,
                                                    participation,
                                                    clusterName,
                                                    section.name);
                  derivedObj.addCapacityReservation(area, participation, clusterName, section.name);
              }
          });

        ini.each(
          [&](const IniFile::Section& section)
          {
              if (section.name == "symmetries")
              {
                  derived().readSymmetrySection(area, section);
              }
          });

        return true;
    }

private:
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    const Derived& derived() const
    {
        return static_cast<const Derived&>(*this);
    }
};

void static errorIfNegativeValue(const std::string& propertyName,
                                 double value,
                                 const std::string& areaName,
                                 const std::optional<std::string>& clusterName,
                                 const std::string& reserveName)
{
    if (value < 0)
    {
        logs.error() << "in area " << areaName
                     << (clusterName.has_value() ? ", cluster: " + clusterName.value() : "")
                     << ", reservation capacity in reserve: " << reserveName << ", " << propertyName
                     << " can not be negative";
    }
}

class HydroReserveLoader;

template<typename Derived, typename ParticipationT>
class ReserveLoaderMixin: public ReserveParticipationLoader<Derived>
{
public:
    using participation_type = ParticipationT;

    std::pair<ParticipationT, std::optional<std::string>> readCapacityReservationSection(
      const IniFile::Section& section)
    {
        participation_type rp{};
        std::optional<std::string> clusterName;

        derived().readProperties(section, clusterName, rp);
        return {rp, clusterName};
    }

    void validateCapacityInputs(const std::string& areaName,
                                const participation_type& rp,
                                const std::optional<std::string>& clusterName,
                                const std::string& reserveName)
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
                             reserveName);
        derived().validateSpecificInputs(areaName, rp, clusterName.value_or(""), reserveName);
    }

    void addCapacityReservation(Area& area,
                                participation_type& rp,
                                const std::optional<std::string>& clusterName,
                                const std::string& reserveName)
    {
        const auto* reserve = area.allCapacityReservations.value().getReserveByName(reserveName);
        auto* cluster = derived().findCluster(area, clusterName.value_or(""));

        if (!reserve || !cluster)
        {
            derived().reportMissing(area, reserveName, reserve, cluster);
            return;
        }

        rp.capacityReservation = reserve;

        auto& container = derived().getContainer(cluster);
        if (container && container->isParticipatingInReserve(reserveName))
        {
            derived().duplicateParticipation(area.name, clusterName.value_or(""), reserveName);
        }
        else if (!container)
        {
            container.emplace();
        }

        container->addReserveParticipation(reserveName, rp);
        addGroupToResIndex(area, reserveName, cluster);
    }

    template<class ClusterT>
    void addGroupToResIndex(Area& area, const std::string& reserveName, ClusterT* cluster)
    {
        if constexpr (std::is_same_v<ClusterT, ShortTermStorage::STStorageCluster>)
        {
            area.allCapacityReservations.value().reserveGroupPartSTS[reserveName].insert(
              cluster->getGroup());
        }
        else if constexpr (std::is_same_v<ClusterT, ThermalCluster>)
        {
            area.allCapacityReservations.value().reserveGroupPartThermal[reserveName].insert(
              cluster->getGroup());
        }
    }

    void readSymmetrySection(Area& area, const IniFile::Section& section)
    {
        for (auto* p = section.firstProperty; p; p = p->next)
        {
            std::string clusterName;
            TransformNameIntoID(p->key, clusterName);

            auto symGroups = Symmetries::makeGroupsOfSymmetries(p->value);

            auto* cluster = derived().findCluster(area, clusterName);
            if (!cluster)
            {
                derived().reportInvalidSymmetry(area, clusterName);
                continue;
            }
            auto& reserveContainer = derived().getContainer(cluster);
            if (!reserveContainer.has_value())
            {
                derived().reportLackOfReserveParticipation(area, clusterName);
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
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    const Derived& derived() const
    {
        return static_cast<const Derived&>(*this);
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
                                       const std::string& reserveName)
    {
        errorIfNegativeValue("max-power", rp.maxPower, areaName, clusterName, reserveName);
        errorIfNegativeValue("max-power-off", rp.maxPowerOff, areaName, clusterName, reserveName);
        errorIfNegativeValue("participation-cost-off",
                             rp.participationCostOff,
                             areaName,
                             clusterName,
                             reserveName);
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
                              const std::string& reserveName,
                              bool reserveOK,
                              bool clusterOK)
    {
        if (!reserveOK)
        {
            logs.error() << area.name << " : missing reserve " << reserveName
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
                                       const std::string& reserveName)
    {
        logs.error() << areaName << ", cluster " << clusterName
                     << " : duplicate participation to reserve " << reserveName;
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
                                       const std::string& reserveName)
    {
        errorIfNegativeValue("max-release", rp.maxRelease, areaName, clusterName, reserveName);
        errorIfNegativeValue("max-store", rp.maxStore, areaName, clusterName, reserveName);
    }

    static void duplicateParticipation(const std::string& areaName,
                                       const std::string& clusterName,
                                       const std::string& reserveName)
    {
        logs.error() << areaName << ", cluster " << clusterName
                     << " : duplicate participation to reserve " << reserveName;
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
                              const std::string& reserveName,
                              bool reserveOK,
                              bool clusterOK)
    {
        if (!reserveOK)
        {
            logs.error() << area.name << ": missing reserve " << reserveName
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
                                       const std::string& reserveName)
    {
        errorIfNegativeValue("max-release", rp.maxRelease, areaName, clusterName, reserveName);
        errorIfNegativeValue("max-store", rp.maxStore, areaName, clusterName, reserveName);
    }

    static void duplicateParticipation(const std::string& areaName,
                                       const std::string&,
                                       const std::string& reserveName)
    {
        logs.error() << areaName << ", hydro: duplicate participation to reserve " << reserveName;
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

    static void reportMissing(const Area& area,
                              const std::string& reserveName,
                              bool reserveOK,
                              bool)
    {
        if (!reserveOK)
        {
            logs.error() << area.name << " : missing reserve " << reserveName
                         << " when loading hydro reserve participations";
        }
    }

    static void reportLackOfReserveParticipation(const Area& area, const std::string&)
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
