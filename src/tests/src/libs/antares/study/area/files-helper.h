// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <string_view>
#include <vector>

void remove_files(const std::vector<std::string>& filesToRemove);
std::string readFileIntoString(const std::string& path);
bool files_identical(const std::string& fileName_1, const std::string& fileName_2);
bool fileContainsLine(const std::string& fileName, std::string_view line_to_find);
