#pragma once

#include <string>
#include <vector>
#include <string_view>

#ifdef __GNUC__
#if __GNUC__ > 7
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif
#endif


void remove_files(const std::vector<std::string>& filesToRemove);
std::string readFileIntoString(const std::string& path);
bool files_identical(const std::string& fileName_1, const std::string& fileName_2);
bool fileContainsLine(const std::string& fileName, std::string_view line_to_find);