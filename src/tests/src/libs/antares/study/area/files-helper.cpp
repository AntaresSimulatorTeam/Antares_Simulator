/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "files-helper.h"

#include <filesystem>
#include <fstream>
#include <string_view>

using namespace std;
namespace fs = std::filesystem;

// ==================================
// Remove a list of files
// ==================================

void remove_files(const vector<string>& filesToRemove)
{
    for (const auto& fileToRemove: filesToRemove)
    {
        if (fs::exists(fileToRemove))
        {
            fs::remove(fileToRemove);
        }
    }
}

// ================
// Compare files
// ================

string readFileIntoString(const string& path)
{
    ifstream input_file;
    input_file.open(path);
    return string(istreambuf_iterator<char>(input_file), istreambuf_iterator<char>());
}

bool files_identical(const string& fileName_1, const string& fileName_2)
{
    string content_1 = readFileIntoString(fileName_1);
    string content_2 = readFileIntoString(fileName_2);
    return content_1 == content_2;
}

// ============================
// Finds a string in a file
// ============================

bool fileContainsLine(const string& fileName, string_view line_to_find)
{
    if (string fileContent = readFileIntoString(fileName);
        fileContent.find(line_to_find) == string::npos)
    {
        return false;
    }
    return true;
}
