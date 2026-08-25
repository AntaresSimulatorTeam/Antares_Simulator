// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/model-converter/modelConverter.h"

#include <set>

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

namespace
{

/// used for error reporting
std::string buildFileAndLineNb(const std::string& filename, size_t lineNb)
{
    if (lineNb == 0)
    {
        return {};
    }
    return fmt::format("'{}', line {}", filename, lineNb);
}
} // anonymous namespace

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
    throw InputError("Invalid out-of-bounds processing mode: " + mode);
}

AreaConnection convert_to_system(const YmlModel::AreaConnection& ac)
{
    return {ac.inject_to_balance, ac.spillage_bound, ac.unsupplied_energy_bound};
}

std::vector<PortType> convertPortTypes(const YmlModel::Library& library)
{
    std::vector<PortType> out;
    out.reserve(library.port_types.size());
    for (const auto& ymlPortType: library.port_types)
    {
        if (ymlPortType.fields.empty()) // Can't have a port type without fields
        {
            throw InputError("This port type doesn't contains fields: " + ymlPortType.id);
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
            throw InputError("Port type with this id already exists: " + ymlPortType.id);
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
    std::vector<Parameter> parameters;
    parameters.reserve(model.parameters.size());
    for (const auto& parameter: model.parameters)
    {
        parameters.emplace_back(parameter.id,
                                SM::fromBool<TimeDependent>(parameter.time_dependent),
                                SM::fromBool<ScenarioDependent>(parameter.scenario_dependent));
    }
    return parameters;
}

Solver::Config::Location convertLocation(const std::string& locationStr)
{
    std::string locLower = locationStr;
    std::ranges::transform(locLower, locLower.begin(), tolower);
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

    throw InputError("Unknown location: " + locationStr);
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
        throw InputError("Unknown variable type: " + YmlModel::toString(type));
    }
}

/**
 * \brief Converts variables from YmlModel::Model to SystemModel::Variable.
 *
 * \param model The YmlModel::Model object containing variables.
 * \return A vector of SystemModel::Variable objects.
 */
std::vector<Variable> convertVariables(const YmlModel::Model& model,
                                       const std::string& libraryFileName)
{
    std::vector<Variable> variables;
    variables.reserve(model.variables.size());

    for (const auto& variable: model.variables)
    {
        Expression lb(variable.lower_bound.input_expr,
                      convertExpressionToNode(
                        variable.lower_bound.input_expr,
                        model,
                        buildFileAndLineNb(libraryFileName, variable.lower_bound.line_number)));
        if (lb.RootNode())
        {
            ForbiddenNodesVisitor(forbiddenInVariableBounds, variable.lower_bound.input_expr)
              .dispatch(lb.RootNode());
        }
        Expression ub(variable.upper_bound.input_expr,
                      convertExpressionToNode(
                        variable.upper_bound.input_expr,
                        model,
                        buildFileAndLineNb(libraryFileName, variable.upper_bound.line_number)));
        if (ub.RootNode())
        {
            ForbiddenNodesVisitor(forbiddenInVariableBounds, variable.upper_bound.input_expr)
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
            throw InputError("For the port: " + port.id + " , port type not found: " + port.type);
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
                                                             std::vector<Port>& ports,
                                                             const std::string& libraryFileName)
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
            throw InputError(
              fmt::format("In port-field-definitions, port not found: {}", pfdefinition.port));
        }

        // second check if the field exists in type
        const auto& portFields = itPort->Type().Fields();
        auto itField = std::ranges::find_if(portFields,
                                            [&pfdefinition](const auto& field)
                                            { return field.Id() == pfdefinition.field; });
        if (itField == portFields.end())
        {
            throw InputError(
              fmt::format("In port-field-definitions, for port: {} , field not found: {}",
                          pfdefinition.port,
                          pfdefinition.field));
        }

        auto nodeRegistry = convertExpressionToNode(
          pfdefinition.definition.input_expr,
          model,
          buildFileAndLineNb(libraryFileName, pfdefinition.definition.line_number));

        using namespace Antares::Expressions::Nodes;
        AST preorder(nodeRegistry.node);
        auto it = std::find_if(preorder.begin(),
                               preorder.end(),
                               [](const Node& node)
                               { return dynamic_cast<const PortFieldNode*>(&node) != nullptr; });
        if (it != preorder.end())
        {
            throw InputError(

              fmt::format("In port-field-definitions, for port: {} , found another port in the "
                          "definition: {}",
                          pfdefinition.port,
                          dynamic_cast<const PortFieldNode&>(*it).getPortName()));
        }

        ForbiddenNodesVisitor(forbiddenInPortFieldDef, pfdefinition.definition.input_expr)
          .dispatch(nodeRegistry.node);

        portFieldDefinitions.emplace_back(*itPort,
                                          *itField,
                                          Expression(pfdefinition.definition.input_expr,
                                                     std::move(nodeRegistry)));

        // A definition for a port field means this field is a sender
        itPort->setFieldRole(itField->Id(), FieldRole::Sender);
    }

    return portFieldDefinitions;
}

static Constraint createConstraint(const YmlModel::Constraint& constraint,
                                   const YmlModel::Model& model,
                                   const ForbiddenNodes& forbiddenNodes,
                                   const std::string& libraryFileName,
                                   bool isBindingConstraint = false)
{
    auto nodeRegistry = convertExpressionToNode(
      constraint.expression.input_expr,
      model,
      buildFileAndLineNb(libraryFileName, constraint.expression.line_number));
    ForbiddenNodesVisitor(forbiddenNodes, constraint.expression.input_expr)
      .dispatch(nodeRegistry.node);
    return {constraint.id,
            Expression{constraint.expression.input_expr, std::move(nodeRegistry)},
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
std::vector<Constraint> convertConstraints(const YmlModel::Model& model,
                                           const std::string& libraryFileName)
{
    std::vector<Constraint> constraints;
    constraints.reserve(model.constraints.size());

    for (const auto& constraint: model.constraints)
    {
        constraints.push_back(
          createConstraint(constraint, model, forbiddenInConstraint, libraryFileName));
    }

    for (const auto& constraint: model.binding_constraints)
    {
        constraints.push_back(
          createConstraint(constraint, model, forbiddenInBindingConstraint, libraryFileName, true));
    }
    return constraints;
}

/**
 * \brief Converts extra outputs from YmlModel::Model to SystemModel::ExtraOutput.
 *
 * \param model The YmlModel::Model object containing extra outputs.
 * \return A vector of SystemModel::ExtraOutput objects.
 */
std::vector<ExtraOutput> convertExtraOutputs(const YmlModel::Model& model,
                                             const std::string& libraryFileName)
{
    std::vector<ExtraOutput> extraOutputs;
    extraOutputs.reserve(model.extra_outputs.size());

    for (const auto& extraOutput: model.extra_outputs)
    {
        auto nodeRegistry = convertExpressionToNode(
          extraOutput.expression.input_expr,
          model,
          buildFileAndLineNb(libraryFileName, extraOutput.expression.line_number));
        ForbiddenNodesVisitor(forbiddenInExtraOutput, extraOutput.expression.input_expr)
          .dispatch(nodeRegistry.node);
        extraOutputs.emplace_back(extraOutput.id,
                                  Expression{extraOutput.expression.input_expr,
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
std::vector<Objective> convertObjectives(const YmlModel::Model& model,
                                         const std::string& libraryFileName)
{
    std::vector<Objective> objectives;
    objectives.reserve(model.objectives.size());
    for (const auto& objective: model.objectives)
    {
        auto nodeRegistry = convertExpressionToNode(
          objective.expression.input_expr,
          model,
          buildFileAndLineNb(libraryFileName, objective.expression.line_number));
        ForbiddenNodesVisitor(forbiddenInObjective, objective.expression.input_expr)
          .dispatch(nodeRegistry.node);
        objectives.emplace_back(objective.id,
                                Expression{objective.expression.input_expr,
                                           std::move(nodeRegistry)},
                                convertLocation(objective.location));
    }
    return objectives;
}

/**
 * \brief Checks that no field is both defined in a port-field-definition and used
 * in a sum_connections in a binding constraint within the same model.
 *
 * A field defined in a port-field-definition has the Sender role, while sum_connections
 * consumes fields from the Receiver side. Having both in the same model is contradictory.
 */
void checkPortFieldDefinitionConflictWithSumConnections(
  const std::vector<Constraint>& constraints,
  const std::vector<YmlModel::PortFieldDefinition>& portFieldDefs,
  const std::string& modelId)
{
    if (portFieldDefs.empty())
    {
        return;
    }

    std::set<std::pair<std::string, std::string>> senderFields;
    for (const auto& pfd: portFieldDefs)
    {
        senderFields.insert({pfd.port, pfd.field});
    }

    for (const auto& constraint: constraints)
    {
        if (!constraint.isBindingConstraint())
        {
            continue;
        }
        if (!constraint.expression().RootNode())
        {
            continue;
        }

        AST ast(constraint.expression().RootNode());
        for (const auto& node: ast)
        {
            if (const auto* sumNode = dynamic_cast<const PortFieldSumNode*>(&node))
            {
                auto key = std::make_pair(sumNode->getPortName(), sumNode->getFieldName());
                if (senderFields.contains(key))
                {
                    throw InputError(
                      fmt::format("In model '{}', field '{}' of port '{}' is defined in a "
                                  "port-field-definition and also used in a sum_connections "
                                  "in binding constraint '{}'. "
                                  "A field cannot be both a sender and a receiver in the "
                                  "same model.",
                                  modelId,
                                  sumNode->getFieldName(),
                                  sumNode->getPortName(),
                                  constraint.Id()));
                }
            }
        }
    }
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
        std::vector<Variable> variables = convertVariables(model, library.filename);
        std::vector<Port> ports = convertPorts(model, portTypes);
        std::vector<PortFieldDefinition>
          portFieldDefinitions = convertPortFieldDefinitions(model, ports, library.filename);
        std::vector<Constraint> constraints = convertConstraints(model, library.filename);
        std::vector<ExtraOutput> extraOutputs = convertExtraOutputs(model, library.filename);
        std::vector<Objective> objectives = convertObjectives(model, library.filename);

        checkPortFieldDefinitionConflictWithSumConnections(
          constraints, model.port_field_definitions, model.id);

        auto modelObj = modelBuilder.withId(model.id)
                          .withLibraryId(library.id)
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
