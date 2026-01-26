// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/exception/UnfeasibleProblemError.hpp>

namespace Antares::Data
{
UnfeasibleProblemError::UnfeasibleProblemError(const std::string& message):
    std::runtime_error(message)
{
}

} // namespace Antares::Data
