#include "file_content.h"

namespace Benchmarking
{
	FileContent::iterator FileContent::begin()
	{
		return items_.begin();
	}

	FileContent::iterator FileContent::end()
	{
		return items_.end();
	}

	void FileContent::addItem(FileContentLine* item)
	{
		items_.emplace_back(item);
	}
}
