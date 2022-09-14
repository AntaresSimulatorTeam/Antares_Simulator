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
    // Loop on sections
    for (const auto& [title, sections] : *this)
    {
        auto* section = ini.addSection(title);
        // Loop on properties
        for (const auto& line : sections)
            section->add(line.first, line.second);
    }
    return ini;
}
}
#endif
