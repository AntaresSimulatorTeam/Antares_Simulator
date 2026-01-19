// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stdexcept>

#include <antares/expressions/NodeRegistry.h>
#include "antares/io/inputs/yml-model/Library.h"

namespace Antares::IO::Inputs::ModelConverter
{
class NoPortWithThisId final: public std::runtime_error
{
public:
    explicit NoPortWithThisId(const std::string& name);
};

class NoParameterOrVariableWithThisName final: public std::runtime_error
{
public:
    explicit NoParameterOrVariableWithThisName(const std::string& name):
        runtime_error("No parameter or variable found for this identifier: " + name)
    {
    }
};

class ReducedCostNoVariableWithThisName final: public std::runtime_error
{
public:
    explicit ReducedCostNoVariableWithThisName(const std::string& modelName,
                                               const std::string& varName):
        runtime_error("reduced_cost called with unknown variable '" + varName + "' in model '"
                      + modelName + "'")
    {
    }
};

class DualNoConstraintWithThisName final: public std::runtime_error
{
public:
    explicit DualNoConstraintWithThisName(const std::string& modelName,
                                          const std::string& constraintName):
        runtime_error("dual called with unknown constraint '" + constraintName + "' in model '"
                      + modelName + "'")
    {
    }
};

class AntlrParsingError final: public std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

Expressions::NodeRegistry convertExpressionToNode(const std::string& exprStr,
                                                  const YmlModel::Model& model);
} // namespace Antares::IO::Inputs::ModelConverter
