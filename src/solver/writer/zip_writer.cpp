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
 pState(writer.pState),
 pEntryPath(entryPath),
 pContent(std::move(content)),
 pDurationCollector(duration_collector)
{
}

template<class ContentT>
void ZipWriteJob<ContentT>::onExecute()
{
    // Don't write data if finalize() has been called
    if (pState != ZipState::can_receive_data)
    {
        return;
    }

    mz_zip_file file_info;

    Benchmarking::Timer timer_wait;
    std::lock_guard<std::mutex> guard(pZipMutex); // Wait
    timer_wait.stop();
    pDurationCollector->addDuration("zip_wait", timer_wait.get_duration());

    memset(&file_info, 0, sizeof(file_info));
    file_info.filename = pEntryPath.c_str();
    file_info.zip64 = MZ_ZIP64_FORCE;
    file_info.compression_method = MZ_COMPRESS_METHOD_DEFLATE;
    file_info.modified_date = file_info.creation_date = time(0);
    Benchmarking::Timer timer_write;
    mz_zip_writer_entry_open(pZipHandle, &file_info);
    mz_zip_writer_entry_write(pZipHandle, pContent.data(), pContent.size());
    timer_write.stop();
    pDurationCollector->addDuration("zip_write", timer_write.get_duration());
}

// Class ZipWriter
ZipWriter::ZipWriter(std::shared_ptr<Yuni::Job::QueueService> qs,
                     const char* archivePath,
                     Benchmarking::IDurationCollector* duration_collector) :
 pQueueService(qs),
 pState(ZipState::can_receive_data),
 pArchivePath(std::string(archivePath) + ".zip"),
 pDurationCollector(duration_collector)
{
    mz_zip_writer_create(&pZipHandle);
    // TODO : check existence of file
    mz_zip_writer_open_file(pZipHandle, pArchivePath.c_str(), 0, 0);
    // TODO : make level of compression configurable
    mz_zip_writer_set_compress_level(pZipHandle, MZ_COMPRESS_LEVEL_FAST);
}

ZipWriter::~ZipWriter()
{
    this->finalize(false);
}

void ZipWriter::addJob(const std::string& entryPath, Yuni::Clob& entryContent)
{
    if (pState != ZipState::can_receive_data)
        return;

    EnsureQueueStartedIfNeeded ensureQueue(this, pQueueService);
    pQueueService->add(
      new ZipWriteJob<Yuni::Clob>(*this, entryPath, entryContent, pDurationCollector),
      Yuni::Job::priorityLow);
}

void ZipWriter::addJob(const std::string& entryPath, std::string& entryContent)
{
    if (pState != ZipState::can_receive_data)
        return;

    EnsureQueueStartedIfNeeded ensureQueue(this, pQueueService);
    pQueueService->add(
      new ZipWriteJob<std::string>(*this, entryPath, entryContent, pDurationCollector),
      Yuni::Job::priorityLow);
}

void ZipWriter::addJob(const std::string& entryPath, Antares::IniFile& entryContent)
{
    if (pState != ZipState::can_receive_data)
        return;

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

void ZipWriter::finalize(bool verbose)
{
    // Prevent new jobs from being submitted
    pState = ZipState::blocking;

    if (!pZipHandle)
        return;

    if (verbose)
        logs.notice() << "Writing results...";

    std::lock_guard<std::mutex> guard(pZipMutex);
    mz_zip_writer_close(pZipHandle);
    mz_zip_writer_delete(&pZipHandle);
    pZipHandle = nullptr;

    if (verbose)
        logs.notice() << "Done";
}

} // namespace Solver
} // namespace Antares
