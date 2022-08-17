#ifndef __BENCHMARKING_FILE_CONTENT_HXX__
#define __BANCHMARKING_FILE_CONTENT_HXX__
#include <yuni/yuni.h>
#include <antares/inifile.h>

namespace Benchmarking {
template<>
// inlining is necessary here to avoid multiple definitions
inline Antares::IniFile FileContent::to<Antares::IniFile>()
{
    Antares::IniFile ini;
    auto it_section = this->firstSection();
    // Loop on sections
    for (; it_section != this->endSections(); it_section++)
    {
        auto* section = ini.addSection(it_section->first);
        // Loop on properties
        for (const auto& line : it_section->second)
            section->add(line.first, line.second);
    }
    return ini;
}
}
#endif
