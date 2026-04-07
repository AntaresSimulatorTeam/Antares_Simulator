// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-system/converter.h"

#include <algorithm>
#include <sstream>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include "antares/io/inputs/forbidden-nodes/ForbiddenNodesVisitor.h"
#include "antares/io/inputs/yml-system/system.h"
#include "antares/logs/logs.h"
#include "antares/study/system-model/connection.h"
#include "antares/study/system-model/system.h"

using namespace Antares::ModelerStudy;
using namespace Antares::ModelerStudy::SystemModel; // Mainly for type ConnexionEnd
using namespace Antares::IO::Inputs::ForbidNodes;

namespace Antares::IO::Inputs::SystemConverter
{

namespace
{
class ErrorWhileSplittingLibraryAndModel final: public std::runtime_error
{
public:
    explicit ErrorWhileSplittingLibraryAndModel(const std::string& s):
        runtime_error("'.' not found while splitting library and model: " + s)
    {
    }
};

class LibraryNotFound final: public std::runtime_error
{
public:
    explicit LibraryNotFound(const std::string& s):
        runtime_error("No library found with this name: " + s)
    {
    }
};

class ModelNotFound final: public std::runtime_error
{
public:
    explicit ModelNotFound(const std::string& s):
        runtime_error("No model found with this name: " + s)
    {
    }
};

std::pair<std::string, std::string> splitLibraryModelString(const std::string& s)
{
    size_t pos = s.find('.');
    if (pos == std::string::npos)
    {
        throw ErrorWhileSplittingLibraryAndModel(s);
    }

    std::string library = s.substr(0, pos);
    std::string model = s.substr(pos + 1);
    return {library, model};
}

const Model& getModel(const std::vector<Library>& libraries,
                      const std::string& libraryId,
                      const std::string& modelId)
{
    auto lib = std::ranges::find_if(libraries,
                                    [&libraryId](const auto& l) { return l.Id() == libraryId; });
    if (lib == libraries.end())
    {
        throw LibraryNotFound(libraryId);
    }

    auto search = lib->Models().find(modelId);
    if (search == lib->Models().end())
    {
        throw ModelNotFound(modelId);
    }

    return search->second;
}

Component createComponent(const YmlSystem::Component& c, const std::vector<Library>& libraries)
{
    const auto [libraryId, modelId] = splitLibraryModelString(c.model);

    const Model& model = getModel(libraries, libraryId, modelId);

    ComponentBuilder component_builder;

    std::map<std::string, ParameterTypeAndValue> parameters;
    for (const auto& [id, time_dependent, scenario_dependent, value]: c.parameters)
    {
        parameters.try_emplace(
          id,
          ParameterTypeAndValue{.id = id,
                                .type = Optimisation::variability(time_dependent,
                                                                  scenario_dependent),
                                .value = value});
    }

    auto component = component_builder.withId(c.id)
                       .withModel(&model)
                       .withScenarioGroupId(c.scenarioGroup)
                       .withParameterValues(parameters)
                       .build();
    return component;
}

Component& findComponent(const std::string& id, std::vector<Component>& components)

{
    const auto it = std::ranges::find_if(components,
                                         [&id](const Component& c) { return c.Id() == id; });
    if (it == components.end())
    {
        throw std::invalid_argument("Component with id '" + id + "' not found in system.");
    }
    return *it;
}

void CheckPortSelfConnection(const std::string& firstComponentId,
                             const std::string& firstPortId,
                             const std::string& secondComponentId,
                             const std::string& secondPortId)
{
    if (firstComponentId == secondComponentId && firstPortId == secondPortId)
    {
        std::ostringstream msg;
        msg << "Can not connect Port '" << firstPortId << "' from component '" << firstComponentId
            << "' to itself!";
        throw ConnectingPortToItSelf(msg.str());
    }
}

void CheckPortsType(const Port& firstPort, const Port& secondPort)
{
    if (firstPort.Type() != secondPort.Type())
    {
        std::ostringstream msg;
        msg << "Ports '" << firstPort.Id() << "' and '" << secondPort.Id()
            << "' are not of the same type!";
        throw std::invalid_argument(msg.str());
    }
}

void CheckFieldsRoleCompatibility(const Port& port_1, const Port& port_2)
{
    for (const auto& field: port_1.Type().Fields())
    {
        const auto portFieldRole_1 = port_1.fieldRole(field.Id());
        const auto portFieldRole_2 = port_2.fieldRole(field.Id());

        if (portFieldRole_1 == portFieldRole_2)
        {
            std::ostringstream msg;
            msg << "Field '" << field.Id() << "' is " << portFieldRole_1 << " in both ports '"
                << port_1.Id() << "' and '" << port_2.Id() << "'";
            throw TwoFieldsOfSameRole(msg.str());
        }
    }
}

/**
 * @brief Uses a YmlSystem::Connection to connect component via ports
 *
 * A YmlSystem::Connection has two entries, which are the two ends of a connexion
 * between components.
 * Caution : components can be connected via ports which can be different (different id),
 * but of the same type.
 * So, from a YmlSystem::Connection, this function connects two components via ports :
 * Each component receives a SystemModel::ConnexionEnd representing the connexion it has with
 * the other component.
 * Doing this, this function ensures that the connected ports are of the same type and that
 * fields are correctly configured for sending and receiving.
 *
 * @param connection A YmlSystem::Connection object containing the connection details.
 * @param components An unordered map of component IDs to SystemModel::Component objects.
 *
 * @return void
 *
 * @throw std::invalid_argument if a component or port is not found, if the ports are not
 *        of the same type, or if fields are incorrectly configured for sending/receiving.
 */
void connectComponents(const YmlSystem::Connection& connection, std::vector<Component>& components)
{
    const auto& componentId_1 = connection.firstEntry.componentId;
    const auto& portId_1 = connection.firstEntry.portId;
    const auto& componentId_2 = connection.secondEntry.componentId;
    const auto& portId_2 = connection.secondEntry.portId;
    CheckPortSelfConnection(componentId_1, portId_1, componentId_2, portId_2);

    auto& component_1 = findComponent(componentId_1, components);
    const auto& port_1 = component_1.findPort(portId_1, "");
    auto& component_2 = findComponent(componentId_2, components);
    const auto& port_2 = component_2.findPort(portId_2, "");
    CheckPortsType(port_1, port_2);

    CheckFieldsRoleCompatibility(port_1, port_2);

    // TODO : Do we need to connect both components to one another ?
    // TODO : Or should we rather consider the field role and only connect receiver to the sender ?
    component_1.addComponentConnection(port_1.Id(), ConnectionEnd(&component_2, &port_2));
    component_2.addComponentConnection(port_2.Id(), ConnectionEnd(&component_1, &port_1));
}

/**
 * @brief Uses a YmlSystem::AreaConnection to connect areas and components via ports
 *
 * @param connection A YmlSystem::AreaConnection object containing the connection details.
 * @param components An unordered map of component IDs to SystemModel::Component objects.
 *
 * @return void
 *
 * @throw std::invalid_argument if a component is not found, or if the connection could not be
 * established
 */
void connectAreas(const YmlSystem::AreaConnection& connection, std::vector<Component>& components)
{
    auto& component = findComponent(connection.componentId, components);
    component.addAreaConnection(connection.portId, connection.areaId);
}

/**
 * @brief Uses a YmlSystem::ThermalCapacityConnection to connect (areas, clusters) and components
 * via ports
 *
 * @param connection A YmlSystem::ThermalCapacityConnection object containing the connection
 * details.
 * @param components An unordered map of component IDs to SystemModel::Component objects.
 *
 * @return void
 *
 * @throw std::invalid_argument if a component is not found, or if the connection could not be
 * established
 */
void connectThermalCapacity(const YmlSystem::ThermalCapacityConnection& connection,
                            std::vector<Component>& components)
{
    // TODO : check that area exists in legacy study? seems complicated here
    auto& component = findComponent(connection.componentId, components);

    component.addThermalCapacityConnection(connection.portId,
                                           connection.thermalComponent.areaId,
                                           connection.thermalComponent.clusterId);
}

} // anonymous namespace

void checkForNonLinearityBehindConnections(const std::vector<Component>& components)
{
    // Now that connections are made between components, we need to check for
    // linearity of binding constraints in the current component's model.
    // Thus, if the constaints expressions hold a sum_connections(...) operator containing
    // port field resolution, we have to make sure that, on the port field other side, only
    // a linear expression is found.
    auto filterBC = std::views::filter([](const auto& c) { return c.isBindingConstraint(); });
    for (const auto& component: components)
    {
        // We only consider binding constraints.
        for (const auto& binding_constraint: component.getModel()->Constraints() | filterBC)
        {
            const auto& expression = binding_constraint.expression();
            ForbiddenNodesInComponentVisitor visitor(forbidNonLinearNodes,
                                                     expression.Value(),
                                                     component);
            visitor.dispatch(expression.RootNode());
        }
    }
}

System convert(const YmlSystem::System& ymlSystem, const std::vector<Library>& libraries)
{
    // Create components from system
    std::vector<Component> components;
    for (const auto& c: ymlSystem.components)
    {
        auto it = std::ranges::find_if(std::as_const(components),
                                       [&c](const Component& compo) { return compo.Id() == c.id; });
        if (it != components.end())
        {
            throw std::invalid_argument("System has at least two components with the same id ('"
                                        + c.id + "'), this is not supported");
        }
        components.push_back(createComponent(c, libraries));
        logs.debug() << "Loaded component `" << c.id << "`";
    }

    // Create connections from system
    for (const auto& connection: ymlSystem.connections)
    {
        connectComponents(connection, components);
        logs.debug() << "Loaded connection (component1=`" << connection.firstEntry.componentId
                     << "` component2=`" << connection.secondEntry.componentId << "`)";
    }

    checkForNonLinearityBehindConnections(components);

    // Create area connections from system
    for (const auto& connection: ymlSystem.areaConnections)
    {
        connectAreas(connection, components);
        logs.debug() << "Loaded area connection (component=`" << connection.componentId
                     << "` area=`" << connection.areaId << "`)";
    }
    // Create thermal capacity connections from system
    for (const auto& connection: ymlSystem.thermalCapacityConnections)
    {
        connectThermalCapacity(connection, components);
        logs.debug() << "Loaded thermal-capacity connection (component=`" << connection.componentId
                     << "` area=`" << connection.thermalComponent.areaId << "` clusterId=`"
                     << connection.thermalComponent.clusterId << "`)";
    }

    // Build system from components and connections
    SystemBuilder builder;
    return builder.withId(ymlSystem.id).withComponents(std::move(components)).build();
}

} // namespace Antares::IO::Inputs::SystemConverter
