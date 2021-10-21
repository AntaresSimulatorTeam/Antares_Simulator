#pragma once

#include <string>
using namespace std;

void remove_files(const vector<string>& filesToRemove);
string readFileIntoString(const string& path);
bool files_identical(const string& fileName_1, const string& fileName_2);
bool fileContainsLine(const string& fileName, const string& line_to_find);