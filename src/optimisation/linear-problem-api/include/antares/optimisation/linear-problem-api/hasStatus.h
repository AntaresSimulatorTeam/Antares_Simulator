// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <optional>

namespace Antares::Optimisation::LinearProblemApi
{
// TODO this is dummy copy of ortools basis status, to avoid including and linking against
// that lib
enum class MipBasisStatus : unsigned int
{
    FREE = 0,
    AT_LOWER_BOUND,
    AT_UPPER_BOUND,
    FIXED_VALUE,
    BASIC,
    NOT_AVAILABLE
};

inline std::string StatusToString(const std::optional<MipBasisStatus>& status)
{
    using namespace Antares::Optimisation::LinearProblemApi;
    // TODO shorten returns
    if (status.has_value())
    {
        switch (*status)
        {
        case MipBasisStatus::FREE:
            return "Free";
        case MipBasisStatus::AT_LOWER_BOUND:
            return "At lower bound";
        case MipBasisStatus::AT_UPPER_BOUND:
            return "At upper bound";
        case MipBasisStatus::FIXED_VALUE:
            return "Fixed value";
        case MipBasisStatus::BASIC:
            return "Basic";
        case MipBasisStatus::NOT_AVAILABLE:
        default:
            return "None";
        }
    }
    else
    {
        return "None";
    }
}

class IHasStatus
{
public:
    virtual MipBasisStatus getMipBasisStatus() const = 0;
};

} // namespace Antares::Optimisation::LinearProblemApi
