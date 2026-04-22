// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/NodeRegistry.h>
#include "antares/io/inputs/InputError.h"
#include "antares/io/inputs/yml-model/Library.h"

namespace Antares::IO::Inputs::ModelConverter
{

class AntlrParsingError final: public IO::Inputs::InputError
{
    using IO::Inputs::InputError::InputError;
};

Expressions::NodeRegistry convertExpressionToNode(const std::string& exprStr,
                                                  const YmlModel::Model& model);
} // namespace Antares::IO::Inputs::ModelConverter
