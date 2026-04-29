// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stdexcept>

namespace Antares::Solver::Initialization::Error
{
class NoQueueService final: public std::runtime_error
{
public:
    NoQueueService():
        std::runtime_error("No queue service")
    {
    }
};
} // namespace Antares::Solver::Initialization::Error
