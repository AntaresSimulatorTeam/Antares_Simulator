/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

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
