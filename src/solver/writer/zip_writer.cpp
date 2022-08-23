#include <antares/logs.h>

#include "zip_writer.h"
#include "ensure_queue_started.h"

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
template<class ContentT>
ZipWriteJob<ContentT>::ZipWriteJob(ZipWriter& writer,
                                   const std::string& entryPath,
                                   ContentT& content,
                                   Benchmarking::IDurationCollector* duration_collector) :
 pZipHandle(writer.pZipHandle),
 pZipMutex(writer.pZipMutex),
 pEntryPath(entryPath),
 pContent(std::move(content)),
 pDurationCollector(duration_collector)
{
}

static void fillInfo(mz_zip_file& info, const std::string& entryPath)
{
    memset(&info, 0, sizeof(mz_zip_file));
    info.filename = entryPath.c_str();
    info.zip64 = MZ_ZIP64_FORCE;
    info.compression_method = MZ_COMPRESS_METHOD_DEFLATE;
    info.modified_date = info.creation_date = std::time(0);
}

template<class ContentT>
void ZipWriteJob<ContentT>::onExecute()
{
    int32_t bw, ret;
    mz_zip_file file_info;
    fillInfo(file_info, pEntryPath);

    Benchmarking::Timer timer_wait;
    std::lock_guard<std::mutex> guard(pZipMutex); // Wait
    timer_wait.stop();
    pDurationCollector->addDuration("zip_wait", timer_wait.get_duration());

    Benchmarking::Timer timer_write;
    ret = mz_zip_writer_entry_open(pZipHandle, &file_info);
    if (ret != MZ_OK)
    {
        logs.error() << "Error opening entry " << pEntryPath << " (" << ret << ")";
    }

    bw = mz_zip_writer_entry_write(pZipHandle, pContent.data(), pContent.size());
    if (static_cast<unsigned int>(bw) != pContent.size())
    {
        logs.error() << "Error writing entry " << pEntryPath << "(written = " << bw
                     << ", size = " << pContent.size() << ")";
    }

    timer_write.stop();
    pDurationCollector->addDuration("zip_write", timer_write.get_duration());
}

// Class ZipWriter
ZipWriter::ZipWriter(std::shared_ptr<Yuni::Job::QueueService> qs,
                     const char* archivePath,
                     Benchmarking::IDurationCollector* duration_collector) :
 pQueueService(qs),
 pArchivePath(std::string(archivePath) + ".zip"),
 pDurationCollector(duration_collector)
{
    mz_zip_writer_create(&pZipHandle);
    int32_t ret = mz_zip_writer_open_file(pZipHandle, pArchivePath.c_str(), 0, 0);
    if (ret != MZ_OK)
    {
        logs.error() << "Error opening zip file " << pArchivePath << " (" << ret << ")";
    }
    // TODO : make level of compression configurable
    mz_zip_writer_set_compress_level(pZipHandle, MZ_COMPRESS_LEVEL_FAST);
}

ZipWriter::~ZipWriter()
{
    int ret;
    ret = mz_zip_writer_close(pZipHandle);
    if (ret != MZ_OK)
    {
        logs.warning() << "Error closing the zip file " << pArchivePath << " (" << ret << ")";
    }
    mz_zip_writer_delete(&pZipHandle);
}

void ZipWriter::addJob(const std::string& entryPath, Yuni::Clob& entryContent)
{
    EnsureQueueStartedIfNeeded ensureQueue(this, pQueueService);
    pQueueService->add(
      new ZipWriteJob<Yuni::Clob>(*this, entryPath, entryContent, pDurationCollector),
      Yuni::Job::priorityLow);
}

void ZipWriter::addJob(const std::string& entryPath, std::string& entryContent)
{
    EnsureQueueStartedIfNeeded ensureQueue(this, pQueueService);
    pQueueService->add(
      new ZipWriteJob<std::string>(*this, entryPath, entryContent, pDurationCollector),
      Yuni::Job::priorityLow);
}

void ZipWriter::addJob(const std::string& entryPath, Antares::IniFile& entryContent)
{
    EnsureQueueStartedIfNeeded ensureQueue(this, pQueueService);
    std::string buffer;
    entryContent.saveToString(buffer);
    pQueueService->add(new ZipWriteJob<std::string>(*this, entryPath, buffer, pDurationCollector),
                       Yuni::Job::priorityLow);
}

bool ZipWriter::needsTheJobQueue() const
{
    return true;
}
} // namespace Solver
} // namespace Antares
