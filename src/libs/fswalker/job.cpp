// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "job.h"

namespace FSWalker
{
void IJob::decrementCounter()
{
    auto counter = pJobCounter;
    if (!(!counter))
    {
        --(*counter);
        pJobCounter = nullptr;
    }
}

IJob::~IJob()
{
    InternalJobCounter counter = pJobCounter;
    if (!(!counter))
    {
        --(*counter);
    }
}

} // namespace FSWalker
