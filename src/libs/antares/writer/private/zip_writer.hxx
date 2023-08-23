#pragma once

#include "ensure_queue_started.h"

namespace Antares::Solver
{
template<class ContentType>
void ZipWriter::addEntryFromBufferHelper(const std::string& entryPath, ContentType& entryContent)
{
    if (pState != ZipState::can_receive_data)
        return;

    EnsureQueueStartedIfNeeded ensureQueue(this, pQueueService);
    pQueueService->add(
      new ZipWriteJob<ContentType>(*this, entryPath, entryContent, pDurationCollector),
      Yuni::Job::priorityLow);
}
} // namespace Antares::Solver
