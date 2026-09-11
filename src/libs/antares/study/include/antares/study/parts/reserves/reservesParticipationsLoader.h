// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <fstream>
#include <unordered_map>
#include <yaml-cpp/yaml.h>

#include <antares/study/area/area.h>

namespace Antares::Data
{
template<typename Derived>
class ReserveParticipationLoader
{
public:
    [[nodiscard]] bool load(Area& area, const std::filesystem::path& file)
    {
        if (!std::filesystem::exists(file))
        {
            return false;
        }

        std::ifstream f(file, std::ios_base::binary | std::ios_base::in);
        if (!f.is_open())
        {
            return false;
        }

        std::string content(std::istreambuf_iterator<char>{f}, {});
        if (!f && !f.eof())
        {
            return false;
        }

        YAML::Node root;
        try
        {
            root = YAML::Load(content);
        }
        catch (const YAML::Exception& e)
        {
            logs.error() << "Error while reading reserve participations file " << file << ": "
                         << e.what();
            return false;
        }

        Derived::parseParticipations(area, root);
        return true;
    }
};

void errorIfNegativeValue(const std::string& propertyName,
                          double value,
                          const std::string& areaName,
                          const std::optional<std::string>& clusterName,
                          const std::string& resserveID);

template<typename T>
void readYamlProperties(const YAML::Node& cert,
                        T& rp,
                        const std::string& typeName,
                        const std::unordered_map<std::string, double T::*>& propMap)
{
    for (const auto& prop: cert)
    {
        auto k = prop.first.as<std::string>();
        if (k == "reserve")
        {
            continue;
        }

        auto it = propMap.find(k);
        if (it != propMap.end())
        {
            try
            {
                rp.*(it->second) = prop.second.as<double>();
            }
            catch (const YAML::Exception&)
            {
                logs.error() << "invalid " << typeName << " reserve property " << k;
            }
        }
        else
        {
            logs.error() << "invalid " << typeName << " reserve property " << k;
        }
    }
}

class HydroReserveLoader;

template<typename Derived, typename ParticipationT>
class ReserveLoaderMixin: public ReserveParticipationLoader<Derived>
{
public:
    using participation_type = ParticipationT;

    static void validateCapacityInputs(const std::string& areaName,
                                       const participation_type& rp,
                                       const std::optional<std::string>& clusterName,
                                       const std::string& reserveID)
    {
        if ((!clusterName || clusterName->empty())
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
        const auto* reserve = area.allCapacityReservations->getReserveByID(reserveID);
        auto* cluster = Derived::findCluster(area, clusterName.value_or(""));

        if (!reserve || !cluster)
        {
            Derived::reportMissing(area, reserveID, reserve != nullptr, cluster != nullptr);
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

    static void parsePerClusterParticipations(Area& area, const YAML::Node& root)
    {
        const auto& participations = root["participations"];
        if (!participations)
        {
            return;
        }

        for (const auto& entry: participations)
        {
            std::string clusterName = Derived::extractName(entry);
            if (clusterName.empty())
            {
                logs.error() << area.name
                             << " : cluster/storage name is missing in a participation entry";
                continue;
            }

            auto* cluster = Derived::findCluster(area, clusterName);
            if (!cluster)
            {
                Derived::reportMissingCluster(area, clusterName);
                continue;
            }

            for (const auto& cert: entry["certifications"])
            {
                participation_type rp{};
                std::string reserveId;
                try
                {
                    reserveId = transformNameIntoID(cert["reserve"].as<std::string>());
                }
                catch (const YAML::Exception&)
                {
                    logs.error() << area.name
                                 << " : missing or invalid reserve name in certification";
                    continue;
                }

                Derived::readProperties(cert, rp);

                validateCapacityInputs(area.name, rp, clusterName, reserveId);
                addCapacityReservation(area, rp, clusterName, reserveId);
            }

            for (const auto& sym: entry["symmetries"])
            {
                std::set<ReserveID> symGroup;
                for (const auto& reserveName: sym["reserves"])
                {
                    try
                    {
                        symGroup.insert(transformNameIntoID(reserveName.as<std::string>()));
                    }
                    catch (const YAML::Exception&)
                    {
                        logs.error() << area.name << " : invalid reserve name in symmetries";
                    }
                }
                if (symGroup.size() < 2)
                {
                    logs.error() << area.name
                                 << " : symmetry group must have at least two reserves";
                    continue;
                }
                auto& container = Derived::getContainer(cluster);
                if (container)
                {
                    container->addReserveParticipationSymmetry(symGroup);
                }
                else
                {
                    Derived::reportLackOfReserveParticipation(area, clusterName);
                }
            }
        }
    }

protected:
    static void duplicateParticipation(const std::string& areaName,
                                       const std::string& clusterName,
                                       const std::string& reserveID)
    {
        logs.error() << areaName << ", cluster " << clusterName
                     << " : duplicate participation to reserve " << reserveID;
    }

    static void reportLackOfReserveParticipation(const Area& area, const std::string& clusterName)
    {
        logs.error() << "Area " << area.name << ", " << clusterName
                     << " : trying to add symmetries without any reserve participation";
    }

    template<class ClusterT>
    static void addGroupToResIndex(Area& area, const std::string& reserveID, ClusterT* cluster)
    {
        if constexpr (std::is_same_v<ClusterT, ShortTermStorage::STStorageCluster>)
        {
            area.allCapacityReservations->reserveGroupPartSTS[reserveID].insert(
              cluster->getGroup());
        }
        else if constexpr (std::is_same_v<ClusterT, ThermalCluster>)
        {
            area.allCapacityReservations->reserveGroupPartThermal[reserveID].insert(
              cluster->getGroup());
        }
    }
};

class ThermalReserveLoader
    : public ReserveLoaderMixin<ThermalReserveLoader, ThermalClusterReserveParticipation>
{
public:
    static void parseParticipations(Area& area, const YAML::Node& root);
    static std::string extractName(const YAML::Node& entry);
    static void readProperties(const YAML::Node& cert, ThermalClusterReserveParticipation& rp);
    static void validateSpecificInputs(const std::string& areaName,
                                       const ThermalClusterReserveParticipation& rp,
                                       const std::string& clusterName,
                                       const std::string& reserveID);

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
                              bool clusterOK);
    static void reportMissingCluster(const Area& area, const std::string& clusterName);

    using ReserveLoaderMixin::duplicateParticipation;
    using ReserveLoaderMixin::reportLackOfReserveParticipation;
};

class STStorageReserveLoader
    : public ReserveLoaderMixin<STStorageReserveLoader, StorageClusterReserveParticipation>
{
public:
    static void parseParticipations(Area& area, const YAML::Node& root);
    static std::string extractName(const YAML::Node& entry);
    static void readProperties(const YAML::Node& cert, StorageClusterReserveParticipation& rp);
    static void validateSpecificInputs(const std::string& areaName,
                                       const StorageClusterReserveParticipation& rp,
                                       const std::string& clusterName,
                                       const std::string& reserveID);

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
                              bool clusterOK);
    static void reportMissingCluster(const Area& area, const std::string& clusterName);

    using ReserveLoaderMixin::duplicateParticipation;
    using ReserveLoaderMixin::reportLackOfReserveParticipation;
};

class HydroReserveLoader
    : public ReserveLoaderMixin<HydroReserveLoader, StorageClusterReserveParticipation>
{
public:
    static void parseParticipations(Area& area, const YAML::Node& root);
    static void readProperties(const YAML::Node& cert, StorageClusterReserveParticipation& rp);
    static void validateSpecificInputs(const std::string& areaName,
                                       const StorageClusterReserveParticipation& rp,
                                       const std::string& clusterName,
                                       const std::string& reserveID);
    static void duplicateParticipation(const std::string& areaName,
                                       const std::string&,
                                       const std::string& reserveID);

    static auto* findCluster(Area& area, const std::string&)
    {
        return &area.hydro;
    }

    static auto& getContainer(auto* hydro)
    {
        return hydro->reserveParticipationContainer;
    }

    static void reportMissing(const Area& area, const std::string& resserveID, bool reserveOK, bool);
};

} // namespace Antares::Data
