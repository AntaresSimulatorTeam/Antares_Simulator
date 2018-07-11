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
#include <yuni/io/file.h>
#include <yuni/io/directory.h>
#include "management.h"
#include <antares/emergency.h>
#include "../daily/h2o_j_donnees_mensuelles.h"
#include "../daily/h2o_j_fonctions.h"
#include "../../simulation/sim_extern_variables_globales.h"
#include <cassert>

using namespace Yuni;

# define SEP IO::Separator



namespace Antares
{

	# if HYDRO_MANAGEMENT_DEBUG != 0
	static uint debugYearCount = 0;
	# endif

	enum { maxOPP = 32 };
	enum { maxDTG = 32 };



	
	inline void HydroManagement::prepareDailyOptimalGenerations(Data::Area& area, uint numSpace)
	{
		
		uint z = area.index;
		assert(z < study.areas.size());
		
		auto& data = pAreas[numSpace][z];
		
		auto& scratchpad = *(area.scratchpad[numSpace]);

		
		auto const& lmi = area.hydro.reservoirLevel[Data::PartHydro::minimum];
		
		auto const& lav = area.hydro.reservoirLevel[Data::PartHydro::average];
		
		auto const& lma = area.hydro.reservoirLevel[Data::PartHydro::maximum];

		
		double opp[maxOPP];
		
		double dtg[maxDTG];
		
		uint dayYear = 0;

		
		auto const& maxPower = area.hydro.maxPower;
		
		auto const& mip = maxPower[Data::PartHydro::minimum];
		
		auto const& avp = maxPower[Data::PartHydro::average];
		
		auto const& map = maxPower[Data::PartHydro::maximum];
		
		auto const& valgen = *ValeursGenereesParPays[numSpace][z];

		# if HYDRO_MANAGEMENT_DEBUG != 0
		double debugOPP[366];
		double debugDTG[366];
		# endif

		for (uint month = 0; month != 12; ++month)
		{
			uint realmonth = study.calendar.months[month].realmonth;
			auto lavm = lav[realmonth];
			auto molm = data.MOL[realmonth];
			auto daysPerMonth = study.calendar.months[month].days;
			assert(daysPerMonth <= maxOPP);
			assert(daysPerMonth <= maxDTG);
			assert(daysPerMonth + dayYear - 1 < maxPower.height);

			if (molm > lma[realmonth])
			{
				for (uint day = 0; day != daysPerMonth; ++day)
					opp[day] = map[day + dayYear];
			}
			else if (molm < lmi[realmonth])
			{
				for (uint day = 0; day != daysPerMonth; ++day)
					opp[day] = mip[day + dayYear];
			}
			else if (molm > lavm)
			{
				
				assert(Math::Abs(lma[realmonth] - lavm) > 1e-12);
				double x = (molm - lavm) / (lma[realmonth] - lavm);

				for (uint day = 0; day != daysPerMonth; ++day)
				{
					auto dYear = day + dayYear;
					opp[day] = avp[dYear] * (1. - x) + map[dYear] * x;
				}
			}
			else if (Math::Equals<double>(molm, lavm))
			{
				for (uint day = 0; day != daysPerMonth; ++day)
					opp[day] = avp[day + dayYear];
			}
			else
			{
				
				assert(Math::Abs(lmi[realmonth] - lavm) > 1e-12);
				double x = (molm - lavm) / (lmi[realmonth] - lavm);

				for (uint day = 0; day != daysPerMonth; ++day)
				{
					auto dYear = day + dayYear;
					opp[day] = avp[dYear] * (1. - x) + mip[dYear] * x;
				}
			}


			auto demandMax = - std::numeric_limits<double>::infinity();
			for (uint day = 0; day != daysPerMonth; ++day)
			{
				auto dYear = day + dayYear;
				if (data.DLE[dYear] > demandMax)
					demandMax = data.DLE[dYear];

				
				assert(day < 32);
				assert(dYear < 366);
				scratchpad.optimalMaxPower[dYear] = opp[day];
				# if HYDRO_MANAGEMENT_DEBUG != 0
				debugOPP[dYear] = opp[day];
				# endif
			}


			if (not Math::Zero(demandMax))
			{
				assert(Math::Abs(demandMax) > 1e-12);
				double coeff = 0.;

				for (uint day = 0; day != daysPerMonth; ++day)
				{
					auto dYear = day + dayYear;
					coeff += Math::Power(data.DLE[dYear] / demandMax, area.hydro.interDailyBreakdown);
				}
				coeff = data.MOG[realmonth] / coeff;

				for (uint day = 0; day != daysPerMonth; ++day)
				{
					auto dYear = day + dayYear;
					dtg[day] = coeff * Math::Power(data.DLE[dYear] / demandMax, area.hydro.interDailyBreakdown);
				}
			}
			else
			{
				
				assert(daysPerMonth > 0);
				double coeff = data.MOG[realmonth] / daysPerMonth;

				for (uint day = 0; day != daysPerMonth; ++day)
					dtg[day] = coeff;
			}

			# if HYDRO_MANAGEMENT_DEBUG != 0
			for (uint day = 0; day != daysPerMonth; ++day)
			{
				auto dYear = day + dayYear;
				debugDTG[dYear] = dtg[day];
			}
			# endif


			auto& problem = *H2O_J_Instanciation();
			problem.NombreDeJoursDuMois = (int) daysPerMonth;
			problem.TurbineDuMois = data.MOG[realmonth];

			for (uint day = 0; day != daysPerMonth; ++day)
			{
				
				problem.TurbineMax[day]   = opp[day] * 24;
				problem.TurbineCible[day] = dtg[day];
			}

			H2O_J_OptimiserUnMois(&problem);
			switch (problem.ResultatsValides)
			{
				case OUI:
					for (uint day = 0; day != daysPerMonth; ++day)
						valgen.HydrauliqueModulableQuotidien[dayYear + day] = problem.Turbine[day];
					break;
				case NON:
					logs.fatal() << "Hydro: " << area.name << " [daily] no solution found";
					break;
				case EMERGENCY_SHUT_DOWN:
					AntaresSolverEmergencyShutdown();
					break;
			}

			H2O_J_Free(&problem);

			# ifndef NDEBUG
			for (uint day = 0; day != daysPerMonth; ++day)
			{
				assert(!Math::NaN(valgen.HydrauliqueModulableQuotidien[dayYear + day]));
				assert(!Math::Infinite(valgen.HydrauliqueModulableQuotidien[dayYear + day]));
			}
			# endif

			
			dayYear += daysPerMonth;

		} 


		
		# if HYDRO_MANAGEMENT_DEBUG != 0
		{
			String folder;
			folder << study.folderOutput << SEP << "debug" << SEP << "hydro" << SEP << (1 + debugYearCount);
			if (IO::Directory::Create(folder))
			{
				String filename = folder;
				filename << SEP << "daily." << area.name << ".txt";
				IO::File::Stream file;
				if (file.openRW(filename))
				{
					file << "\tTurbine\tOPP\tTurbine Cible\tDLE\tDLN\n";
					for (uint day = 0; day != 365; ++day)
					{
						double value = valgen.HydrauliqueModulableQuotidien[day];
						file
							<< day << '\t'
							<< value << '\t'
							<< debugOPP[day] << '\t'
							<< debugDTG[day] << '\t'
							<< data.DLE[day] << '\t'
							<< data.DLN[day];
						file << '\n';
					}
				}
			}
		}
		# endif
	}



	void HydroManagement::prepareDailyOptimalGenerations(uint numSpace)
	{
		study.areas.each([&] (Data::Area& area)
		{
			prepareDailyOptimalGenerations(area, numSpace);
		});

		# if HYDRO_MANAGEMENT_DEBUG != 0
		++debugYearCount;
		# endif
	}



} 

