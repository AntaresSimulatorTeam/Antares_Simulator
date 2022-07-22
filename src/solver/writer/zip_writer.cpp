#include <antares/logs.h>
#include <antares/timeelapsed.h>

#include "zip_writer.h"

namespace Antares
{
namespace Solver
{
// Class ZipWriteJob
ZipWriteJob::ZipWriteJob(ZipWriter& writer,
                         const std::string& path,
                         const char* content,
                         size_t size) :
 pZipArchive(writer.pZipArchive), pZipMutex(writer.pZipMutex), pPath(path)
{
    // We need to copy the content, since it will be de-allocated right after
    // or overwritten. RAM usage may be high in some cases.
    pContent.assign(content, content + size);
}

void ZipWriteJob::onExecute()
{
    TimeElapsed::Timer timer_wait("[zip] Waiting", "[zip] Waited", true);

    std::lock_guard<std::mutex> guard(pZipMutex);
    timer_wait.stop();
    TimeElapsed::Timer timer_write("[zip] Writing", "[zip] Wrote took", true);
    pZipArchive->open(libzippp::ZipArchive::Write);
    pZipArchive->addData(pPath, pContent.data(), pContent.size());
    pZipArchive->close();
    timer_write.stop();
}

// Class ZipWriter
ZipWriter::ZipWriter(Yuni::Job::QueueService& qs, libzippp::ZipArchive* archive) :
 pQueueService(qs), pZipArchive(archive)
{
}

void ZipWriter::addJob(const std::string& path, const char* content, size_t size)
{
    pQueueService.add(new ZipWriteJob(*this, path, content, size), Yuni::Job::priorityLow);
}
} // namespace Solver
} // namespace Antares
