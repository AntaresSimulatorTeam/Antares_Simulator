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
#ifndef YUNI_OS_MSVC
#include <unistd.h>
#endif
#ifndef YUNI_OS_WINDOWS
#include <netdb.h>
#else
#include <yuni/core/system/windows.hdr.h>
#include <Windns.h>
#endif

namespace // anonymous
{
template<class AnyStringT>
void InternalAppendHostname(AnyStringT& out)
{
#ifndef YUNI_OS_WINDOWS
    char hostname[256];
    if (0 != gethostname(hostname, sizeof(hostname)))
    {
        out << "<unknown>";
        return;
    }
    hostname[sizeof(hostname) - 1] = '\0';

    struct addrinfo hints, *info, *p;
    int gai_result;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0)
    {
        out << "<unknown>";
        return;
    }

    for (p = info; p != NULL; p = p->ai_next)
    {
        const char* const name = p->ai_canonname;
        if (name and '\0' != *name)
            out << name;
    }

    freeaddrinfo(info);

#else // windows

    WSADATA wsaData;
    const WORD wVersionRequested = MAKEWORD(2, 0);

    if (WSAStartup(wVersionRequested, &wsaData) == 0)
    {
        char name[256];
        if (gethostname(name, sizeof(name)) == 0)
        {
            name[sizeof(name) - 1] = '\0'; // paranoid
            out << (const char*)name;
        }
        else
            out << "<unknown>";
        WSACleanup();
    }
    else
        out << "<unknown>";

#endif
}

} // anonymous namespace
