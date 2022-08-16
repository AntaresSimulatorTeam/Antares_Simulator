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

	void FileContent::addItemToSection(const string& section, const string& key, int value)
	{
		sections_[section][key] = to_string(value);
	}

	void FileContent::addItemToSection(const string& section, const string& key, const string& value)
	{
		sections_[section][key] = value;
	}

	void FileContent::addDurationItem(const string& name, unsigned int duration, int nbCalls)
	{
		addItemToSection("Durations", name + "_duration", duration);
		addItemToSection("Durations", name + "_nb_calls", nbCalls);
	}
}
