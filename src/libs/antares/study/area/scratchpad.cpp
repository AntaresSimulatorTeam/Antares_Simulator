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

#include "scratchpad.h"
#include "constants.h"
#include "../study.h"
#include <limits>

using namespace Yuni;



namespace Antares
{
namespace Data
{


	AreaScratchpad::TimeseriesData::TimeseriesData(Area& area) :
		load(area.load.series->series),
		solar(area.solar.series->series),
		wind(area.wind.series->series)
	{
	}


	AreaScratchpad::AreaScratchpad(const StudyRuntimeInfos& rinfos, Area& area) :
		ts(area)
	{
		// alias to the simulation mode
		auto mode = rinfos.mode;
		
		for (uint i = 0; i != 168; ++i)
			dispatchableGenerationMargin[i] = 0;

		for (uint h = 0; h != HOURS_PER_YEAR; ++h)
		{
			mustrunSum[h]         = std::numeric_limits<double>::quiet_NaN();
			originalMustrunSum[h] = std::numeric_limits<double>::quiet_NaN();
		}

		for (uint d = 0; d != DAYS_PER_YEAR; ++d)
		{
			optimalMaxPower[d] = std::numeric_limits<double>::quiet_NaN();
			pumpingMaxPower[d] = std::numeric_limits<double>::quiet_NaN();
		}
		
		// Fatal hors hydro
		{
			double sum;
			uint w;
			assert(area.miscGen.height > 0);
			assert(area.miscGen.width  > 0);
			uint height = area.miscGen.height;
			for (uint h = 0; h != height; ++h)
			{
				sum = 0.;
				for (w = 0; w != area.miscGen.width; ++w)
					sum += area.miscGen[w][h];
				miscGenSum[h] = sum;
			}
			if (mode == Data::stdmAdequacy)
			{
				for (uint h = 0; h != area.miscGen.height; ++h)
					miscGenSum[h] -= area.reserves[Data::fhrPrimaryReserve][h];
			}
		}

		// ===============
		// hydroHasMod
		// ===============
		if (mode != stdmAdequacyDraft)
		{
			// ------------------------------
			// Hydro generation permission
			// ------------------------------
			// Useful whether we use a heuristic target or not
			bool hydroGenerationPermission = false;

			// ... Getting hydro max power
			auto const& maxPower = area.hydro.maxPower;

			// ... Hydro max generating power and energy
			auto const& maxGenP = maxPower[Data::PartHydro::genMaxP];
			auto const& maxGenE = maxPower[Data::PartHydro::genMaxE];

			double value = 0.;
			for (uint d = 0; d < DAYS_PER_YEAR; ++d)
				value += maxGenP[d] * maxGenE[d];

			// If generating energy is nil over the whole year, hydroGenerationPermission is false, true otherwise.
			hydroGenerationPermission = (value > 0.);

			// ---------------------
			// Hydro has inflows
			// ---------------------
			bool hydroHasInflows = false;
			if (!area.hydro.prepro) // not in prepro mode
			{
				assert(area.hydro.series);
				hydroHasInflows = MatrixTestForAtLeastOnePositiveValue(area.hydro.series->storage);
			}
			else
			{
				auto& m = area.hydro.prepro->data;
				double value = 0.;
				auto& colPowerOverWater = m[PreproHydro::powerOverWater];
				auto& colMaxEnergy = m[PreproHydro::maximumEnergy];

				for (uint m = 0; m < rinfos.nbMonthsPerYear; ++m)
					value += colMaxEnergy[m] * (1. - colPowerOverWater[m]);

				hydroHasInflows = (value > 0.);
			}

			// --------------------------
			// hydroHasMod definition
			// --------------------------
			hydroHasMod = hydroHasInflows || hydroGenerationPermission;
		}
		else
		{
			// No modulation in adequacy
			hydroHasMod = false;
		}

		
		// ===============
		// Pumping
		// ===============
		// ... Hydro max power
		auto const& maxPower = area.hydro.maxPower;

		// ... Hydro max pumping power and energy
		auto const& maxPumpingP = maxPower[Data::PartHydro::pumpMaxP];
		auto const& maxPumpingE = maxPower[Data::PartHydro::pumpMaxE];

		// ... Pumping max power
		for (uint d = 0; d != DAYS_PER_YEAR; ++d)
			pumpingMaxPower[d] = maxPumpingP[d];

		// ... Computing 'pumpHasMod' parameter
		if (mode != stdmAdequacyDraft)
		{
			double value = 0.;
			for (uint d = 0; d < DAYS_PER_YEAR; ++d)
				value += maxPumpingP[d] * maxPumpingE[d];

			// If pumping energy is nil over the whole year, pumpHasMod is false, true otherwise.
			pumpHasMod = (value > 0.);
		}

		// Spinning reserves
		memcpy(spinningReserve, area.reserves[fhrPrimaryReserve], sizeof(double) * rinfos.nbHoursPerYear);
	}





} // namespace Data
} // namespace Antares

