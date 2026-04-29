// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_DATA_ASSERTIONERROR_HPP
#define ANTARES_DATA_ASSERTIONERROR_HPP

#include <stdexcept>

namespace Antares::Data
{
class AssertionError final: public std::runtime_error
{
public:
    explicit AssertionError(const std::string& message);
};

} // namespace Antares::Data

#endif // ANTARES_DATA_ASSERTIONERROR_HPP
