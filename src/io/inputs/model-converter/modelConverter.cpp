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

#include "antares/io/inputs/model-converter/modelConverter.h"

#include <antares/expressions/iterators/pre-order.h>
#include <antares/expressions/nodes/PortFieldNode.h>
#include "antares/expressions/expression.h"
#include "antares/io/inputs/model-converter/convertorVisitor.h"
#include "antares/study/system-model/constraint.h"
#include "antares/study/system-model/library.h"
#include "antares/study/system-model/model.h"
#include "antares/study/system-model/parameter.h"
#include "antares/study/system-model/port.h"
#include "antares/study/system-model/portType.h"
#include "antares/study/system-model/variable.h"

namespace Antares::IO::Inputs::ModelConverter
{
UnknownTypeException::UnknownTypeException(const std::string& type):
    std::runtime_error("Unknown variable type: " + type)
{
}

PortTypeDoesntContainsFields::PortTypeDoesntContainsFields(const std::string& id):
    std::runtime_error("This port type doesn't contains fields: " + id)
{
}

PortWithThisIdAlreadyExists::PortWithThisIdAlreadyExists(const std::string& id):
    std::runtime_error("Port with this id already exists: " + id)
{
}

PortTypeWithThisIdAlreadyExists::PortTypeWithThisIdAlreadyExists(const std::string& id):
    std::runtime_error("Port type with this id already exists: " + id)
{
}

ConstraintWithThisIdAlreadyExists::ConstraintWithThisIdAlreadyExists(const std::string& id):
    std::runtime_error("Constraint with this id already exists: " + id)
{
}

PortTypeNotFound::PortTypeNotFound(const std::string& portId, const std::string& portTypeId):
    std::runtime_error("For the port: " + portId + " , port type not found: " + portTypeId)
{
}

PortNotFoundForDefinition::PortNotFoundForDefinition(const std::string& portId):
    std::runtime_error("In port-field-definitions, port not found: " + portId)
{
}

FieldNotFoundForDefinition::FieldNotFoundForDefinition(const std::string& portId,
                                                       const std::string& fieldId):
    std::runtime_error("In port-field-definitions, for port: " + portId
                       + " , field not found: " + fieldId)
{
}

PortInDefinition::PortInDefinition(const std::string& portId, const std::string& portInDefId):
    std::runtime_error("In port-field-definitions, for port: " + portId
                       + " , found another port in the definition: " + portInDefId)
{
}

/// Convert portTypes to Antares::Study::SystemModel::PortType
std::vector<Study::SystemModel::PortType> convertTypes(const IO::Inputs::YmlModel::Library& library)
{
    std::vector<Study::SystemModel::PortType> out;
    out.reserve(library.port_types.size());
    for (const auto& portType: library.port_types)
    {
        if (portType.fields.empty()) // Can't have a port type without fields
        {
            throw PortTypeDoesntContainsFields(portType.id);
        }
        std::vector<Study::SystemModel::PortField> fields;
        for (const auto& field: portType.fields)
        {
            fields.emplace_back(field);
        }

        // Can't have port types with the same ID
        if (std::ranges::find_if(out, [&portType](const auto& p) { return p.Id() == portType.id; })
            != out.end())
        {
            throw PortTypeWithThisIdAlreadyExists(portType.id);
        }

        Study::SystemModel::PortType portTypeModel(portType.id, std::move(fields));
        out.emplace_back(std::move(portTypeModel));
    }
    return out;
}

/**
 * \brief Converts parameters from YmlModel::Model to SystemModel::Parameter.
 *
 * \param model The YmlModel::Model object containing parameters.
 * \return A vector of SystemModel::Parameter objects.
 */
std::vector<Study::SystemModel::Parameter> convertParameters(
  const IO::Inputs::YmlModel::Model& model)
{
    namespace SM = Study::SystemModel;
    std::vector<SM::Parameter> parameters;
    parameters.reserve(model.parameters.size());
    for (const auto& parameter: model.parameters)
    {
        parameters.emplace_back(parameter.id,
                                SM::fromBool<SM::TimeDependent>(parameter.time_dependent),
                                SM::fromBool<SM::ScenarioDependent>(parameter.scenario_dependent));
    }
    return parameters;
}

/**
 * \brief Converts a YmlModel::ValueType to an SystemModel::ValueType.
 *
 * \param type The YmlModel::ValueType to convert.
 * \return The corresponding SystemModel::ValueType.
 * \throws UnknownType if the type is unknown.
 */
Study::SystemModel::ValueType convertType(IO::Inputs::YmlModel::ValueType type)
{
    using namespace std::string_literals;
    switch (type)
    {
    case IO::Inputs::YmlModel::ValueType::CONTINUOUS:
        return Study::SystemModel::ValueType::FLOAT;
    case IO::Inputs::YmlModel::ValueType::INTEGER:
        return Study::SystemModel::ValueType::INTEGER;
    case IO::Inputs::YmlModel::ValueType::BOOL:
        return Study::SystemModel::ValueType::BOOL;
    default:
        throw UnknownTypeException(IO::Inputs::YmlModel::toString(type));
    }
}

/**
 * \brief Converts variables from YmlModel::Model to SystemModel::Variable.
 *
 * \param model The YmlModel::Model object containing variables.
 * \return A vector of SystemModel::Variable objects.
 */
std::vector<Study::SystemModel::Variable> convertVariables(const YmlModel::Model& model)
{
    namespace SM = Antares::Study::SystemModel;

    std::vector<SM::Variable> variables;
    variables.reserve(model.variables.size());
    for (const auto& variable: model.variables)
    {
        SM::Expression lb(variable.lower_bound,
                          convertExpressionToNode(variable.lower_bound, model));
        SM::Expression ub(variable.upper_bound,
                          convertExpressionToNode(variable.upper_bound, model));
        variables.emplace_back(variable.id,
                               std::move(lb),
                               std::move(ub),
                               convertType(variable.variable_type),
                               SM::fromBool<SM::TimeDependent>(variable.time_dependent),
                               SM::fromBool<SM::ScenarioDependent>(variable.scenario_dependent));
    }

    return variables;
}

/**
 * \brief Converts ports from YmlModel::Model to SystemModel::Port.
 *
 * \param model The YmlModel::Model object containing ports.
 * \return A vector of SystemModel::Port objects.
 */
std::vector<Study::SystemModel::Port> convertPorts(
  const IO::Inputs::YmlModel::Model& model,
  const std::vector<Study::SystemModel::PortType>& portTypes)
{
    std::vector<Study::SystemModel::Port> ports;
    ports.reserve(model.ports.size());
    for (const auto& port: model.ports)
    {
        // Can't have port with the same ID
        if (std::ranges::find_if(ports, [&port](const auto& p) { return p.Id() == port.id; })
            != ports.end())
        {
            throw PortWithThisIdAlreadyExists(port.id);
        }

        const auto it = std::ranges::find_if(portTypes,
                                             [&port](const auto& pt)
                                             { return pt.Id() == port.type; });
        if (it == portTypes.end())
        {
            throw PortTypeNotFound(port.id, port.type);
        }
        ports.emplace_back(port.id, *it);
    }
    return ports;
}

/**
 * \brief Converts PortFieldDefinition from YmlModel::Model to SystemModel::PortFieldDefinition.
 *
 * \param model The YmlModel::Model object containing port field definitions.
 * \return A vector of SystemModel::PortFieldDefinition objects.
 */
std::vector<Study::SystemModel::PortFieldDefinition> convertPortFieldDefinitions(
  const IO::Inputs::YmlModel::Model& model,
  const std::vector<Study::SystemModel::Port>& ports,
  const std::vector<Study::SystemModel::PortType>& portTypes)
{
    std::vector<Study::SystemModel::PortFieldDefinition> portFieldDefinitions;
    portFieldDefinitions.reserve(model.port_field_definitions.size());
    for (const auto& pfdefinition: model.port_field_definitions)
    {
        // first check if the port exists
        auto itPort = std::ranges::find_if(ports,
                                           [&pfdefinition](const auto& p)
                                           { return p.Id() == pfdefinition.port; });
        if (itPort == ports.end())
        {
            throw PortNotFoundForDefinition(pfdefinition.port);
        }

        // second check if the field exists in type
        auto itType = std::ranges::find_if(portTypes,
                                           [&itPort](const auto& pt)
                                           { return pt.Id() == itPort->Type().Id(); });
        auto itField = std::ranges::find_if(itType->Fields(),
                                            [&pfdefinition](const auto& field)
                                            { return field.Id() == pfdefinition.field; });
        if (itField == itType->Fields().end())
        {
            throw FieldNotFoundForDefinition(pfdefinition.port, pfdefinition.field);
        }

        auto nodeRegistry = convertExpressionToNode(pfdefinition.definition, model);

        using namespace Antares::Expressions::Nodes;
        AST preorder(nodeRegistry.node);
        auto it = std::find_if(preorder.begin(),
                               preorder.end(),
                               [](const Node& node) { return node.name() == "PortFieldNode"; });
        if (it != preorder.end())
        {
            throw PortInDefinition(pfdefinition.port,
                                   dynamic_cast<PortFieldNode*>(&*it)->getPortName());
        }

        portFieldDefinitions.emplace_back(*itPort,
                                          *itField,
                                          Study::SystemModel::Expression(pfdefinition.definition,
                                                                         std::move(nodeRegistry)));
    }
    return portFieldDefinitions;
}

static void addSingleConstraint(std::vector<Study::SystemModel::Constraint>& constraints,
                                const IO::Inputs::YmlModel::Constraint& constraint,
                                const IO::Inputs::YmlModel::Model& model)
{
    // Can't have constraints with the same ID
    if (std::ranges::find_if(constraints,
                             [&constraint](const auto& c) { return c.Id() == constraint.id; })
        != constraints.end())
    {
        throw ConstraintWithThisIdAlreadyExists(constraint.id);
    }

    auto nodeRegistry = convertExpressionToNode(constraint.expression, model);
    constraints.emplace_back(constraint.id,
                             Study::SystemModel::Expression{constraint.expression,
                                                            std::move(nodeRegistry)});
}

/**
 * \brief Converts constraints from YmlModel::Model to SystemModel::Constraint.
 *
 * \param model The YmlModel::Model object containing constraints.
 * \return A vector of SystemModel::Constraint objects.
 */
std::vector<Study::SystemModel::Constraint> convertConstraints(
  const IO::Inputs::YmlModel::Model& model)
{
    std::vector<Study::SystemModel::Constraint> constraints;
    constraints.reserve(model.constraints.size());

    for (const auto& constraint: model.constraints)
    {
        addSingleConstraint(constraints, constraint, model);
    }

    for (const auto& constraint: model.binding_constraints)
    {
        addSingleConstraint(constraints, constraint, model);
    }
    return constraints;
}

/**
 * \brief Converts models from YmlModel::Library to SystemModel::Model.
 *
 * \param library The YmlModel::Library object containing models.
 * \return A vector of SystemModel::Model objects.
 */
std::vector<Study::SystemModel::Model> convertModels(
  const IO::Inputs::YmlModel::Library& library,
  const std::vector<Study::SystemModel::PortType>& portTypes)
{
    std::vector<Study::SystemModel::Model> models;
    models.reserve(library.models.size());
    for (const auto& model: library.models)
    {
        Study::SystemModel::ModelBuilder modelBuilder;
        std::vector<Study::SystemModel::Parameter> parameters = convertParameters(model);
        std::vector<Study::SystemModel::Variable> variables = convertVariables(model);
        std::vector<Study::SystemModel::Port> ports = convertPorts(model, portTypes);
        std::vector<Study::SystemModel::PortFieldDefinition>
          portFieldDefinitions = convertPortFieldDefinitions(model, ports, portTypes);
        std::vector<Study::SystemModel::Constraint> constraints = convertConstraints(model);

        auto nodeObjective = convertExpressionToNode(model.objective, model);

        auto modelObj = modelBuilder.withId(model.id)
                          .withObjective(Study::SystemModel::Expression{model.objective,
                                                                        std::move(nodeObjective)})
                          .withParameters(std::move(parameters))
                          .withVariables(std::move(variables))
                          .withPorts(std::move(ports))
                          .withConstraints(std::move(constraints))
                          .withPortFieldDefinitions(std::move(portFieldDefinitions))
                          .build();
        models.emplace_back(std::move(modelObj));
    }
    return models;
}

/**
 * \brief Converts a YmlModel::Library object to an SystemModel::Library object.
 *
 * \param library The YmlModel::Library object to convert.
 * \return The corresponding SystemModel::Library object.
 */
Study::SystemModel::Library convert(const IO::Inputs::YmlModel::Library& library)
{
    Study::SystemModel::LibraryBuilder builder;
    std::vector<Study::SystemModel::PortType> portTypes = convertTypes(library);
    std::vector<Study::SystemModel::Model> models = convertModels(library, portTypes);
    Study::SystemModel::Library lib = builder.withId(library.id)
                                        .withDescription(library.description)
                                        .withPortTypes(std::move(portTypes))
                                        .withModels(std::move(models))
                                        .build();
    return lib;
}
} // namespace Antares::IO::Inputs::ModelConverter
