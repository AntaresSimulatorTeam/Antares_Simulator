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
#include <yuni/core/math.h>
#include <antares/study/study.h>
#include <antares/study/memory-usage.h>
#include "common-eco-adq.h"
#include <antares/logs.h>
#include <cassert>
#include "simulation.h"
#include <antares/study/area/scratchpad.h>

using namespace Yuni;

#define EPSILON 1e-6



namespace Antares
{
namespace Solver
{
namespace Simulation
{


	template<uint step> 
	static bool Remix(const Data::Study& study, PROBLEME_HEBDO& problem, uint numSpace, uint hourInYear)
	{
		
		double HE[168];
		
		double DE[168];
		
		bool remix[168];
		
		double G[168];

		bool status = true;

		study.areas.each([&] (const Data::Area& area)
		{
			
			auto index = area.index;
			
			auto& weeklyResults = *(problem.ResultatsHoraires[index]);
			
			auto& D = weeklyResults.ValeursHorairesDeDefaillancePositive;
			
			auto& S = weeklyResults.ValeursHorairesDeDefaillanceNegative;
			
			auto& H = weeklyResults.TurbinageHoraire;

			
			memset(remix, 0, sizeof(remix));
			memset(G, 0, sizeof(remix));

			uint endHour = step;
			uint offset = 0;
			for (; offset < 168; offset += step, endHour += step)
			{
				
				{
					
					double WD = 0.;
					for (uint i = offset; i != endHour; ++i)
						WD += D[i];
					if (WD < EPSILON)
						continue;
				}

				
				double WH = 0.;

				for (uint i = offset; i != endHour; ++i)
				{
					if (S[i] < EPSILON)
						WH += H[i];
				}

				if (WH < EPSILON)
					continue;

				
				
				WH = 0.;

				double bottom = std::numeric_limits<double>::max();
				double top = 0;

				
				uint loadTS = NumeroChroniquesTireesParPays[numSpace][index]->Consommation;
				auto& load = area.load.series->series;
				assert(load.width > 0);
				
				auto& L = (loadTS < load.width) ? load[loadTS] : load[0];

				
				const double* M = area.scratchpad[numSpace]->dispatchableGenerationMargin;

				for (uint i = offset; i < endHour; ++i)
				{
					double h_d = H[i] + D[i];
					if (h_d > 0. && Math::Zero(S[i] + M[i]))
					{
						assert(i + hourInYear < load.height);
						double Li = L[i + hourInYear];

						remix[i] = true;
						G[i] = Li - h_d;

						if (G[i] < bottom)
							bottom = G[i];
						if (Li > top)
							top = Li;

						WH += H[i];
					}
				}

				
				
				
				

				auto& P = problem.CaracteristiquesHydrauliques[index]->ContrainteDePmaxHydrauliqueHoraire;

				double ecart = 1.;
				uint loop = 100; 
				do
				{
					double niveau = (top + bottom) * 0.5;
					double stock = 0.;

					for (uint i = offset; i != endHour; ++i)
					{
						if (remix[i])
						{
							double HEi;
							uint iYear = i + hourInYear;
							if (niveau > L[iYear])
							{
								HEi = H[i] + D[i];
								if (HEi > P[i]) 
								{
									HEi = P[i];
									DE[i] = H[i] + D[i] - HEi;
								}
								else
									DE[i] = 0;
							}
							else
							{
								if (G[i] > niveau)
								{
									HEi = 0;
									DE[i] = H[i] + D[i];
								}
								else
								{
									HEi = niveau - G[i];
									if (HEi > P[i]) 
										HEi = P[i];
									DE[i] = H[i] + D[i] - HEi;
								}
							}
							stock += HEi;
							HE[i] = HEi;
						}
						else
						{
							HE[i] = H[i];
							DE[i] = D[i];
						}
					}

					ecart = WH - stock;
					if (ecart > 0.)
						bottom = niveau;
					else
						top = niveau;

					if (!--loop)
					{
						status = false;
						logs.error() << "hydro remix: " << area.name << ": infinite loop detected. please check input data";
						break;
					}
				}
				while (Math::Abs(ecart)>0.01);


				
				for (uint i = offset; i != endHour; ++i)
				{
					H[i] = HE[i];
					assert(not Math::NaN(HE[i]) && "hydro remix: nan detected");
				}
				for (uint i = offset; i != endHour; ++i)
				{
					D[i] = DE[i];
					assert(not Math::NaN(DE[i]) && "hydro remix: nan detected");
				}

			} 

		}); 

		return status;
	}




	bool RemixHydroForAllAreas(const Data::Study& study, PROBLEME_HEBDO& problem, uint numSpace, uint hourInYear, uint nbHour)
	{
		assert(nbHour == 168 && "endHour seems invalid");
		(void) nbHour;
		assert(study.parameters.mode != Data::stdmAdequacyDraft);

		if (study.parameters.shedding.policy != Data::shpShavePeaks)
			return true;

		switch (study.parameters.simplexOptimizationRange)
		{
			case Data::sorWeek:
				return Remix<  168 >(study, problem, numSpace, hourInYear);
			case Data::sorDay:
				return Remix<   24 >(study, problem, numSpace, hourInYear);
			case Data::sorUnknown:
				logs.fatal() << "invalid simplex optimization range";
				break;
		}
		return false;
	}





} 
} 
} 

