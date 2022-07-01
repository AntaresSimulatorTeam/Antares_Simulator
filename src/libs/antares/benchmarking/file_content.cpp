#include "file_content.h"

namespace Benchmarking
{
	FileContent::iterator FileContent::begin()
	{
		return lines_.begin();
	}

	FileContent::iterator FileContent::end()
	{
		return lines_.end();
	}

	void FileContent::addNameValueLine(std::string name, std::string value)
	{
		lines_.push_back(name + " : " + value);
	}

	void FileContent::addTitleLine(std::string title)
	{
		lines_.push_back(title);
	}

	void FileContent::addDurationLine(std::string name, unsigned int duration, int nbCalls)
	{
		addNameValueLine(name, std::to_string(duration) + "\t" + std::to_string(nbCalls));
	}

	void FileContent::addNameValueLine(std::string name, int value)
	{
		addNameValueLine(name, std::to_string(value));
	}
}
