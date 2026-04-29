// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "private/ensure_queue_started.h"

namespace Antares::Solver
{
EnsureQueueStartedIfNeeded::EnsureQueueStartedIfNeeded(IResultWriter* writer,
                                                       std::shared_ptr<Yuni::Job::QueueService> qs):
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
} // namespace Antares::Solver
