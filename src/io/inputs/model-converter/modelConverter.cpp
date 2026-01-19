// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/model-converter/modelConverter.h"

#include <antares/expressions/iterators/pre-order.h>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include "antares/expressions/expression.h"
#include "antares/io/inputs/model-converter/ForbiddenNodesVisitor.h"
#include "antares/io/inputs/model-converter/convertorVisitor.h"
#include "antares/study/system-model/constraint.h"
#include "antares/study/system-model/library.h"
#include "antares/study/system-model/parameter.h"
#include "antares/study/system-model/port.h"
#include "antares/study/system-model/portType.h"
#include "antares/study/system-model/variable.h"

using namespace Antares::Expressions::Nodes;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::IO::Inputs;

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
    f.forbidGlobally<FunctionNodeType::reduced_cost, FunctionNodeType::dual>();

    // Forbid VariableNode, PortFieldNode, and PortFieldSumNode in max(...)
    f.parentForbidsChild<FunctionNodeType::max, VariableNode>();
    f.parentForbidsChild<FunctionNodeType::max, PortFieldNode>();
    f.parentForbidsChild<FunctionNodeType::max, PortFieldSumNode>();

    // Forbid VariableNode, PortFieldNode, and PortFieldSumNode in min(...)
    f.parentForbidsChild<FunctionNodeType::min, VariableNode>();
    f.parentForbidsChild<FunctionNodeType::min, PortFieldNode>();
    f.parentForbidsChild<FunctionNodeType::min, PortFieldSumNode>();

    // Forbid VariableNode, PortFieldNode, and PortFieldSumNode in floor(node)
    f.parentForbidsChild<FunctionNodeType::floor, VariableNode>();
    f.parentForbidsChild<FunctionNodeType::floor, PortFieldNode>();
    f.parentForbidsChild<FunctionNodeType::floor, PortFieldSumNode>();

    // Forbid VariableNode, PortFieldNode, and PortFieldSumNode in ceil(node)
    f.parentForbidsChild<FunctionNodeType::ceil, VariableNode>();
    f.parentForbidsChild<FunctionNodeType::ceil, PortFieldNode>();
    f.parentForbidsChild<FunctionNodeType::ceil, PortFieldSumNode>();
}

static ForbiddenNodes ForbiddenInConstraint()
{
    static ForbiddenNodes forbidden = []()
    {
        // Initialization code executed ONCE
        ForbiddenNodes f;
        CommonPreSolve(f);
        f.forbidGlobally<PortFieldSumNode>();
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
        f.forbidGlobally<ComparisonNode,
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
        // TODO check        //   f.forbidGlobally<PortFieldSumNode>();
        return f;
    }();
    return forbidden;
}

std::vector<PortType> convertTypes(const ::YmlModel::Library& library)
{
    std::vector<PortType> out;
    out.reserve(library.port_types.size());
    for (const auto& portType: library.port_types)
    {
        if (portType.fields.empty()) // Can't have a port type without fields
        {
            throw PortTypeDoesntContainsFields(portType.id);
        }
        std::vector<PortField> fields;
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

        PortType portTypeModel(portType.id,
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
std::vector<Parameter> convertParameters(const YmlModel::Model& model)
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
ValueType convertType(YmlModel::ValueType type)
{
    using namespace std::string_literals;
    switch (type)
    {
    case YmlModel::ValueType::CONTINUOUS:
        return ValueType::FLOAT;
    case YmlModel::ValueType::INTEGER:
        return ValueType::INTEGER;
    case YmlModel::ValueType::BOOL:
        return ValueType::BOOL;
    default:
        throw UnknownTypeException(YmlModel::toString(type));
    }
}

/**
 * \brief Converts variables from YmlModel::Model to SystemModel::Variable.
 *
 * \param model The YmlModel::Model object containing variables.
 * \return A vector of SystemModel::Variable objects.
 */
std::vector<Variable> convertVariables(const YmlModel::Model& model)
{
    std::vector<Variable> variables;
    variables.reserve(model.variables.size());
    const auto& forbiddenNodesInVarBounds = PreSolveNonConstraint();

    for (const auto& variable: model.variables)
    {
        Expression lb(variable.lower_bound, convertExpressionToNode(variable.lower_bound, model));
        if (lb.RootNode())
        {
            ForbiddenNodesVisitor(forbiddenNodesInVarBounds, variable.lower_bound)
              .dispatch(lb.RootNode());
        }
        Expression ub(variable.upper_bound, convertExpressionToNode(variable.upper_bound, model));
        if (ub.RootNode())
        {
            ForbiddenNodesVisitor(forbiddenNodesInVarBounds, variable.upper_bound)
              .dispatch(ub.RootNode());
        }
        variables.emplace_back(variable.id,
                               std::move(lb),
                               std::move(ub),
                               convertType(variable.variable_type),
                               fromBool<TimeDependent>(variable.time_dependent),
                               fromBool<ScenarioDependent>(variable.scenario_dependent),
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
std::vector<Port> convertPorts(const YmlModel::Model& model, const std::vector<PortType>& portTypes)
{
    std::vector<Port> ports;
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
std::vector<PortFieldDefinition> convertPortFieldDefinitions(const YmlModel::Model& model,
                                                             const std::vector<Port>& ports)
{
    std::vector<PortFieldDefinition> portFieldDefinitions;
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
        ForbiddenNodesVisitor(PreSolveNonConstraint(), pfdefinition.definition)
          .dispatch(nodeRegistry.node);
        portFieldDefinitions.emplace_back(*itPort,
                                          *itField,
                                          Expression(pfdefinition.definition,
                                                     std::move(nodeRegistry)));
    }
    return portFieldDefinitions;
}

static void addSingleConstraint(std::vector<Constraint>& constraints,
                                const YmlModel::Constraint& constraint,
                                const YmlModel::Model& model,
                                const ForbiddenNodes& forbiddenNodes)
{
    auto nodeRegistry = convertExpressionToNode(constraint.expression, model);
    ForbiddenNodesVisitor(forbiddenNodes, constraint.expression).dispatch(nodeRegistry.node);
    constraints.emplace_back(constraint.id,
                             Expression{constraint.expression, std::move(nodeRegistry)},
                             convertLocation(constraint.location));
}

/**
 * \brief Converts constraints from YmlModel::Model to SystemModel::Constraint.
 *
 * \param model The YmlModel::Model object containing constraints.
 * \return A vector of SystemModel::Constraint objects.
 */
std::vector<Constraint> convertConstraints(const YmlModel::Model& model)
{
    std::vector<Constraint> constraints;
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
std::vector<ExtraOutput> convertExtraOutputs(const YmlModel::Model& model)
{
    std::vector<ExtraOutput> extraOutputs;
    extraOutputs.reserve(model.extra_outputs.size());

    for (const auto& extraOutput: model.extra_outputs)
    {
        auto nodeRegistry = convertExpressionToNode(extraOutput.expression, model);
        ForbiddenNodesVisitor(ForbiddenInExtraOutput(), extraOutput.expression)
          .dispatch(nodeRegistry.node);
        extraOutputs.emplace_back(extraOutput.id,
                                  Expression{extraOutput.expression, std::move(nodeRegistry)});
    }
    return extraOutputs;
}

/**
 * \brief Converts objectives from YmlModel::Model to SystemModel::Expression.
 *
 * \param model The YmlModel::Model object containing objectives.
 * \return A vector of SystemModel::Expression objects.
 */
std::vector<Objective> convertObjectives(const YmlModel::Model& model)
{
    std::vector<Objective> objectives;
    objectives.reserve(model.objectives.size());
    for (const auto& objective: model.objectives)
    {
        auto nodeRegistry = convertExpressionToNode(objective.expression, model);
        ForbiddenNodesVisitor(PreSolveNonConstraint(), objective.expression)
          .dispatch(nodeRegistry.node);
        objectives.emplace_back(objective.id,
                                Expression{objective.expression, std::move(nodeRegistry)},
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
std::vector<Model> convertModels(const YmlModel::Library& library,
                                 const std::vector<PortType>& portTypes)
{
    std::vector<Model> models;
    models.reserve(library.models.size());
    for (const auto& model: library.models)
    {
        ModelBuilder modelBuilder;
        std::vector<Parameter> parameters = convertParameters(model);
        std::vector<Variable> variables = convertVariables(model);
        std::vector<Port> ports = convertPorts(model, portTypes);
        std::vector<PortFieldDefinition> portFieldDefinitions = convertPortFieldDefinitions(model,
                                                                                            ports);
        std::vector<Constraint> constraints = convertConstraints(model);
        std::vector<ExtraOutput> extraOutputs = convertExtraOutputs(model);
        std::vector<Objective> objectives = convertObjectives(model);

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
Library convert(const YmlModel::Library& library)
{
    std::vector<PortType> portTypes = convertTypes(library);
    std::vector<Model> models = convertModels(library, portTypes);

    LibraryBuilder builder;
    Library lib = builder.withId(library.id)
                    .withDescription(library.description)
                    .withPortTypes(std::move(portTypes))
                    .withModels(std::move(models))
                    .build();
    return lib;
}
} // namespace Antares::IO::Inputs::ModelConverter
