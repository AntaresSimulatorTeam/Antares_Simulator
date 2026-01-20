// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <stdexcept>

#include "antares/io/inputs/yml-model/Library.h"

namespace Antares
{
namespace ModelerStudy::SystemModel
{
class Library;
}

namespace Inputs::YmlModel
{
class Library;
}
} // namespace Antares

namespace Antares::Expressions::Nodes
{
class Node;
}

namespace Antares::IO::Inputs::ModelConverter
{

ModelerStudy::SystemModel::Library convert(const YmlModel::Library& library);

// EXCEPTIONS
class UnknownTypeException final: public std::runtime_error
{
public:
    explicit UnknownTypeException(const std::string& type);
};

class PortWithThisIdAlreadyExists final: public std::runtime_error
{
public:
    explicit PortWithThisIdAlreadyExists(const std::string& id);
};

class PortTypeWithThisIdAlreadyExists final: public std::runtime_error
{
public:
    explicit PortTypeWithThisIdAlreadyExists(const std::string& id);
};

class ConstraintWithThisIdAlreadyExists final: public std::runtime_error
{
public:
    explicit ConstraintWithThisIdAlreadyExists(const std::string& id);
};

class PortTypeDoesntContainsFields final: public std::runtime_error
{
public:
    explicit PortTypeDoesntContainsFields(const std::string& id);
};

class PortTypeNotFound final: public std::runtime_error
{
public:
    explicit PortTypeNotFound(const std::string& portId, const std::string& portTypeId);
};

class PortNotFoundForDefinition final: public std::runtime_error
{
public:
    explicit PortNotFoundForDefinition(const std::string& portId);
};

class FieldNotFoundForDefinition final: public std::runtime_error
{
public:
    explicit FieldNotFoundForDefinition(const std::string& portId, const std::string& fieldId);
};

class PortInDefinition final: public std::runtime_error
{
public:
    explicit PortInDefinition(const std::string& portId, const std::string& portInDefId);
};
} // namespace Antares::IO::Inputs::ModelConverter
