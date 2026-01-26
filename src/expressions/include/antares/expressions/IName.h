// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <string>

namespace Antares::Expressions
{
class IName
{
public:
    virtual ~IName() = default;
    virtual std::string name() const = 0;
    bool operator==(const IName& other) const = default;
};
} // namespace Antares::Expressions
