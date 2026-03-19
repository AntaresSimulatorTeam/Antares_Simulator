// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <vector>

namespace Antares::IO::Inputs::YmlSystem
{

struct Parameter
{
    std::string id;
    bool time_dependent;
    bool scenario_dependent;
    std::string value;
};

struct Component
{
    std::string id;
    std::string model;
    std::string scenarioGroup;
    std::vector<Parameter> parameters;
};

struct ConnectionEntry
{
    std::string componentId;
    std::string portId;
};

struct Connection
{
    ConnectionEntry firstEntry;
    ConnectionEntry secondEntry;
};

struct AreaConnection
{
    std::string componentId;
    std::string portId;
    std::string areaId;
};

struct ThermalComponent
{
    std::string areaId;
    std::string clusterId;
};

struct ThermalCapacityConnection
{
    std::string componentId;
    std::string portId;
    ThermalComponent thermalComponent;
};

struct System
{
    std::string id;
    std::vector<std::string> libraries;
    std::vector<Component> components;
    std::vector<Connection> connections;
    std::vector<AreaConnection> areaConnections;
    std::vector<ThermalCapacityConnection> thermalCapacityConnections;
};

} // namespace Antares::IO::Inputs::YmlSystem
