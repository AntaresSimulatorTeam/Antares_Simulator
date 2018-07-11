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
#include "cpu.h"
#if defined(YUNI_OS_LINUX) || defined(YUNI_OS_DARWIN) || defined(YUNI_OS_FREEBSD) || defined(YUNI_OS_NETBSD) || defined(YUNI_OS_OPENBSD)
#include <unistd.h>
#endif
#include "windows.hdr.h"




namespace Yuni
{
namespace System
{
namespace CPU
{


	#if defined(YUNI_OS_WINDOWS) || defined(YUNI_OS_CYGWIN)
	# define YUNI_CPU_COUNT_HAS_IMPLEMENTATION
	uint Count()
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		return si.dwNumberOfProcessors;
	}
	#endif


	#if defined(YUNI_OS_LINUX) || defined(YUNI_OS_DARWIN) || defined(YUNI_OS_FREEBSD) || defined(YUNI_OS_NETBSD) || defined(YUNI_OS_OPENBSD)
	# define YUNI_CPU_COUNT_HAS_IMPLEMENTATION
	uint Count()
	{
		// The number of processors online (capable of running processes)
		long cpus = sysconf(_SC_NPROCESSORS_ONLN);
		return (cpus < 1) ? 1 : static_cast<uint>(cpus);
	}
	#endif




	#ifndef YUNI_CPU_COUNT_HAS_IMPLEMENTATION
	#  warning "The method Yuni::System::CPU::Count() has not been implemented for the current platform"
	uint Count()
	{
		return 1; // Default value
	}
	#endif





} // namespace CPU
} // namespace System
} // namespace Yuni
