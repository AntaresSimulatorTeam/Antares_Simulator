// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/exception/LoadingError.hpp>

namespace Antares::IO::Inputs
{
/// Base class for all input-related errors in the io/inputs layer.
class InputError: public Error::LoadingError
{
public:
    using Error::LoadingError::LoadingError;
};
} // namespace Antares::IO::Inputs
