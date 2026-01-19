// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stdexcept>

namespace Antares::Error
{
class RuntimeError final: public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};
} // namespace Antares::Error
