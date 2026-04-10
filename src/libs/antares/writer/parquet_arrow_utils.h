// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stdexcept>
#include <arrow/status.h>

// Throw std::runtime_error on Arrow Status failure.
// Use instead of ARROW_RETURN_NOT_OK in non-returning contexts (tests, constructors, etc.).
#define ARROW_THROW_NOT_OK(expr)                      \
    do                                                \
    {                                                 \
        arrow::Status _st = (expr);                   \
        if (!_st.ok())                                \
            throw std::runtime_error(_st.ToString()); \
    } while (0)

// Assign the value of an Arrow Result<T>, or throw on failure.
#define ARROW_THROW_ASSIGN(lhs, expr)                           \
    do                                                          \
    {                                                           \
        auto _res = (expr);                                     \
        if (!_res.ok())                                         \
            throw std::runtime_error(_res.status().ToString()); \
        lhs = std::move(_res).ValueOrDie();                     \
    } while (0)
