#pragma once

#include <mutex>

#include <yuni/job/queue/service.h>
#include <yuni/job/job.h>
#include <libzippp.h>

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
    // Pointer to Zip object
    libzippp::ZipArchive* pZipArchive;
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
    ZipWriter(Yuni::Job::QueueService& qs, libzippp::ZipArchive* archive);
    void addJob(const std::string& path, const char* content, size_t size);
    friend class ZipWriteJob;

private:
    Yuni::Job::QueueService& pQueueService;
    libzippp::ZipArchive* pZipArchive;
    std::mutex pZipMutex;
};
} // namespace Solver
} // namespace Antares
