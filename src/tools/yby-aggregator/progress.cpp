// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "progress.h"

#include <antares/logs/logs.h>

using namespace Yuni;
using namespace Antares;

Progress progressBar;
uint Progress::Total = 0;
std::atomic<int> Progress::Current = 0;

Progress::Progress():
    state(stReading),
    pCompleted(false),
    pLastPercent(0)
{
    Current = 0;
    Total = 0;
}

bool Progress::completed() const
{
    return pCompleted;
}

bool Progress::onInterval(uint)
{
    // Do nothing if in silent mode
    if (state == stSilent)
    {
        return true;
    }

    const uint current = (uint)Current;
    const uint total = Total;
    uint remains = total - current;

    if (!total)
    {
        return true;
    }

    switch (state)
    {
    case stJobs:
    {
        if (!remains)
        {
            pCompleted = true;
            state = stSilent;
            suspend(100);
            return false;
        }

        if (remains == 1)
        {
            logs.info() << "Running...  (1 task remains)";
        }
        else
        {
            logs.info() << "Running...  (" << remains << " tasks remain)";
        }
        break;
    }
    case stWrite:
    {
        const uint percent = (current * 100u) / total;
        if (percent != pLastPercent)
        {
            logs.info() << pMessage << "  (" << percent << "%)";
            pLastPercent = percent;
        }
        break;
    }
    case stReading:
    {
        logs.info() << pMessage << "  (" << total << " folders)";
        break;
    }
    case stSilent:
        break;
    }

    return true;
}
