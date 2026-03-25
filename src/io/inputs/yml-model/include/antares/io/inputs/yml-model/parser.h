
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <stdexcept>

#include "Library.h"

namespace Antares::IO::Inputs::YmlModel
{
class LibraryIdNotDefined final: public std::runtime_error
{
public:
    LibraryIdNotDefined();
};

class Parser final
{
public:
    Parser() = default;
    ~Parser() = default;

    static Library parse(const std::string& content);
};
} // namespace Antares::IO::Inputs::YmlModel
