// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/logs/hostinfo.h"

#include <yuni/yuni.h>
#include <yuni/core/system/cpu.h>

#include <antares/logs/logs.h>
#include "antares/logs/hostname.hxx"

using namespace Yuni;
using namespace Antares;

void WriteHostInfoIntoLogs()
{
#ifdef YUNI_OS_32
    logs.info() << "  :: built for 32-bit architectures, "
#endif
#ifdef YUNI_OS_64
                   logs.info()
                << "  :: built for 64-bit architectures, "
#endif
                << YUNI_OS_NAME << ", " << System::CPU::Count() << " cpu(s)";

    ShortString256 buffer;
    InternalAppendHostname(buffer);
    logs.info() << "  :: hostname = " << buffer;
}

void AppendHostName(Yuni::String& out)
{
    InternalAppendHostname(out);
}
