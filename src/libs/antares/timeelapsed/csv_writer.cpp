#include "csv_writer.h"
#include "../logs.h"

namespace TimeElapsed
{
bool CSVWriter::setOutputFile(const Yuni::String& filename)
{
    if (!mOutputFile.openRW(filename))
    {
        Antares::logs.error() << "I/O error: " << filename
                              << ": Impossible to write the file (not enough permission ?)";
        return false;
    }
    return true;
}

void CSVWriter::flush(const TimeInfoMapType& imt)
{
    mOutputFile << "#item\tduration_ms\tNumber of calls\n";
    for (const auto& it : imt)
    {
        const auto& label = it.first;
        const auto& info = it.second;
        mOutputFile << label << "\t" << info.duration_ms_ << "\t" << info.numberOfCalls_ << "\n";
    }
}
} // namespace TimeElapsed
