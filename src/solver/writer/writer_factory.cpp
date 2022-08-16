#include "writer_factory.h"

namespace Antares
{
namespace Solver
{
IResultWriter::Ptr resultWriterFactory(Antares::Data::ResultFormat fmt,
                                       const YString& folderOutput,
                                       Yuni::Job::QueueService& qs,
                                       Benchmarking::IDurationCollector* duration_collector)
{
    using namespace Antares::Data;
    switch (fmt)
    {
    case zipArchive:
        return std::make_shared<ZipWriter>(qs, folderOutput.c_str(), duration_collector);
    case legacyFilesDirectories:
    default:
        return std::make_shared<ImmediateFileResultWriter>(folderOutput.c_str());
    }
}
} // namespace Solver
} // namespace Antares
