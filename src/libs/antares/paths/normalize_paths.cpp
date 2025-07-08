#include "antares/paths/normalize_paths.h"

namespace fs = std::filesystem;

namespace Antares::Data
{

std::u8string to_u8string(const std::string& folder_path)
{
    const char* s = folder_path.c_str();
    std::u8string u8_string(s, s + std::strlen(s));
    return u8_string;
}

fs::path normalize(const std::string& folder_path)
{
    fs::path abspath = fs::absolute(to_u8string(folder_path));
    abspath = abspath.lexically_normal();
    return abspath;
}

std::string denormalize(const fs::path& folder_path)
{
    std::u8string u8_string = folder_path.u8string();
    std::string s(u8_string.cbegin(), u8_string.cend());
    return s;
}
} // namespace Antares::Data
