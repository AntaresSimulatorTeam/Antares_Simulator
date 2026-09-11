// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/study/parts/reserves/reservesParticipationsLoader.h>

namespace Antares::Data
{
void errorIfNegativeValue(const std::string& propertyName,
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

static void validateStorageSpecificInputs(const std::string& areaName,
                                          const StorageClusterReserveParticipation& rp,
                                          const std::string& clusterName,
                                          const std::string& reserveID)
{
    errorIfNegativeValue("max-release", rp.maxRelease, areaName, clusterName, reserveID);
    errorIfNegativeValue("max-store", rp.maxStore, areaName, clusterName, reserveID);
}

// ThermalReserveLoader

void ThermalReserveLoader::parseParticipations(Area& area, const YAML::Node& root)
{
    parsePerClusterParticipations(area, root);
}

std::string ThermalReserveLoader::extractName(const YAML::Node& entry)
{
    try
    {
        auto name = entry["cluster"].as<std::string>();
        if (name.empty())
        {
            return {};
        }
        return transformNameIntoID(name);
    }
    catch (const YAML::Exception&)
    {
        return {};
    }
}

void ThermalReserveLoader::readProperties(const YAML::Node& cert,
                                          ThermalClusterReserveParticipation& rp)
{
    static const std::unordered_map<std::string, double ThermalClusterReserveParticipation::*>
      propMap = {{"participation-cost", &ThermalClusterReserveParticipation::participationCost},
                 {"max-power", &ThermalClusterReserveParticipation::maxPower},
                 {"max-power-off", &ThermalClusterReserveParticipation::maxPowerOff},
                 {"participation-cost-off",
                  &ThermalClusterReserveParticipation::participationCostOff}};
    readYamlProperties(cert, rp, "thermal", propMap);
}

void ThermalReserveLoader::validateSpecificInputs(const std::string& areaName,
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

void ThermalReserveLoader::reportMissing(const Area& area,
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

void ThermalReserveLoader::reportMissingCluster(const Area& area, const std::string& clusterName)
{
    logs.error() << "Thermal cluster " << clusterName << " does not exist in area " << area.name;
}

// STStorageReserveLoader

void STStorageReserveLoader::parseParticipations(Area& area, const YAML::Node& root)
{
    parsePerClusterParticipations(area, root);
}

std::string STStorageReserveLoader::extractName(const YAML::Node& entry)
{
    try
    {
        auto name = entry["storage"].as<std::string>();
        if (name.empty())
        {
            return {};
        }
        return transformNameIntoID(name);
    }
    catch (const YAML::Exception&)
    {
        return {};
    }
}

void STStorageReserveLoader::readProperties(const YAML::Node& cert,
                                            StorageClusterReserveParticipation& rp)
{
    static const std::unordered_map<std::string, double StorageClusterReserveParticipation::*>
      propMap = {{"participation-cost", &StorageClusterReserveParticipation::participationCost},
                 {"max-release", &StorageClusterReserveParticipation::maxRelease},
                 {"max-store", &StorageClusterReserveParticipation::maxStore}};
    readYamlProperties(cert, rp, "STS", propMap);
}

void STStorageReserveLoader::validateSpecificInputs(const std::string& areaName,
                                                    const StorageClusterReserveParticipation& rp,
                                                    const std::string& clusterName,
                                                    const std::string& reserveID)
{
    validateStorageSpecificInputs(areaName, rp, clusterName, reserveID);
}

void STStorageReserveLoader::reportMissing(const Area& area,
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
        logs.error() << area.name << " : missing STStorage when loading STS reserve participation";
    }
}

void STStorageReserveLoader::reportMissingCluster(const Area& area, const std::string& clusterName)
{
    logs.error() << "Short term storage " << clusterName << " does not exist in area " << area.name;
}

// HydroReserveLoader

void HydroReserveLoader::parseParticipations(Area& area, const YAML::Node& root)
{
    const auto& participationsNode = root["participations"];
    if (!participationsNode)
    {
        return;
    }

    for (const auto& cert: participationsNode["certifications"])
    {
        StorageClusterReserveParticipation rp{};
        std::string reserveId;
        try
        {
            reserveId = transformNameIntoID(cert["reserve"].as<std::string>());
        }
        catch (const YAML::Exception&)
        {
            logs.error() << area.name << " : missing or invalid reserve name in certification";
            continue;
        }

        readProperties(cert, rp);

        const auto* reserve = area.allCapacityReservations->getReserveByID(reserveId);
        if (!reserve)
        {
            logs.error() << area.name << " : missing reserve " << reserveId
                         << " when loading hydro reserve participations";
            continue;
        }
        rp.capacityReservation = reserve;

        validateCapacityInputs(area.name, rp, std::nullopt, reserveId);
        addCapacityReservation(area, rp, std::nullopt, reserveId);
    }

    for (const auto& sym: participationsNode["symmetries"])
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
            logs.error() << area.name << " : symmetry group must have at least two reserves";
            continue;
        }
        if (area.hydro.reserveParticipationContainer)
        {
            area.hydro.reserveParticipationContainer->addReserveParticipationSymmetry(symGroup);
        }
        else
        {
            logs.error() << "Area " << area.name
                         << ", hydro : trying to add symmetries without any reserve participation";
        }
    }
}

void HydroReserveLoader::readProperties(const YAML::Node& cert,
                                        StorageClusterReserveParticipation& rp)
{
    static const std::unordered_map<std::string, double StorageClusterReserveParticipation::*>
      propMap = {{"participation-cost", &StorageClusterReserveParticipation::participationCost},
                 {"max-release", &StorageClusterReserveParticipation::maxRelease},
                 {"max-store", &StorageClusterReserveParticipation::maxStore}};
    readYamlProperties(cert, rp, "hydro", propMap);
}

void HydroReserveLoader::validateSpecificInputs(const std::string& areaName,
                                                const StorageClusterReserveParticipation& rp,
                                                const std::string& clusterName,
                                                const std::string& reserveID)
{
    validateStorageSpecificInputs(areaName, rp, clusterName, reserveID);
}

void HydroReserveLoader::duplicateParticipation(const std::string& areaName,
                                                const std::string&,
                                                const std::string& reserveID)
{
    logs.error() << areaName << ", hydro: duplicate participation to reserve " << reserveID;
}

void HydroReserveLoader::reportMissing(const Area& area,
                                       const std::string& resserveID,
                                       bool reserveOK,
                                       bool)
{
    if (!reserveOK)
    {
        logs.error() << area.name << " : missing reserve " << resserveID
                     << " when loading hydro reserve participations";
    }
}

} // namespace Antares::Data
