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
#include "../daily2/h2o2_j_donnees_mensuelles.h"
#include "../daily2/h2o2_j_fonctions.h"
#include "../../simulation/sim_extern_variables_globales.h"
#include <cassert>
#include <variable/state.h>

using namespace Yuni;

#define SEP IO::Separator

#define HYDRO_DAILY_SOLVER_DEBUG 0

namespace Antares
{
enum
{
    maxOPP = 32
};
enum
{
    maxDTG = 32
};

inline void HydroManagement::prepareDailyOptimalGenerations(Solver::Variable::State& state,
                                                            Data::Area& area,
                                                            uint y,
                                                            uint numSpace)
{
    uint z = area.index;
    assert(z < study.areas.size());

    auto& ptchro = *NumeroChroniquesTireesParPays[numSpace][z];

    auto& inflowsmatrix = area.hydro.series->storage;
    auto tsIndex = (uint)ptchro.Hydraulique;
    auto const& srcinflows = inflowsmatrix[tsIndex < inflowsmatrix.width ? tsIndex : 0];

    auto& data = pAreas[numSpace][z];

    auto& scratchpad = *(area.scratchpad[numSpace]);

    int initReservoirLvlMonth = area.hydro.initializeReservoirLevelDate;

    double reservoirCapacity = area.hydro.reservoirCapacity;

    auto& lowLevel = area.hydro.reservoirLevel[Data::PartHydro::minimum];

    double dtg[12 * maxDTG];

    uint dayYear = 0;

    auto const& maxPower = area.hydro.maxPower;

    auto const& maxP = maxPower[Data::PartHydro::genMaxP];
    auto const& maxE = maxPower[Data::PartHydro::genMaxE];

    auto const& valgen = *ValeursGenereesParPays[numSpace][z];

#if HYDRO_DAILY_SOLVER_DEBUG != 0
    double debugOPP[366];
    double debugDTG[366];
#endif

    for (uint month = 0; month != 12; ++month)
    {
        auto daysPerMonth = study.calendar.months[month].days;
        assert(daysPerMonth <= maxOPP);
        assert(daysPerMonth <= maxDTG);
        assert(daysPerMonth + dayYear - 1 < maxPower.height);

        for (uint day = 0; day != daysPerMonth; ++day)
        {
            auto dYear = day + dayYear;
            assert(day < 32);
            assert(dYear < 366);
            scratchpad.optimalMaxPower[dYear] = maxP[dYear];

#if HYDRO_DAILY_SOLVER_DEBUG != 0
            debugOPP[dYear] = maxP[dYear] * maxE[dYear];
#endif
        }

        dayYear += daysPerMonth;
    }

    if (not area.hydro.useHeuristicTarget)
    {
        dayYear = 0;
        for (uint month = 0; month != 12; ++month)
        {
            auto daysPerMonth = study.calendar.months[month].days;
            for (uint day = 0; day != daysPerMonth; ++day)
            {
                dtg[dayYear + day] = srcinflows[dayYear + day];
            }

            dayYear += daysPerMonth;
        }
    }

    else
    {
        dayYear = 0;
        for (uint month = 0; month != 12; ++month)
        {
            uint realmonth = study.calendar.months[month].realmonth;
            auto daysPerMonth = study.calendar.months[month].days;

            if (area.hydro.followLoadModulations)
            {
                auto demandMax = -std::numeric_limits<double>::infinity();
                for (uint day = 0; day != daysPerMonth; ++day)
                {
                    auto dYear = day + dayYear;
                    if (data.DLE[dYear] > demandMax)
                        demandMax = data.DLE[dYear];
                }

                if (not Math::Zero(demandMax))
                {
                    assert(Math::Abs(demandMax) > 1e-12);
                    double coeff = 0.;

                    for (uint day = 0; day != daysPerMonth; ++day)
                    {
                        auto dYear = day + dayYear;
                        coeff += Math::Power(data.DLE[dYear] / demandMax,
                                             area.hydro.interDailyBreakdown);
                    }
                    coeff = data.MOG[realmonth] / coeff;

                    for (uint day = 0; day != daysPerMonth; ++day)
                    {
                        auto dYear = day + dayYear;
                        dtg[dYear] = coeff
                                     * Math::Power(data.DLE[dYear] / demandMax,
                                                   area.hydro.interDailyBreakdown);
                    }
                }
                else
                {
                    assert(daysPerMonth > 0);
                    double coeff = data.MOG[realmonth] / daysPerMonth;

                    for (uint day = 0; day != daysPerMonth; ++day)
                        dtg[day + dayYear] = coeff;
                }
            }
            else
            {
                for (uint day = 0; day != daysPerMonth; ++day)
                    dtg[day + dayYear] = srcinflows[dayYear + day];
            }

#if HYDRO_DAILY_SOLVER_DEBUG != 0
            for (uint day = 0; day != daysPerMonth; ++day)
            {
                auto dYear = day + dayYear;
                debugDTG[dYear] = dtg[dYear];
            }
#endif

            dayYear += daysPerMonth;
        }
    }

    if (not area.hydro.reservoirManagement)
    {
        for (uint month = 0; month != 12; ++month)
        {
            uint realmonth = (initReservoirLvlMonth + month) % 12;
            uint simulationMonth = study.calendar.mapping.months[realmonth];

            auto daysPerMonth = study.calendar.months[simulationMonth].days;

            uint firstDay = study.calendar.months[simulationMonth].daysYear.first;
            uint endDay = firstDay + daysPerMonth;

            DONNEES_MENSUELLES& problem = *H2O_J_Instanciation();
            problem.NombreDeJoursDuMois = (int)daysPerMonth;
            problem.TurbineDuMois = data.MOG[realmonth];

            uint dayMonth = 0;
            for (uint day = firstDay; day != endDay; ++day)
            {
                problem.TurbineMax[dayMonth] = maxP[day] * maxE[day];
                problem.TurbineCible[dayMonth] = dtg[day];
                dayMonth++;
            }

            H2O_J_OptimiserUnMois(&problem);
            switch (problem.ResultatsValides)
            {
            case OUI:
                dayMonth = 0;
                for (uint day = firstDay; day != endDay; ++day)
                {
                    valgen.HydrauliqueModulableQuotidien[day] = problem.Turbine[dayMonth];
                    dayMonth++;
                }
                break;
            case NON:
                logs.fatal() << "Year : " << y + 1 << " - hydro: " << area.name
                             << " [daily] no solution found";
                AntaresSolverEmergencyShutdown();
                break;
            case EMERGENCY_SHUT_DOWN:
                AntaresSolverEmergencyShutdown();
                break;
            }

            H2O_J_Free(&problem);

#ifndef NDEBUG
            for (uint day = firstDay; day != endDay; ++day)
            {
                assert(!Math::NaN(valgen.HydrauliqueModulableQuotidien[day]));
                assert(!Math::Infinite(valgen.HydrauliqueModulableQuotidien[day]));
            }
#endif
        }

#if HYDRO_DAILY_SOLVER_DEBUG != 0
        {
            String folder;
            folder << study.folderOutput << SEP << "debug" << SEP << "solver" << SEP << (1 + y);
            if (IO::Directory::Create(folder))
            {
                String filename = folder;
                filename << SEP << "daily." << area.name << ".txt";
                IO::File::Stream file;
                if (file.openRW(filename))
                {
                    file << "\tTurbine\t\t\tOPP\t\t\t\tTurbine Cible\tDLE\t\t\t\tDLN\n";
                    for (uint day = 0; day != 365; ++day)
                    {
                        double value = valgen.HydrauliqueModulableQuotidien[day];
                        file << day << '\t' << value << '\t' << debugOPP[day] << '\t'
                             << debugDTG[day] << '\t' << data.DLE[day] << '\t' << data.DLN[day];
                        file << '\n';
                    }
                }
            }
        }
#endif
    }

    else
    {
        double monthInitialLevel = data.MOL[initReservoirLvlMonth];
        double wasteFromPreviousMonth = 0.;

#if HYDRO_DAILY_SOLVER_DEBUG != 0
        double OVF[365];
        double DEV[365];
        double VIO[365];
        double deviationMax[12];
        double violationMax[12];
        double WASTE[12];
        double CoutTotal[12];
        double previousMonthWaste[12];

        for (int i = 0; i < 365; i++)
        {
            OVF[i] = 0.;
            DEV[i] = 0.;
            VIO[i] = 0.;
        }

        for (int i = 0; i < 12; i++)
        {
            deviationMax[i] = 0.;
            violationMax[i] = 0.;
            WASTE[i] = 0.;
            CoutTotal[i] = 0.;
        }
#endif

        for (uint month = 0; month != 12; ++month)
        {
            uint realmonth = (initReservoirLvlMonth + month) % 12;
            uint simulationMonth = study.calendar.mapping.months[realmonth];

            auto daysPerMonth = study.calendar.months[simulationMonth].days;

            uint firstDay = study.calendar.months[simulationMonth].daysYear.first;
            uint endDay = firstDay + daysPerMonth;

            DONNEES_MENSUELLES_ETENDUES& problem = *H2O2_J_Instanciation();

#if HYDRO_DAILY_SOLVER_DEBUG != 0
            previousMonthWaste[realmonth] = wasteFromPreviousMonth / reservoirCapacity;
#endif

            problem.NombreDeJoursDuMois = (int)daysPerMonth;

            problem.TurbineDuMois
              = (data.MOG[realmonth] + wasteFromPreviousMonth) / reservoirCapacity;
            problem.NiveauInitialDuMois = monthInitialLevel;
            problem.reservoirCapacity = reservoirCapacity;

            uint dayMonth = 0;
            for (uint day = firstDay; day != endDay; ++day)
            {
                problem.TurbineMax[dayMonth] = maxP[day] * maxE[day] / reservoirCapacity;

                problem.TurbineCible[dayMonth]
                  = (dtg[day] + wasteFromPreviousMonth / daysPerMonth) / reservoirCapacity;

                problem.niveauBas[dayMonth] = lowLevel[(day + 1) % 365];
                problem.apports[dayMonth] = srcinflows[day] / reservoirCapacity;

                dayMonth++;
            }

            H2O2_J_OptimiserUnMois(&problem);

            switch (problem.ResultatsValides)
            {
            case OUI:

#if HYDRO_DAILY_SOLVER_DEBUG != 0
                deviationMax[realmonth] = problem.deviationMax;
                violationMax[realmonth] = problem.violationMax;
                WASTE[realmonth] = problem.waste;
                CoutTotal[realmonth] = problem.CoutSolution;
#endif

                dayMonth = 0;
                for (uint day = firstDay; day != endDay; ++day)
                {
                    valgen.HydrauliqueModulableQuotidien[day]
                      = problem.Turbine[dayMonth] * reservoirCapacity;

                    valgen.NiveauxReservoirsFinJours[day] = problem.niveauxFinJours[dayMonth];

#if HYDRO_DAILY_SOLVER_DEBUG != 0
                    OVF[day] = problem.overflows[dayMonth];
                    DEV[day] = problem.deviations[dayMonth];
                    VIO[day] = problem.violations[dayMonth];
#endif

                    dayMonth++;
                }

                valgen.NiveauxReservoirsDebutJours[firstDay] = monthInitialLevel;
                for (uint day = firstDay + 1; day != endDay; ++day)
                    valgen.NiveauxReservoirsDebutJours[day]
                      = valgen.NiveauxReservoirsFinJours[day - 1];

                monthInitialLevel = problem.niveauxFinJours[dayMonth - 1];

                wasteFromPreviousMonth = problem.waste * reservoirCapacity;

                break;
            case NON:
                logs.fatal() << "Year : " << y + 1 << " - hydro: " << area.name
                             << " [daily] no solution found";
                AntaresSolverEmergencyShutdown();
                break;
            case EMERGENCY_SHUT_DOWN:
                AntaresSolverEmergencyShutdown();
                break;
            }

            H2O2_J_Free(&problem);
        }

        uint firstDaySimu = study.parameters.simulationDays.first;
        state.problemeHebdo->previousSimulationFinalLevel[z]
          = valgen.NiveauxReservoirsDebutJours[firstDaySimu] * reservoirCapacity;

#if HYDRO_DAILY_SOLVER_DEBUG != 0
        {
            String folder;
            folder << study.folderOutput << SEP << "debug" << SEP << "solver" << SEP << (1 + y);
            if (IO::Directory::Create(folder))
            {
                String filename = folder;
                filename << SEP << "daily." << area.name << ".txt";
                IO::File::Stream file;
                if (file.openRW(filename))
                {
                    file << "\tNiveau init : " << data.MOL[initReservoirLvlMonth] << "\n";

                    for (uint month = 0; month != 12; ++month)
                    {
                        uint realmonth = (initReservoirLvlMonth + month) % 12;
                        uint simulationMonth = study.calendar.mapping.months[realmonth];

                        auto daysPerMonth = study.calendar.months[simulationMonth].days;

                        auto monthName = study.calendar.text.months[simulationMonth].name;

                        uint firstDay = study.calendar.months[simulationMonth].daysYear.first;
                        uint endDay = firstDay + daysPerMonth;

                        file << "\n";
                        file << "-------------\n";
                        file << monthName << "\n";
                        file << "-------------\n";
                        file << "\t\t\tNiveauMin\tApports\t\tTurbMax\t\tTurbCible\tTurbCible "
                                "MAJ\tNiveaux D\tNiveaux F\tTurbines\t";
                        file << "Overflows\tDeviations\tViolations\tDeviation Max\tViolation "
                                "Max\tWaste\t\tCout total\tTurb mois no previous W\t\tTurb mois + "
                                "previous W \n";

                        uint dayMonth = 1;
                        for (uint day = firstDay; day != endDay; ++day)
                        {
                            double turbines
                              = valgen.HydrauliqueModulableQuotidien[day] / reservoirCapacity;
                            double niveauDeb = valgen.NiveauxReservoirsDebutJours[day];
                            double niveauFin = valgen.NiveauxReservoirsFinJours[day];
                            double apports = srcinflows[day] / reservoirCapacity;
                            double turbMax = maxP[day] * maxE[day] / reservoirCapacity;
                            double turbCible = dtg[day] / reservoirCapacity;
                            double turbCibleUpdated
                              = dtg[day] / reservoirCapacity
                                + previousMonthWaste[realmonth] / daysPerMonth;
                            file << day << '\t' << '\t' << dayMonth << '\t' << lowLevel[day] * 100
                                 << '\t' << apports * 100 << '\t' << turbMax * 100 << '\t'
                                 << turbCible * 100 << '\t' << turbCibleUpdated * 100 << '\t'
                                 << '\t' << niveauDeb * 100 << '\t' << niveauFin * 100 << '\t'
                                 << turbines * 100 << '\t' << OVF[day] * 100 << '\t'
                                 << DEV[day] * 100 << '\t' << VIO[day] * 100;
                            if (dayMonth == 1)
                            {
                                file << '\t' << deviationMax[realmonth] * 100 << '\t' << '\t'
                                     << violationMax[realmonth] * 100 << '\t' << '\t'
                                     << WASTE[realmonth] * 100 << '\t' << CoutTotal[realmonth]
                                     << '\t' << (data.MOG[realmonth] / reservoirCapacity) * 100
                                     << '\t' << '\t' << '\t' << '\t' << '\t'
                                     << (data.MOG[realmonth] / reservoirCapacity
                                         + previousMonthWaste[realmonth])
                                          * 100;
                            }
                            file << '\n';

                            dayMonth++;
                        }
                    }
                }
                file.close();
            }
        }
#endif
    }
}

void HydroManagement::prepareDailyOptimalGenerations(Solver::Variable::State& state,
                                                     uint y,
                                                     uint numSpace)
{
    study.areas.each(
      [&](Data::Area& area) { prepareDailyOptimalGenerations(state, area, y, numSpace); });
}

} // namespace Antares
