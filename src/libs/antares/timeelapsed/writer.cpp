#include "writer.h"

// FIXME : antares/logs.h
#include "../logs.h"

namespace TimeElapsed
{
bool FileWriter::setOutputFile(const Yuni::String& filename)
{
    if (!mOutputFile.openRW(filename))
    {
        Antares::logs.error() << "I/O error: " << filename
                              << ": Impossible to write the file (not enough permission ?)";
        return false;
    }
    return true;
}

void FileWriter::flush(const InfoMapType& imt)
{
    mOutputFile << "#item\tduration_ms\tNbOccurences\n";
    for (const auto& it : imt)
    {
        const auto& label = it.first;
        const auto& info = it.second;
        mOutputFile << label << "\t" << info.duration_ms << "\t" << info.nCalls << "\n";
    }
}
} // namespace TimeElapsed
