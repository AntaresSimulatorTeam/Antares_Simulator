
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "Library.h"

namespace Antares::IO::Inputs::YmlModel
{
class Parser final
{
public:
    Parser() = default;
    ~Parser() = default;

    Library parse(const std::string& content);
};
} // namespace Antares::IO::Inputs::YmlModel
