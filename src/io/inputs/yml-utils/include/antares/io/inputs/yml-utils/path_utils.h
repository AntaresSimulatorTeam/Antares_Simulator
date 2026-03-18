// Copyright 2007-2026, RTE
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <string>

namespace Antares::IO::Inputs::YmlUtils
{

std::string printPathTree(const std::filesystem::path& p);

std::string getBaseTree(const std::filesystem::path& nodeTagPath);

} // namespace Antares::IO::Inputs::YmlUtils
