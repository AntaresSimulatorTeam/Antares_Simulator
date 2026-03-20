// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <optional>

#include <antares/expressions/nodes/PortFieldNode.h>
#include "antares/study/system-model/connection.h"

#include "model.h"
#include "variabilityType.h"

namespace Antares::ModelerStudy::SystemModel
{

// this struct contains more or less the same infos as the one in system.h
struct ParameterTypeAndValue
{
    std::string id;
    Optimisation::VariabilityType type;
    std::string value;
};

/**
 * Defines the attributes of the Component class
 * Made into a struct to avoid duplication in ComponentBuilder
 */
class ComponentData final
{
public:
    std::string id;
    const Model* model = nullptr;
    std::map<std::string, ParameterTypeAndValue> parameter_values;
    std::string scenario_group_id;
    unsigned index = 0;

    void reset()
    {
        id.clear();
        model = nullptr;
        parameter_values.clear();
        scenario_group_id.clear();
    }
};

struct ThermalComponent
{
    std::string areaId;
    std::string clusterId;
};

/**
 * Defines an actual component of the simulated system.
 */
class Component final
{
public:
    // Only allowing one private constructor (see below) to forbid empty Components
    Component() = delete;

    const std::string& Id() const
    {
        return data_.id;
    }

    const Model* getModel() const
    {
        return data_.model;
    }

    const std::map<std::string, ParameterTypeAndValue>& getParameterValues() const
    {
        return data_.parameter_values;
    }

    std::string getParameterValue(const std::string& parameter_id) const
    {
        if (!data_.parameter_values.contains(parameter_id))
        {
            throw std::invalid_argument("Parameter '" + parameter_id + "' not found in component '"
                                        + data_.id + "'");
        }
        return data_.parameter_values.at(parameter_id).value;
    }

    std::string getScenarioGroupId() const
    {
        return data_.scenario_group_id;
    }

    void addComponentConnection(const std::string localPortId, ConnectionEnd&& connection);
    const std::optional<AreaConnection>& areaConnectionAtPort(const std::string& portId) const;

    std::vector<ConnectionEnd> componentConnectionsViaPort(const std::string& portId) const;

    Expressions::Nodes::Node* nodeAtPortField(const std::string& portId,
                                              const std::string& fieldId) const;

    const Expression& expressionAtPortField(const std::string& portId,
                                            const std::string& fieldId) const;

    void addAreaConnection(const std::string& localPortId, const std::string& areaId);

    void addThermalCapacityConnection(const std::string& portId,
                                      const std::string& areaId,
                                      const std::string& clusterId);
    std::optional<std::string> areaConnectedToPort(const std::string& portId) const;

    const std::map<std::string, std::string>& portToAreaConnections() const;

    const std::map<std::string, ThermalComponent>& portToThermalCapacityConnections() const;

    std::optional<ThermalComponent> thermalCapacityConnectedToPort(const std::string& portId) const;

    unsigned int Index() const
    {
        return data_.index;
    }

    const Port& findPort(const std::string& portId, const std::string& prefixMessage) const;

private:
    void checkPortFieldDefinitionExists(const std::string& portName,
                                        const std::string& fieldName,
                                        const std::string& errMsgPrefix) const;

    // Only ComponentBuilder is allowed to build Component instances
    friend class ComponentBuilder;
    explicit Component(const ComponentData& component_data);
    std::map<std::string, std::vector<ConnectionEnd>> componentConnectionEnds_;
    std::map<std::string, std::string> portToAreaConnections_;
    std::map<std::string, ThermalComponent> portToThermalConnections_;
    ComponentData data_;
};

class ComponentBuilder final
{
public:
    ComponentBuilder& withId(std::string_view id);
    ComponentBuilder& withModel(const Model* model);
    ComponentBuilder& withIndex(unsigned int index);
    ComponentBuilder& withParameterValues(
      std::map<std::string, ParameterTypeAndValue> parameter_values);
    ComponentBuilder& withScenarioGroupId(const std::string& scenario_group_id);
    Component build();

private:
    ComponentData data_;
};

} // namespace Antares::ModelerStudy::SystemModel
