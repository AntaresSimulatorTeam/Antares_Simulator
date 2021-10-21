#pragma once

#include <string>
#include <string_view>

using namespace std;

void remove_files(const vector<string>& filesToRemove);
string readFileIntoString(const string& path);
bool files_identical(const string& fileName_1, const string& fileName_2);
bool fileContainsLine(const string& fileName, string_view line_to_find);