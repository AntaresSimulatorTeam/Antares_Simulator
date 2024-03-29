/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <yuni/yuni.h>
#include <antares/study/study.h>
#include <antares/study/area/scratchpad.h>
#include <antares/utils/utils.h>
#include <yuni/io/file.h>
#include <yuni/io/directory.h>
#include "antares/solver/hydro/management/management.h"
#include <antares/antares/fatal-error.h>
#include <antares/writer/i_writer.h>
#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"
#include "antares/solver/hydro/daily2/h2o2_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily2/h2o2_j_fonctions.h"
#include "antares/solver/simulation/sim_extern_variables_globales.h"
#include <sstream>
#include <cassert>
#include <limits>
#include "antares/solver/variable/state.h"
#include <array>
#include <numeric>

using namespace Yuni;

#define SEP IO::Separator

namespace
{
FatalError fatalError(const std::string& areaName, int year)
{
    std::ostringstream msg;
    msg << "Year : " << year + 1 << " - hydro: " << areaName
        << " [daily] fatal error";
    return FatalError(msg.str());
}

FatalError solutionNotFound(const std::string& areaName, int year)
{
    std::ostringstream msg;
    msg << "Year : " << year + 1 << " - hydro: " << areaName
        << " [daily] no solution found";
    return FatalError(msg.str());
}

}

namespace Antares
{
enum
{
    maxOPP = 32
};
enum
{
    maxDailyTargetGen = 32
};

struct DebugData
{
    using MaxPowerType = Matrix<double, double>::ColumnType;
    using ReservoirLevelType = Matrix<double>::ColumnType;

    std::array<double, 366> OPP{0};
    std::array<double, 366> DailyTargetGen{0};

    std::array<double, 365> OVF{0};
    std::array<double, 365> DEV{0};
    std::array<double, 365> VIO{0};
    std::array<double, 12> deviationMax{0};
    std::array<double, 12> violationMax{0};
    std::array<double, 12> WASTE{0};
    std::array<double, 12> CoutTotal{0};
    std::array<double, 12> previousMonthWaste{0};

    Solver::IResultWriter& pWriter;
    const TmpDataByArea& data;
    const VENTILATION_HYDRO_RESULTS_BY_AREA& ventilationResults;
    const double* srcinflows;
    const MaxPowerType& maxP;
    const MaxPowerType& maxE;
    const double* dailyTargetGen;
    const ReservoirLevelType& lowLevel;
    const double reservoirCapacity;

    DebugData(Solver::IResultWriter& writer,
              const TmpDataByArea& data,
              const VENTILATION_HYDRO_RESULTS_BY_AREA& ventilationResults,
              const double* srcinflows,
              const MaxPowerType& maxP,
              const MaxPowerType& maxE,
              const double* dailyTargetGen,
              const ReservoirLevelType& lowLevel,
              double reservoirCapacity) :
     pWriter(writer),
     data(data),
     ventilationResults(ventilationResults),
     srcinflows(srcinflows),
     maxP(maxP),
     maxE(maxE),
     dailyTargetGen(dailyTargetGen),
     lowLevel(lowLevel),
     reservoirCapacity(reservoirCapacity)
    {
        OVF.fill(0);
        DEV.fill(0);
        VIO.fill(0);

        deviationMax.fill(0);
        violationMax.fill(0);
        WASTE.fill(0);
        CoutTotal.fill(0);
    }

    void writeTurb(const std::string filename, uint y) const
    {
        std::ostringstream buffer, path;
        path << "debug" << SEP << "solver" << SEP << (1 + y) << SEP << filename;

        buffer << "\tTurbine\t\t\tOPP\t\t\t\tTurbine Cible\tDLE\t\t\t\tDLN\n";
        for (uint day = 0; day != 365; ++day)
        {
            double value = ventilationResults.HydrauliqueModulableQuotidien[day];
            buffer << day << '\t' << value << '\t' << OPP[day] << '\t' << DailyTargetGen[day]
                   << '\t' << data.DLE[day] << '\t' << data.DLN[day];
            buffer << '\n';
        }
        auto buffer_str = buffer.str();
        pWriter.addEntryFromBuffer(path.str(), buffer_str);
    }

    void writeDailyDebugData(const Date::Calendar& calendar,
                             int initReservoirLvlMonth,
                             uint y,
                             const Data::AreaName& areaName) const
    {
        std::ostringstream buffer, path;
        path << "debug" << SEP << "solver" << SEP << (1 + y) << SEP << "daily." << areaName.c_str()
             << ".txt";

        buffer << "\tNiveau init : " << data.MOL[initReservoirLvlMonth] << "\n";
        for (uint month = 0; month != 12; ++month)
        {
            uint realmonth = (initReservoirLvlMonth + month) % 12;
            uint simulationMonth = calendar.mapping.months[realmonth];

            auto daysPerMonth = calendar.months[simulationMonth].days;

            auto monthName = calendar.text.months[simulationMonth].name;

            uint firstDay = calendar.months[simulationMonth].daysYear.first;
            uint endDay = firstDay + daysPerMonth;

            buffer << "\n";
            buffer << "-------------\n";
            buffer << monthName.c_str() << "\n";
            buffer << "-------------\n";
            buffer << "\t\t\tNiveauMin\tApports\t\tTurbMax\t\tTurbCible\tTurbCible "
                      "MAJ\tNiveaux D\tNiveaux F\tTurbines\t";
            buffer << "Overflows\tDeviations\tViolations\tDeviation Max\tViolation "
                      "Max\tWaste\t\tCout total\tTurb mois no previous W\t\tTurb mois + "
                      "previous W \n";

            uint dayMonth = 1;
            for (uint day = firstDay; day != endDay; ++day)
            {
                double turbines = ventilationResults.HydrauliqueModulableQuotidien[day] / reservoirCapacity;
                double niveauDeb = ventilationResults.NiveauxReservoirsDebutJours[day];
                double niveauFin = ventilationResults.NiveauxReservoirsFinJours[day];
                double apports = srcinflows[day] / reservoirCapacity;
                double turbMax = maxP[day] * maxE[day] / reservoirCapacity;
                double turbCible = dailyTargetGen[day] / reservoirCapacity;
                double turbCibleUpdated = dailyTargetGen[day] / reservoirCapacity
                                          + previousMonthWaste[realmonth] / daysPerMonth;
                buffer << day << '\t' << '\t' << dayMonth << '\t' << lowLevel[day] * 100 << '\t'
                       << apports * 100 << '\t' << turbMax * 100 << '\t' << turbCible * 100 << '\t'
                       << turbCibleUpdated * 100 << '\t' << '\t' << niveauDeb * 100 << '\t'
                       << niveauFin * 100 << '\t' << turbines * 100 << '\t' << OVF[day] * 100
                       << '\t' << DEV[day] * 100 << '\t' << VIO[day] * 100;
                if (dayMonth == 1)
                {
                    buffer << '\t' << deviationMax[realmonth] * 100 << '\t' << '\t'
                           << violationMax[realmonth] * 100 << '\t' << '\t'
                           << WASTE[realmonth] * 100 << '\t' << CoutTotal[realmonth] << '\t'
                           << (data.MOG[realmonth] / reservoirCapacity) * 100 << '\t' << '\t'
                           << '\t' << '\t' << '\t'
                           << (data.MOG[realmonth] / reservoirCapacity
                               + previousMonthWaste[realmonth])
                                * 100;
                }
                buffer << '\n';

                dayMonth++;
            }
        }
        auto buffer_str = buffer.str();
        pWriter.addEntryFromBuffer(path.str(), buffer_str);
    }
};

inline void HydroManagement::prepareDailyOptimalGenerations(Solver::Variable::State& state,
                                                            Data::Area& area,
                                                            uint y,
                                                            Antares::Data::Area::ScratchMap& scratchmap)
{
    auto const srcinflows = area.hydro.series->storage.getColumn(y);

    auto& data = tmpDataByArea_[&area];

    auto& scratchpad = scratchmap.at(&area);

    auto& meanMaxDailyGenPower = scratchpad.meanMaxDailyGenPower;

    const uint tsIndex =  meanMaxDailyGenPower.getSeriesIndex(y);

    int initReservoirLvlMonth = area.hydro.initializeReservoirLevelDate;

    double reservoirCapacity = area.hydro.reservoirCapacity;

    auto& lowLevel = area.hydro.reservoirLevel[Data::PartHydro::minimum];

    double dailyTargetGen[12 * maxDailyTargetGen];

    uint dayYear = 0;

    auto const& dailyNbHoursAtGenPmax = area.hydro.dailyNbHoursAtGenPmax;

    
    auto const& maxP = meanMaxDailyGenPower[tsIndex];
    auto const& maxE = dailyNbHoursAtGenPmax[0];

    auto& ventilationResults = ventilationResults_[area.index];

    std::shared_ptr<DebugData> debugData(nullptr);

    if (parameters_.hydroDebug)
    {
        debugData = std::make_shared<DebugData>(resultWriter_,
                                                data,
                                                ventilationResults,
                                                srcinflows,
                                                maxP,
                                                maxE,
                                                dailyTargetGen,
                                                lowLevel,
                                                reservoirCapacity);
    }

    for (uint month = 0; month != 12; ++month)
    {
        auto daysPerMonth = calendar_.months[month].days;
        assert(daysPerMonth <= maxOPP);
        assert(daysPerMonth <= maxDailyTargetGen);
        assert(daysPerMonth + dayYear - 1 < meanMaxDailyGenPower.timeSeries.height);

        for (uint day = 0; day != daysPerMonth; ++day)
        {
            auto dYear = day + dayYear;
            assert(day < 32);
            assert(dYear < 366);

            if (debugData)
                debugData->OPP[dYear] = maxP[dYear] * maxE[dYear];
        }

        dayYear += daysPerMonth;
    }

    if (!area.hydro.useHeuristicTarget
        || (area.hydro.useHeuristicTarget && !area.hydro.followLoadModulations))
    {
        dayYear = 0;
        for (uint month = 0; month != 12; ++month)
        {
            auto daysPerMonth = calendar_.months[month].days;
            for (uint day = 0; day != daysPerMonth; ++day)
            {
                dailyTargetGen[dayYear + day] = srcinflows[dayYear + day];
            }

            dayYear += daysPerMonth;
        }
    }

    else
    {
        dayYear = 0;
        for (uint month = 0; month != 12; ++month)
        {
            uint realmonth = calendar_.months[month].realmonth;
            auto daysPerMonth = calendar_.months[month].days;

            if (area.hydro.followLoadModulations)
            {
                auto demandMax = -std::numeric_limits<double>::infinity();
                for (uint day = 0; day != daysPerMonth; ++day)
                {
                    auto dYear = day + dayYear;
                    if (data.DLE[dYear] > demandMax)
                        demandMax = data.DLE[dYear];
                }

                if (!Utils::isZero(demandMax))
                {
                    assert(std::abs(demandMax) > 1e-12);
                    double coeff = 0.;

                    for (uint day = 0; day != daysPerMonth; ++day)
                    {
                        auto dYear = day + dayYear;
                        coeff += std::pow(data.DLE[dYear] / demandMax,
                                area.hydro.interDailyBreakdown);
                    }
                    coeff = data.MOG[realmonth] / coeff;

                    for (uint day = 0; day != daysPerMonth; ++day)
                    {
                        auto dYear = day + dayYear;
                        dailyTargetGen[dYear] = coeff
                                                * std::pow(data.DLE[dYear] / demandMax,
                                                              area.hydro.interDailyBreakdown);
                    }
                }
                else
                {
                    assert(daysPerMonth > 0);
                    double coeff = data.MOG[realmonth] / daysPerMonth;

                    for (uint day = 0; day != daysPerMonth; ++day)
                        dailyTargetGen[day + dayYear] = coeff;
                }
            }

            dayYear += daysPerMonth;
        }
    }

    if (debugData)
    {
        for (uint month = 0; month != 12; ++month)
        {
            auto daysPerMonth = calendar_.months[month].days;

            for (uint day = 0; day != daysPerMonth; ++day)
            {
                auto dYear = day + dayYear;
                debugData->DailyTargetGen[dYear] = dailyTargetGen[dYear];
            }
        }
    }

    if (not area.hydro.reservoirManagement)
    {
        for (uint month = 0; month != 12; ++month)
        {
            uint realmonth = (initReservoirLvlMonth + month) % 12;
            uint simulationMonth = calendar_.mapping.months[realmonth];

            auto daysPerMonth = calendar_.months[simulationMonth].days;

            uint firstDay = calendar_.months[simulationMonth].daysYear.first;
            uint endDay = firstDay + daysPerMonth;

            DONNEES_MENSUELLES* problem = H2O_J_Instanciation();
            H2O_J_AjouterBruitAuCout(*problem);
            problem->NombreDeJoursDuMois = (int)daysPerMonth;
            problem->TurbineDuMois = data.MOG[realmonth];

            uint dayMonth = 0;
            for (uint day = firstDay; day != endDay; ++day)
            {
                problem->TurbineMax[dayMonth] = maxP[day] * maxE[day];
                problem->TurbineMin[dayMonth] = data.dailyMinGen[day];
                problem->TurbineCible[dayMonth] = dailyTargetGen[day];
                dayMonth++;
            }

            H2O_J_OptimiserUnMois(problem);
            switch (problem->ResultatsValides)
            {
            case OUI:
                dayMonth = 0;
                for (uint day = firstDay; day != endDay; ++day)
                {
                    ventilationResults.HydrauliqueModulableQuotidien[day] = problem->Turbine[dayMonth];
                    dayMonth++;
                }
                break;
            case NON:
                throw solutionNotFound(area.name.c_str(), y);
                break;
            case EMERGENCY_SHUT_DOWN:
                throw fatalError(area.name.c_str(), y);
                break;
            }

            H2O_J_Free(problem);

#ifndef NDEBUG
            for (uint day = firstDay; day != endDay; ++day)
            {
                assert(!std::isnan(ventilationResults.HydrauliqueModulableQuotidien[day]));
                assert(!std::isinf(ventilationResults.HydrauliqueModulableQuotidien[day]));
            }
#endif
        }

        if (debugData)
        {
            const auto filename = std::string("daily.") + area.name.c_str() + ".txt";
            debugData->writeTurb(filename, y);
        }
    }

    else
    {
        double monthInitialLevel = data.MOL[initReservoirLvlMonth];
        double wasteFromPreviousMonth = 0.;

        Hydro_problem_costs h2o2_optim_costs(parameters_);

        for (uint month = 0; month != 12; ++month)
        {
            uint realmonth = (initReservoirLvlMonth + month) % 12;
            uint simulationMonth = calendar_.mapping.months[realmonth];

            auto daysPerMonth = calendar_.months[simulationMonth].days;

            uint firstDay = calendar_.months[simulationMonth].daysYear.first;
            uint endDay = firstDay + daysPerMonth;

            DONNEES_MENSUELLES_ETENDUES problem = H2O2_J_Instanciation();
            H2O2_J_apply_costs(h2o2_optim_costs, problem);

            if (debugData)
                debugData->previousMonthWaste[realmonth]
                  = wasteFromPreviousMonth / reservoirCapacity;

            problem.NombreDeJoursDuMois = (int)daysPerMonth;

            problem.TurbineDuMois
              = (data.MOG[realmonth] + wasteFromPreviousMonth) / reservoirCapacity;
            problem.NiveauInitialDuMois = monthInitialLevel;
            problem.reservoirCapacity = reservoirCapacity;

            uint dayMonth = 0;
            for (uint day = firstDay; day != endDay; ++day)
            {
                problem.TurbineMax[dayMonth] = maxP[day] * maxE[day] / reservoirCapacity;
                problem.TurbineMin[dayMonth] = data.dailyMinGen[day] / reservoirCapacity;

                problem.TurbineCible[dayMonth]
                  = (dailyTargetGen[day] + wasteFromPreviousMonth / daysPerMonth)
                    / reservoirCapacity;

                problem.niveauBas[dayMonth] = lowLevel[(day + 1) % 365];
                problem.apports[dayMonth] = srcinflows[day] / reservoirCapacity;

                dayMonth++;
            }

            H2O2_J_OptimiserUnMois(problem);

            switch (problem.ResultatsValides)
            {
            case OUI:

                if (debugData)
                {
                    debugData->deviationMax[realmonth] = problem.deviationMax;
                    debugData->violationMax[realmonth] = problem.violationMax;
                    debugData->WASTE[realmonth] = problem.waste;
                    debugData->CoutTotal[realmonth] = problem.CoutSolution;
                }

                dayMonth = 0;
                for (uint day = firstDay; day != endDay; ++day)
                {
                    ventilationResults.HydrauliqueModulableQuotidien[day]
                      = problem.Turbine[dayMonth] * reservoirCapacity;

                    ventilationResults.NiveauxReservoirsFinJours[day] = problem.niveauxFinJours[dayMonth];

                    if (debugData)
                    {
                        debugData->OVF[day] = problem.overflows[dayMonth];
                        debugData->DEV[day] = problem.deviations[dayMonth];
                        debugData->VIO[day] = problem.violations[dayMonth];
                    }

                    dayMonth++;
                }

                ventilationResults.NiveauxReservoirsDebutJours[firstDay] = monthInitialLevel;
                for (uint day = firstDay + 1; day != endDay; ++day)
                    ventilationResults.NiveauxReservoirsDebutJours[day]
                      = ventilationResults.NiveauxReservoirsFinJours[day - 1];

                monthInitialLevel = problem.niveauxFinJours[dayMonth - 1];

                wasteFromPreviousMonth = problem.waste * reservoirCapacity;

                break;
            case NON:
                throw solutionNotFound(area.name.c_str(), y);
                break;
            case EMERGENCY_SHUT_DOWN:
                throw fatalError(area.name.c_str(), y);
                break;
            }

            H2O2_J_Free(problem);
        }

        uint firstDaySimu = parameters_.simulationDays.first;
        state.problemeHebdo->previousSimulationFinalLevel[area.index]
          = ventilationResults.NiveauxReservoirsDebutJours[firstDaySimu] * reservoirCapacity;

        if (debugData)
        {
            debugData->writeDailyDebugData(calendar_, initReservoirLvlMonth, y, area.name);
        }
    }
}

void HydroManagement::prepareDailyOptimalGenerations(Solver::Variable::State& state,
                                                     uint y,
                                                     Antares::Data::Area::ScratchMap& scratchmap)
{
    areas_.each(
      [&](Data::Area& area) {
          prepareDailyOptimalGenerations(state, area, y, scratchmap);
          });
}
} // namespace Antares
