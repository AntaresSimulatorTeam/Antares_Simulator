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

#include "antares/io/inputs/yml-system/converter.h"

#include <algorithm>
#include <sstream>

#include "antares/io/inputs/yml-system/system.h"
#include "antares/logs/logs.h"
#include "antares/study/system-model/connection.h"
#include "antares/study/system-model/system.h"

using namespace Antares::ModelerStudy;
using namespace Antares::ModelerStudy::SystemModel; // Mainly for type ConnexionEnd

namespace Antares::IO::Inputs::SystemConverter
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

static std::pair<std::string, std::string> splitLibraryModelString(const std::string& s)
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

static Model& getModel(const std::vector<Library>& libraries,
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

static Component createComponent(const YmlSystem::Component& c,
                                 const std::vector<Library>& libraries,
                                 unsigned int componentIndex)
{
    const auto [libraryId, modelId] = splitLibraryModelString(c.model);

    Model& model = getModel(libraries, libraryId, modelId);

    ComponentBuilder component_builder;

    std::map<std::string, ParameterTypeAndValue> parameters;
    for (const auto& [id, time_dependent, scenario_dependent, value]: c.parameters)
    {
        parameters.try_emplace(id,
                               ParameterTypeAndValue{.id = id,
                                                     .type = time_dependent
                                                               ? ParameterType::TIMESERIE
                                                               : ParameterType::CONSTANT,
                                                     .value = value});
    }

    auto component = component_builder.withId(c.id)
                       .withIndex(componentIndex)
                       .withModel(&model)
                       .withScenarioGroupId(c.scenarioGroup)
                       .withParameterValues(parameters)
                       .build();
    return component;
}

static Component& findComponent(const std::string& id, std::vector<Component>& components)

{
    const auto it = std::ranges::find_if(components,
                                         [&id](const Component& c) { return c.Id() == id; });
    if (it == components.end())
    {
        throw std::invalid_argument("Component with id '" + id + "' not found in system.");
    }
    return *it;
}

static const Port& findPort(const Component& component, const std::string& portId)
{
    const auto& ports = component.getModel()->Ports();
    const auto& it = ports.find(portId);
    if (it == ports.end())
    {
        throw std::invalid_argument("Port with id '" + portId + "' not found in component '"
                                    + component.Id() + "'.");
    }
    return it->second;
}

static void CheckPortSelfConnection(const std::string& firstComponentId,
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

static void CheckPortsType(const Port& firstPort, const Port& secondPort)
{
    if (firstPort.Type() != secondPort.Type())
    {
        std::ostringstream msg;
        msg << "Ports '" << firstPort.Id() << "' and '" << secondPort.Id()
            << "' are not of the same type!";
        throw std::invalid_argument(msg.str());
    }
}

static FieldRole ExposeFieldRole(const std::string& portId,
                                 const std::string& field,
                                 const PortFieldMap& portFieldDefinitions)
{
    const auto& it = portFieldDefinitions.find(PortFieldKey{.portId = portId, .fieldId = field});
    if (it == portFieldDefinitions.end())
    {
        return FieldRole::Receiver;
    }
    return FieldRole::Sender;
}

static std::pair<PortFieldsRole, PortFieldsRole> ResolveFieldsRole(const Component& firstComponent,
                                                                   const Port& firstPort,
                                                                   const Component& secondComponent,
                                                                   const Port& secondPort)
{
    PortFieldsRole firstPortFieldsRole;
    PortFieldsRole secondPortFieldsRole;

    const auto& firstPortDefs = firstComponent.getModel()->PortFieldDefinitions();
    const auto& secondPortDefs = secondComponent.getModel()->PortFieldDefinitions();
    for (const auto& field: firstPort.Type().Fields())
    {
        const auto firstPortFieldRole = ExposeFieldRole(firstPort.Id(), field.Id(), firstPortDefs);
        const auto secondPortFieldRole = ExposeFieldRole(secondPort.Id(),
                                                         field.Id(),
                                                         secondPortDefs);

        if (firstPortFieldRole == secondPortFieldRole)
        {
            std::ostringstream msg;
            msg << "Field '" << field.Id() << "' is " << firstPortFieldRole << " in both ports '"
                << firstPort.Id() << "' and '" << secondPort.Id() << "'";
            throw TwoFieldsOfSameRole(msg.str());
        }
        firstPortFieldsRole.emplace(field, firstPortFieldRole);
        secondPortFieldsRole.emplace(field, secondPortFieldRole);
    }
    return {std::move(firstPortFieldsRole), std::move(secondPortFieldsRole)};
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
static void connectComponents(const YmlSystem::Connection& connection,
                              std::vector<Component>& components)
{
    const auto& firstComponentId = connection.firstEntry.componentId;
    const auto& firstPortId = connection.firstEntry.portId;
    const auto& secondComponentId = connection.secondEntry.componentId;
    const auto& secondPortId = connection.secondEntry.portId;

    CheckPortSelfConnection(firstComponentId, firstPortId, secondComponentId, secondPortId);

    auto& firstComponent = findComponent(firstComponentId, components);
    const auto& firstPort = findPort(firstComponent, firstPortId);
    auto& secondComponent = findComponent(secondComponentId, components);
    const auto& secondPort = findPort(secondComponent, secondPortId);
    CheckPortsType(firstPort, secondPort);

    const auto [firstPortFieldsRole, secondPortFieldsRole] = ResolveFieldsRole(firstComponent,
                                                                               firstPort,
                                                                               secondComponent,
                                                                               secondPort);
    // TODO : Do we need to connect both components to one another ?
    // TODO : Or should we rather consider the field role and only connect receiver to the sender ?
    firstComponent.addComponentConnection(firstPort.Id(),
                                          ConnectionEnd(&secondComponent, &secondPort));
    secondComponent.addComponentConnection(secondPort.Id(),
                                           ConnectionEnd(&firstComponent, &firstPort));
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
static void connectAreas(const YmlSystem::AreaConnection& connection,
                         std::vector<Component>& components)
{
    // TODO : check that area exists in legacy study? seems complicated here
    auto& component = findComponent(connection.componentId, components);
    component.addAreaConnection(connection.portId, connection.areaId);
}

System convert(const YmlSystem::System& ymlSystem, const std::vector<Library>& libraries)
{
    // Create components from system
    std::vector<Component> components;
    unsigned int componentIndex = 0;
    for (const auto& c: ymlSystem.components)
    {
        auto it = std::ranges::find_if(std::as_const(components),
                                       [&c](const Component& compo) { return compo.Id() == c.id; });
        if (it != components.end())
        {
            throw std::invalid_argument("System has at least two components with the same id ('"
                                        + c.id + "'), this is not supported");
        }
        components.push_back(createComponent(c, libraries, componentIndex));
        ++componentIndex;
        logs.debug() << "Loaded component `" << c.id << "`";
    }

    // Create connections from system
    for (const auto& connection: ymlSystem.connections)
    {
        connectComponents(connection, components);
        logs.debug() << "Loaded connection (component1=`" << connection.firstEntry.componentId
                     << "` component2=`" << connection.secondEntry.componentId << "`)";
    }

    // Create area connections from system
    for (const auto& connection: ymlSystem.areaConnections)
    {
        connectAreas(connection, components);
        logs.debug() << "Loaded area connection (component=`" << connection.componentId
                     << "` area=`" << connection.areaId << "`)";
    }

    // Build system from components and connections
    SystemBuilder builder;
    return builder.withId(ymlSystem.id).withComponents(std::move(components)).build();
}

} // namespace Antares::IO::Inputs::SystemConverter
