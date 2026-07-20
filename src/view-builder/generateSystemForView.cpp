// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <unordered_set>

#include <yaml-cpp/yaml.h>

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

void appendModelerData(YAML::Node& systemYaml,
                       const Antares::Solver::ModelerData& modelerData,
                       const std::unordered_set<std::string>& legacyIds)
{
    YAML::Node system = systemYaml["system"];

    for (const auto& lib: modelerData.libraries)
    {
        system["model-libraries"].push_back(lib.Id());
    }

    for (const auto& component: modelerData.system->Components())
    {
        if (legacyIds.contains(component.Id()))
        {
            throw Antares::IO::Inputs::InputError(
              "Duplicate component id '" + component.Id()
              + "' found in both legacy and modeler components");
        }

        YAML::Node compNode;
        compNode["id"] = component.Id();
        compNode["model"] = component.getModel()->LibraryId() + "." + component.getModel()->Id();
        compNode["parameters"] = YAML::Node(YAML::NodeType::Sequence);
        compNode["parameters"].SetStyle(YAML::EmitterStyle::Flow);
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
    std::unordered_set<std::string> legacyIds;
    YAML::Node systemYaml = generateSystemLegacyComponents(study, legacyIds);
    if (auto* modelerData = study.getModelerData())
    {
        appendModelerData(systemYaml, *modelerData, legacyIds);
    }
    return systemYaml;
}

YAML::Node generateSystemLegacyComponents(const Antares::Data::Study& study,
                                         std::unordered_set<std::string>& legacyComponentIds)
{
    YAML::Node system;
    system["id"] = study.folder.string();

    YAML::Node libs = YAML::Node(YAML::NodeType::Sequence);
    libs.push_back("antares_legacy_models");
    system["model-libraries"] = libs;

    YAML::Node components = YAML::Node(YAML::NodeType::Sequence);
    YAML::Node connections = YAML::Node(YAML::NodeType::Sequence);

    study.areas.each(
      [&components, &connections, &legacyComponentIds](const Area& area)
      {
          std::string areaLoc = BuildAreaNodeComponentId(area.id);

          components.push_back(areaToYaml(area));
          legacyComponentIds.insert(areaLoc);

          components.push_back(loadToYaml(area));
          legacyComponentIds.insert(area.id + "_load");
          connections.push_back(
            makeConnection(area.id + "_load", "balance_port", areaLoc, "balance_port"));

          components.push_back(windToYaml(area));
          legacyComponentIds.insert(area.id + "_wind");
          connections.push_back(
            makeConnection(area.id + "_wind", "balance_port", areaLoc, "balance_port"));

          components.push_back(solarToYaml(area));
          legacyComponentIds.insert(area.id + "_solar");
          connections.push_back(
            makeConnection(area.id + "_solar", "balance_port", areaLoc, "balance_port"));

          components.push_back(rorToYaml(area));
          legacyComponentIds.insert(area.id + "_ror");
          connections.push_back(
            makeConnection(area.id + "_ror", "balance_port", areaLoc, "balance_port"));

          for (int i = 0; i < MiscGenIndex::fhhMax; ++i)
          {
              components.push_back(miscGenToYaml(area, i));
              legacyComponentIds.insert(area.id + "_miscgen_" + miscGenTypeName(i));
              connections.push_back(makeConnection(area.id + "_miscgen_" + miscGenTypeName(i),
                                                   "balance_port",
                                                   areaLoc,
                                                   "balance_port"));
          }

          for (const auto& cluster: area.thermal.list.all())
          {
              components.push_back(thermalClusterToYaml(*cluster));
              legacyComponentIds.insert(
                BuildThermalClusterComponentId(area.id, cluster->id()));
              connections.push_back(
                makeConnection(BuildThermalClusterComponentId(area.id, cluster->id()),
                               "balance_port",
                               areaLoc,
                               "balance_port"));
          }

          for (const auto& cluster: area.renewable.list.all())
          {
              components.push_back(renewableClusterToYaml(*cluster));
              legacyComponentIds.insert(
                BuildRenewableClusterComponentId(area.id, cluster->id()));
              connections.push_back(
                makeConnection(BuildRenewableClusterComponentId(area.id, cluster->id()),
                               "balance_port",
                               areaLoc,
                               "balance_port"));
          }

          for (const auto& st: area.shortTermStorage.storagesByIndex)
          {
              components.push_back(shortTermStorageToYaml(area, st));
              legacyComponentIds.insert(
                BuildSTStorageClusterComponentId(area.id, st.id));
              connections.push_back(
                makeConnection(BuildSTStorageClusterComponentId(area.id, st.id),
                               "balance_port",
                               areaLoc,
                               "balance_port"));
          }

          components.push_back(longTermStorageToYaml(area));
          legacyComponentIds.insert(BuildHydroStorageComponentId(area.id));
          connections.push_back(makeConnection(BuildHydroStorageComponentId(area.id),
                                               "balance_port",
                                               areaLoc,
                                               "balance_port"));

          for (const auto& [_, link]: area.links)
          {
              components.push_back(linkToYaml(*link));

              std::string linkId = BuildLinkComponentId(link->from->id, link->with->id);
              legacyComponentIds.insert(linkId);
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
