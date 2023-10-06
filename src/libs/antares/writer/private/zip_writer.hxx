#pragma once

#include "antares/concurrency/concurrency.h"
#include "ensure_queue_started.h"

namespace Antares::Solver
{
template<class ContentType>
void ZipWriter::addEntryFromBufferHelper(const std::string& entryPath, ContentType& entryContent)
{
    if (pState != ZipState::can_receive_data)
        return;

    EnsureQueueStartedIfNeeded ensureQueue(this, pQueueService);
    Antares::Concurrency::AddTask(*pQueueService,
                                  ZipWriteJob<ContentType>(*this, entryPath, entryContent, pDurationCollector),
                                  Yuni::Job::priorityLow);
}

} // namespace Antares::Solver
