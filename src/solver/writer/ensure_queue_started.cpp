#include "ensure_queue_started.h"

namespace Antares
{
namespace Solver
{
EnsureQueueStartedIfNeeded::EnsureQueueStartedIfNeeded(
  IResultWriter* writer,
  std::shared_ptr<Yuni::Job::QueueService> qs) :
 qs(qs)
{
    startQueue = !qs->started() && writer->needsTheJobQueue();
    if (startQueue)
    {
        qs->start();
    }
}
EnsureQueueStartedIfNeeded::~EnsureQueueStartedIfNeeded()
{
    if (startQueue)
    {
        qs->wait(Yuni::qseIdle);
        qs->stop();
    }
}
} // namespace Solver
} // namespace Antares
