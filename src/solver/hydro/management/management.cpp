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
#include <antares/study/study.h>
#include <antares/study/area/scratchpad.h>
#include "management.h"
#include "../../simulation/sim_extern_variables_globales.h"
#include <yuni/core/math.h>
#include <limits>
#include <antares/study/parts/hydro/container.h>

using namespace Yuni;



namespace Antares
{

	 double HydroManagement::GammaVariable(double r)
	{
		double x = 0.;
		do
		{
			double s = r - 1.;
			double u = random();
			double v = random();
			double w = u * (1. - u);
			assert(Math::Abs(w) > 1e-12);
			assert(3. * (r - 0.25) / w > 0.);
			double y = Math::SquareRootNoCheck(3. * (r - 0.25) / w) * (u - 0.5);

			x = y + s;
			if (v < 1e-12)
				break;

			w *= 4.;
			v *= w;
			double z =  w * v * v;

			assert(Math::Abs(s) > 1e-12);
			assert(z > 0.);
			assert(z /s > 0.);
			if (log(z) <= 2. * (s * log(x / s) - y))
				break;
		}
		while (true);
		return x;
	}


	inline double HydroManagement::BetaVariable(double a, double b)
	{
		double y = GammaVariable(a);
		double z = GammaVariable(b);
		assert(Math::Abs(y + z) > 1e-12);
		return y / (y + z);
	}




	HydroManagement::HydroManagement(Data::Study& study) :
		study(study),
		parameters(study.parameters)
	{
		
		pAreas = new PerArea*[study.maxNbYearsInParallel];
		for(uint numSpace = 0; numSpace < study.maxNbYearsInParallel; numSpace++)
			pAreas[numSpace] = new PerArea[study.areas.size()];

		
		random.reset(study.parameters.seed[Data::seedHydroManagement]);
	}


	HydroManagement::~HydroManagement()
	{
		for(uint numSpace = 0; numSpace < study.maxNbYearsInParallel; numSpace++)
			delete[] pAreas[numSpace];
		delete[] pAreas;
	}


	void HydroManagement::prepareInflowsScaling(uint numSpace)
	{
		study.areas.each([&] (Data::Area& area)
		{
			
			uint z = area.index;
			
			auto& ptchro = *NumeroChroniquesTireesParPays[numSpace][z];
			
			auto& inflowsmatrix  = area.hydro.series->storage;
			assert(inflowsmatrix.width && inflowsmatrix.height);
			auto  tsIndex = (uint) ptchro.Hydraulique;
			auto const& srcinflows = inflowsmatrix[tsIndex < inflowsmatrix.width ? tsIndex : 0];

			
			auto& data = pAreas[numSpace][z];

			assert(area.hydro.reservoirCapacity != 0 && "Divide by zero");

			if (not Math::Zero(area.hydro.reservoirCapacity))
			{
				for (uint realmonth = 0; realmonth != 12; ++realmonth)
				{
					data.inflows[realmonth] = srcinflows[realmonth] / (area.hydro.reservoirCapacity);
					assert(!Math::NaN(data.inflows[realmonth]) && "nan value detect in inflows");
				}
			}
		});
	}


	template<enum Data::StudyMode ModeT>
	void HydroManagement::prepareNetDemand(uint numSpace)
	{
		study.areas.each([&] (Data::Area& area)
		{
			
			uint z = area.index;
			
			auto& scratchpad =*(area.scratchpad[numSpace]);

			
			auto& ptchro = *NumeroChroniquesTireesParPays[numSpace][z];
			
			auto& rormatrix = area.hydro.series->ror;
			auto  tsIndex     = (uint) ptchro.Hydraulique;
			auto& ror         = rormatrix[tsIndex < rormatrix.width ? tsIndex : 0];
			
			auto& data = pAreas[numSpace][z];

			for (uint hour = 0; hour != 8760; ++hour)
			{
				auto month     = study.calendar.hours[hour].month;
				auto realmonth = study.calendar.months[month].realmonth;
				auto dayYear   = study.calendar.hours[hour].dayYear;

				double netdemand =
					+ scratchpad.ts.load[ptchro.Consommation][hour]
					- scratchpad.ts.wind[ptchro.Eolien][hour]
					- scratchpad.miscGenSum[hour]
					- scratchpad.ts.solar[ptchro.Solar][hour]
					- ror[hour]
					- ((ModeT != Data::stdmAdequacy)
						? scratchpad.mustrunSum[hour]           
						: scratchpad.originalMustrunSum[hour]); 

				assert(!Math::NaN(netdemand)
					&& "hydro management: NaN detected when calculating the net demande");
				data.MLN[realmonth] += netdemand;
				data.DLN[dayYear]   += netdemand;
			}
		});
	}



	void HydroManagement::prepareEffectiveDemand(uint numSpace)
	{
		study.areas.each([&] (Data::Area& area)
		{
			
			auto z = area.index;
			
			auto& data = pAreas[numSpace][z];

			for (uint day = 0; day != 365; ++day)
			{
				auto month = study.calendar.days[day].month;
				assert(month < 12 && "Invalid month index");
				auto realmonth = study.calendar.months[month].realmonth;

				double effectiveDemand = 0;
				area.hydro.allocation.eachNonNull([&] (unsigned areaindex, double value)
				{
					effectiveDemand += (pAreas[numSpace][areaindex]).DLN[day] * value;
				});

				assert(!Math::NaN(effectiveDemand) && "nan value detected for effectiveDemand");
				data.DLE[day]       += effectiveDemand;
				data.MLE[realmonth] += effectiveDemand;

				assert(not Math::NaN(data.DLE[day]) && "nan value detected for DLE");
				assert(not Math::NaN(data.MLE[realmonth]) && "nan value detected for DLE");
			}

			
			auto minimumYear  = std::numeric_limits<double>::infinity();
			auto dayYear = 0u;

			for (uint month = 0; month != 12; ++month)
			{
				auto minimumMonth = + std::numeric_limits<double>::infinity();
				auto daysPerMonth = study.calendar.months[month].days;
				auto realmonth    = study.calendar.months[month].realmonth;

				for (uint d = 0; d != daysPerMonth; ++d)
				{
					auto dYear = d + dayYear;
					if (data.DLE[dYear] < minimumMonth)
						minimumMonth = data.DLE[dYear];
				}

				if (minimumMonth < 0.)
				{
					for (uint d = 0; d != daysPerMonth; ++d)
						data.DLE[dayYear + d] -= minimumMonth - 1e-4; 
				}

				if (data.MLE[realmonth] < minimumYear)
					minimumYear = data.MLE[realmonth];

				dayYear += daysPerMonth;
			}

			
			if (minimumYear < 0.)
			{
				for (uint realmonth = 0; realmonth != 12; ++realmonth)
					data.MLE[realmonth] -= minimumYear - 1e-4;
			}

		}); 
	}


	double HydroManagement::randomReservoirLevel(double min, double avg, double max)
	{
		
		if (Math::Equals(min, max))
			return avg;
		if (Math::Equals(avg, min) || Math::Equals(avg, max))
			return avg;

		double e  = (avg - min) / (max - min);
		double re = 1. - e;

		assert(Math::Abs(1. + e) > 1e-12);
		assert(Math::Abs(2. - e) > 1e-12);

		double v1 = (e * e) * re / (1. + e);
		double v2 = e * re * re / (2. - e);
		double v  = Math::Min(v1, v2) * .5;

		assert(Math::Abs(v) > 1e-12);

		double a = e * (e * re / v - 1.);
		double b = re * (e * re / v - 1.);

		double x = BetaVariable(a, b);
		return x * max + (1. - x) * min;
	}

	void HydroManagement::operator () (double * randomReservoirLevel, uint numSpace)
	{
		
		memset(pAreas[numSpace], 0, sizeof(PerArea) * study.areas.size());

		
		prepareInflowsScaling(numSpace);

		
		if (parameters.adequacy())
			prepareNetDemand<Data::stdmAdequacy>(numSpace);
		else
			prepareNetDemand<Data::stdmEconomy>(numSpace);

		
		prepareEffectiveDemand(numSpace);

		
		prepareMonthlyOptimalGenerations(randomReservoirLevel, numSpace);
		prepareDailyOptimalGenerations(numSpace);
	}





} 

