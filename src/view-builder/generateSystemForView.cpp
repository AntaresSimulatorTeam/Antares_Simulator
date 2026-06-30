// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <yaml-cpp/yaml.h>

#include <antares/solver/modeler/ModelerData.h>
#include <antares/solver/optimisation/opt_rename_problem.h>
#include <antares/study/area/constants.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/system.h>
#include <antares/view-builder/legacyToYaml.h>

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

} // anonymous namespace

namespace Antares::ViewBuilder
{

YAML::Node generateSystemForView(const Antares::Data::Study& study)
{
    YAML::Node legacyYaml = generateSystemLegacyComponents(study);
    if (study.getModelerData())
    {
        return mergeHybridSystemYaml(study, legacyYaml);
    }
    return legacyYaml;
}

YAML::Node generateSystemLegacyComponents(const Antares::Data::Study& study)
{
    YAML::Node system;
    system["id"] = "legacy_converted";
    YAML::Node libs = YAML::Node(YAML::NodeType::Sequence);
    libs.push_back("antares_legacy_models");
    system["model-libraries"] = libs;

    YAML::Node components = YAML::Node(YAML::NodeType::Sequence);
    YAML::Node connections = YAML::Node(YAML::NodeType::Sequence);

    study.areas.each(
      [&](const Area& area)
      {
          std::string areaLoc = areaLocation(area.id);

          components.push_back(areaToYaml(area));
          components.push_back(loadToYaml(area));
          connections.push_back(makeConnection(makeComponentId(area.id, "Load"),
                                               "balance_port",
                                               areaLoc,
                                               "balance_port"));

          components.push_back(windToYaml(area));
          connections.push_back(makeConnection(makeComponentId(area.id, "Wind"),
                                               "balance_port",
                                               areaLoc,
                                               "balance_port"));

          components.push_back(solarToYaml(area));
          connections.push_back(makeConnection(makeComponentId(area.id, "Solar"),
                                               "balance_port",
                                               areaLoc,
                                               "balance_port"));

          components.push_back(rorToYaml(area));
          connections.push_back(makeConnection(makeComponentId(area.id, "ROR"),
                                               "balance_port",
                                               areaLoc,
                                               "balance_port"));

          for (int i = 0; i < MiscGenIndex::fhhMax; ++i)
          {
              components.push_back(miscGenToYaml(area, i));
              connections.push_back(
                makeConnection(makeComponentId(area.id, "MiscGen", miscGenTypeName(i)),
                               "balance_port",
                               areaLoc,
                               "balance_port"));
          }

          for (const auto& cluster: area.thermal.list.all())
          {
              components.push_back(thermalClusterToYaml(*cluster));
              connections.push_back(
                makeConnection(makeComponentId(area.id, "ThermalCluster", cluster->id()),
                               "balance_port",
                               areaLoc,
                               "balance_port"));
          }

          for (const auto& cluster: area.renewable.list.all())
          {
              components.push_back(renewableClusterToYaml(*cluster));
              connections.push_back(
                makeConnection(makeComponentId(area.id, "RenewableCluster", cluster->id()),
                               "balance_port",
                               areaLoc,
                               "balance_port"));
          }

          for (const auto& st: area.shortTermStorage.storagesByIndex)
          {
              components.push_back(shortTermStorageToYaml(area, st));
              connections.push_back(
                makeConnection(makeComponentId(area.id, "ShortTermStorage", st.id),
                               "balance_port",
                               areaLoc,
                               "balance_port"));
          }

          components.push_back(longTermStorageToYaml(area));
          connections.push_back(makeConnection(makeComponentId(area.id, "Hydro"),
                                               "balance_port",
                                               areaLoc,
                                               "balance_port"));

          for (const auto& [_, link]: area.links)
          {
              components.push_back(linkToYaml(*link));

              std::string linkId = LocationIdentifier(link->from->id + AREA_SEP + link->with->id,
                                                      LINK);
              std::string area1Loc = areaLocation(link->from->id);
              std::string area2Loc = areaLocation(link->with->id);

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

YAML::Node mergeHybridSystemYaml(const Antares::Data::Study& study, const YAML::Node& legacyYaml)
{
    auto* modelerData = study.getModelerData();
    YAML::Node legacyComponents = legacyYaml["system"]["components"];
    YAML::Node legacyConnections = legacyYaml["system"]["connections"];

    YAML::Node system;
    system["id"] = modelerData->system->Id();

    YAML::Node libs(YAML::NodeType::Sequence);
    libs.push_back("antares_legacy_models");
    for (const auto& lib: modelerData->libraries)
    {
        libs.push_back(lib.Id());
    }
    system["model-libraries"] = libs;

    auto findModelName = [&](const ModelerStudy::SystemModel::Component& component) -> std::string
    {
        const auto* model = component.getModel();
        return model->LibraryId() + "." + model->Id();
    };

    YAML::Node components(YAML::NodeType::Sequence);
    for (const auto& component: modelerData->system->Components())
    {
        YAML::Node compNode;
        compNode["id"] = component.Id();
        compNode["model"] = findModelName(component);
        compNode["parameters"] = YAML::Node(YAML::NodeType::Sequence);
        compNode["parameters"].SetStyle(YAML::EmitterStyle::Flow);
        components.push_back(compNode);
    }
    for (const auto& comp: legacyComponents)
    {
        components.push_back(comp);
    }
    system["components"] = components;

    YAML::Node connections(YAML::NodeType::Sequence);
    for (const auto& component: modelerData->system->Components())
    {
        for (const auto& [portId, areaId]: component.portToAreaConnections())
        {
            YAML::Node conn;
            conn["component1"] = component.Id();
            conn["port1"] = portId;
            conn["component2"] = areaLocation(areaId);
            conn["port2"] = "balance_port";
            connections.push_back(conn);
        }
    }
    for (const auto& component: modelerData->system->Components())
    {
        auto* model = component.getModel();
        for (const auto& [portId, port]: model->Ports())
        {
            auto connEnds = component.componentConnectionsViaPort(portId);
            for (const auto& connEnd: connEnds)
            {
                if (component.Id() < connEnd.component()->Id())
                {
                    YAML::Node conn;
                    conn["component1"] = component.Id();
                    conn["port1"] = portId;
                    conn["component2"] = connEnd.component()->Id();
                    conn["port2"] = connEnd.port()->Id();
                    connections.push_back(conn);
                }
            }
        }
    }
    for (const auto& conn: legacyConnections)
    {
        connections.push_back(conn);
    }
    system["connections"] = connections;

    YAML::Node root;
    root["system"] = system;
    return root;
}

} // namespace Antares::ViewBuilder
