#pragma once

#include <mutex>

#include <yuni/job/queue/service.h>
#include <yuni/job/job.h>

#include "i_writer.h"

namespace Antares
{
namespace Solver
{
class ZipWriter;
class ZipWriteJob final : public Yuni::Job::IJob
{
public:
    ZipWriteJob(ZipWriter& writer,
                const std::string& entryPath,
                const char* entryContent,
                size_t entrySize);
    virtual void onExecute() override;

private:
    // Pointer to Zip handle
    void* pZipHandle;
    // Protect pZipArchive against concurrent writes, since minizip-ng isn't thread-safe
    std::mutex& pZipMutex;
    // Entry path for the new file within the zip archive
    const std::string pEntryPath;
    // Content of the new file
    std::vector<char> pContent;
};

class ZipWriter : public IResultWriter
{
public:
    ZipWriter(Yuni::Job::QueueService& qs, const char* archivePath);
    ~ZipWriter();
    void addJob(const std::string& entryPath, const char* entryContent, size_t entrySize) override;
    friend class ZipWriteJob;

private:
    Yuni::Job::QueueService& pQueueService;
    std::mutex pZipMutex;
    void* pZipHandle;
    const std::string pArchivePath;
};
} // namespace Solver
} // namespace Antares
