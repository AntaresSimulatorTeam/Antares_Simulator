// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacyVariableInfo.h"

namespace Antares::Optimization
{

std::string ExtractLegacyComponentName(const std::string& location)
{
    const auto lt = location.rfind('<');
    if (lt == std::string::npos)
    {
        return {};
    }
    const auto gt = location.find('>', lt);
    if (gt == std::string::npos || gt <= lt + 1)
    {
        return {};
    }
    return location.substr(lt + 1, gt - lt - 1);
}

} // namespace Antares::Optimization
