#pragma once

#include <mutex>

#include <yuni/job/queue/service.h>
#include <yuni/job/job.h>

namespace Antares
{
namespace Solver
{
class ZipWriter;
class ZipWriteJob final : public Yuni::Job::IJob
{
public:
    ZipWriteJob(ZipWriter& writer, const std::string& path, const char* content, size_t size);
    virtual void onExecute() override;

private:
    // Pointer to Zip handle
    void* pHandle;
    // Protect pZipArchive against concurrent writes, since libzip isn't thread-safe
    std::mutex& pZipMutex;
    // File path & content
    std::string pPath;
    // Data to write
    std::vector<char> pContent;
};

class ZipWriter
{
public:
    ZipWriter(Yuni::Job::QueueService& qs, const char* path);
    ~ZipWriter();
    void addJob(const std::string& path, const char* content, size_t size);
    friend class ZipWriteJob;

private:
    Yuni::Job::QueueService& pQueueService;
    std::mutex pZipMutex;
    void* pHandle;
};
} // namespace Solver
} // namespace Antares
