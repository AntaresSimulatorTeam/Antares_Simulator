// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/concurrency/concurrency.h"

#include "ensure_queue_started.h"

namespace Antares::Solver
{
template<class ContentType>
void ZipWriter::addEntryFromBufferHelper(const std::filesystem::path& entryPath,
                                         ContentType& entryContent)
{
    if (pState != ZipState::can_receive_data)
    {
        return;
    }

    EnsureQueueStartedIfNeeded ensureQueue(this, pQueueService);
    pendingTasks_.add(Concurrency::AddTask(
      *pQueueService,
      ZipWriteJob<ContentType>(*this, entryPath.string(), entryContent, pDurationCollector),
      Yuni::Job::priorityLow));
}

} // namespace Antares::Solver
