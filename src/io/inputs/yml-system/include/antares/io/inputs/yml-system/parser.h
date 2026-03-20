// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "antares/io/inputs/yml-system/system.h"

namespace Antares::IO::Inputs::YmlSystem
{
class Parser final
{
public:
    System parse(const std::string& content);
};
} // namespace Antares::IO::Inputs::YmlSystem
