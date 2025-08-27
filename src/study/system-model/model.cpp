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

#include <boost/container_hash/hash.hpp>

#include <antares/study/system-model/model.h>
#include "antares/exception/RuntimeError.hpp"

namespace
{
template<class OutT, class InT>
void fillMapFrom(OutT& out,
                 InT&& in,
                 Antares::ModelerStudy::SystemModel::UniqueIDChecker& uniqueIdChecker)
{
    using InnerT = std::remove_cvref_t<InT>::value_type;

    for (const InnerT& x: in)
    {
        uniqueIdChecker.add(x.Id());
    }

    std::transform(in.begin(),
                   in.end(),
                   std::inserter(out, out.end()),
                   [](/*Non const to prevent copy*/ InnerT& x)
                   {
                       std::string id = x.Id();
                       return std::make_pair(std::move(id), std::move(x));
                   });
}
} // namespace

namespace Antares::ModelerStudy::SystemModel
{
std::size_t PortFieldKeyHash::operator()(const PortFieldKey& input) const
{
    std::size_t seed = 0;
    boost::hash_combine(seed, input.portId);
    boost::hash_combine(seed, input.fieldId);
    return seed;
}

void UniqueIDChecker::add(const std::string& id)
{
    attribute_ids_[id]++;
}

void UniqueIDChecker::check(const std::string& modelId) const
{
    for (const auto& [id, count]: attribute_ids_)
    {
        if (count > 1)
        {
            throw Error::RuntimeError("Model \"" + modelId
                                      + "\" contains multiple objects with ID \"" + id + "\".");
        }
    }
}

void UniqueIDChecker::clear()
{
    attribute_ids_.clear();
}

/**
 * \brief Builds and returns the Model object.
 *
 * \return The constructed Model object.
 */
Model ModelBuilder::build()
{
    Model model = std::move(model_);
    try
    {
        uniqueIdChecker_.check(model.Id());
    }
    catch (...)
    {
        reset();
        throw;
    }
    reset();
    return model;
}

void ModelBuilder::reset()
{
    model_ = Model(); // makes ModelBuilder re-usable
    uniqueIdChecker_.clear();
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
 * inputs are not guaranteed to be valid after the call
 */
ModelBuilder& ModelBuilder::withParameters(std::vector<Parameter>&& parameters)
{
    fillMapFrom(model_.parameters_, parameters, uniqueIdChecker_);
    return *this;
}

/**
 * \brief Sets the variables of the model.
 *
 * \param variables A vector of Variable objects to set.
 * \return Reference to the ModelBuilder object.
 *
 * inputs are not guaranteed to be valid after the call
 */
ModelBuilder& ModelBuilder::withVariables(std::vector<Variable>&& variables)
{
    fillMapFrom(model_.variables_, variables, uniqueIdChecker_);
    return *this;
}

/**
 * \brief Sets the ports of the model.
 *
 * \param ports A vector of Port objects to set.
 * \return Reference to the ModelBuilder object.
 *
 * inputs are not garanteed to be valid after the call
 */
ModelBuilder& ModelBuilder::withPorts(std::vector<Port>&& ports)
{
    fillMapFrom(model_.ports_, ports, uniqueIdChecker_);
    return *this;
}

/**
 * \brief Sets the constraints of the model.
 *
 * \param constraints A vector of Constraint objects to set.
 * \return Reference to the ModelBuilder object.
 *
 * inputs are not guaranteed to be valid after the call
 */
ModelBuilder& ModelBuilder::withConstraints(std::vector<Constraint>&& constraints)
{
    fillMapFrom(model_.constraints_, constraints, uniqueIdChecker_);
    return *this;
}

/**
 * \brief Sets the port-field definitions of the model.
 *
 * \param portFieldDefinitions A vector of PortFieldDefinition objects to set.
 * \return Reference to the ModelBuilder object.
 *
 * inputs are not guaranteed to be valid after the call
 */
ModelBuilder& ModelBuilder::withPortFieldDefinitions(
  std::vector<PortFieldDefinition>&& portFieldDefinitions)
{
    std::transform(portFieldDefinitions.begin(),
                   portFieldDefinitions.end(),
                   std::inserter(model_.portFieldDefinitions_, model_.portFieldDefinitions_.end()),
                   [](/*Non const to prevent copy*/ PortFieldDefinition& pfd)
                   {
                       auto id = pfd.port().Id();
                       auto fieldId = pfd.field().Id();
                       return std::make_pair(PortFieldKey{.portId = id, .fieldId = fieldId},
                                             std::move(pfd));
                   });
    return *this;
}

/**
 * \brief Sets the extra outputs of the model.
 *
 * \param extraOutputs A vector of ExtraOutput objects to set.
 * \return Reference to the ModelBuilder object.
 *
 * inputs are not guaranteed to be valid after the call
 */
ModelBuilder& ModelBuilder::withExtraOutputs(std::vector<ExtraOutput>&& extraOutputs)
{
    fillMapFrom(model_.extraOutputs_, extraOutputs, uniqueIdChecker_);
    return *this;
}

} // namespace Antares::ModelerStudy::SystemModel
