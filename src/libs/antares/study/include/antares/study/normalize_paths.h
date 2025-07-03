#pragma once

#include <filesystem>
#include <string>

namespace Antares::Data
{
std::filesystem::path normalize(const std::string& folder_path);
std::string denormalize(const std::filesystem::path& folder_path);
} // namespace Antares::Data
