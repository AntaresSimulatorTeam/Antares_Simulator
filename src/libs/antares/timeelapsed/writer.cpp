#include "writer.h"

// FIXME : antares/logs.h
#include "../logs.h"

namespace TimeElapsed
{
FileWriter::FileWriter(const Yuni::String& filename) : mFilename(filename)
{
}

bool FileWriter::checkAndPrepareOutput()
{
    if (not mOutputFile.openRW(mFilename))
    {
        Antares::logs.error() << "I/O error: " << mFilename
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
