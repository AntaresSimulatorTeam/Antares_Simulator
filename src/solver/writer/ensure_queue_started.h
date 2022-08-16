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
    explicit EnsureQueueStartedIfNeeded(IResultWriter::Ptr writer, Yuni::Job::QueueService& qs);
    ~EnsureQueueStartedIfNeeded();

private:
    Yuni::Job::QueueService& qs;
    bool startQueue;
};
} // namespace Solver
} // namespace Antares
