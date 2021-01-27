/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <yuni/yuni.h>
#include <yuni/core/system/cpu.h>
#include "hostinfo.h"
#include <string.h>
#include <stdio.h>
#include "logs.h"

#ifdef ANTARES_SWAP_SUPPORT
#include "memory/memory.h"
#endif

#include "sys/hostname.hxx"

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

#ifdef ANTARES_SWAP_SUPPORT
    logs.info() << "  :: memory swapping support (swap file: " << (Memory::swapSize / 1024 / 1024)
                << "Mo, block: " << (Memory::blockSize / 1024) << "Ko)";
#endif

    ShortString256 buffer;
    InternalAppendHostname(buffer);
    logs.info() << "  :: hostname = " << buffer;
}

void AppendHostName(Yuni::String& out)
{
    InternalAppendHostname(out);
}
