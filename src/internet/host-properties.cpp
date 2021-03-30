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
#include <antares/sys/appdata.h>
#include <yuni/core/system/environment.h>
#include "../internet/limits.h"

#include "license.h"
#include "base64/cencode.h"
#include "decrypt.hxx"

#include <fstream>

#ifdef YUNI_OS_LINUX
#include <yuni/core/dictionary.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#endif
#ifdef YUNI_OS_WINDOWS
#include <stdio.h>
#include <intrin.h>
#include <yuni/core/system/windows.hdr.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <intrin.h>
#pragma comment(lib, "IPHLPAPI.lib")
#endif

using namespace Yuni;

namespace Antares
{
namespace License
{
#ifndef YUNI_OS_WINDOWS
template<class StringT>
static inline void RemoveProcCPUInfoKey(String& out, const StringT& key)
{
    // we must remove all occurrences
    do
    {
        auto start = out.find(key);
        if (start < out.size())
        {
            auto end = out.find('\n', start);
            if (end < out.size())
                out.erase(start, end - start + 1);
            else
                out.resize(start);
        }
        else
            break;
    } while (true);
}
#endif

#ifdef YUNI_OS_LINUX
static inline void GetMACAddress(String& out, const AnyString& iface)
{
#ifndef SIOCGIFADDR
    // The kernel does not support the required ioctls
    (void)iface;
    return;
#else

    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (fd >= 0)
    {
        struct ifreq ifr;
        ifr.ifr_addr.sa_family = AF_INET;
        strncpy(ifr.ifr_name, iface.c_str(), IFNAMSIZ - 1);
        if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr))
        {
            if (not out.empty())
                out.append(", ", 2);
            out << iface << '=';
            out.appendFormat("%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
                             (int)((unsigned char)ifr.ifr_hwaddr.sa_data[0]),
                             (int)((unsigned char)ifr.ifr_hwaddr.sa_data[1]),
                             (int)((unsigned char)ifr.ifr_hwaddr.sa_data[2]),
                             (int)((unsigned char)ifr.ifr_hwaddr.sa_data[3]),
                             (int)((unsigned char)ifr.ifr_hwaddr.sa_data[4]),
                             (int)((unsigned char)ifr.ifr_hwaddr.sa_data[5]));
        }
        close(fd);
    }
#endif
}

static inline void ListAllMacAddressesLinux(String& out)
{
    struct ifaddrs* ifaddr;

    if (getifaddrs(&ifaddr) == -1)
        return;

    // must be used while ifaddr is alive
    Set<AnyString>::Hash alreadyProcessed;

    for (struct ifaddrs* ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL or ifa->ifa_addr->sa_family == AF_PACKET)
            continue;
        AnyString name = ifa->ifa_name;
        if (name != "lo" and alreadyProcessed.count(name) == 0)
        {
            alreadyProcessed.insert(name);
            GetMACAddress(out, name);
        }
    }
    freeifaddrs(ifaddr);
}
#endif

#ifdef YUNI_OS_WINDOWS
static inline void ListAllMacAddressesWindows(String& out)
{
    IP_ADAPTER_INFO adapterInfo[32];
    DWORD dwBufLen = sizeof(adapterInfo);
    DWORD dwStatus = GetAdaptersInfo(adapterInfo, &dwBufLen);

    if (dwStatus == ERROR_SUCCESS)
    {
        // temporary string for description
        AnyString description;
        // temporary string
        char buffer[48];
        (void)::memset(buffer, '\0', sizeof(buffer));

        PIP_ADAPTER_INFO pAdapterInfo = adapterInfo;
        while (pAdapterInfo)
        {
            if (MIB_IF_TYPE_ETHERNET == pAdapterInfo->Type)
            {
                if (not out.empty())
                    out.append(", ", 2);

                description = (const char*)pAdapterInfo->Description;
                description.trim();
                out << description << '=';

                uint bindex = 0;
                for (uint i = 0; i < pAdapterInfo->AddressLength and bindex < sizeof(buffer) - 4;
                     ++i)
                {
                    if (i != 0)
                    {
                        buffer[bindex] = ':';
                        ++bindex;
                    }
                    int v = (int)((unsigned char)pAdapterInfo->Address[i]);
                    int n = sprintf_s(((char*)buffer) + bindex, 3, "%.2x", v);
                    if (n <= 0)
                        break;
                    bindex += n; // n should be equal to 2
                }

                if (bindex != 0)
                    out.append((const char*)buffer, (uint)bindex);
                else
                    out += "<unknown>";
            }
            pAdapterInfo = pAdapterInfo->Next;
        }
    }
}
#endif

static inline void ListAllMacAddresses(String& out)
{
#ifdef YUNI_OS_LINUX
    ListAllMacAddressesLinux(out);
#endif // LINUX
#ifdef YUNI_OS_WINDOWS
    ListAllMacAddressesWindows(out);
#endif
}

bool RetrieveHostProperties(Properties& properties, String& n)
{
    return true;
}

#ifdef YUNI_OS_LINUX

static inline void GetFirstMACAddress(String& out, const AnyString& iface)
{
#ifndef SIOCGIFADDR
    // The kernel does not support the required ioctls
    (void)iface;
    return;
#else

    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (fd >= 0)
    {
        struct ifreq ifr;
        ifr.ifr_addr.sa_family = AF_INET;
        strncpy(ifr.ifr_name, iface.c_str(), IFNAMSIZ - 1);
        if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr))
        {
            if (not out.empty())
                out.append(", ", 2);
            out.appendFormat("%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
                             (int)((unsigned char)ifr.ifr_hwaddr.sa_data[0]),
                             (int)((unsigned char)ifr.ifr_hwaddr.sa_data[1]),
                             (int)((unsigned char)ifr.ifr_hwaddr.sa_data[2]),
                             (int)((unsigned char)ifr.ifr_hwaddr.sa_data[3]),
                             (int)((unsigned char)ifr.ifr_hwaddr.sa_data[4]),
                             (int)((unsigned char)ifr.ifr_hwaddr.sa_data[5]));
        }
        close(fd);
    }
#endif
}

static inline void firstMacAddressLinux(String& out)
{
    struct ifaddrs* ifaddr;

    if (getifaddrs(&ifaddr) == -1)
        return;

    // must be used while ifaddr is alive
    Set<AnyString>::Hash alreadyProcessed;

    struct ifaddrs* ifa = ifaddr;
    if (ifa != NULL)
    {
        if (!(ifa->ifa_addr == NULL or ifa->ifa_addr->sa_family == AF_PACKET))
        {
            AnyString name = ifa->ifa_name;
            if (name != "lo" and alreadyProcessed.count(name) == 0)
            {
                alreadyProcessed.insert(name);
                GetFirstMACAddress(out, name);
            }
        }
    }
    freeifaddrs(ifaddr);
}
#endif

#ifdef YUNI_OS_WINDOWS
static inline void firstMacAddressWindows(String& out)
{
    IP_ADAPTER_INFO adapterInfo[32];
    DWORD dwBufLen = sizeof(adapterInfo);
    DWORD dwStatus = GetAdaptersInfo(adapterInfo, &dwBufLen);

    if (dwStatus == ERROR_SUCCESS)
    {
        // temporary string for description
        AnyString description;
        // temporary string
        char buffer[48];
        (void)::memset(buffer, '\0', sizeof(buffer));

        PIP_ADAPTER_INFO pAdapterInfo = adapterInfo;
        while (pAdapterInfo && out.empty())
        {
            if (MIB_IF_TYPE_ETHERNET == pAdapterInfo->Type)
            {
                uint bindex = 0;
                for (uint i = 0; i < pAdapterInfo->AddressLength and bindex < sizeof(buffer) - 4;
                     ++i)
                {
                    if (i != 0)
                    {
                        buffer[bindex] = ':';

                        ++bindex;
                    }
                    int v = (int)((unsigned char)pAdapterInfo->Address[i]);
                    int n = sprintf_s(((char*)buffer) + bindex, 3, "%.2x", v);
                    if (n <= 0)
                        break;
                    bindex += n; // n should be equal to 2
                }

                if (bindex != 0)
                    out.append((const char*)buffer, (uint)bindex);
            }
            pAdapterInfo = pAdapterInfo->Next;
        }
    }
}
#endif

void firstMacAddress(YString& out)
{
#ifdef YUNI_OS_LINUX
    firstMacAddressLinux(out);
#endif // LINUX
#ifdef YUNI_OS_WINDOWS
    firstMacAddressWindows(out);
#endif
}
} // namespace License
} // namespace Antares
