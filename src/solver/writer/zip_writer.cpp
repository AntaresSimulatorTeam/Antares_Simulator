#include <antares/logs.h>
#include <antares/timeelapsed.h>

#include "zip_writer.h"

#include <mz.h>
#include <mz_zip.h>
#include <mz_strm.h>
#include <mz_zip_rw.h>

#include <ctime>

namespace Antares
{
namespace Solver
{
// Class ZipWriteJob
ZipWriteJob::ZipWriteJob(ZipWriter& writer,
                         const std::string& entryPath,
                         const char* entryContent,
                         size_t entrySize) :
 pZipMutex(writer.pZipMutex), pHandle(writer.pHandle), pEntryPath(entryPath)
{
    /* We need to copy the content, since it will be de-allocated right after
       or overwritten. RAM usage may be high in some cases, especially if disk
       writes are slow.
       TODO : use swap / std::move to avoid this copy
     */
    pContent.assign(entryContent, entryContent + entrySize);
}

void ZipWriteJob::onExecute()
{
    TimeElapsed::Timer timer_wait("[zip] Waiting", "[zip] Waited", true);
    std::lock_guard<std::mutex> guard(pZipMutex);
    timer_wait.stop();
    TimeElapsed::Timer timer_write("[zip] Writing", "[zip] Wrote took", true);
    mz_zip_file file_info = {0};
    file_info.filename = pEntryPath.c_str();
    file_info.compression_method = MZ_COMPRESS_METHOD_DEFLATE;
    file_info.modified_date = file_info.creation_date = time(0);
    mz_zip_writer_entry_open(pHandle, &file_info);
    mz_zip_writer_entry_write(pHandle, pContent.data(), pContent.size());
    timer_write.stop();
}

// Class ZipWriter
ZipWriter::ZipWriter(Yuni::Job::QueueService& qs, const char* archivePath) :
 pQueueService(qs), pArchivePath(std::string(archivePath) + ".zip")
{
    mz_zip_writer_create(&pHandle);
    mz_zip_writer_open_file(pHandle, pArchivePath.c_str(), 0, 0);
    // TODO : make level of compression configurable
    mz_zip_writer_set_compress_level(pHandle, MZ_COMPRESS_LEVEL_FAST);
}

ZipWriter::~ZipWriter()
{
    mz_zip_writer_close(pHandle);
    mz_zip_writer_delete(&pHandle);
}

void ZipWriter::addJob(const std::string& entryPath, const char* entryContent, size_t entrySize)
{
    pQueueService.add(new ZipWriteJob(*this, entryPath, entryContent, entrySize),
                      Yuni::Job::priorityLow);
}
} // namespace Solver
} // namespace Antares
