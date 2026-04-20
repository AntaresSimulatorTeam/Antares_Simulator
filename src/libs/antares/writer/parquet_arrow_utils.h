// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <arrow/result.h>
#include <arrow/status.h>
#include <stdexcept>

inline void throwOnStatusKO(const arrow::Status& status)
{
    if (!status.ok())
    {
        throw std::runtime_error(status.ToString());
    }
}

template<typename T>
T throwOnResultKO(arrow::Result<T> result)
{
    if (!result.ok())
    {
        throw std::runtime_error(result.status().ToString());
    }
    return std::move(result).ValueOrDie();
}
