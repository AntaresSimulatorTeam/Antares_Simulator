#pragma once

#include <filesystem>
#include <string>

namespace Antares::Data
{
std::filesystem::path normalize(const std::string& folder_path);
std::string to_utf8_string(const std::filesystem::path& folder_path);
} // namespace Antares::Data
