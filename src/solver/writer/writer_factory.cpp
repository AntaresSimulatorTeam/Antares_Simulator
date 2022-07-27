#include "writer_factory.h"

namespace Antares
{
namespace Solver
{
IResultWriter::Ptr resultWriterFactory(Antares::Data::ResultFormat fmt,
                                       const YString& folderOutput,
                                       Yuni::Job::QueueService& qs)
{
    using namespace Antares::Data;
    switch (fmt)
    {
    case zipArchive:
        return std::make_shared<ZipWriter>(qs, folderOutput.c_str());
    case legacyFilesDirectories:
    default:
        return std::make_shared<ImmediateFileResultWriter>(folderOutput.c_str());
    }
}
} // namespace Solver
} // namespace Antares
