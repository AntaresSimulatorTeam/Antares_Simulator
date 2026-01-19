// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stdexcept>

namespace Antares::Error
{
class InvalidArgumentError final: public std::invalid_argument
{
public:
    using std::invalid_argument::invalid_argument;
};
} // namespace Antares::Error
