// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <yuni/job/queue/service.h>

#include "antares/writer/i_writer.h"

namespace Antares::Solver
{
class EnsureQueueStartedIfNeeded final
{
public:
    explicit EnsureQueueStartedIfNeeded(IResultWriter* writer,
                                        std::shared_ptr<Yuni::Job::QueueService> qs);
    ~EnsureQueueStartedIfNeeded();

private:
    std::shared_ptr<Yuni::Job::QueueService> qs;
    bool startQueue;
};
} // namespace Antares::Solver
