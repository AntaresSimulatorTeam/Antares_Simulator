/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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
#include "antares/study/system-model/system.h"

using namespace Antares::ModelerStudy;

namespace Antares::IO::Inputs::SystemConverter
{

class ErrorWhileSplittingLibraryAndModel: public std::runtime_error
{
public:
    explicit ErrorWhileSplittingLibraryAndModel(const std::string& s):
        runtime_error("'.' not found while splitting library and model: " + s)
    {
    }
};

class LibraryNotFound: public std::runtime_error
{
public:
    explicit LibraryNotFound(const std::string& s):
        runtime_error("No library found with this name: " + s)
    {
    }
};

class ModelNotFound: public std::runtime_error
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

static const SystemModel::Model& getModel(const std::vector<SystemModel::Library>& libraries,
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

static SystemModel::Component createComponent(const YmlSystem::Component& c,
                                              const std::vector<SystemModel::Library>& libraries)
{
    const auto [libraryId, modelId] = splitLibraryModelString(c.model);
    SystemModel::ModelBuilder model_builder;

    const SystemModel::Model& model = getModel(libraries, libraryId, modelId);

    SystemModel::ComponentBuilder component_builder;

    std::map<std::string, Expressions::Visitors::ParameterTypeAndValue> parameters;
    for (const auto& [id, time_dependent, scenario_dependent, value]: c.parameters)
    {
        parameters.try_emplace(id,
                               Expressions::Visitors::ParameterTypeAndValue{
                                 .id = id,
                                 .type = time_dependent
                                           ? Expressions::Visitors::ParameterType::TIMESERIE
                                           : Expressions::Visitors::ParameterType::CONSTANT,
                                 .value = value});
    }

    auto component = component_builder.withId(c.id)
                       .withModel(&model)
                       .withScenarioGroupId(c.scenarioGroup)
                       .withParameterValues(parameters)
                       .build();
    return component;
}

static const SystemModel::Component& findComponent(
  const std::string& id,
  const std::unordered_map<std::string, SystemModel::Component>& components)

{
    const auto& it = components.find(id);
    if (it == components.end())
    {
        throw std::invalid_argument("Component with id '" + id + "' not found in system.");
    }
    return it->second;
}

static const SystemModel::Port& findPort(const SystemModel::Component& component,
                                         const std::string& portId)
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

static void CheckPortsType(const SystemModel::Port& firstPort, const SystemModel::Port& secondPort)
{
    if (firstPort.Type() != secondPort.Type())
    {
        std::ostringstream msg;
        msg << "Ports '" << firstPort.Id() << "' and '" << secondPort.Id()
            << "' are not of the same type!";
        throw std::invalid_argument(msg.str());
    }
}

static SystemModel::FieldRole ExposeFieldRole(const std::string& portId,
                                              const std::string& field,
                                              const SystemModel::PortFieldMap& portFieldDefinitions)
{
    const auto& it = portFieldDefinitions.find(
      SystemModel::PortFieldKey{.portId = portId, .fieldId = field});
    if (it == portFieldDefinitions.end())
    {
        return SystemModel::FieldRole::Receiver;
    }
    return SystemModel::FieldRole::Sender;
}

static std::pair<SystemModel::PortFieldsRole, SystemModel::PortFieldsRole> ResolveFieldsRole(
  const SystemModel::Component& firstComponent,
  const SystemModel::Port& firstPort,
  const SystemModel::Component& secondComponent,
  const SystemModel::Port& secondPort)
{
    SystemModel::PortFieldsRole firstPortFieldsRole;
    SystemModel::PortFieldsRole secondPortFieldsRole;

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
 * @brief Creates a SystemModel::Connection from a YmlSystem::Connection and a map of components.
 *
 * This function constructs a SystemModel::Connection by looking up components and ports
 * based on the IDs provided in the YmlSystem::Connection. It ensures that the ports are
 * of the same type and that fields are correctly configured for sending and receiving.
 *
 * @param connection A YmlSystem::Connection object containing the connection details.
 * @param components An unordered map of component IDs to SystemModel::Component objects.
 *
 * @return A SystemModel::Connection object representing the created connection.
 *
 * @throw std::invalid_argument if a component or port is not found, if the ports are not
 *        of the same type, or if fields are incorrectly configured for sending/receiving.
 */
static SystemModel::Connection createConnection(
  const YmlSystem::Connection& connection,
  const std::unordered_map<std::string, SystemModel::Component>& components)
{
    const auto& firstComponentId = connection.firstEntry.componentId;
    const auto& firstPortId = connection.firstEntry.portId;
    const auto& secondComponentId = connection.secondEntry.componentId;
    const auto& secondPortId = connection.secondEntry.portId;

    CheckPortSelfConnection(firstComponentId, firstPortId, secondComponentId, secondPortId);

    const auto& first_component = findComponent(firstComponentId, components);
    const auto& firstPort = findPort(first_component, firstPortId);
    const auto& secondComponent = findComponent(secondComponentId, components);
    const auto& secondPort = findPort(secondComponent, secondPortId);
    CheckPortsType(firstPort, secondPort);

    const auto [firstPortFieldsRole, secondPortFieldsRole] = ResolveFieldsRole(first_component,
                                                                               firstPort,
                                                                               secondComponent,
                                                                               secondPort);

    return {
      SystemModel::ConnectionEntry(&first_component, &firstPort, std::move(firstPortFieldsRole)),
      SystemModel::ConnectionEntry(&secondComponent, &secondPort, std::move(secondPortFieldsRole))};
}

SystemModel::System convert(const YmlSystem::System& ymlSystem,
                            const std::vector<SystemModel::Library>& libraries)
{
    std::unordered_map<std::string, SystemModel::Component> components;
    for (const auto& c: ymlSystem.components)
    {
        if (components.contains(c.id))
        {
            throw std::invalid_argument("System has at least two components with the same id ('"
                                        + c.id + "'), this is not supported");
        }
        components.emplace(c.id, createComponent(c, libraries));
    }
    std::vector<SystemModel::Connection> connections;
    connections.reserve(ymlSystem.connections.size());
    for (const auto& connection: ymlSystem.connections)
    {
        connections.push_back(createConnection(connection, components));
    }
    SystemModel::SystemBuilder builder;
    return builder.withId(ymlSystem.id)
      .withComponents(std::move(components))
      .withConnections(std::move(connections))
      .build();
}

} // namespace Antares::IO::Inputs::SystemConverter
