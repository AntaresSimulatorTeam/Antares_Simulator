#include "include/antares/study/normalize_paths.h"

namespace fs = std::filesystem;

namespace Antares::Data
{

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h> // For MultiByteToWideChar

static std::wstring to_wstring(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
    return wstr;
}
#endif

fs::path normalize(const std::string& folder_path)
{
#if defined(_WIN32) || defined(_WIN64)
    fs::path abspath = fs::absolute(to_wstring(folder_path));
#else
    fs::path abspath = fs::absolute(folder_path);
#endif
    abspath = abspath.lexically_normal();
    return abspath;
}
} // namespace Antares::Data
