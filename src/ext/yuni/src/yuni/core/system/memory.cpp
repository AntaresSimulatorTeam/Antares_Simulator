/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
// memory.h: The relative path is to avoid conflict with <memory.h>, which
// can sometimes occur...
#include "../system/memory.h"

// The define SYSTEM_MEMORY_IS_IMPLEMENTED will be used to know if
// a valid implementation is available.
// Otherwise we will use some fallbacks and generate a compiler warning


#ifdef YUNI_OS_WINDOWS
# include "windows.hdr.h"
#endif

#if defined(YUNI_OS_LINUX) || defined(YUNI_OS_CYGWIN)
# include <string.h>
# include <sys/sysinfo.h> // sysinfo (2)
#endif

#ifdef YUNI_OS_MAC
#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include <mach/mach.h>
#endif

#include <stdio.h>
#include <stdlib.h>




namespace Yuni
{
namespace System
{
namespace Memory
{

	//! Constants to use when information about the memory usage could not be retrieved
	enum
	{
		//! The default amount of available physical memory
		defaultAvailable = 1024 * 1024 * 512,  // 512Mo
		//! The default amount of total physical memory
		defaultTotal     = 1024 * 1024 * 1024, // 1Go
	};







	#ifdef YUNI_OS_WINDOWS
	#define SYSTEM_MEMORY_IS_IMPLEMENTED

	uint64 Total()
	{
		// see http://msdn.microsoft.com/en-us/library/aa366589(VS.85).aspx
		MEMORYSTATUSEX statex;
		statex.dwLength = (DWORD) sizeof(statex);
		return (GlobalMemoryStatusEx(&statex)) ? (uint64)statex.ullTotalPhys
			: (uint64)defaultTotal;
	}


	uint64 Available()
	{
		// see http://msdn.microsoft.com/en-us/library/aa366589(VS.85).aspx
		MEMORYSTATUSEX statex;
		statex.dwLength = (DWORD) sizeof(statex);
		return (GlobalMemoryStatusEx(&statex)) ? (uint64)statex.ullAvailPhys
			: (uint64)defaultAvailable;
	}


	bool Usage::update()
	{
		// see http://msdn.microsoft.com/en-us/library/aa366589(VS.85).aspx
		MEMORYSTATUSEX statex;
		statex.dwLength = (DWORD) sizeof(statex);

		if (GlobalMemoryStatusEx(&statex))
		{
			total = (uint64)statex.ullTotalPhys;
			available = (uint64)statex.ullAvailPhys;
			return true;
		}
		available = defaultAvailable;
		total = defaultTotal;
		return false;
	}

	#endif // YUNI_OS_WINDOWS






	#if defined(YUNI_OS_LINUX) || defined(YUNI_OS_CYGWIN)
	#define SYSTEM_MEMORY_IS_IMPLEMENTED

	namespace // anonymous
	{

		//! Read a line from a file
		static inline int fgetline(FILE* fp, char* s, int maxlen)
		{
			int i = 0;
			char c;

			while ((c = (char)fgetc(fp)) != EOF)
			{
				if (c == '\n')
				{
					*s = '\0';
					return i;
				}
				if (i >= maxlen)
					return i;

				*s++ = c;
				++i;
			}
			return (!i) ? EOF : i;
		}


		static inline uint64 readvalue(char* line)
		{
			// Here is a sample for /proc/meminfo :
			//
			// MemTotal:      1929228 kB
			// MemFree:         12732 kB
			// Buffers:         72176 kB
			// Cached:        1076572 kB
			// SwapCached:     151412 kB
			// Active:        1491184 kB
			// Inactive:       190832 kB
			// HighTotal:           0 kB
			// HighFree:            0 kB
			// LowTotal:      1929228 kB
			// LowFree:         12732 kB
			// SwapTotal:     2096472 kB
			// SwapFree:      1732964 kB
			// Dirty:             736 kB
			// Writeback:           0 kB
			// AnonPages:      512004 kB
			// Mapped:         702148 kB
			// Slab:           154320 kB
			// PageTables:      34712 kB
			// NFS_Unstable:        0 kB
			// Bounce:              0 kB
			// CommitLimit:   3061084 kB
			// Committed_AS:  1357596 kB
			// VmallocTotal: 34359738367 kB
			// VmallocUsed:    263492 kB
			// VmallocChunk: 34359474679 kB
			// HugePages_Total:     0
			// HugePages_Free:      0
			// HugePages_Rsvd:      0
			// Hugepagesize:     2048 kB

			// Trimming the string from the begining
			while (*line == ' ' and *line != '\0')
				++line;
			const char* first = line;

			// Looking for the end of the number
			while (*line != ' ' and *line != '\0')
				++line;
			// Tagging the end of the number
			*line = '\0';

			# ifdef YUNI_OS_32
			return (uint64) atol(first) * 1024u;
			# else
			return (uint64) atoll(first) * 1024u;
			# endif
		}

	} // anonymous namespace


	bool Usage::update()
	{
		// The only good way to retrieve the memory usage is to read /proc/meminfo.
		// The routine sysinfo (2) is not enough since it does not take care of
		// the cache memory, returning a not valid amount of available physsical
		// memory for our uses.
		FILE* fd;
		if ((fd = fopen("/proc/meminfo", "r")))
		{
			// Resetting the amount of the total physical memory
			total = 0;
			// The amount of the available physical memory is the sum of 3 values :
			// MemFree, Cached and Buffers.
			available = 0;

			// A small buffer
			char line[90];
			// A counter to reduce to abort as soon as we have read all
			int remains = 8;

			// Analysing each line in /proc/meminfo, until the end-of-file or
			// until we have read the 4 values that interrest us.
			while (EOF != fgetline(fd, line, (uint)sizeof(line)))
			{
				if (!strncmp("MemTotal:", line, 9))
				{
					total = readvalue(line + 10);
					if (!(remains >> 1))
						break;
				}
				if (!strncmp("MemFree:", line, 8))
				{
					available += readvalue(line + 9);
					if (!(remains >> 1))
						break;
				}
				if (!strncmp("Cached:", line, 7))
				{
					available += readvalue(line + 8);
					if (!(remains >> 1))
						break;
				}
				if (!strncmp("Buffers:", line, 8))
				{
					available += readvalue(line + 9);
					if (!(remains >> 1))
						break;
				}
			}

			// Closing /proc/meminfo
			fclose(fd);

			// Checking the amount of the total physical memory, which can not be equal to 0
			if (!total)
			{
				total = (uint64) defaultTotal;
				return false;
			}
			return true;
		}

		// Error, using default values
		total     = (uint64) defaultTotal;
		available = (uint64) defaultAvailable;
		return false;
	}


	uint64 Available()
	{
		return Usage().available;
	}


	uint64 Total()
	{
		#ifdef YUNI_OS_LINUX
		{
			// Directly using sysinfo (2), which should be faster than parsing /proc/meminfo
			struct sysinfo s;
			return (!sysinfo(&s)) ? (s.mem_unit * s.totalram) : (uint64) defaultTotal;
		}
		#else
		{
			return Usage().total;
		}
		#endif
	}

	#endif // YUNI_OS_LINUX





	#ifdef YUNI_OS_MAC
	#define SYSTEM_MEMORY_IS_IMPLEMENTED

	uint64 Total()
	{
		int mib[2] = {CTL_HW, HW_MEMSIZE};
		uint64 memory;
		size_t len = sizeof(uint64);
		return (!sysctl(mib, 2, &memory, &len, NULL, 0)) ? memory : (uint64) defaultTotal;
	}


	uint64 Available()
	{
		// Good readings :)
		// http://www.booktou.com/node/148/0321278542/ch06lev1sec3.html

		// Host
		host_name_port_t host = mach_host_self();
		// Statistics about the VM
		vm_statistics_data_t vm_stat;
		mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
		host_statistics(host, HOST_VM_INFO, (host_info_t) &vm_stat, &count);
		// A page size
		vm_size_t  page_size;
		host_page_size(host, &page_size);

		return (uint64) (vm_stat.free_count) * (uint64) page_size;
	}


	bool Usage::update()
	{
		// Available
		available = Available();

		// Total
		int mib[2] = {CTL_HW, HW_MEMSIZE};
		size_t len = sizeof(uint64);
		size_t sttotal;
		if (sysctl(mib, 2, &sttotal, &len, NULL, 0))
		{
			total = (uint64) defaultTotal;
			return false;
		}
		total = (uint64) sttotal;
		return true;
	}

	#endif // YUNI_OS_MAC








	#ifndef SYSTEM_MEMORY_IS_IMPLEMENTED
	#warning Yuni::System::Memory: The implementation is missing for this operating system


	uint64 Total()
	{
		return defaultTotal;
	}

	uint64 Available()
	{
		return defaultAvailable;
	}

	bool Usage::update()
	{
		available = defaultAvailable;
		total     = defaultTotal;
		return false;
	}

	#endif // Fallback



} // namespace Memory
} // namespace System
} // namespace Yuni
