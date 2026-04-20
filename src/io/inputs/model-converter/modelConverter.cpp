// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/model-converter/modelConverter.h"

#include <antares/expressions/iterators/pre-order.h>
#include <antares/expressions/nodes/ExpressionsNodes.h> // gp : why do we need this inclusion ?
#include "antares/expressions/expression.h"
#include "antares/io/inputs/forbidden-nodes/ForbiddenNodes.h"
#include "antares/io/inputs/forbidden-nodes/ForbiddenNodesVisitor.h"
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
using namespace Antares::IO::Inputs::ForbidNodes;

namespace Antares::IO::Inputs::ModelConverter
{
static OutOfBoundsProcessingMode convertOutOfBoundsProcessingMode(const std::string& mode)
{
    if (mode.empty() || mode == "cyclic")
    {
        return OutOfBoundsProcessingMode::CYCLIC;
    }
    if (mode == "drop")
    {
        return OutOfBoundsProcessingMode::DROP;
    }
    throw IO::Inputs::InputError("Invalid out-of-bounds processing mode: " + mode);
}

AreaConnection convert_to_system(const YmlModel::AreaConnection& ac)
{
    return {ac.inject_to_balance, ac.spillage_bound, ac.unsupplied_energy_bound};
}

std::vector<PortType> convertPortTypes(const ::YmlModel::Library& library)
{
    std::vector<PortType> out;
    out.reserve(library.port_types.size());
    for (const auto& ymlPortType: library.port_types)
    {
        if (ymlPortType.fields.empty()) // Can't have a port type without fields
        {
            throw IO::Inputs::InputError("This port type doesn't contains fields: "
                                         + ymlPortType.id);
        }
        std::vector<PortField> fields;
        for (const auto& field: ymlPortType.fields)
        {
            fields.emplace_back(field);
        }

        // Can't have port types with the same ID
        auto predicate = [&ymlPortType](const auto& p) { return p.Id() == ymlPortType.id; };
        if (std::ranges::find_if(out, predicate) != out.end())
        {
            throw IO::Inputs::InputError("Port type with this id already exists: "
                                         + ymlPortType.id);
        }

        out.emplace_back(ymlPortType.id,
                         std::move(fields),
                         convert_to_system(ymlPortType.area_connection),
                         ymlPortType.thermal_capacity_connection_field);
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

Solver::Config::Location convertLocation(const std::string& locationStr)
{
    std::string locLower = locationStr;
    std::ranges::transform(locLower, locLower.begin(), ::tolower);
    if (locLower == "master")
    {
        return Solver::Config::Location::MASTER;
    }
    if (locLower == "master-and-subproblems")
    {
        return Solver::Config::Location::MASTER_AND_SUBPROBLEMS;
    }
    if (locLower == "subproblems")
    {
        return Solver::Config::Location::SUBPROBLEMS;
    }

    throw IO::Inputs::InputError("Unknown location: " + locationStr);
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
        throw IO::Inputs::InputError("Unknown variable type: " + YmlModel::toString(type));
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

    for (const auto& variable: model.variables)
    {
        Expression lb(variable.lower_bound, convertExpressionToNode(variable.lower_bound, model));
        if (lb.RootNode())
        {
            ForbiddenNodesVisitor(forbiddenInVariableBounds, variable.lower_bound)
              .dispatch(lb.RootNode());
        }
        Expression ub(variable.upper_bound, convertExpressionToNode(variable.upper_bound, model));
        if (ub.RootNode())
        {
            ForbiddenNodesVisitor(forbiddenInVariableBounds, variable.upper_bound)
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
        const auto port_type = std::ranges::find_if(portTypes,
                                                    [&port](const auto& pt)
                                                    { return pt.Id() == port.type; });
        if (port_type == portTypes.end())
        {
            throw IO::Inputs::InputError("For the port: " + port.id
                                         + " , port type not found: " + port.type);
        }
        ports.emplace_back(port.id, *port_type);
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
                                                             std::vector<Port>& ports)
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
            throw IO::Inputs::InputError("In port-field-definitions, port not found: "
                                         + pfdefinition.port);
        }

        // second check if the field exists in type
        const auto& portFields = itPort->Type().Fields();
        auto itField = std::ranges::find_if(portFields,
                                            [&pfdefinition](const auto& field)
                                            { return field.Id() == pfdefinition.field; });
        if (itField == portFields.end())
        {
            throw IO::Inputs::InputError("In port-field-definitions, for port: " + pfdefinition.port
                                         + " , field not found: " + pfdefinition.field);
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
            throw IO::Inputs::InputError("In port-field-definitions, for port: " + pfdefinition.port
                                         + " , found another port in the definition: "
                                         + dynamic_cast<const PortFieldNode&>(*it).getPortName());
        }

        ForbiddenNodesVisitor(forbiddenInPortFieldDef, pfdefinition.definition)
          .dispatch(nodeRegistry.node);

        portFieldDefinitions.emplace_back(*itPort,
                                          *itField,
                                          Expression(pfdefinition.definition,
                                                     std::move(nodeRegistry)));

        // A definition for a port field means this field is a sender
        itPort->setFieldRole(itField->Id(), FieldRole::Sender);
    }
    return portFieldDefinitions;
}

static Constraint createConstraint(const YmlModel::Constraint& constraint,
                                   const YmlModel::Model& model,
                                   const ForbiddenNodes& forbiddenNodes,
                                   bool isBindingConstraint = false)
{
    auto nodeRegistry = convertExpressionToNode(constraint.expression, model);
    ForbiddenNodesVisitor(forbiddenNodes, constraint.expression).dispatch(nodeRegistry.node);
    return {constraint.id,
            Expression{constraint.expression, std::move(nodeRegistry)},
            convertLocation(constraint.location),
            convertOutOfBoundsProcessingMode(constraint.out_of_bounds_processing_mode),
            isBindingConstraint};
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
        constraints.push_back(createConstraint(constraint, model, forbiddenInConstraint));
    }

    for (const auto& constraint: model.binding_constraints)
    {
        constraints.push_back(
          createConstraint(constraint, model, forbiddenInBindingConstraint, true));
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
        ForbiddenNodesVisitor(forbiddenInExtraOutput, extraOutput.expression)
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
        ForbiddenNodesVisitor(forbiddenInObjective, objective.expression)
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
    std::vector<PortType> portTypes = convertPortTypes(library);
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
