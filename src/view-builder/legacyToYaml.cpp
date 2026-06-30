// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <utility>
#include <yaml-cpp/yaml.h>

#include <antares/solver/optimisation/opt_rename_problem.h>
#include <antares/study/area/constants.h>
#include <antares/study/study.h>
#include <antares/view-builder/legacyToYaml.h>

using namespace Antares::Data;

namespace
{

YAML::Node makeComponent(const std::string& id,
                         const std::string& modelId,
                         const std::vector<std::pair<std::string, std::string>>& properties)
{
    YAML::Node component;
    component["id"] = id;
    component["model"] = modelId;
    component["parameters"] = YAML::Node(YAML::NodeType::Sequence);
    component["parameters"].SetStyle(YAML::EmitterStyle::Flow);

    if (!properties.empty())
    {
        YAML::Node props = YAML::Node(YAML::NodeType::Sequence);
        for (const auto& [propId, propValue]: properties)
        {
            YAML::Node prop;
            prop["id"] = propId;
            prop["value"] = propValue;
            props.push_back(prop);
        }
        component["properties"] = props;
    }

    return component;
}

std::string miscGenTechnologyValue(int index)
{
    switch (index)
    {
    case MiscGenIndex::fhhCHP:
        return "chp";
    case MiscGenIndex::fhhBioMass:
        return "biomass";
    case MiscGenIndex::fhhBioGaz:
        return "biogaz";
    case MiscGenIndex::fhhWaste:
        return "waste";
    case MiscGenIndex::fhhGeoThermal:
        return "geothermal";
    case MiscGenIndex::fhhOther:
        return "other";
    case MiscGenIndex::fhhPSP:
        return "psp";
    case MiscGenIndex::fhhRowBalance:
        return "rowbalance";
    default:
        return "unknown";
    }
}

std::string miscGenMiscellaneousType(int index)
{
    switch (index)
    {
    case MiscGenIndex::fhhPSP:
        return "pumped_storage_power";
    case MiscGenIndex::fhhRowBalance:
        return "rest_world";
    default:
        return "misc_ndg";
    }
}

} // anonymous namespace

namespace Antares::ViewBuilder
{

std::string areaLocation(const std::string& areaId)
{
    return LocationIdentifier(areaId, AREA);
}

std::string makeComponentId(const std::string& areaId,
                            const std::string& childType,
                            const std::string& childId)
{
    std::string result = areaLocation(areaId) + SEP + childType;
    if (!childId.empty())
    {
        result += "<" + childId + ">";
    }
    return result;
}

std::string miscGenTypeName(int index)
{
    switch (index)
    {
    case MiscGenIndex::fhhCHP:
        return "CHP";
    case MiscGenIndex::fhhBioMass:
        return "BioMass";
    case MiscGenIndex::fhhBioGaz:
        return "BioGaz";
    case MiscGenIndex::fhhWaste:
        return "Waste";
    case MiscGenIndex::fhhGeoThermal:
        return "GeoThermal";
    case MiscGenIndex::fhhOther:
        return "Other";
    case MiscGenIndex::fhhPSP:
        return "PSP";
    case MiscGenIndex::fhhRowBalance:
        return "RowBalance";
    default:
        return "Unknown";
    }
}

YAML::Node areaToYaml(const Area& area)
{
    return makeComponent(areaLocation(area.id),
                         "antares_legacy_models.area",
                         {{"carrier", "electricity"}});
}

YAML::Node loadToYaml(const Area& area)
{
    return makeComponent(makeComponentId(area.id, "Load"),
                         "antares_legacy_models.load",
                         {{"carrier", "electricity"}});
}

YAML::Node windToYaml(const Area& area)
{
    return makeComponent(makeComponentId(area.id, "Wind"),
                         "antares_legacy_models.renewable",
                         {{"carrier", "electricity"}, {"technology", "wind"}});
}

YAML::Node solarToYaml(const Area& area)
{
    return makeComponent(makeComponentId(area.id, "Solar"),
                         "antares_legacy_models.renewable",
                         {{"carrier", "electricity"}, {"technology", "solar"}});
}

YAML::Node rorToYaml(const Area& area)
{
    return makeComponent(makeComponentId(area.id, "ROR"),
                         "antares_legacy_models.renewable",
                         {{"carrier", "electricity"}, {"technology", "run_of_river"}});
}

YAML::Node linkToYaml(const AreaLink& link)
{
    std::string id = LocationIdentifier(link.from->id + AREA_SEP + link.with->id, LINK);
    return makeComponent(id, "antares_legacy_models.link", {{"carrier", "electricity"}});
}

YAML::Node thermalClusterToYaml(const ThermalCluster& cluster)
{
    return makeComponent(makeComponentId(cluster.parentArea->id, "ThermalCluster", cluster.id()),
                         "antares_legacy_models.thermal",
                         {{"carrier", "electricity"}, {"technology", cluster.getGroup()}});
}

YAML::Node renewableClusterToYaml(const RenewableCluster& cluster)
{
    return makeComponent(makeComponentId(cluster.parentArea->id, "RenewableCluster", cluster.id()),
                         "antares_legacy_models.renewable",
                         {{"carrier", "electricity"}, {"technology", cluster.getGroup()}});
}

YAML::Node miscGenToYaml(const Area& area, int miscGenIndex)
{
    return makeComponent(makeComponentId(area.id, "MiscGen", miscGenTypeName(miscGenIndex)),
                         "antares_legacy_models.miscellaneous_generation",
                         {{"carrier", "electricity"},
                          {"technology", miscGenTechnologyValue(miscGenIndex)},
                          {"miscellaneous_type", miscGenMiscellaneousType(miscGenIndex)}});
}

YAML::Node shortTermStorageToYaml(const Area& area,
                                  const ShortTermStorage::STStorageCluster& storage)
{
    return makeComponent(makeComponentId(area.id, "ShortTermStorage", storage.id),
                         "antares_legacy_models.short_term_storage",
                         {{"carrier", "electricity"}, {"group", storage.getGroup()}});
}

YAML::Node longTermStorageToYaml(const Area& area)
{
    return makeComponent(makeComponentId(area.id, "Hydro"),
                         "antares_legacy_models.long_term_storage",
                         {{"carrier", "electricity"}, {"group", "hydro"}});
}

} // namespace Antares::ViewBuilder
