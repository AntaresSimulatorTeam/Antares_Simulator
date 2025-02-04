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

#include "antares/solver/modelConverter/modelConverter.h"

#include "antares/solver/expressions/visitors/TimeIndex.h"
#include "antares/solver/modelConverter/convertorVisitor.h"
#include "antares/study/system-model/constraint.h"
#include "antares/study/system-model/expression.h"
#include "antares/study/system-model/library.h"
#include "antares/study/system-model/model.h"
#include "antares/study/system-model/parameter.h"
#include "antares/study/system-model/port.h"
#include "antares/study/system-model/portType.h"
#include "antares/study/system-model/variable.h"

namespace Antares::Solver::ModelConverter
{
UnknownTypeException::UnknownTypeException(ModelParser::ValueType type):
    std::runtime_error("Unknown type: " + ModelParser::toString(type))
{
}

/**
 * \brief Converts parameters from ModelParser::Model to SystemModel::Parameter.
 *
 * \param model The ModelParser::Model object containing parameters.
 * \return A vector of SystemModel::Parameter objects.
 */
std::vector<Antares::Study::SystemModel::PortType> convertTypes(
  const Antares::Solver::ModelParser::Library& library)
{
    // Convert portTypes to Antares::Study::SystemModel::PortType
    std::vector<Antares::Study::SystemModel::PortType> out;
    out.reserve(library.port_types.size());
    for (const auto& portType: library.port_types)
    {
        std::vector<Antares::Study::SystemModel::PortField> fields;
        for (const auto& field: portType.fields)
        {
            fields.emplace_back(Antares::Study::SystemModel::PortField{field});
        }
        Antares::Study::SystemModel::PortType portTypeModel(portType.id,
                                                            portType.description,
                                                            std::move(fields));
        out.emplace_back(std::move(portTypeModel));
    }
    return out;
}

/**
 * \brief Converts a ModelParser::ValueType to an SystemModel::ValueType.
 *
 * \param type The ModelParser::ValueType to convert.
 * \return The corresponding SystemModel::ValueType.
 * \throws UnknownType if the type is unknown.
 */
std::vector<Antares::Study::SystemModel::Parameter> convertParameters(
  const Antares::Solver::ModelParser::Model& model)
{
    namespace SM = Antares::Study::SystemModel;
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
 * \brief Converts variables from ModelParser::Model to SystemModel::Variable.
 *
 * \param model The ModelParser::Model object containing variables.
 * \return A vector of SystemModel::Variable objects.
 */
Antares::Study::SystemModel::ValueType convertType(Antares::Solver::ModelParser::ValueType type)
{
    using namespace std::string_literals;
    switch (type)
    {
    case Antares::Solver::ModelParser::ValueType::CONTINUOUS:
        return Antares::Study::SystemModel::ValueType::FLOAT;
    case Antares::Solver::ModelParser::ValueType::INTEGER:
        return Antares::Study::SystemModel::ValueType::INTEGER;
    case Antares::Solver::ModelParser::ValueType::BOOL:
        return Antares::Study::SystemModel::ValueType::BOOL;
    default:
        throw UnknownTypeException(type);
    }
}

/**
 * \brief Converts ports from ModelParser::Model to SystemModel::Port.
 *
 * \param model The ModelParser::Model object containing ports.
 * \return A vector of SystemModel::Port objects.
 */
std::vector<Antares::Study::SystemModel::Variable> convertVariables(const ModelParser::Model& model)
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
 * \brief Converts constraints from ModelParser::Model to SystemModel::Constraint.
 *
 * \param model The ModelParser::Model object containing constraints.
 * \return A vector of SystemModel::Constraint objects.
 */
std::vector<Antares::Study::SystemModel::Port> convertPorts(
  [[maybe_unused]] const Antares::Solver::ModelParser::Model& model)
{
    return {};
}

std::vector<Antares::Study::SystemModel::Constraint> convertConstraints(
  const Antares::Solver::ModelParser::Model& model)
{
    std::vector<Antares::Study::SystemModel::Constraint> constraints;
    constraints.reserve(model.constraints.size());
    for (const auto& constraint: model.constraints)
    {
        auto nodeRegistry = convertExpressionToNode(constraint.expression, model);
        constraints.emplace_back(constraint.id,
                                 Antares::Study::SystemModel::Expression{constraint.expression,
                                                                         std::move(nodeRegistry)});
    }
    return constraints;
}

/**
 * \brief Converts models from ModelParser::Library to SystemModel::Model.
 *
 * \param library The ModelParser::Library object containing models.
 * \return A vector of SystemModel::Model objects.
 */
std::vector<Antares::Study::SystemModel::Model> convertModels(
  const Antares::Solver::ModelParser::Library& library)
{
    std::vector<Antares::Study::SystemModel::Model> models;
    models.reserve(library.models.size());
    for (const auto& model: library.models)
    {
        Antares::Study::SystemModel::ModelBuilder modelBuilder;
        std::vector<Antares::Study::SystemModel::Parameter> parameters = convertParameters(model);
        std::vector<Antares::Study::SystemModel::Variable> variables = convertVariables(model);
        std::vector<Antares::Study::SystemModel::Port> ports = convertPorts(model);
        std::vector<Antares::Study::SystemModel::Constraint> constraints = convertConstraints(
          model);

        auto nodeObjective = convertExpressionToNode(model.objective, model);

        auto modelObj = modelBuilder.withId(model.id)
                          .withObjective(
                            Antares::Study::SystemModel::Expression{model.objective,
                                                                    std::move(nodeObjective)})
                          .withParameters(std::move(parameters))
                          .withVariables(std::move(variables))
                          .withPorts(std::move(ports))
                          .withConstraints(std::move(constraints))
                          .build();
        models.emplace_back(std::move(modelObj));
    }
    return models;
}

/**
 * \brief Converts a ModelParser::Library object to an SystemModel::Library object.
 *
 * \param library The ModelParser::Library object to convert.
 * \return The corresponding SystemModel::Library object.
 */
Antares::Study::SystemModel::Library convert(const Antares::Solver::ModelParser::Library& library)
{
    Antares::Study::SystemModel::LibraryBuilder builder;
    std::vector<Antares::Study::SystemModel::PortType> portTypes = convertTypes(library);
    std::vector<Antares::Study::SystemModel::Model> models = convertModels(library);
    Antares::Study::SystemModel::Library lib = builder.withId(library.id)
                                                 .withDescription(library.description)
                                                 .withPortTypes(std::move(portTypes))
                                                 .withModels(std::move(models))
                                                 .build();
    return lib;
}
} // namespace Antares::Solver::ModelConverter
