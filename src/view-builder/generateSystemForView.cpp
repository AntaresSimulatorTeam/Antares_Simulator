// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <cmath>
#include <ranges>

#include <antares/io/inputs/InputError.h>
#include <antares/solver/modeler/ModelerData.h>
#include <antares/solver/optimisation/opt_rename_problem.h>
#include <antares/study/area/constants.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/system.h>
#include <antares/view-builder/legacyToYaml.h>
#include <antares/view-builder/viewBuilder.h>

using namespace Antares::Data;

namespace
{

YAML::Node makeConnection(const std::string& component1,
                          const std::string& port1,
                          const std::string& component2,
                          const std::string& port2)
{
    YAML::Node conn;
    conn["component1"] = component1;
    conn["port1"] = port1;
    conn["component2"] = component2;
    conn["port2"] = port2;
    return conn;
}

void checkForDuplicatesBetweenLegacyAndModeler(YAML::Node& systemYaml,
                                               const Antares::Solver::ModelerData* modelerData)
{
    std::set<std::string> legacyIds;
    std::set<std::string> gemsIds;

    for (const auto& component: systemYaml["system"]["components"])
    {
        legacyIds.insert(component["id"].as<std::string>());
    }

    for (const auto& component: modelerData->system->Components())
    {
        gemsIds.insert(component.Id());
    }

    std::vector<std::string> duplicates;
    std::set_intersection(legacyIds.begin(),
                          legacyIds.end(),
                          gemsIds.begin(),
                          gemsIds.end(),
                          std::back_inserter(duplicates));

    if (!duplicates.empty())
    {
        throw Antares::IO::Inputs::InputError(
          fmt::format("GEMS component(s) '{}' cannot be named like that due to a conflict with a "
                      "legacy component(s), please rename it",
                      boost::join(duplicates, ", ")));
    }
}

constexpr double zeroTolerance = 1e-12;

bool hasNonZeroValues(const TimeSeries& series)
{
    return std::ranges::any_of(std::views::iota(uint32_t{0}, series.timeSeries.width),
                               [&series](const auto column)
                               {
                                   return std::ranges::any_of(
                                     std::views::iota(uint32_t{0}, series.timeSeries.height),
                                     [&series, column](const auto row)
                                     { return std::abs(series.timeSeries[column][row]) > zeroTolerance; });
                               });
}

bool hasHydroInflows(const Area& area)
{
    return std::any_of(area.hydro.managementData.begin(),
                       area.hydro.managementData.end(),
                       [](const auto& entry)
                       {
                           const auto& inflows = entry.second.inflows;
                           return std::any_of(inflows.begin(),
                                              inflows.end(),
                                              [](const auto inflow)
                                              { return std::abs(inflow) > zeroTolerance; });
                       });
}

bool hasNonZeroMiscGeneration(const Area& area, const int index)
{
    return std::any_of(area.miscGen[index],
                       area.miscGen[index] + area.miscGen.height,
                       [](const auto value) { return std::abs(value) > zeroTolerance; });
}

void appendModelerData(YAML::Node& systemYaml, const Antares::Solver::ModelerData& modelerData)
{
    YAML::Node system = systemYaml["system"];

    for (const auto& component: modelerData.system->Components())
    {
        YAML::Node compNode;
        compNode["id"] = component.Id();
        compNode["model"] = component.getModel()->LibraryId() + "." + component.getModel()->Id();
        compNode["parameters"] = YAML::Node(YAML::NodeType::Sequence);
        compNode["parameters"].SetStyle(YAML::EmitterStyle::Flow);

        auto propsIt = modelerData.componentProperties.find(component.Id());
        if (propsIt != modelerData.componentProperties.end())
        {
            YAML::Node props = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& [propId, propValue]: propsIt->second)
            {
                YAML::Node prop;
                prop["id"] = propId;
                prop["value"] = propValue;
                props.push_back(prop);
            }
            compNode["properties"] = props;
        }

        system["components"].push_back(compNode);
    }

    for (const auto& component: modelerData.system->Components())
    {
        for (const auto& [portId, areaId]: component.portToAreaConnections())
        {
            YAML::Node conn;
            conn["component1"] = component.Id();
            conn["port1"] = portId;
            conn["component2"] = Antares::ViewBuilder::areaLocation(areaId);
            conn["port2"] = "balance_port";
            system["connections"].push_back(conn);
        }
    }

    for (const auto& component: modelerData.system->Components())
    {
        for (const auto& [portId, port]: component.getModel()->Ports())
        {
            for (const auto& connEnd: component.componentConnectionsViaPort(portId))
            {
                if (component.Id() < connEnd.component()->Id())
                {
                    YAML::Node conn;
                    conn["component1"] = component.Id();
                    conn["port1"] = portId;
                    conn["component2"] = connEnd.component()->Id();
                    conn["port2"] = connEnd.port()->Id();
                    system["connections"].push_back(conn);
                }
            }
        }
    }
}

} // anonymous namespace

namespace Antares::ViewBuilder
{

void exportSystemForView(const Antares::Data::Study& study, Solver::IResultWriter* resultWriter)
{
    YAML::Node systemYaml = generateSystemForView(study);
    std::string yamlContent = YAML::Dump(systemYaml);
    resultWriter->addEntryFromBuffer("system-for-views.yml", yamlContent);
}

YAML::Node generateSystemForView(const Antares::Data::Study& study)
{
    YAML::Node systemYaml = generateSystemLegacyComponents(study);
    if (auto* modelerData = study.getModelerData())
    {
        checkForDuplicatesBetweenLegacyAndModeler(systemYaml, modelerData);
        appendModelerData(systemYaml, *modelerData);
    }
    return systemYaml;
}

YAML::Node generateSystemLegacyComponents(const Antares::Data::Study& study)
{
    YAML::Node system;
    system["id"] = study.folder.string();

    YAML::Node components = YAML::Node(YAML::NodeType::Sequence);
    YAML::Node connections = YAML::Node(YAML::NodeType::Sequence);

    study.areas.each(
      [&components, &connections, &study](const Area& area)
      {
          std::string areaLoc = BuildAreaNodeComponentId(area.id);

          components.push_back(areaToYaml(area));

          if (hasNonZeroValues(area.load.series))
          {
              components.push_back(loadToYaml(area));
              connections.push_back(
                makeConnection(BuildLoadComponentId(area.id), "balance_port", areaLoc, "balance_port"));
          }

          if (hasNonZeroValues(area.wind.series))
          {
              components.push_back(windToYaml(area));
              connections.push_back(
                makeConnection(BuildWindComponentId(area.id), "balance_port", areaLoc, "balance_port"));
          }

          if (hasNonZeroValues(area.solar.series))
          {
              components.push_back(solarToYaml(area));
              connections.push_back(makeConnection(BuildSolarComponentId(area.id),
                                                   "balance_port",
                                                   areaLoc,
                                                   "balance_port"));
          }

          if (area.hydro.series && hasNonZeroValues(area.hydro.series->ror))
          {
              components.push_back(rorToYaml(area));
              connections.push_back(
                makeConnection(BuildRorComponentId(area.id), "balance_port", areaLoc, "balance_port"));
          }

          for (int i = 0; i < MiscGenIndex::fhhMax; ++i)
          {
              if (!hasNonZeroMiscGeneration(area, i))
                  continue;
              components.push_back(miscGenToYaml(area, i));
              connections.push_back(
                makeConnection(BuildMiscGenComponentId(area.id,
                                                       std::string(miscGenComponentNames[i])),
                               "balance_port",
                               areaLoc,
                               "balance_port"));
          }

          for (const auto& cluster: area.thermal.list.all())
          {
              if (!cluster->isEnabled())
                  continue;
              components.push_back(thermalClusterToYaml(*cluster));
              connections.push_back(
                makeConnection(BuildThermalClusterComponentId(area.id, cluster->id()),
                               "balance_port",
                               areaLoc,
                               "balance_port"));
          }

          for (const auto& cluster: area.renewable.list.all())
          {
              if (!cluster->isEnabled() || study.parameters.renewableGeneration() != rgClusters)
                  continue;
              components.push_back(renewableClusterToYaml(*cluster));
              connections.push_back(
                makeConnection(BuildRenewableClusterComponentId(area.id, cluster->id()),
                               "balance_port",
                               areaLoc,
                               "balance_port"));
          }

          for (const auto& st: area.shortTermStorage.storagesByIndex)
          {
              components.push_back(shortTermStorageToYaml(area, st));
              connections.push_back(makeConnection(BuildSTStorageClusterComponentId(area.id, st.id),
                                                   "balance_port",
                                                   areaLoc,
                                                   "balance_port"));
          }

          if (area.hydro.reservoirManagement || hasHydroInflows(area))
          {
              components.push_back(longTermStorageToYaml(area));
              connections.push_back(makeConnection(BuildHydroStorageComponentId(area.id),
                                                   "balance_port",
                                                   areaLoc,
                                                   "balance_port"));
          }

          for (const auto& [_, link]: area.links)
          {
              if (link->transmissionCapacities == LocalTransmissionCapacities::null)
                  continue;
              components.push_back(linkToYaml(*link));

              std::string linkId = BuildLinkComponentId(link->from->id, link->with->id);
              std::string area1Loc = BuildAreaNodeComponentId(link->from->id);
              std::string area2Loc = BuildAreaNodeComponentId(link->with->id);

              if (link->from->id < link->with->id)
              {
                  connections.push_back(
                    makeConnection(linkId, "in_port", area1Loc, "balance_port"));
                  connections.push_back(
                    makeConnection(linkId, "out_port", area2Loc, "balance_port"));
              }
              else
              {
                  connections.push_back(
                    makeConnection(linkId, "in_port", area2Loc, "balance_port"));
                  connections.push_back(
                    makeConnection(linkId, "out_port", area1Loc, "balance_port"));
              }
          }
      });

    system["components"] = components;
    system["connections"] = connections;

    YAML::Node root;
    root["system"] = system;
    return root;
}

} // namespace Antares::ViewBuilder
