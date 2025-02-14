/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

#include <antares/study/system-model/model.h>

namespace Antares::Study::SystemModel
{

/**
 * \brief Builds and returns the Model object.
 *
 * \return The constructed Model object.
 */
Model ModelBuilder::build()
{
    Model model = std::move(model_);
    model_ = Model(); // makes ModelBuilder re-usable
    return std::move(model);
}

/**
 * \brief Sets the ID of the model.
 *
 * \param id The ID to set.
 * \return Reference to the ModelBuilder object.
 */
ModelBuilder& ModelBuilder::withId(std::string_view id)
{
    model_.id_ = id;
    return *this;
}

/**
 * \brief Sets the objective of the model.
 *
 * \param objective The Expression object representing the objective.
 * \return Reference to the ModelBuilder object.
 */
ModelBuilder& ModelBuilder::withObjective(Expression&& objective)
{
    model_.objective_ = std::move(objective);
    return *this;
}

/**
 * \brief Sets the parameters of the model.
 *
 * \param parameters A vector of Parameter objects to set.
 * \return Reference to the ModelBuilder object.
 *
 * inputs it not garanteed to be valid after the call
 */
ModelBuilder& ModelBuilder::withParameters(std::vector<Parameter>&& parameters)
{
    std::transform(parameters.begin(),
                   parameters.end(),
                   std::inserter(model_.parameters_, model_.parameters_.end()),
                   [](/*Non const to prevent copy*/ Parameter& parameter)
                   { return std::make_pair(parameter.Id(), std::move(parameter)); });
    return *this;
}

/**
 * \brief Sets the variables of the model.
 *
 * \param variables A vector of Variable objects to set.
 * \return Reference to the ModelBuilder object.
 *
 * inputs it not garanteed to be valid after the call
 */
ModelBuilder& ModelBuilder::withVariables(std::vector<Variable>&& variables)
{
    std::ranges::transform(variables,
                           std::inserter(model_.variables_, model_.variables_.end()),
                           [](/*Non const to prevent copy*/ Variable& variable)
                           { return std::make_pair(variable.Id(), std::move(variable)); });
    return *this;
}

/**
 * \brief Sets the ports of the model.
 *
 * \param ports A vector of Port objects to set.
 * \return Reference to the ModelBuilder object.
 *
 * inputs it not garanteed to be valid after the call
 */
ModelBuilder& ModelBuilder::withPorts(std::vector<Port>&& ports)
{
    std::transform(ports.begin(),
                   ports.end(),
                   std::inserter(model_.ports_, model_.ports_.end()),
                   [](/*Non const to prevent copy*/ Port& port)
                   { return std::make_pair(port.Id(), std::move(port)); });
    return *this;
}

/**
 * \brief Sets the ID of the library.
 *
 * \param id The ID to set.
 * \return Reference to the LibraryBuilder object.
 *
 * inputs it not garanteed to be valid after the call
 */
ModelBuilder& ModelBuilder::withConstraints(std::vector<Constraint>&& constraints)
{
    std::transform(constraints.begin(),
                   constraints.end(),
                   std::inserter(model_.constraints_, model_.constraints_.end()),
                   [](/*Non const to prevent copy*/ Constraint& constraint)
                   { return std::make_pair(constraint.Id(), std::move(constraint)); });
    return *this;
}

} // namespace Antares::Study::SystemModel
