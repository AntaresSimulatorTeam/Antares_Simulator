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
#include <antares/study.h>
#include <antares/study/memory-usage.h>
#include "max-mrg.h"
#include <antares/study/area/scratchpad.h>

using namespace Yuni;



namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{

	template<bool WithSimplexT>
	struct SpillageSelector
	{
		template<class U>
		static auto Value(const State&, const U& weeklyResults, uint) -> decltype(weeklyResults.ValeursHorairesDeDefaillanceNegative)
		{
			return weeklyResults.ValeursHorairesDeDefaillanceNegative;
		}
	};

	template<>
	struct SpillageSelector<false>
	{
		template<class U>
		static auto Value(const State& state, const U&, uint index) -> decltype(state.resSpilled[index])
		{
			return state.resSpilled[index];
		}
	};




	template<bool WithSimplexT>
	inline void PrepareMaxMRGFor(const State& state, double* opmrg, uint numSpace)
	{
		assert(168 + state.hourInTheYear <= HOURS_PER_YEAR);
		assert(opmrg && "Invalid OP.MRG target");
		enum
		{
			offset  = 0,
			endHour = 168,
		};

		// current area
		auto& area = *state.area;
		// index of the current area
		auto index = area.index;
		assert(area.index < 50000 && "seems invalid");

		// current problem
		auto& problem = *state.problemeHebdo;
		// Weekly results from solver for the current area
		auto& weeklyResults = *(problem.ResultatsHoraires[index]);
		// Unsupplied enery for the current area
		auto& D = weeklyResults.ValeursHorairesDeDefaillancePositive;
		// Spillage
		auto  S = SpillageSelector<WithSimplexT>::Value(state, weeklyResults, area.index);

		double OI[168];

		// H.STOR
		double* H = weeklyResults.TurbinageHoraire;

		// energie turbinee de la semaine
		{
			// DTG MRG
			const double* M = area.scratchpad[numSpace]->dispatchableGenerationMargin;

			double WH = 0.;
			{
				// H.STOR
				auto& H = weeklyResults.TurbinageHoraire;
				for (uint i = offset; i != endHour; ++i)
					WH += H[i];
			}

			if (Math::Zero(WH)) // no hydro
			{
				for (uint i = offset; i != endHour; ++i)
					opmrg[i] = + S[i] + M[i] - D[i];
				return;
			}

			// initialisation
			for (uint i = offset; i != endHour; ++i)
				OI[i] = + S[i] + M[i] - D[i];
		}

		double bottom = + std::numeric_limits<double>::max();
		double top = 0;

		for (uint i = offset; i != endHour; ++i)
		{
			double oii = OI[i];
			if (oii > top)
				top = oii;
			if (oii < bottom)
				bottom = oii;
		}

		double ecart = 1.;
		uint loop = 100; // arbitrary - maximum number of iterations

		// ref to the study calendar
		auto& calendar = state.study.calendar;
		// Pmax
		auto& P = area.hydro.maxPower[Data::PartHydro::maximum];
		//auto& P = problem.CaracteristiquesHydrauliques[index]->ContrainteDePmaxHydrauliqueHoraire;

		do
		{
			double niveau = (top + bottom) * 0.5;
			double SP = 0; // S+
			double SM = 0; // S-

			for (uint i = offset; i != endHour; ++i)
			{
				assert(i < HOURS_PER_YEAR && "calendar overflow");
				if (niveau > OI[i])
				{
					uint dayYear = calendar.hours[i + state.hourInTheYear].dayYear;
					opmrg[i] = Math::Min(niveau, OI[i] + P[dayYear] - H[i]);
					SM += opmrg[i] - OI[i];
				}
				else
				{
					opmrg[i] = Math::Max(niveau, OI[i] - H[i]);
					SP += OI[i] - opmrg[i];
				}
			}

			ecart = SP - SM;
			if (ecart > 0)
				bottom = niveau;
			else
				top = niveau;

			if (!--loop)
			{
				logs.error() << "OP.MRG: " << area.name << ": infinite loop detected. please check input data";
				return;
			}
		}
		while (ecart * ecart > 0.25);
	}



	void PrepareMaxMRG(const State& state, double* opmrg, uint numSpace)
	{
		if (state.simplexHasBeenRan)
			PrepareMaxMRGFor<true>(state, opmrg, numSpace);
		else
			PrepareMaxMRGFor<false>(state, opmrg, numSpace);
	}





} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

