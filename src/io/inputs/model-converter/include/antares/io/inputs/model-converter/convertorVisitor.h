/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

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
