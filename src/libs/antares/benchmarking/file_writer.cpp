#include "file_writer.h"
#include <antares/exception/LoadingError.hpp>
#include "antares/inifile/inifile.h"

using namespace Antares::Data;
using namespace std;

namespace Benchmarking
{
	
	FileWriter::FileWriter(FileContent& fileContent) : fileContent_(fileContent) {}
	
	FileCSVwriter::FileCSVwriter(Yuni::String& filePath, FileContent& fileContent)
		: FileWriter(fileContent), filePath_(filePath)
	{}
	
	
	void FileCSVwriter::flush()
	{
		Antares::IniFile ini;

		FileContent::iterator it_section = fileContent_.firstSection();
		for (; it_section != fileContent_.endSections(); it_section++)
		{
			Antares::IniFile::Section* section = ini.addSection(it_section->first);
			for (pair<string, string> line : it_section->second)
				section->add(line.first, line.second);
		}

		ini.save(filePath_);
	}
}
