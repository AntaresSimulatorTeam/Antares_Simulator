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

#include "emergency.h"
#include "logs.h"
#include "study/study.h"
#include <yuni/core/system/suspend.h>


using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;


void AntaresSolverEmergencyShutdown(int code)
{
	{
		// Releasing all locks held by the study
		auto currentStudy = Data::Study::Current::Get();
		if (!(!currentStudy))
			currentStudy->releaseAllLocks();

		// Releasing all swap files held by the program
		Antares::memory.releaseAll();

		// Importing logs
		if (!logs.logfile())
		{
			logs.fatal()   << "Aborting now. (warning: no file log available)";
			logs.warning() << "No log file available";
		}
		else
		{
			if (!(!currentStudy))
				currentStudy->importLogsToOutputFolder();
			logs.error() << "Aborting now. See logs for more details";
		}
		// release currentStudy
	}

	exit(code);
}



