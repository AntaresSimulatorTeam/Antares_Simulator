#include <antares/logs.h>
#include <antares/timeelapsed.h>

#include "zip_writer.h"

#include <mz_zip.h>
#include <mz_strm.h>
#include <mz_zip_rw.h>

namespace Antares
{
namespace Solver
{
// Class ZipWriteJob
ZipWriteJob::ZipWriteJob(ZipWriter& writer,
                         const std::string& path,
                         const char* content,
                         size_t size) :
  pZipMutex(writer.pZipMutex), pHandle(writer.pHandle), pPath(path)
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
    mz_zip_file file_info = {0};
    file_info.filename = pPath.c_str();
    // TODO : set creation date, etc.
    mz_zip_writer_entry_open(pHandle, &file_info);
    mz_zip_writer_entry_write(pHandle, pContent.data(), pContent.size());
    timer_write.stop();
}

// Class ZipWriter
ZipWriter::ZipWriter(Yuni::Job::QueueService& qs, const char* path) :
 pQueueService(qs)
{
    mz_zip_writer_create(&pHandle);
    // TODO : try mz_zip_writer_open_file_in_memory
    mz_zip_writer_open_file(pHandle, path, 0, 0);
}

ZipWriter::~ZipWriter()
{
    mz_zip_writer_close(pHandle);
    mz_zip_writer_delete(&pHandle);
}

void ZipWriter::addJob(const std::string& path, const char* content, size_t size)
{
    pQueueService.add(new ZipWriteJob(*this, path, content, size), Yuni::Job::priorityLow);
}
} // namespace Solver
} // namespace Antares
