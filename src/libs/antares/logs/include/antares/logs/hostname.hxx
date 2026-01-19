// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <yuni/yuni.h>
#ifndef YUNI_OS_MSVC
#include <unistd.h>
#endif
#ifndef YUNI_OS_WINDOWS
#include <netdb.h>
#else
#include <Windns.h>

#include <yuni/core/system/windows.hdr.h>
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
        {
            out << name;
        }
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
        {
            out << "<unknown>";
        }
        WSACleanup();
    }
    else
    {
        out << "<unknown>";
    }

#endif
}

} // anonymous namespace
