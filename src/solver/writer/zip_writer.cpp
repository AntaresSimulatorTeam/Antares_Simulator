#include <antares/logs.h>
#include <antares/timeelapsed.h>

#include "zip_writer.h"

extern "C"
{
#include <mz.h>
#include <mz_zip.h>
#include <mz_strm.h>
#include <mz_zip_rw.h>
}

#include <ctime>

namespace Antares
{
namespace Solver
{
// Class ZipWriteJob
ZipWriteJob::ZipWriteJob(ZipWriter& writer,
                         const std::string& entryPath,
                         Yuni::Clob& content) :
  pZipMutex(writer.pZipMutex), pZipHandle(writer.pZipHandle), pEntryPath(entryPath), pContent(std::move(content))
{
}

void ZipWriteJob::onExecute()
{
    TimeElapsed::Timer timer_wait("[zip] Waiting", "[zip] Waited", true);
    std::lock_guard<std::mutex> guard(pZipMutex);
    timer_wait.stop();
    TimeElapsed::Timer timer_write(
      "[zip] Writing " + pEntryPath, "[zip] Wrote " + pEntryPath + " ", true);
    mz_zip_file file_info;
    memset(&file_info, 0, sizeof(file_info));
    file_info.filename = pEntryPath.c_str();
    file_info.zip64 = MZ_ZIP64_FORCE;
    file_info.compression_method = MZ_COMPRESS_METHOD_DEFLATE;
    file_info.modified_date = file_info.creation_date = time(0);
    mz_zip_writer_entry_open(pZipHandle, &file_info);
    mz_zip_writer_entry_write(pZipHandle, pContent.data(), pContent.size());
    timer_write.stop();
}

// Class ZipWriter
ZipWriter::ZipWriter(Yuni::Job::QueueService& qs, const char* archivePath) :
 pQueueService(qs), pArchivePath(std::string(archivePath) + ".zip")
{
    mz_zip_writer_create(&pZipHandle);
    mz_zip_writer_open_file(pZipHandle, pArchivePath.c_str(), 0, 0);
    // TODO : make level of compression configurable
    mz_zip_writer_set_compress_level(pZipHandle, MZ_COMPRESS_LEVEL_FAST);
}

ZipWriter::~ZipWriter()
{
    mz_zip_writer_close(pZipHandle);
    mz_zip_writer_delete(&pZipHandle);
}

void ZipWriter::addJob(const std::string& entryPath, Yuni::Clob& entryContent)
{
    pQueueService.add(new ZipWriteJob(*this, entryPath, entryContent),
                      Yuni::Job::priorityLow);
}

bool ZipWriter::needsTheJobQueue() const
{
    return true;
}
} // namespace Solver
} // namespace Antares
