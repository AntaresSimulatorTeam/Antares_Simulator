#include "file_writer.h"
#include <antares/exception/LoadingError.hpp>

using namespace Antares::Data;

namespace Benchmarking
{
	
	FileWriter::FileWriter(FileContent& fileContent) : fileContent_(fileContent) {}
	
	FileCSVwriter::FileCSVwriter(Yuni::String& filePath, FileContent& fileContent)
		: FileWriter(fileContent), filePath_(filePath)
	{}
	
	
	void FileCSVwriter::flush()
	{
		if (!outputFile_.openRW(filePath_))
		{
			throw Antares::Error::CreatingStudyInfoFile(filePath_);
		}

		FileContent::iterator it = fileContent_.begin();
		for (; it != fileContent_.end(); it++)
			outputFile_ << *it << "\n";
	}
}
