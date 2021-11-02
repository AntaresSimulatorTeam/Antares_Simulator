#include <fstream>
#include <string_view>
#include "files-helper.h"

using namespace std;


// ==================================
// Remove a list of files
// ==================================

void remove_files(const vector<string>& filesToRemove)
{
	for (int i = 0; i != filesToRemove.size(); i++)
	{
		fs::path fileToRemove = filesToRemove[i];
		if (fs::exists(fileToRemove))
			fs::remove(fileToRemove);
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
	if (string fileContent = readFileIntoString(fileName); fileContent.find(line_to_find) == string::npos)
		return false;
	return true;
}
