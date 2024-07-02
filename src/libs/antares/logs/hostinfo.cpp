/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

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
