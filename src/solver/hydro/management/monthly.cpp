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
#include <yuni/io/directory.h>
#include <yuni/io/file.h>
#include "management.h"
#include <antares/emergency.h>
#include "../../simulation/sim_extern_variables_globales.h"
#include "../monthly/h2o_m_donnees_annuelles.h"
#include "../monthly/h2o_m_fonctions.h"

using namespace Yuni;

# define SEP IO::Separator



namespace Antares
{

	# if HYDRO_MANAGEMENT_DEBUG != 0
	static uint debugYearCount = 0;
	# endif




	template<class ProblemT>
	static void CheckHydroAllocationProblem(Data::Area& area, ProblemT& problem, double lvi)
	{
		bool error = false;

		
		
		
		
		

		
		double X = problem.Volume[11] - problem.Turbine[11] + problem.Apport[11];
		if (not Math::Zero(X - lvi))
		{
			logs.fatal() << area.id << ": hydro management: monthly: reservoir error";
			error = true;
		}

		if (error)
		{
			logs.warning() << area.id << ": lvi = " << lvi;
			logs.warning() << area.id << ": cost = " << problem.CoutDepassementVolume;
			for (uint realmonth = 0; realmonth != 12; ++realmonth)
			{
				logs.warning() << "month: " << ((realmonth < 10) ? "0" : "") << realmonth
					<< ", turb.max: " << problem.TurbineMax[realmonth]
					<< ", turb.cible: " << problem.TurbineCible[realmonth]
					<< ", apport: " << problem.Apport[realmonth]
					<< ", volume: [" << problem.VolumeMin[realmonth] << " .. "
					<< problem.VolumeMax[realmonth] << "]";
			}

			logs.info();
			problem.Volume[0] = lvi;
			for (uint realmonth = 0; realmonth != 12; ++realmonth)
			{
				logs.warning() << "month: " << ((realmonth < 10) ? "0" : "") << realmonth
					<< ", turbine: " << problem.Turbine[realmonth]
					<< ", volume: " << problem.Volume[realmonth];
			}
		}
	}





	double HydroManagement::prepareMonthlyTargetGenerations(Data::Area& area, PerArea& data)
	{
		double total = 0;
		double monthlyMaxDemand = - std::numeric_limits<double>::infinity();

		for (uint realmonth = 0; realmonth != 12; ++realmonth)
		{
			total += data.inflows[realmonth];
			if (data.MLE[realmonth] > monthlyMaxDemand)
				monthlyMaxDemand = data.MLE[realmonth];
		}

		if (not Math::Zero(monthlyMaxDemand))
		{
			double coeff = 0.;
			for (uint realmonth = 0; realmonth != 12; ++realmonth)
			{
				assert(data.MLE[realmonth] / monthlyMaxDemand >= 0.);
				coeff += Math::Power(data.MLE[realmonth] / monthlyMaxDemand, area.hydro.intermonthlyBreakdown);
			}

			if (!Math::Zero(coeff))
				coeff = total / coeff;

			for (uint realmonth = 0; realmonth != 12; ++realmonth)
			{
				assert(data.MLE[realmonth] / monthlyMaxDemand >= 0.);
				data.MTG[realmonth] =
					coeff * Math::Power(data.MLE[realmonth] / monthlyMaxDemand, area.hydro.intermonthlyBreakdown);
			}
		}
		else
		{
			double coeff = total / 12.;

			for (uint realmonth = 0; realmonth != 12; ++realmonth)
				data.MTG[realmonth] = coeff;
		}

		return total;
	}

	void HydroManagement::prepareMonthlyOptimalGenerations(double * random_reservoir_level, uint numSpace)
	{
		uint indexArea = 0;
		study.areas.each([&] (Data::Area& area)
		{
			
			uint z = area.index;
			
			auto& data = pAreas[numSpace][z];

			
			auto& min = area.hydro.reservoirLevel[Data::PartHydro::minimum];
			auto& avg = area.hydro.reservoirLevel[Data::PartHydro::average];
			auto& max = area.hydro.reservoirLevel[Data::PartHydro::maximum];
			
			
			
			double lvi = random_reservoir_level[indexArea];
			indexArea++;

			if (area.hydro.reservoirManagement)
			{
				auto& problem = *H2O_M_Instanciation(1);

				
				double totalInflowsYear = prepareMonthlyTargetGenerations(area, data);
				assert(totalInflowsYear >= 0.);

				problem.CoutDepassementVolume = 1e5;
				problem.VolumeInitial = lvi;

				for (unsigned realmonth = 0; realmonth != 12; ++realmonth)
				{
					problem.TurbineMax[realmonth]   = totalInflowsYear;
					problem.TurbineCible[realmonth] = data.MTG[realmonth];
					problem.Apport[realmonth]       = data.inflows[realmonth];
					problem.VolumeMin[realmonth]    = min[realmonth];
					problem.VolumeMax[realmonth]    = max[realmonth];
				}

				H2O_M_OptimiserUneAnnee(&problem, 0);
				switch (problem.ResultatsValides)
				{
					case OUI:
						{
							if (Logs::Verbosity::Debug::enabled)
								CheckHydroAllocationProblem(area, problem, lvi);

							for (uint realmonth = 0; realmonth != 12; ++realmonth)
							{
								data.MOG[realmonth] = problem.Turbine[realmonth] * area.hydro.reservoirCapacity;
								data.MOL[realmonth] = problem.Volume[realmonth];
								# if HYDRO_MANAGEMENT_DEBUG != 0
									logs.info()<<"debug hydro data.MOG[realmonth] = "<<data.MOG[realmonth] << ", turb : " <<problem.Turbine[realmonth]<< " ,turb.cible: " << problem.TurbineCible[realmonth];
								# endif
							}
							data.MOL[0] = lvi;
							break;
						}
					case NON:
						logs.fatal() << "Hydro: " << area.name << " [month] no solution found";
						break;
					case EMERGENCY_SHUT_DOWN:
						AntaresSolverEmergencyShutdown();
						break;
				}

				H2O_M_Free(&problem);
			}

			else
			{
				
				auto& reservoirLevel = area.hydro.reservoirLevel[Data::PartHydro::average];
				
				for (uint realmonth = 0; realmonth != 12; ++realmonth)
				{
					data.MOG[realmonth] = data.inflows[realmonth] * area.hydro.reservoirCapacity;
					data.MOL[realmonth] = reservoirLevel[realmonth];
				}
			}

			# ifndef NDEBUG
			for (uint realmonth = 0; realmonth != 12; ++realmonth)
			{
				assert(!Math::NaN(data.MOG[realmonth]) && "nan value detected for MOG");
				assert(!Math::NaN(data.MOL[realmonth]) && "nan value detected for MOL");
				assert(!Math::Infinite(data.MOG[realmonth]) && "infinite value detected for MOG");
				assert(!Math::Infinite(data.MOL[realmonth]) && "infinite value detected for MOL");
			}
			# endif

			
			# if HYDRO_MANAGEMENT_DEBUG != 0
			{
				String folder;
				folder << study.folderOutput << SEP << "debug" << SEP << "hydro" << SEP << (1 + debugYearCount);
				if (IO::Directory::Create(folder))
				{
					String filename = folder;
					filename << SEP << "monthly." << area.name << ".txt";
					IO::File::Stream file;
					if (file.openRW(filename))
					{
						file << "Initial Reservoir Level\t" << lvi;
						file << "Generation target\n";
                        for (uint realmonth = 0; realmonth != 12; ++realmonth)
                        {
                                file << data.MTG[realmonth];
                                file << '\n';
                        }


						file << "MOG\tMOL\tMLE\tMLN\n";
						for (uint realmonth = 0; realmonth != 12; ++realmonth)
						{
							file << data.MOG[realmonth] << '\t' << data.MOL[realmonth];
							file << '\t' << data.MLE[realmonth];
							file << '\t' << data.MLN[realmonth];
							file << '\n';
						}
					}
				}
			}
			# endif

		}); 

		# if HYDRO_MANAGEMENT_DEBUG != 0
		++debugYearCount;
		# endif
	}





} 

