#pragma once

#include <string>
#include <vector>
#include <string_view>

void remove_files(const std::vector<std::string>& filesToRemove);
std::string readFileIntoString(const std::string& path);
bool files_identical(const std::string& fileName_1, const std::string& fileName_2);
bool fileContainsLine(const std::string& fileName, std::string_view line_to_find);