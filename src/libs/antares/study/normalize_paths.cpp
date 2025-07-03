#include "include/antares/study/normalize_paths.h"

namespace fs = std::filesystem;

namespace Antares::Data
{

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h> // For MultiByteToWideChar

static std::wstring to_wstring(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &wstr[0], size_needed);
    return wstr;
}

static std::string to_string(const std::wstring& wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8,
                                          0,
                                          wstr.data(),
                                          (int)wstr.size(),
                                          nullptr,
                                          0,
                                          nullptr,
                                          nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8,
                        0,
                        wstr.data(),
                        (int)wstr.size(),
                        &str[0],
                        size_needed,
                        nullptr,
                        nullptr);
    return str;
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

std::string denormalize(const fs::path& folder_path)
{
#if defined(_WIN32) || defined(_WIN64)
    return to_string(folder_path.wstring());
#else
    return folder_path.string();
#endif
}
} // namespace Antares::Data
