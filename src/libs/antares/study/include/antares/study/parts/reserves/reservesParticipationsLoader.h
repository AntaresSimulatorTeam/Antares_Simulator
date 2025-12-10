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
                  readCapacityReservationSection(area, section);
              }
          });

        // Load symmetries after the reserves
        ini.each(
          [&](const IniFile::Section& section)
          {
              if (section.name == "symmetries")
              {
                  readSymmetrySection(area, section);
              }
          });

        return true;
    }

private:
    virtual void readCapacityReservationSection(Area& area, const IniFile::Section& section) = 0;
    virtual void readSymmetrySection(Area& area, const IniFile::Section& section) = 0;
};

template<class ClusterT>
class ThermalReserveLoader: public ReserveParticipationLoader<ThermalReserveLoader<ClusterT>>
{
    void readCapacityReservationSection(Area& area, const IniFile::Section& section) override
    {
        std::string clusterName;
        ThermalClusterReserveParticipation reserveParticipation;

        section.each(
          [&](const IniFile::Property& p)
          {
              if (p.key == "cluster-name")
              {
                  TransformNameIntoID(p.value, clusterName);
              }
              else if (p.key == "max-power")
              {
                  p.value.to<double>(reserveParticipation.maxPower);
              }
              else if (p.key == "participation-cost")
              {
                  p.value.to<double>(reserveParticipation.participationCost);
              }
              else if (p.key == "max-power-off")
              {
                  p.value.to<double>(reserveParticipation.maxPowerOff);
              }
              else if (p.key == "participation-cost-off")
              {
                  p.value.to<double>(reserveParticipation.participationCostOff);
              }
              else
              {
                  logs.warning() << area.name
                                 << " : invalid property in thermal reserves implementation : "
                                 << p.key;
              }
          });
        if (clusterName.empty())
        {
            logs.error()
              << area.name
              << " : Please provide a cluster name when declaring a capacity reservation";
        }
        auto cluster = area.thermal.list.findInAll(clusterName);
        auto reserve = area.allCapacityReservations.value().getReserveByName(section.name);

        if (reserve && cluster)
        {
            reserveParticipation.capacityReservation = reserve;
            if (!cluster->reserveParticipationContainer)
            {
                cluster->reserveParticipationContainer.emplace();
            }
            if (cluster->reserveParticipationContainer.value().isParticipatingInReserve(
                  section.name))
            {
                logs.error() << area.name << ", cluster " << cluster->name()
                             << " : duplicate participation to reserve " << section.name;
            }
            cluster->reserveParticipationContainer.value()
              .addReserveParticipation(section.name, reserveParticipation);
        }
        else
        {
            if (!reserve)
            {
                logs.error() << area.name << ": missing reserve " << section.name
                             << " when loading thermal reserve participations";
            }
            if (!cluster)
            {
                logs.error() << area.name << " : missing cluster " << clusterName
                             << " when loading thermal reserve participations";
            }
        }
    }

    void readSymmetrySection(Area& area, const IniFile::Section& section) override
    {
        for (auto* p = section.firstProperty; p; p = p->next)
        {
            std::string clusterName;
            TransformNameIntoID(p->key, clusterName);
            auto symmetries = Symmetries::makeGroupsOfSymmetries(p->value);
            for (auto& sym: symmetries)
            {
                auto cluster = area.thermal.list.findInAll(clusterName);
                if (cluster)
                {
                    if (!cluster->reserveParticipationContainer)
                    {
                        throw std::runtime_error(
                          "Area " + area.name + ", " + clusterName
                          + " : trying to add symmetries without any reserves participations");
                    }
                    cluster->reserveParticipationContainer.value().addReserveParticipationSymmetry(
                      sym);
                }
                else
                {
                    logs.error() << "Thermal cluster " << clusterName
                                 << " not participating to reserves of " << area.name;
                }
            }
        }
    }
};

class STStorageReserveLoader: public ReserveParticipationLoader<STStorageReserveLoader>
{
    void readCapacityReservationSection(Area& area, const IniFile::Section& section) override
    {
        std::string clusterName;
        StorageClusterReserveParticipation reserveParticipation;

        section.each(
          [&](const IniFile::Property& p)
          {
              if (p.key == "cluster-name")
              {
                  TransformNameIntoID(p.value, clusterName);
              }
              else if (p.key == "max-release")
              {
                  p.value.to<double>(reserveParticipation.maxRelease);
              }
              else if (p.key == "max-store")
              {
                  p.value.to<double>(reserveParticipation.maxStore);
              }
              else if (p.key == "participation-cost")
              {
                  p.value.to<double>(reserveParticipation.participationCost);
              }
              else
              {
                  logs.warning() << area.name
                                 << " : invalid property in STS reserves implementation : "
                                 << p.key;
              }
          });

        auto reserve = area.allCapacityReservations.value().getReserveByName(section.name);
        auto cluster = area.shortTermStorage.findInAll(clusterName);
        if (clusterName.empty())
        {
            logs.error()
              << area.name
              << " : Please provide a cluster name when declaring a capacity reservation";
        }
        if (reserve && cluster)
        {
            reserveParticipation.capacityReservation = reserve;
            if (!cluster->reserveParticipationContainer)
            {
                cluster->reserveParticipationContainer.emplace();
            }

            cluster->reserveParticipationContainer.value()
              .addReserveParticipation(section.name, reserveParticipation);
        }
        else
        {
            if (!reserve)
            {
                logs.error() << area.name << ": missing reserve " << section.name
                             << " when loading STS reserve participations";
            }
            if (!cluster)
            {
                logs.error() << area.name << " : missing cluster " << clusterName
                             << " when loading STS reserve participations";
            }
        }
    }

    void readSymmetrySection(Area& area, const IniFile::Section& section) override
    {
        for (auto* p = section.firstProperty; p; p = p->next)
        {
            std::string clusterName;
            TransformNameIntoID(p->key, clusterName);
            auto symmetries = Symmetries::makeGroupsOfSymmetries(p->value);

            for (auto& sym: symmetries)
            {
                auto cluster = area.shortTermStorage.findInAll(clusterName);
                if (cluster && cluster->reserveParticipationContainer)
                {
                    cluster->reserveParticipationContainer.value().addReserveParticipationSymmetry(
                      sym);
                }
                else
                {
                    logs.error()
                      << "Area " << area.name << ", " << clusterName
                      << " : trying to add symmetries to a non existing cluster or participation";
                }
            }
        }
    }
};

class HydroReserveLoader: public ReserveParticipationLoader<HydroReserveLoader>
{
    void readCapacityReservationSection(Area& area, const IniFile::Section& section) override
    {
        StorageClusterReserveParticipation reserveParticipation;

        section.each(
          [&](const IniFile::Property& p)
          {
              if (p.key == "max-release")
              {
                  p.value.to<double>(reserveParticipation.maxRelease);
              }
              else if (p.key == "max-store")
              {
                  p.value.to<double>(reserveParticipation.maxStore);
              }
              else if (p.key == "participation-cost")
              {
                  p.value.to<double>(reserveParticipation.participationCost);
              }
              else
              {
                  logs.warning() << area.name
                                 << " : invalid property in hydro reserves implementation : "
                                 << p.key;
              }
          });

        auto reserve = area.allCapacityReservations.value().getReserveByName(section.name);
        if (reserve)
        {
            reserveParticipation.capacityReservation = reserve;
            auto& reserveParticipationContainer = area.hydro.reserveParticipationContainer;
            if (!reserveParticipationContainer)
            {
                reserveParticipationContainer.emplace();
            }

            reserveParticipationContainer.value().addReserveParticipation(section.name,
                                                                          reserveParticipation);
        }
        else
        {
            logs.error() << area.name << ": missing reserve " << section.name
                         << " when loading hydro reserve participations";
        }
    }

    void readSymmetrySection(Area& area, const IniFile::Section& section) override
    {
        auto& reserveParticipationContainer = area.hydro.reserveParticipationContainer;
        if (!area.hydro.reserveParticipationContainer)
        {
            throw std::runtime_error(
              "Area " + area.name
              + ", hydro : trying to add symmetries without any reserves participations");
        }
        for (auto* p = section.firstProperty; p; p = p->next)
        {
            std::string clusterName;
            TransformNameIntoID(p->key, clusterName);
            if (!(boost::iequals(clusterName, "hydro") || boost::iequals(clusterName, "lt")))
            {
                logs.error() << area.name << " : invalid cluster name for hydro symmetry "
                             << clusterName << " please use 'hydro' or 'lt'";
            }
            auto symmetries = Symmetries::makeGroupsOfSymmetries(p->value);
            for (auto& sym: symmetries)
            {
                reserveParticipationContainer.value().addReserveParticipationSymmetry(sym);
            }
        }
    }
};
} // namespace Antares::Data
