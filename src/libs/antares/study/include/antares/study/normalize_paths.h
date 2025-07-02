#pragma once

#include <filesystem>
#include <string>

namespace Antares::Data
{
std::filesystem::path normalize(const std::string& folder_path);
}
