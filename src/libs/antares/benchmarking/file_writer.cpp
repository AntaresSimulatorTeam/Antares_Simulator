#include "file_writer.h"
#include "antares/inifile/inifile.h"

using namespace std;

namespace Benchmarking
{
	
	FileWriter::FileWriter(FileContent& fileContent) : fileContent_(fileContent) {}
	
	iniFilewriter::iniFilewriter(Yuni::String& filePath, FileContent& fileContent)
		: FileWriter(fileContent), filePath_(filePath)
	{}
	
	
	void iniFilewriter::flush()
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
