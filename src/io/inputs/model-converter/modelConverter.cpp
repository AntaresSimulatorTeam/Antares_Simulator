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

#include "antares/io/inputs/model-converter/modelConverter.h"

#include <antares/expressions/iterators/pre-order.h>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include "antares/expressions/expression.h"
#include "antares/io/inputs/model-converter/NodeChecker.h"
#include "antares/io/inputs/model-converter/convertorVisitor.h"
#include "antares/study/system-model/constraint.h"
#include "antares/study/system-model/library.h"
#include "antares/study/system-model/parameter.h"
#include "antares/study/system-model/port.h"
#include "antares/study/system-model/portType.h"
#include "antares/study/system-model/variable.h"
using namespace Antares::Expressions::Nodes;

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

PortTypeWithThisIdAlreadyExists::PortTypeWithThisIdAlreadyExists(const std::string& id):
    std::runtime_error("Port type with this id already exists: " + id)
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

static void CommonPreSolve(ForbiddenNodes& f)
{
    // constraint, objective and variable bounds should not contain dual or reduced_cost
    f.addGlobalForbidden<FunctionNodeType::reduced_cost, FunctionNodeType::dual>();
    // Forbid VariableNode, PortFieldNode, and PortFieldSumNode in max and min
    f.addForbiddenFor<FunctionNodeType::max, VariableNode>();
    f.addForbiddenFor<FunctionNodeType::min, VariableNode>();
    f.addForbiddenFor<FunctionNodeType::max, PortFieldNode>();
    f.addForbiddenFor<FunctionNodeType::min, PortFieldNode>();
    f.addForbiddenFor<FunctionNodeType::max, PortFieldSumNode>();
    f.addForbiddenFor<FunctionNodeType::min, PortFieldSumNode>();
}

static ForbiddenNodes ForbiddenInConstraint()
{
    static ForbiddenNodes forbidden = []()
    {
        // Initialization code executed ONCE
        ForbiddenNodes f;
        CommonPreSolve(f);
        f.addGlobalForbidden<PortFieldSumNode>();
        return f;
    }();
    return forbidden;
}

static ForbiddenNodes ForbiddenInBindingConstraint()
{
    static ForbiddenNodes forbidden = []()
    {
        // Initialization code executed ONCE
        ForbiddenNodes f;
        CommonPreSolve(f);
        return f;
    }();
    return forbidden;
}

static ForbiddenNodes PreSolveNonConstraint()
{
    static ForbiddenNodes forbidden = []()
    {
        // Initialization code executed ONCE
        ForbiddenNodes f;
        CommonPreSolve(f);
        f.addGlobalForbidden<ComparisonNode,
                             EqualNode,
                             LessThanOrEqualNode,
                             GreaterThanOrEqualNode,
                             PortFieldSumNode>();
        return f;
    }();
    return forbidden;
}

static ForbiddenNodes ForbiddenInExtraOutput()
{
    static ForbiddenNodes forbidden = []()
    {
        // Initialization code executed ONCE
        ForbiddenNodes f;
        // TODO check        //   f.addGlobalForbidden<PortFieldSumNode>();
        return f;
    }();
    return forbidden;
}

/// Convert portTypes to Antares::ModelerStudy::SystemModel::PortType
std::vector<ModelerStudy::SystemModel::PortType> convertTypes(
  const IO::Inputs::YmlModel::Library& library)
{
    std::vector<ModelerStudy::SystemModel::PortType> out;
    out.reserve(library.port_types.size());
    for (const auto& portType: library.port_types)
    {
        if (portType.fields.empty()) // Can't have a port type without fields
        {
            throw PortTypeDoesntContainsFields(portType.id);
        }
        std::vector<ModelerStudy::SystemModel::PortField> fields;
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

        ModelerStudy::SystemModel::PortType portTypeModel(portType.id,
                                                          std::move(fields),
                                                          portType.area_connection_injection_field);
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
std::vector<ModelerStudy::SystemModel::Parameter> convertParameters(
  const IO::Inputs::YmlModel::Model& model)
{
    namespace SM = ModelerStudy::SystemModel;
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

Modeler::Config::Location convertLocation(const std::string& locationStr)
{
    std::string locLower = locationStr;
    std::ranges::transform(locLower, locLower.begin(), ::tolower);
    if (locLower == "master")
    {
        return Modeler::Config::Location::MASTER;
    }
    if (locLower == "master-and-subproblems")
    {
        return Modeler::Config::Location::MASTER_AND_SUBPROBLEMS;
    }
    if (locLower == "subproblems")
    {
        return Modeler::Config::Location::SUBPROBLEMS;
    }

    throw std::runtime_error("Unknown location: " + locationStr);
}

/**
 * \brief Converts a YmlModel::ValueType to an SystemModel::ValueType.
 *
 * \param type The YmlModel::ValueType to convert.
 * \return The corresponding SystemModel::ValueType.
 * \throws UnknownType if the type is unknown.
 */
ModelerStudy::SystemModel::ValueType convertType(IO::Inputs::YmlModel::ValueType type)
{
    using namespace std::string_literals;
    switch (type)
    {
    case IO::Inputs::YmlModel::ValueType::CONTINUOUS:
        return ModelerStudy::SystemModel::ValueType::FLOAT;
    case IO::Inputs::YmlModel::ValueType::INTEGER:
        return ModelerStudy::SystemModel::ValueType::INTEGER;
    case IO::Inputs::YmlModel::ValueType::BOOL:
        return ModelerStudy::SystemModel::ValueType::BOOL;
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
std::vector<ModelerStudy::SystemModel::Variable> convertVariables(const YmlModel::Model& model)
{
    namespace SM = Antares::ModelerStudy::SystemModel;

    std::vector<SM::Variable> variables;
    variables.reserve(model.variables.size());
    for (const auto& variable: model.variables)
    {
        const auto& whatIsForbiddenInVariableBound = PreSolveNonConstraint();
        SM::Expression lb(variable.lower_bound,
                          convertExpressionToNode(variable.lower_bound, model));
        if (lb.RootNode())
        {
            NodeChecker(whatIsForbiddenInVariableBound, variable.lower_bound)
              .dispatch(lb.RootNode());
        }
        SM::Expression ub(variable.upper_bound,
                          convertExpressionToNode(variable.upper_bound, model));
        if (ub.RootNode())
        {
            NodeChecker(whatIsForbiddenInVariableBound, variable.upper_bound)
              .dispatch(ub.RootNode());
        }
        variables.emplace_back(variable.id,
                               std::move(lb),
                               std::move(ub),
                               convertType(variable.variable_type),
                               SM::fromBool<SM::TimeDependent>(variable.time_dependent),
                               SM::fromBool<SM::ScenarioDependent>(variable.scenario_dependent),
                               convertLocation(variable.location));
    }

    return variables;
}

/**
 * \brief Converts ports from YmlModel::Model to SystemModel::Port.
 *
 * \param model The YmlModel::Model object containing ports.
 * \return A vector of SystemModel::Port objects.
 */
std::vector<ModelerStudy::SystemModel::Port> convertPorts(
  const IO::Inputs::YmlModel::Model& model,
  const std::vector<ModelerStudy::SystemModel::PortType>& portTypes)
{
    std::vector<ModelerStudy::SystemModel::Port> ports;
    ports.reserve(model.ports.size());
    for (const auto& port: model.ports)
    {
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
std::vector<ModelerStudy::SystemModel::PortFieldDefinition> convertPortFieldDefinitions(
  const IO::Inputs::YmlModel::Model& model,
  const std::vector<ModelerStudy::SystemModel::Port>& ports)
{
    std::vector<ModelerStudy::SystemModel::PortFieldDefinition> portFieldDefinitions;
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
        const auto& portFields = itPort->Type().Fields();
        auto itField = std::ranges::find_if(portFields,
                                            [&pfdefinition](const auto& field)
                                            { return field.Id() == pfdefinition.field; });
        if (itField == portFields.end())
        {
            throw FieldNotFoundForDefinition(pfdefinition.port, pfdefinition.field);
        }

        auto nodeRegistry = convertExpressionToNode(pfdefinition.definition, model);

        using namespace Antares::Expressions::Nodes;
        AST preorder(nodeRegistry.node);
        auto it = std::find_if(preorder.begin(),
                               preorder.end(),
                               [](const Node& node)
                               { return dynamic_cast<const PortFieldNode*>(&node) != nullptr; });
        if (it != preorder.end())
        {
            throw PortInDefinition(pfdefinition.port,
                                   dynamic_cast<const PortFieldNode&>(*it).getPortName());
        }
        NodeChecker(PreSolveNonConstraint(), pfdefinition.definition).dispatch(nodeRegistry.node);
        portFieldDefinitions.emplace_back(
          *itPort,
          *itField,
          ModelerStudy::SystemModel::Expression(pfdefinition.definition, std::move(nodeRegistry)));
    }
    return portFieldDefinitions;
}

static void addSingleConstraint(std::vector<ModelerStudy::SystemModel::Constraint>& constraints,
                                const IO::Inputs::YmlModel::Constraint& constraint,
                                const IO::Inputs::YmlModel::Model& model,
                                const ForbiddenNodes& forbiddenNodes)
{
    auto nodeRegistry = convertExpressionToNode(constraint.expression, model);
    NodeChecker(forbiddenNodes, constraint.expression).dispatch(nodeRegistry.node);
    constraints.emplace_back(constraint.id,
                             ModelerStudy::SystemModel::Expression{constraint.expression,
                                                                   std::move(nodeRegistry)},
                             convertLocation(constraint.location));
}

/**
 * \brief Converts constraints from YmlModel::Model to SystemModel::Constraint.
 *
 * \param model The YmlModel::Model object containing constraints.
 * \return A vector of SystemModel::Constraint objects.
 */
std::vector<ModelerStudy::SystemModel::Constraint> convertConstraints(
  const IO::Inputs::YmlModel::Model& model)
{
    std::vector<ModelerStudy::SystemModel::Constraint> constraints;
    constraints.reserve(model.constraints.size());

    for (const auto& constraint: model.constraints)
    {
        addSingleConstraint(constraints, constraint, model, ForbiddenInConstraint());
    }

    for (const auto& constraint: model.binding_constraints)
    {
        addSingleConstraint(constraints, constraint, model, ForbiddenInBindingConstraint());
    }
    return constraints;
}

/**
 * \brief Converts extra outputs from YmlModel::Model to SystemModel::ExtraOutput.
 *
 * \param model The YmlModel::Model object containing extra outputs.
 * \return A vector of SystemModel::ExtraOutput objects.
 */
std::vector<ModelerStudy::SystemModel::ExtraOutput> convertExtraOutputs(
  const IO::Inputs::YmlModel::Model& model)
{
    std::vector<ModelerStudy::SystemModel::ExtraOutput> extraOutputs;
    extraOutputs.reserve(model.extra_outputs.size());

    for (const auto& extraOutput: model.extra_outputs)
    {
        auto nodeRegistry = convertExpressionToNode(extraOutput.expression, model);
        NodeChecker(ForbiddenInExtraOutput(), extraOutput.expression).dispatch(nodeRegistry.node);
        extraOutputs.emplace_back(extraOutput.id,
                                  ModelerStudy::SystemModel::Expression{extraOutput.expression,
                                                                        std::move(nodeRegistry)});
    }
    return extraOutputs;
}

/**
 * \brief Converts objectives from YmlModel::Model to SystemModel::Expression.
 *
 * \param model The YmlModel::Model object containing objectives.
 * \return A vector of SystemModel::Expression objects.
 */
std::vector<ModelerStudy::SystemModel::Objective> convertObjectives(
  const IO::Inputs::YmlModel::Model& model)
{
    std::vector<ModelerStudy::SystemModel::Objective> objectives;
    objectives.reserve(model.objectives.size());
    for (const auto& objective: model.objectives)
    {
        auto nodeRegistry = convertExpressionToNode(objective.expression, model);
        NodeChecker(PreSolveNonConstraint(), objective.expression).dispatch(nodeRegistry.node);
        objectives.emplace_back(objective.id,
                                ModelerStudy::SystemModel::Expression{objective.expression,
                                                                      std::move(nodeRegistry)},
                                convertLocation(objective.location));
    }
    return objectives;
}

/**
 * \brief Converts models from YmlModel::Library to SystemModel::Model.
 *
 * \param library The YmlModel::Library object containing models.
 * \return A vector of SystemModel::Model objects.
 */
std::vector<ModelerStudy::SystemModel::Model> convertModels(
  const IO::Inputs::YmlModel::Library& library,
  const std::vector<ModelerStudy::SystemModel::PortType>& portTypes)
{
    std::vector<ModelerStudy::SystemModel::Model> models;
    models.reserve(library.models.size());
    for (const auto& model: library.models)
    {
        ModelerStudy::SystemModel::ModelBuilder modelBuilder;
        std::vector<ModelerStudy::SystemModel::Parameter> parameters = convertParameters(model);
        std::vector<ModelerStudy::SystemModel::Variable> variables = convertVariables(model);
        std::vector<ModelerStudy::SystemModel::Port> ports = convertPorts(model, portTypes);
        std::vector<ModelerStudy::SystemModel::PortFieldDefinition>
          portFieldDefinitions = convertPortFieldDefinitions(model, ports);
        std::vector<ModelerStudy::SystemModel::Constraint> constraints = convertConstraints(model);
        std::vector<ModelerStudy::SystemModel::ExtraOutput> extraOutputs = convertExtraOutputs(
          model);
        std::vector<ModelerStudy::SystemModel::Objective> objectives = convertObjectives(model);

        auto modelObj = modelBuilder.withId(model.id)
                          .withObjectives(std::move(objectives))
                          .withParameters(std::move(parameters))
                          .withVariables(std::move(variables))
                          .withPorts(std::move(ports))
                          .withConstraints(std::move(constraints))
                          .withPortFieldDefinitions(std::move(portFieldDefinitions))
                          .withExtraOutputs(std::move(extraOutputs))
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
ModelerStudy::SystemModel::Library convert(const IO::Inputs::YmlModel::Library& library)
{
    std::vector<ModelerStudy::SystemModel::PortType> portTypes = convertTypes(library);
    std::vector<ModelerStudy::SystemModel::Model> models = convertModels(library, portTypes);

    ModelerStudy::SystemModel::LibraryBuilder builder;
    ModelerStudy::SystemModel::Library lib = builder.withId(library.id)
                                               .withDescription(library.description)
                                               .withPortTypes(std::move(portTypes))
                                               .withModels(std::move(models))
                                               .build();
    return lib;
}
} // namespace Antares::IO::Inputs::ModelConverter
