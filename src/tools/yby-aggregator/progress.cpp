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

#include "progress.h"
#include <antares/logs.h>
#include <antares/memory/memory.h>


using namespace Yuni;
using namespace Antares;



Progress progressBar;
uint Progress::Total = 0;
Atomic::Int<> Progress::Current = 0;



Progress::Progress() :
	state(stReading),
	pCompleted(false),
	pLastPercent(0)
{
	Current = 0;
	Total = 0;
}


bool Progress::completed() const
{
	return pCompleted;
}


bool Progress::onInterval(uint)
{
	// Do nothing if in silent mode
	if (state == stSilent)
		return true;

	const uint current = (uint) Current;
	const uint total   = Total;
	uint remains = total - current;

	if (!total)
		return true;

	switch (state)
	{
		case stJobs:
			{
				if (!remains)
				{
					pCompleted = true;
					state = stSilent;
					suspend(100);
					return false;
				}

				if (remains == 1)
					logs.info() << "Running...  (1 task remains)";
				else
					logs.info() << "Running...  (" << remains << " tasks remain)";
				break;
			}
		case stWrite:
			{
				const uint percent = (current * 100u) / total;
				if (percent != pLastPercent)
				{
					logs.info() << pMessage << "  (" << percent << "%)";
					pLastPercent = percent;
				}
				break;
			}
		case stReading:
			{
				logs.info() << pMessage << "  (" << total << " folders)";
				break;
			}
		case stSilent:
			break;
	}

	return true;
}

