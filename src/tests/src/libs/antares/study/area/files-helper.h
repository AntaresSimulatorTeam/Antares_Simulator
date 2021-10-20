#pragma once

#include <filesystem>

namespace fs = std::filesystem;

// ==================================
// Remove a list of files
// ==================================

void clean_output(const vector<string>& filesToRemove)
{
	for (int i = 0; i != filesToRemove.size(); i++)
	{
		fs::path fileToRemove = filesToRemove[i];
		if (exists(fileToRemove))
			remove(fileToRemove);
	}
}

// ================
// Compare files
// ================

string readFileIntoString(const string& path) {
	ifstream input_file;
	input_file.open(path);
	return string(istreambuf_iterator<char>(input_file), istreambuf_iterator<char>());
}

bool compare_files(const string& fileName_1, const string& fileName_2)
{
	string content_1 = readFileIntoString(fileName_1);
	string content_2 = readFileIntoString(fileName_2);
	if (content_1.compare(content_2))
		return false;
	return true;
}

// ============================
// Finds a string in a file
// ============================

bool fileContainsLine(const string& fileName, const string& line_to_find)
{
	string fileContent = readFileIntoString(fileName);
	if (fileContent.find(line_to_find) == string::npos)
		return false;
	return true;
}
