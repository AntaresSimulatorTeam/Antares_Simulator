// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <utility>
#include <yaml-cpp/yaml.h>

#include <boost/algorithm/string/case_conv.hpp>

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

std::string miscGenTypeName(int index)
{
    switch (index)
    {
    case MiscGenIndex::fhhCHP:
        return "combined_heat_power";
    case MiscGenIndex::fhhBioMass:
        return "biomass";
    case MiscGenIndex::fhhBioGaz:
        return "biogas";
    case MiscGenIndex::fhhWaste:
        return "waste";
    case MiscGenIndex::fhhGeoThermal:
        return "geothermal";
    case MiscGenIndex::fhhOther:
        return "other";
    case MiscGenIndex::fhhPSP:
        return "pumped_storage_power";
    case MiscGenIndex::fhhRowBalance:
        return "rest_world";
    default:
        return "unknown";
    }
}

std::string areaLocation(const std::string& areaId)
{
    return BuildAreaNodeComponentId(areaId);
}

YAML::Node areaToYaml(const Area& area)
{
    return makeComponent(BuildAreaNodeComponentId(area.id),
                         "antares_legacy_models.area",
                         {{"carrier", "electricity"}});
}

YAML::Node loadToYaml(const Area& area)
{
    return makeComponent(BuildLoadComponentId(area.id),
                         "antares_legacy_models.load",
                         {{"carrier", "electricity"}});
}

YAML::Node windToYaml(const Area& area)
{
    return makeComponent(BuildWindComponentId(area.id),
                         "antares_legacy_models.renewable",
                         {{"carrier", "electricity"}, {"technology", "wind"}});
}

YAML::Node solarToYaml(const Area& area)
{
    return makeComponent(BuildSolarComponentId(area.id),
                         "antares_legacy_models.renewable",
                         {{"carrier", "electricity"}, {"technology", "solar"}});
}

YAML::Node rorToYaml(const Area& area)
{
    return makeComponent(BuildRorComponentId(area.id),
                         "antares_legacy_models.renewable",
                         {{"carrier", "electricity"}, {"technology", "run_of_river"}});
}

YAML::Node linkToYaml(const AreaLink& link)
{
    return makeComponent(BuildLinkComponentId(link.from->id, link.with->id),
                         "antares_legacy_models.link",
                         {{"carrier", "electricity"}});
}

YAML::Node thermalClusterToYaml(const ThermalCluster& cluster)
{
    return makeComponent(BuildThermalClusterComponentId(cluster.parentArea->id, cluster.id()),
                         "antares_legacy_models.thermal",
                         {{"carrier", "electricity"},
                          {"technology", boost::to_lower_copy(cluster.getGroup())}});
}

YAML::Node renewableClusterToYaml(const RenewableCluster& cluster)
{
    return makeComponent(BuildRenewableClusterComponentId(cluster.parentArea->id, cluster.id()),
                         "antares_legacy_models.renewable",
                         {{"carrier", "electricity"},
                          {"technology", boost::to_lower_copy(cluster.getGroup())}});
}

YAML::Node miscGenToYaml(const Area& area, int miscGenIndex)
{
    return makeComponent(BuildMiscGenComponentId(area.id, miscGenTypeName(miscGenIndex)),
                         "antares_legacy_models.miscellaneous_generation",
                         {{"carrier", "electricity"},
                          {"technology", miscGenTypeName(miscGenIndex)},
                          {"miscellaneous_type", miscGenMiscellaneousType(miscGenIndex)}});
}

YAML::Node shortTermStorageToYaml(const Area& area,
                                  const ShortTermStorage::STStorageCluster& storage)
{
    return makeComponent(BuildSTStorageClusterComponentId(area.id, storage.id),
                         "antares_legacy_models.short_term_storage",
                         {{"carrier", "electricity"},
                          {"group", boost::to_lower_copy(storage.getGroup())}});
}

YAML::Node longTermStorageToYaml(const Area& area)
{
    return makeComponent(BuildHydroStorageComponentId(area.id),
                         "antares_legacy_models.long_term_storage",
                         {{"carrier", "electricity"}});
}

} // namespace Antares::ViewBuilder
