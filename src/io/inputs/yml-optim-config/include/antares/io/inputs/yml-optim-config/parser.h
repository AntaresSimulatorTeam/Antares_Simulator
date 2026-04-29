
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "OptimConfig.h"

namespace Antares::IO::Inputs::YmlOptimConfig
{
class Parser final
{
public:
    Parser() = default;
    ~Parser() = default;

    OptimConfig parse(const std::string& content);
};
} // namespace Antares::IO::Inputs::YmlOptimConfig
