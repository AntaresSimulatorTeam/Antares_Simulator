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

#include <stdexcept>

#include "antares/solver/libObjectModel/constraint.h"
#include "antares/solver/libObjectModel/library.h"
#include "antares/solver/libObjectModel/model.h"
#include "antares/solver/libObjectModel/parameter.h"
#include "antares/solver/libObjectModel/port.h"
#include "antares/solver/libObjectModel/portType.h"
#include "antares/solver/libObjectModel/variable.h"
#include "antares/solver/modelConverter/convertorVisitor.h"
#include "antares/solver/modelParser/Library.h"

namespace Antares::Solver::ModelConverter
{

/**
 * \brief Converts parameters from ModelParser::Model to ObjectModel::Parameter.
 *
 * \param model The ModelParser::Model object containing parameters.
 * \return A vector of ObjectModel::Parameter objects.
 */
std::vector<ObjectModel::PortType> convertTypes(const ModelParser::Library& library)
{
    // Convert portTypes to ObjectModel::PortType
    std::vector<ObjectModel::PortType> out;
    for (const auto& portType: library.port_types)
    {
        std::vector<ObjectModel::PortField> fields;
        for (const auto& field: portType.fields)
        {
            fields.emplace_back(ObjectModel::PortField{field});
        }
        ObjectModel::PortType portTypeModel(portType.id, portType.description, std::move(fields));
        out.emplace_back(std::move(portTypeModel));
    }
    return out;
}

/**
 * \brief Converts a ModelParser::ValueType to an ObjectModel::ValueType.
 *
 * \param type The ModelParser::ValueType to convert.
 * \return The corresponding ObjectModel::ValueType.
 * \throws std::runtime_error if the type is unknown.
 */
std::vector<ObjectModel::Parameter> convertParameters(const ModelParser::Model& model)
{
    std::vector<ObjectModel::Parameter> parameters;
    for (const auto& parameter: model.parameters)
    {
        parameters.emplace_back(ObjectModel::Parameter{
          parameter.id,
          ObjectModel::ValueType::FLOAT, // TODO: change to correct type
          static_cast<ObjectModel::Parameter::TimeDependent>(parameter.time_dependent),
          static_cast<ObjectModel::Parameter::ScenarioDependent>(parameter.scenario_dependent)});
    }
    return parameters;
}

/**
 * \brief Converts variables from ModelParser::Model to ObjectModel::Variable.
 *
 * \param model The ModelParser::Model object containing variables.
 * \return A vector of ObjectModel::Variable objects.
 */
ObjectModel::ValueType convertType(ModelParser::ValueType type)
{
    using namespace std::string_literals;
    switch (type)
    {
    case ModelParser::ValueType::CONTINUOUS:
        return ObjectModel::ValueType::FLOAT;
    case ModelParser::ValueType::INTEGER:
        return ObjectModel::ValueType::INTEGER;
    case ModelParser::ValueType::BOOL:
        return ObjectModel::ValueType::BOOL;
    default:
        throw std::runtime_error("Unknown type: " + ModelParser::toString(type));
    }
}

/**
 * \brief Converts ports from ModelParser::Model to ObjectModel::Port.
 *
 * \param model The ModelParser::Model object containing ports.
 * \return A vector of ObjectModel::Port objects.
 */
std::vector<ObjectModel::Variable> convertVariables(const ModelParser::Model& model,
                                                    Registry<Nodes::Node>& registry)
{
    std::vector<ObjectModel::Variable> variables;
    for (const auto& variable: model.variables)
    {
        variables.emplace_back(ObjectModel::Variable{
          variable.id,
          ObjectModel::Expression{variable.lower_bound,
                                  convertExpressionToNode(variable.lower_bound, registry, model)},
          ObjectModel::Expression{variable.upper_bound,
                                  convertExpressionToNode(variable.upper_bound, registry, model)},
          convertType(variable.variable_type)});
    }
    return variables;
}

/**
 * \brief Converts constraints from ModelParser::Model to ObjectModel::Constraint.
 *
 * \param model The ModelParser::Model object containing constraints.
 * \return A vector of ObjectModel::Constraint objects.
 */
std::vector<ObjectModel::Port> convertPorts([[maybe_unused]] const ModelParser::Model& model)
{
    std::vector<ObjectModel::Port> ports;
    /* for (const auto& port: model.ports) */
    /* { */
    /*     ports.emplace_back(ObjectModel::Port{port.name, port.type}); */
    /* } */
    return ports;
}

std::vector<ObjectModel::Constraint> convertConstraints(const ModelParser::Model& model,
                                                        Registry<Nodes::Node>& registry)
{
    std::vector<ObjectModel::Constraint> constraints;
    for (const auto& constraint: model.constraints)
    {
        Nodes::Node* expr = convertExpressionToNode(constraint.expression, registry, model);
        constraints.emplace_back(
          ObjectModel::Constraint{constraint.id,
                                  ObjectModel::Expression{constraint.expression, expr}});
    }
    return constraints;
}

/**
 * \brief Converts models from ModelParser::Library to ObjectModel::Model.
 *
 * \param library The ModelParser::Library object containing models.
 * \return A vector of ObjectModel::Model objects.
 */
std::vector<ObjectModel::Model> convertModels(const ModelParser::Library& library,
                                              Registry<Nodes::Node>& registry)
{
    std::vector<ObjectModel::Model> models;
    for (const auto& model: library.models)
    {
        ObjectModel::ModelBuilder modelBuilder;
        std::vector<ObjectModel::Parameter> parameters = convertParameters(model);
        std::vector<ObjectModel::Variable> variables = convertVariables(model, registry);
        std::vector<ObjectModel::Port> ports = convertPorts(model);
        std::vector<ObjectModel::Constraint> constraints = convertConstraints(model, registry);

        auto nodeObjective = convertExpressionToNode(model.objective, registry, model);

        auto modelObj = modelBuilder.withId(model.id)
                          .withObjective(ObjectModel::Expression{model.objective, nodeObjective})
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
 * \brief Converts a ModelParser::Library object to an ObjectModel::Library object.
 *
 * \param library The ModelParser::Library object to convert.
 * \return The corresponding ObjectModel::Library object.
 */
ObjectModel::Library convert(const ModelParser::Library& library, Registry<Nodes::Node>& registry)
{
    ObjectModel::LibraryBuilder builder;
    std::vector<ObjectModel::PortType> portTypes = convertTypes(library);
    std::vector<ObjectModel::Model> models = convertModels(library, registry);
    ObjectModel::Library lib = builder.withId(library.id)
                                 .withDescription(library.description)
                                 .withPortTypes(std::move(portTypes))
                                 .withModels(std::move(models))
                                 .build();
    return lib;
}

} // namespace Antares::Solver::ModelConverter
