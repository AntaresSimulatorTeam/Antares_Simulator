#include "file_content.h"

using namespace std;

namespace Benchmarking
{
	FileContent::iterator FileContent::firstSection()
	{
		return sections_.begin();
	}

	FileContent::iterator FileContent::endSections()
	{
		return sections_.end();
	}

	void FileContent::addItemToSection(string section, string key, int value)
	{
		sections_[section][key] = to_string(value);
	}

	void FileContent::addItemToSection(string section, string key, string value)
	{
		sections_[section][key] = value;
	}

	void FileContent::addDurationItem(std::string name, unsigned int duration, int nbCalls)
	{
		sections_["Durations"][name + "--duration"] = to_string(duration);
		sections_["Durations"][name + "--nb_calls"] = to_string(nbCalls);
	}
}
