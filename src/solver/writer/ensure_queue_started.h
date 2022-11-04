#pragma once
#include "i_writer.h"

#include <yuni/job/queue/service.h>

namespace Antares
{
namespace Solver
{
class EnsureQueueStartedIfNeeded
{
public:
    explicit EnsureQueueStartedIfNeeded(IResultWriter* writer,
                                        std::shared_ptr<Yuni::Job::QueueService> qs);
    ~EnsureQueueStartedIfNeeded();

private:
    std::shared_ptr<Yuni::Job::QueueService> qs;
    bool startQueue;
};
} // namespace Solver
} // namespace Antares
