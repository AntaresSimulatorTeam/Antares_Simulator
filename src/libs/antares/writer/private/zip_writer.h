#pragma once

#include <mutex>
#include <string>

#include <yuni/job/queue/service.h>
#include <yuni/core/string.h>

#include "antares/writer/i_writer.h"
#include <antares/benchmarking/DurationCollector.h>
#include "antares/concurrency/concurrency.h"


namespace Antares::Solver
{
enum class ZipState
{
    can_receive_data,
    blocking
};

class ZipWriter;

/*!
 * In charge of writing one entry into the underlying zip.
 * May be used as a function object.
 */
template<class ContentT>
class ZipWriteJob
{
public:
    ZipWriteJob(ZipWriter& writer,
                std::string  entryPath,
                ContentT& content,
                Benchmarking::IDurationCollector& duration_collector);
    void writeEntry();
    void operator()() {
        writeEntry();
    }

private:
    // Pointer to Zip handle
    void* pZipHandle;
    // Protect pZipHandle against concurrent writes, since minizip-ng isn't thread-safe
    std::mutex& pZipMutex;
    // State
    ZipState& pState;
    // Entry path for the new file within the zip archive
    const std::string pEntryPath;
    // Content of the new file
    ContentT pContent;
    // Benchmarking. How long do we wait ? How long does the zip write take ?
    Benchmarking::IDurationCollector& pDurationCollector;
};

class ZipWriter : public IResultWriter
{
public:
    ZipWriter(std::shared_ptr<Yuni::Job::QueueService> qs,
              const char* archivePath,
              Benchmarking::IDurationCollector& duration_collector);
    virtual ~ZipWriter();
    void addEntryFromBuffer(const std::string& entryPath, Yuni::Clob& entryContent) override;
    void addEntryFromBuffer(const std::string& entryPath, std::string& entryContent) override;
    void addEntryFromFile(const std::string& entryPath, const std::string& filePath) override;
    void flush() override;
    bool needsTheJobQueue() const override;
    void finalize(bool verbose) override;
    friend class ZipWriteJob<Yuni::Clob>;
    friend class ZipWriteJob<std::string>;

private:
    // Queue where jobs will be appended
    std::shared_ptr<Yuni::Job::QueueService> pQueueService;
    // Prevent concurrent writes to the zip file
    std::mutex pZipMutex;
    // minizip-ng requires a void* as a zip handle.
    void* pZipHandle;
    // State, to allow/prevent new jobs being added to the queue
    ZipState pState;
    // Absolute path to the archive
    const std::string pArchivePath;
    // Benchmarking. Passed to jobs
    Benchmarking::IDurationCollector& pDurationCollector;

    Concurrency::FutureSet pendingTasks_;

private:
    template<class ContentType>
    void addEntryFromBufferHelper(const std::string& entryPath, ContentType& entryContent);
};
} // namespace Antares::Solver


#include "zip_writer.hxx"
