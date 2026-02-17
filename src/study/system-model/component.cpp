// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <fmt/format.h>

#include <boost/algorithm/string/case_conv.hpp>

#include <antares/study/system-model/component.h>

using namespace Antares::Expressions::Nodes;

std::string toLowerCase(const std::string& str)
{
    std::string lowerCaseStr = str;
    boost::algorithm::to_lower(lowerCaseStr);
    return lowerCaseStr;
}

namespace Antares::ModelerStudy::SystemModel
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
    // Check that parameters values are coherent with the model
    if (data.model->Parameters().size() != data.parameter_values.size())
    {
        throw std::invalid_argument(
          fmt::format("The component \"{}\" has {} parameter(s), but its model has {}",
                      data.id,
                      data.parameter_values.size(),
                      data.model->Parameters().size()));
    }

    auto dependanceMismatchThrow =
      [&data](const std::string& parameterId, const std::string& dependanceType)
    {
        throw std::invalid_argument(
          fmt::format("Model '{}': Component '{}': Parameter '{}': {} dependance mismatch "
                      "between model and system",
                      data.model->Id(),
                      data.id,
                      parameterId,
                      dependanceType));
    };

    for (const auto& [paramName, param]: data.model->Parameters())
    {
        try
        {
            auto value = data.parameter_values.at(paramName);
            bool timeMismatch = !param.isTimeDependent() && isTimeDependent(value.type);
            bool scenarioMismatch = !param.isScenarioDependent() && isScenarioDependent(value.type);
            if (timeMismatch && scenarioMismatch)
            {
                dependanceMismatchThrow(paramName, "Time and Scenario");
            }
            if (timeMismatch)
            {
                dependanceMismatchThrow(paramName, "Time");
            }
            if (scenarioMismatch)
            {
                dependanceMismatchThrow(paramName, "Scenario");
            }
        }
        catch (const std::out_of_range&)
        {
            throw std::invalid_argument(
              fmt::format("The component '{}' has no value for parameter '{}'",
                          data.id,
                          paramName));
        }
    }
}

Component::Component(const ComponentData& component_data)
{
    checkComponentDataValidity(component_data);
    data_ = std::move(component_data);
}

void Component::addComponentConnection(const std::string localPortId, ConnectionEnd&& connexionEnd)
{
    componentConnectionEnds_[localPortId].push_back(std::move(connexionEnd));
}

const std::optional<AreaConnection>& Component::areaConnectionAtPort(
  const std::string& portId) const
{
    if (!getModel()->Ports().contains(portId))
    {
        std::string errMsg = "Port '" + portId + "' not found in component '" + data_.id + "'";
        throw std::invalid_argument(errMsg);
    }
    return getModel()->Ports().at(portId).Type().areaConnection();
}

std::vector<ConnectionEnd> Component::componentConnectionsViaPort(const std::string& portId) const
{
    if (auto it = componentConnectionEnds_.find(portId); it != componentConnectionEnds_.end())
    {
        return it->second;
    }
    return {};
}

Node* Component::nodeAtPortField(const std::string& portId, const std::string& fieldId) const
{
    try
    {
        PortFieldKey key(portId, fieldId);
        return getModel()->PortFieldDefinitions().at(key).Definition().RootNode();
    }
    catch (const std::out_of_range&)
    {
        throw std::invalid_argument(
          fmt::format("Port field '{}.{}' not found in component '{}'", portId, fieldId, data_.id));
    }
}

const Expression& Component::expressionAtPortField(const std::string& portId,
                                                   const std::string& fieldId) const
{
    try
    {
        PortFieldKey key(portId, fieldId);
        return getModel()->PortFieldDefinitions().at(key).Definition();
    }
    catch (const std::out_of_range&)
    {
        throw std::invalid_argument(
          fmt::format("Port field '{}.{}' not found in component '{}'", portId, fieldId, data_.id));
    }
}

void Component::checkPortFieldDefinitionExists(const std::string& portName,
                                               const std::string& fieldName,
                                               const std::string& errMsgPrefix) const
{
    if (!fieldName.empty())
    {
        PortFieldKey key(portName, fieldName);
        if (!getModel()->PortFieldDefinitions().contains(key))
        {
            std::string errMsg = errMsgPrefix + "port field '" + fieldName
                                 + "' is not defined in the component's model '" + getModel()->Id()
                                 + "'";
            throw std::invalid_argument(errMsg);
        }
    }
}

void Component::addAreaConnection(const std::string& localPortId, const std::string& areaId)
{
    std::string errMsgPrefix = "Cannot connect area '" + areaId + "' to port '" + localPortId
                               + "' of component '" + data_.id + "': ";
    if (!getModel()->Ports().contains(localPortId))
    {
        throw std::invalid_argument(errMsgPrefix + "port does not exist in the component's model '"
                                    + getModel()->Id() + "'");
    }
    Port port = getModel()->Ports().at(localPortId);
    const auto& area_connection = port.Type().areaConnection();
    if (!area_connection.has_value())
    {
        throw std::invalid_argument(errMsgPrefix + "port type '" + port.Type().Id()
                                    + "' has no area-connection field ID defined");
    }

    checkPortFieldDefinitionExists(localPortId, area_connection->injection, errMsgPrefix);
    checkPortFieldDefinitionExists(localPortId, area_connection->spillage_bound, errMsgPrefix);
    checkPortFieldDefinitionExists(localPortId,
                                   area_connection->unsupplied_energy_bound,
                                   errMsgPrefix);

    if (portToAreaConnections_.contains(localPortId))
    {
        throw std::invalid_argument(errMsgPrefix + "port is already connected to '"
                                    + portToAreaConnections_.at(localPortId) + "'");
    }

    portToAreaConnections_[localPortId] = toLowerCase(areaId);
}

std::optional<std::string> Component::areaConnectedToPort(const std::string& portId) const
{
    return portToAreaConnections_.contains(portId)
             ? std::optional(portToAreaConnections_.at(portId))
             : std::nullopt;
}

const std::map<std::string, std::string>& Component::portToAreaConnections() const
{
    return portToAreaConnections_;
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

ComponentBuilder& ComponentBuilder::withIndex(unsigned int index)
{
    data_.index = index;
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
  std::map<std::string, ParameterTypeAndValue> parameter_values)
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

} // namespace Antares::ModelerStudy::SystemModel
