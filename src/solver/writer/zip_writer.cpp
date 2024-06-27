#include <memory>
#include <antares/logs.h>
#include <yuni/io/file.h> // Yuni::IO::File::LoadFromFile

#include "zip_writer.h"

extern "C"
{
#include <mz.h>
#include <mz_zip.h>
#include <mz_strm.h>
#include <mz_zip_rw.h>
}

#include <ctime> // std::time

namespace Antares::Solver
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

static std::unique_ptr<mz_zip_file> createInfo(const std::string& entryPath)
{
    auto info = std::make_unique<mz_zip_file>();
    memset(info.get(), 0, sizeof(mz_zip_file));
    info->filename = entryPath.c_str();
    info->zip64 = MZ_ZIP64_FORCE;
    info->compression_method = MZ_COMPRESS_METHOD_DEFLATE;
    info->modified_date = info->creation_date = std::time(0);
    return info;
}

template<class ContentT>
void ZipWriteJob<ContentT>::onExecute()
{
    // Don't write data if finalize() has been called
    if (pState != ZipState::can_receive_data)
        return;

    auto file_info = createInfo(pEntryPath);

    Benchmarking::Timer timer_wait;
    std::lock_guard<std::mutex> guard(pZipMutex); // Wait
    timer_wait.stop();
    if (pDurationCollector)
        pDurationCollector->addDuration("zip_wait", timer_wait.get_duration());

    Benchmarking::Timer timer_write;

    if (int32_t ret = mz_zip_writer_entry_open(pZipHandle, file_info.get()); ret != MZ_OK)
        logs.error() << "Error opening entry " << pEntryPath << " (" << ret << ")";

    int32_t bw = mz_zip_writer_entry_write(pZipHandle, pContent.data(), pContent.size());
    if (static_cast<unsigned int>(bw) != pContent.size())
    {
        logs.error() << "Error writing entry " << pEntryPath << "(written = " << bw
                     << ", size = " << pContent.size() << ")";
    }

    timer_write.stop();
    if (pDurationCollector)
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
    pZipHandle = mz_zip_writer_create();
    if (int32_t ret = mz_zip_writer_open_file(pZipHandle, pArchivePath.c_str(), 0, 0); ret != MZ_OK)
    {
        logs.error() << "Error opening zip file " << pArchivePath << " (" << ret << ")";
    }
    // TODO : make level of compression configurable
    mz_zip_writer_set_compress_level(pZipHandle, MZ_COMPRESS_LEVEL_FAST);
}

ZipWriter::~ZipWriter()
{
    if (!pZipHandle)
        return;

    try
    {
        this->finalize(false);
    }
    catch (...)
    {
        // Catch all, do nothing
    }
}

void ZipWriter::addEntryFromBuffer(const std::string& entryPath, Yuni::Clob& entryContent)
{
    addEntryFromBufferHelper<Yuni::Clob>(entryPath, entryContent);
}

void ZipWriter::addEntryFromBuffer(const std::string& entryPath, std::string& entryContent)
{
    addEntryFromBufferHelper<std::string>(entryPath, entryContent);
}

void ZipWriter::addEntryFromFile(const std::string& entryPath, const std::string& filePath)
{
    // Read file into buffer immediately, write into archive async
    Yuni::Clob buffer;
    switch (Yuni::IO::File::LoadFromFile(buffer, filePath.c_str()))
    {
        using namespace Yuni::IO;
    case errNone:
        addEntryFromBufferHelper<Yuni::Clob>(entryPath, buffer);
        break;
    case errNotFound:
        logs.error() << filePath << ": file does not exist";
        break;
    case errReadFailed:
        logs.error() << "Read failed '" << filePath << "'";
        break;
    case errMemoryLimit:
        logs.error() << "Size limit hit for file '" << filePath << "'";
        break;
    default:
        logs.error() << "Unhandled error";
        break;
    }
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
    if (int ret = mz_zip_writer_close(pZipHandle); ret != MZ_OK && verbose)
        logs.warning() << "Error closing the zip file " << pArchivePath << " (" << ret << ")";

    mz_zip_writer_delete(&pZipHandle);
    pZipHandle = nullptr;

    if (verbose)
        logs.notice() << "Done";
}
} // namespace Antares::Solver
