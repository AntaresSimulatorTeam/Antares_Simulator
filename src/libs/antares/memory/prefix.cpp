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

#include "memory.h"
#include "../hostinfo.h"
#include "../config.h"
#include "../sys/policy.h"
#include <yuni/core/system/process.h>
#include <yuni/core/system/environment.h>


using namespace Yuni;

namespace Antares
{


	void Memory::initializeSwapFilePrefix()
	{
		// getting the current process id
		pProcessID = Yuni::ProcessID();

		pSwapFilePrefix << "antares-" << ANTARES_VERSION << "-swap-";

		// Appending the hostname to differentiate when used from a network
		// folder
		LocalPolicy::Read(pSwapFilePrefix, "hostname");
		if (pSwapFilePrefix.empty())
			AppendHostName(pSwapFilePrefix);

		// User name
		pSwapFilePrefix += '-';
		String user;
		LocalPolicy::Read(user, "user");
		if (!user)
		{
			uint oldsize = pSwapFilePrefix.size();

			# ifdef YUNI_OS_WINDOWS
			System::Environment::Read("USERNAME", pSwapFilePrefix, false);
			# else
			System::Environment::Read("USER", pSwapFilePrefix, false);
			# endif

			// nothing may have been found
			if (oldsize == pSwapFilePrefix.size())
				pSwapFilePrefix += "unknown__user";
		}
		else
			pSwapFilePrefix += user;

		// Appending the process ID
		pSwapFilePrefix += '-';
		pSwapFilePrefix.toLower();
		pSwapFilePrefixProcessID << pProcessID << '-';
	}



} // namespace Antares
