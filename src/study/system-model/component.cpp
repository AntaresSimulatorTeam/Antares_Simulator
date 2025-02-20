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

#include <ranges>

#include <boost/move/utility_core.hpp>

#include <antares/study/system-model/component.h>
#include "antares/expressions/visitors/PrintVisitor.h"
#include <antares/expressions/nodes/ExpressionsNodes.h>

#include "ComponentExpressionVisitor.h"

namespace Antares::Study::SystemModel
{

static void checkComponentDataValidity(const ComponentData& data)
{
    // Check that mandatory attributes are not empty
    if (data.id.empty())
    {
        throw std::invalid_argument("A component can't have an empty id");
    }
    if (data.model == nullptr)
    {
        throw std::invalid_argument("A component can't have an empty model");
    }
    if (data.scenario_group_id.empty())
    {
        throw std::invalid_argument("A component can't have an empty scenario_group_id");
    }
    // Check that parameters values are coherent with the model
    if (data.model->Parameters().size() != data.parameter_values.size())
    {
        throw std::invalid_argument(
          "The component \"" + data.id + "\" has " + std::to_string(data.parameter_values.size())
          + " parameter(s), but its model has " + std::to_string(data.model->Parameters().size()));
    }
    for (const auto param: data.model->Parameters() | std::views::keys)
    {
        if (!data.parameter_values.contains(param))
        {
            throw std::invalid_argument("The component \"" + data.id
                                        + "\" has no value for parameter '" + param + "'");
        }
    }
}

Component::Component(const ComponentData& component_data)
{
    checkComponentDataValidity(component_data);
    data_ = std::move(component_data);
    interpretExpressions();
}

/**
 * \brief Sets the ID of the component.
 *
 * \param id The ID to set.
 * \return Reference to the ComponentBuilder object.
 */
ComponentBuilder& ComponentBuilder::withId(const std::string_view id)
{
    data_.id = id;
    return *this;
}

/**
 * \brief Sets the model of the component.
 *
 * \param model The model to set.
 * \return Reference to the ComponentBuilder object.
 */
ComponentBuilder& ComponentBuilder::withModel(const Model* model)
{
    data_.model = model;
    return *this;
}

/**
 * \brief Sets the parameter values of the component. The parameters included should be all of the
 * model's parameters.
 *
 * \param parameter_values The map of parameter values to set.
 * \return Reference to the ComponentBuilder object.
 */
ComponentBuilder& ComponentBuilder::withParameterValues(
  std::map<std::string, double> parameter_values)
{
    data_.parameter_values = std::move(parameter_values);
    return *this;
}

/**
 * \brief Sets the ID of the scenario group to which the component belongs.
 *
 * \param scenario_group_id The scenario group ID to set.
 * \return Reference to the ComponentBuilder object.
 */
ComponentBuilder& ComponentBuilder::withScenarioGroupId(const std::string& scenario_group_id)
{
    data_.scenario_group_id = scenario_group_id;
    return *this;
}

/**
 * \brief Builds and returns the Component object.
 *
 * \return The constructed Component object.
 */
Component ComponentBuilder::build()
{
    Component component(data_);
    data_.reset(); // makes the ComponentBuilder re-usable
    return component;
}

void Component::interpretExpressions()
{
    Expressions::Registry<Expressions::Nodes::Node> registry1;
    auto visitor1 = ComponentExpressionVisitor(registry1, data_.parameter_values);
    auto printVisitor = Expressions::Visitors::PrintVisitor();
    auto* objectiveNode = visitor1.dispatch(getModel()->Objective().RootNode());
    objective_ = Expression(printVisitor.dispatch(objectiveNode),
                            {objectiveNode, std::move(registry1)});

    for (auto& var: getModel()->Variables())
    {
        Expressions::Registry<Expressions::Nodes::Node> registry2;
        auto visitor2 = ComponentExpressionVisitor(registry2, data_.parameter_values);
        auto* lb = visitor2.dispatch(var.second.LowerBound().RootNode());
        auto lb_expression = Expression(printVisitor.dispatch(lb), {lb, std::move(registry2)});
        Expressions::Registry<Expressions::Nodes::Node> registry3;
        auto visitor3 = ComponentExpressionVisitor(registry3, data_.parameter_values);
        auto* ub = visitor3.dispatch(var.second.UpperBound().RootNode());
        auto ub_expression = Expression(printVisitor.dispatch(ub), {ub, std::move(registry3)});
        auto& model_var = var.second;
        Variable newVar(model_var.Id(),
                           std::move(lb_expression),
                           std ::move(ub_expression),
                           model_var.Type(),
                           model_var.isTimeDependent() ? TimeDependent::YES : TimeDependent::NO,
                           model_var.IsScenarioDependent() ? ScenarioDependent::YES
                                                           : ScenarioDependent::NO);
        variables_.insert({var.first, std::move(newVar)});
    }

    for (auto& ct: getModel()->getConstraints())
    {
        Expressions::Registry<Expressions::Nodes::Node> registry4;
        auto visitor4 = ComponentExpressionVisitor(registry4, data_.parameter_values);
        auto* expressionNode = visitor4.dispatch(ct.second.expression().RootNode());
        auto expression = Expression(printVisitor.dispatch(expressionNode),
                                     {expressionNode, std::move(registry4)});
        Constraint newCt(ct.second.Id(), std::move(expression));
        constraints_.insert({ct.first, std::move(newCt)});
    }
}

} // namespace Antares::Study::SystemModel
