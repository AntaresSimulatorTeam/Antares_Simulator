#include "file_writer.h"
#include "antares/inifile/inifile.h"

using namespace std;

namespace Benchmarking
{
FileWriter::FileWriter(FileContent& fileContent) : fileContent_(fileContent)
{
}

iniFilewriter::iniFilewriter(FileContent& fileContent) : FileWriter(fileContent)
{
}

void iniFilewriter::saveToBuffer(std::string& buffer) const
{
    Antares::IniFile ini;

    FileContent::iterator it_section = fileContent_.firstSection();
    for (; it_section != fileContent_.endSections(); it_section++)
    {
        auto* section = ini.addSection(it_section->first);
        for (pair<string, string> line : it_section->second)
            section->add(line.first, line.second);
    }
    ini.saveToString(buffer);
}
} // namespace Benchmarking
