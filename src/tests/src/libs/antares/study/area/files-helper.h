#pragma once

#include <string>
#include <vector>
#include <string_view>

// Using GCC
#ifdef __GNUC__
// Recent versions of GCC support std::filesystem
#if __GNUC__ > 7
#include <filesystem>
namespace fs = std::filesystem;
#else
// Older versions support std::experimental::filesystem
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif
// Not using GCC
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif


void remove_files(const std::vector<std::string>& filesToRemove);
std::string readFileIntoString(const std::string& path);
bool files_identical(const std::string& fileName_1, const std::string& fileName_2);
bool fileContainsLine(const std::string& fileName, std::string_view line_to_find);