#pragma once

#include <mutex>
#include <string>

#include <yuni/job/queue/service.h>
#include <yuni/job/job.h>
#include <yuni/core/string.h>

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
                Yuni::Clob& content);
    virtual void onExecute() override;

private:
    // Pointer to Zip handle
    void* pZipHandle;
    // Protect pZipHandle against concurrent writes, since minizip-ng isn't thread-safe
    std::mutex& pZipMutex;
    // Entry path for the new file within the zip archive
    const std::string pEntryPath;
    // Content of the new file
    Yuni::Clob pContent;
};

class ZipWriter : public IResultWriter
{
public:
    ZipWriter(Yuni::Job::QueueService& qs, const char* archivePath);
    ~ZipWriter();
    void addJob(const std::string& entryPath, Yuni::Clob& entryContent) override;
    bool needsTheJobQueue() const override;
    friend class ZipWriteJob;

private:
    Yuni::Job::QueueService& pQueueService;
    std::mutex pZipMutex;
    void* pZipHandle;
    const std::string pArchivePath;
};
} // namespace Solver
} // namespace Antares
