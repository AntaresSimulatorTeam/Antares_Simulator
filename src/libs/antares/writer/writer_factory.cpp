#include "antares/writer/writer_factory.h"
#include "private/immediate_file_writer.h"
#include "private/zip_writer.h"

namespace Antares::Solver
{
IResultWriter::Ptr resultWriterFactory(Antares::Data::ResultFormat fmt,
                                       const YString& folderOutput,
                                       std::shared_ptr<Yuni::Job::QueueService> qs,
                                       Benchmarking::IDurationCollector& duration_collector)
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
} // namespace Antares::Solver

