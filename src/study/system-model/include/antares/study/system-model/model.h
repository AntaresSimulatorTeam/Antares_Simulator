// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <unordered_map>
#include <vector>

#include <antares/expressions/expression.h>

#include "connection.h"
#include "constraint.h"
#include "extraOutput.h"
#include "objective.h"
#include "parameter.h"
#include "port.h"
#include "portFieldDefinition.h"
#include "variable.h"

namespace Antares::ModelerStudy::SystemModel
{
struct PortFieldKey
{
    std::string portId;
    std::string fieldId;
    auto operator<=>(const PortFieldKey&) const = default;
};

class PortFieldKeyHash final
{
public:
    std::size_t operator()(const PortFieldKey& input) const;
};

using PortFieldMap = std::unordered_map<PortFieldKey, PortFieldDefinition, PortFieldKeyHash>;

/**
 * Defines a model that can be referenced by actual components.
 * A model defines the behaviour of those components.
 */
// TODO: add unit tests for this class
class Model final
{
public:
    Model() = default;
    Model(Model&&) = default;
    Model(const Model&) = delete;
    ~Model() = default;

    Model& operator=(Model&&) = default;
    Model& operator=(const Model&) = delete;

    const std::string& Id() const
    {
        return id_;
    }

    const std::vector<Objective>& Objectives() const
    {
        return objectives_;
    }

    const std::vector<Constraint>& Constraints() const
    {
        return constraints_;
    }

    const std::map<std::string, Parameter>& Parameters() const
    {
        return parameters_;
    }

    const std::vector<Variable>& Variables() const
    {
        return variables_;
    }

    const std::map<std::string, Port>& Ports() const
    {
        return ports_;
    }

    const PortFieldMap& PortFieldDefinitions() const
    {
        return portFieldDefinitions_;
    }

    const std::map<std::string, ExtraOutput>& ExtraOutputs() const
    {
        return extraOutputs_;
    }

private:
    friend class ModelBuilder;
    std::string id_;

    std::map<std::string, Parameter> parameters_;
    std::vector<Variable> variables_;
    std::vector<Constraint> constraints_;
    std::map<std::string, Port> ports_;
    std::map<std::string, ExtraOutput> extraOutputs_;
    std::vector<Objective> objectives_;

    PortFieldMap portFieldDefinitions_;
};

// List of IDs used internally to check for uniqueness of IDs at component level
class UniqueIDChecker final
{
public:
    void add(const std::string& id);
    void check(const std::string& modelId) const;
    void clear();

private:
    std::unordered_map<std::string, int> attribute_ids_;
};

class ModelBuilder final
{
public:
    ModelBuilder& withId(std::string_view id);
    ModelBuilder& withObjectives(std::vector<Objective>&& objectives);
    ModelBuilder& withParameters(std::vector<Parameter>&& parameters);
    ModelBuilder& withVariables(std::vector<Variable>&& variables);
    ModelBuilder& withPorts(std::vector<Port>&& ports);
    ModelBuilder& withConstraints(std::vector<Constraint>&& constraints);
    ModelBuilder& withPortFieldDefinitions(std::vector<PortFieldDefinition>&& portFieldDefinitions);
    ModelBuilder& withExtraOutputs(std::vector<ExtraOutput>&& extraOutputs);
    Model build();

private:
    Model model_;
    UniqueIDChecker uniqueIdChecker_;
    void reset();
};

} // namespace Antares::ModelerStudy::SystemModel
