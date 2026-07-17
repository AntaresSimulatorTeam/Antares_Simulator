// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/logs/hostinfo.h"

#include <cstdint>
#include <cstring>
#include <thread>

#ifdef _WIN32
#include <windns.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <unistd.h>
#endif

#include <antares/logs/logs.h>

using namespace Antares;

namespace
{

std::string hostName()
{
#ifndef _WIN32
    char hostname[256];
    if (0 != gethostname(hostname, sizeof(hostname)))
    {
        return "<unknown>";
    }
    hostname[sizeof(hostname) - 1] = '\0';

    struct addrinfo hints
    {
    };
    struct addrinfo* info = nullptr;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // either IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    if (getaddrinfo(hostname, "http", &hints, &info) != 0)
    {
        return "<unknown>";
    }

    std::string result;
    for (struct addrinfo* p = info; p != nullptr; p = p->ai_next)
    {
        if (const char* const name = p->ai_canonname; name && '\0' != *name)
        {
            result += name;
        }
    }
    freeaddrinfo(info);
    return result;

#else // _WIN32

    WSADATA wsaData;
    const WORD wVersionRequested = MAKEWORD(2, 0);

    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        return "<unknown>";
    }

    char name[256];
    std::string result;
    if (gethostname(name, sizeof(name)) == 0)
    {
        name[sizeof(name) - 1] = '\0'; // paranoid
        result = name;
    }
    else
    {
        result = "<unknown>";
    }
    WSACleanup();
    return result;

#endif
}

const char* operatingSystemName()
{
#if defined(_WIN32)
    return "Microsoft Windows";
#elif defined(__APPLE__)
    return "MacOS X";
#elif defined(__linux__)
    return "GNU/Linux";
#else
    return "Unknown";
#endif
}

} // anonymous namespace

void WriteHostInfoIntoLogs()
{
#if INTPTR_MAX == INT64_MAX
    logs.info() << "  :: built for 64-bit architectures, "
#else
    logs.info() << "  :: built for 32-bit architectures, "
#endif
                << operatingSystemName() << ", " << std::thread::hardware_concurrency()
                << " cpu(s)";

    logs.info() << "  :: hostname = " << hostName();
}
