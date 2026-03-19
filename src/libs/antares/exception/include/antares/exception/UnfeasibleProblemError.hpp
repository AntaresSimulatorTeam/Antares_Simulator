// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_DATA_UNFEASIBLEPROBLEMERROR_HPP
#define ANTARES_DATA_UNFEASIBLEPROBLEMERROR_HPP

#include <stdexcept>

namespace Antares::Data
{
class UnfeasibleProblemError final: public std::runtime_error
{
public:
    explicit UnfeasibleProblemError(const std::string& message);

    ~UnfeasibleProblemError() noexcept override = default;
};

} // namespace Antares::Data

#endif // ANTARES_DATA_UNFEASIBLEPROBLEMERROR_HPP
