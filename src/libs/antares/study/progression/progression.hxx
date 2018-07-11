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
#ifndef __ANTARES_LIBS_STUDY_PROGRESSION_PROGRESSION_HXX__
# define __ANTARES_LIBS_STUDY_PROGRESSION_PROGRESSION_HXX__


namespace Antares
{
namespace Solver
{

	inline Progression::Meter::Meter() :
		nbParallelYears(0),
		logsContainer(nullptr)
	{}

	inline void Progression::Meter::allocateLogsContainer(uint nb)
	{
		logsContainer = new Yuni::CString<256, false>[nb];
	}

	inline void Progression::Meter::taskCount(uint n)
	{
		(void) n;
	}


	inline void Progression::add(Section section, int nbTicks)
	{
		add((uint) -1, section, nbTicks);
	}

	inline void Progression::setNumberOfParallelYears(uint nb)
	{
		pProgressMeter.nbParallelYears = nb;
		pProgressMeter.allocateLogsContainer(nb);
	}

	inline Progression::Part& Progression::begin(uint year, Progression::Section section)
	{
		// Alias
		Part& part         = pProgressMeter.parts[year][section];
		// Reset
		part.tickCount     = 0;
		// It is useless to display 0%, so lastTickCount and tickCount can be equals
		part.lastTickCount = 0;
		pProgressMeter.mutex.lock();
		pProgressMeter.inUse.push_front(&part);
		pProgressMeter.mutex.unlock();
		return part;
	}



	inline const char* Progression::SectionToCStr(Section section)
	{
		static const char* const sectName[sectMax] =
		{
			"mc", "output",
			"load", "solar", "wind", "hydro", "thermal",
			"import timeseries"
		};
		assert((uint) section < (uint) sectMax);
		return sectName[section];
	}


} // namespace Solver
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_PROGRESSION_PROGRESSION_HXX__
