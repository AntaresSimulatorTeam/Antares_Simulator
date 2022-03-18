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
#include <iomanip>
#include <sstream>
#include <limits>

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

#define SEP IO::Separator

#define HYDRO_MONTHLY_SOLVER_DEBUG 0

namespace Antares
{
template<class ProblemT>
static void CheckHydroAllocationProblem(Data::Area& area,
                                        ProblemT& problem,
                                        int initLevelMonth,
                                        double lvi)
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
        for (uint month = 0; month != 12; ++month)
        {
            uint realmonth = (initLevelMonth + month) % 12;
            logs.warning() << "month: " << ((realmonth < 10) ? "0" : "") << realmonth
                           << ", turb.max: " << problem.TurbineMax[realmonth]
                           << ", turb.cible: " << problem.TurbineCible[realmonth]
                           << ", apport: " << problem.Apport[realmonth] << ", volume: ["
                           << problem.VolumeMin[realmonth] << " .. " << problem.VolumeMax[realmonth]
                           << "]";
        }

        logs.info();
        problem.Volume[initLevelMonth] = lvi;
        for (uint month = 0; month != 12; ++month)
        {
            uint realmonth = (initLevelMonth + month) % 12;
            logs.warning() << "month: " << ((realmonth < 10) ? "0" : "") << realmonth
                           << ", turbine: " << problem.Turbine[realmonth]
                           << ", volume: " << problem.Volume[realmonth];
        }
    }
}

double HydroManagement::prepareMonthlyTargetGenerations(Data::Area& area, PerArea& data)
{
    double total = 0;

    for (uint realmonth = 0; realmonth != 12; ++realmonth)
        total += data.inflows[realmonth];

    if (not area.hydro.followLoadModulations)
    {
        for (uint realmonth = 0; realmonth != 12; ++realmonth)
            data.MTG[realmonth] = data.inflows[realmonth];

        return total;
    }

    double monthlyMaxDemand = -std::numeric_limits<double>::infinity();

    for (uint realmonth = 0; realmonth != 12; ++realmonth)
    {
        if (data.MLE[realmonth] > monthlyMaxDemand)
            monthlyMaxDemand = data.MLE[realmonth];
    }

    if (not Math::Zero(monthlyMaxDemand))
    {
        double coeff = 0.;
        for (uint realmonth = 0; realmonth != 12; ++realmonth)
        {
            assert(data.MLE[realmonth] / monthlyMaxDemand >= 0.);
            coeff += Math::Power(data.MLE[realmonth] / monthlyMaxDemand,
                                 area.hydro.intermonthlyBreakdown);
        }

        if (!Math::Zero(coeff))
            coeff = total / coeff;

        for (uint realmonth = 0; realmonth != 12; ++realmonth)
        {
            assert(data.MLE[realmonth] / monthlyMaxDemand >= 0.);
            data.MTG[realmonth] = coeff
                                  * Math::Power(data.MLE[realmonth] / monthlyMaxDemand,
                                                area.hydro.intermonthlyBreakdown);
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

void HydroManagement::prepareMonthlyOptimalGenerations(double* random_reservoir_level,
                                                       uint y,
                                                       uint numSpace)
{
    uint indexArea = 0;
    study.areas.each([&](Data::Area& area) {
        uint z = area.index;

        auto& data = pAreas[numSpace][z];

        auto& minLvl = area.hydro.reservoirLevel[Data::PartHydro::minimum];
        auto& maxLvl = area.hydro.reservoirLevel[Data::PartHydro::maximum];

        int initReservoirLvlMonth = area.hydro.initializeReservoirLevelDate;

        double lvi = -1.;
        if (area.hydro.reservoirManagement)
            lvi = random_reservoir_level[indexArea];

        indexArea++;

        double solutionCost = 0.;
        double solutionCostNoised = 0.;

        if (area.hydro.reservoirManagement)
        {
            auto& problem = *H2O_M_Instanciation(1);

            double totalInflowsYear = prepareMonthlyTargetGenerations(area, data);
            assert(totalInflowsYear >= 0.);

            problem.CoutDepassementVolume = 1e2;
            problem.CoutViolMaxDuVolumeMin = 1e5;
            problem.VolumeInitial = lvi;

            for (unsigned month = 0; month != 12; ++month)
            {
                uint realmonth = (initReservoirLvlMonth + month) % 12;

                uint simulationMonth = study.calendar.mapping.months[realmonth];
                uint firstDay = study.calendar.months[simulationMonth].daysYear.first;

                problem.TurbineMax[month] = totalInflowsYear;
                problem.TurbineCible[month] = data.MTG[realmonth];
                problem.Apport[month] = data.inflows[realmonth];
                problem.VolumeMin[month] = minLvl[firstDay];
                problem.VolumeMax[month] = maxLvl[firstDay];
            }

            H2O_M_OptimiserUneAnnee(&problem, 0);
            switch (problem.ResultatsValides)
            {
            case OUI:
            {
                if (Logs::Verbosity::Debug::enabled)
                    CheckHydroAllocationProblem(area, problem, initReservoirLvlMonth, lvi);

                for (uint month = 0; month != 12; ++month)
                {
                    uint realmonth = (initReservoirLvlMonth + month) % 12;

                    data.MOG[realmonth] = problem.Turbine[month] * area.hydro.reservoirCapacity;
                    data.MOL[realmonth] = problem.Volume[month];
                }
                data.MOL[initReservoirLvlMonth] = lvi;
                solutionCost = problem.ProblemeHydraulique->CoutDeLaSolution;
                solutionCostNoised = problem.ProblemeHydraulique->CoutDeLaSolutionBruite;

                break;
            }
            case NON:
                logs.fatal() << "Year : " << y + 1 << " - hydro: " << area.name
                             << " [month] no solution found";
                AntaresSolverEmergencyShutdown();
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
                data.MOG[realmonth] = data.inflows[realmonth];
                data.MOL[realmonth] = reservoirLevel[realmonth];
            }
        }

#ifndef NDEBUG
        for (uint realmonth = 0; realmonth != 12; ++realmonth)
        {
            assert(!Math::NaN(data.MOG[realmonth]) && "nan value detected for MOG");
            assert(!Math::NaN(data.MOL[realmonth]) && "nan value detected for MOL");
            assert(!Math::Infinite(data.MOG[realmonth]) && "infinite value detected for MOG");
            assert(!Math::Infinite(data.MOL[realmonth]) && "infinite value detected for MOL");
        }
#endif

#if HYDRO_MONTHLY_SOLVER_DEBUG != 0
        {
            String folder;
            folder << study.folderOutput << SEP << "debug" << SEP << "solver" << SEP << (1 + y);
            if (IO::Directory::Create(folder))
            {
                String filename = folder;
                filename << SEP << "monthly." << area.name << ".txt";
                IO::File::Stream file;
                if (file.openRW(filename))
                {
                    if (area.hydro.reservoirManagement)
                        file << "Initial Reservoir Level\t" << lvi << "\n";
                    else
                        file << "Initial Reservoir Level : unrelevant (no reservoir mgmt)\n";
                    file << "\n";

                    auto writeSolutionCost = [&file](const std::string& caption, double cost)
                                             {
                                               std::stringstream stream;
                                               stream << caption << std::fixed << std::setprecision(13) << cost;
                                               std::string cs = stream.str();
                                               file << cs << "\n";
                                             };
                    writeSolutionCost("Solution cost : ", solutionCost);
                    writeSolutionCost("Solution cost (noised) : ", solutionCostNoised);
                    file << "\n\n";

                    file << '\t' << "\tInflows" << '\t' << "\tTarget Gen."
                         << "\tTurbined"
                         << "\tLevels" << '\t' << "\tLvl min" << '\t' << "\tLvl max\n";
                    for (uint month = 0; month != 12; ++month)
                    {
                        uint realmonth = (initReservoirLvlMonth + month) % 12;

                        uint simulationMonth = study.calendar.mapping.months[realmonth];

                        uint firstDay = study.calendar.months[simulationMonth].daysYear.first;

                        auto monthName = study.calendar.text.months[simulationMonth].name;

                        file << monthName[0] << monthName[1] << monthName[2] << '\t';
                        file << '\t';
                        file << data.inflows[realmonth] << '\t';
                        file << data.MTG[realmonth] << '\t';
                        file << data.MOG[realmonth] / area.hydro.reservoirCapacity << '\t';
                        file << data.MOL[realmonth] << '\t';
                        file << minLvl[firstDay] << '\t';
                        file << maxLvl[firstDay] << '\t';
                        file << '\n';
                    }
                }
            }
        }
#endif
    });
}

} // namespace Antares
