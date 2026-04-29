// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/misc/system-memory.h"

#include <yuni/core/system/memory.h>

#include <antares/logs/logs.h>

using namespace Yuni;
using namespace Antares;

SystemMemoryLogger::SystemMemoryLogger()
{
}

SystemMemoryLogger::~SystemMemoryLogger()
{
    stop();
}

bool SystemMemoryLogger::onStarting()
{
    onInterval(0);
    return true;
}

bool SystemMemoryLogger::onInterval(uint)
{
    System::Memory::Usage memory;
    memory.available /= 1024 * 1024; // Mib
    memory.total /= 1024 * 1024;     // Mib

    logs.info() << "  system memory report: " << memory.available << " Mib / " << memory.total
                << " Mib,  " << (100. / memory.total * memory.available) << "% free";
    return true;
}
