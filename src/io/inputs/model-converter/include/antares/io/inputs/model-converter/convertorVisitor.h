// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/NodeRegistry.h>
#include "antares/io/inputs/InputError.h"
#include "antares/io/inputs/yml-model/Library.h"

namespace Antares::IO::Inputs::ModelConverter
{
class NoPortWithThisId final: public IO::Inputs::InputError
{
public:
    explicit NoPortWithThisId(const std::string& name);
};

class NoParameterOrVariableWithThisName final: public IO::Inputs::InputError
{
public:
    explicit NoParameterOrVariableWithThisName(const std::string& name):
        IO::Inputs::InputError("No parameter or variable found for this identifier: " + name)
    {
    }
};

class ReducedCostNoVariableWithThisName final: public IO::Inputs::InputError
{
public:
    explicit ReducedCostNoVariableWithThisName(const std::string& modelName,
                                               const std::string& varName):
        IO::Inputs::InputError("reduced_cost called with unknown variable '" + varName
                               + "' in model '" + modelName + "'")
    {
    }
};

class DualNoConstraintWithThisName final: public IO::Inputs::InputError
{
public:
    explicit DualNoConstraintWithThisName(const std::string& modelName,
                                          const std::string& constraintName):
        IO::Inputs::InputError("dual called with unknown constraint '" + constraintName
                               + "' in model '" + modelName + "'")
    {
    }
};

class AntlrParsingError final: public IO::Inputs::InputError
{
    using IO::Inputs::InputError::InputError;
};

Expressions::NodeRegistry convertExpressionToNode(const std::string& exprStr,
                                                  const YmlModel::Model& model);
} // namespace Antares::IO::Inputs::ModelConverter
