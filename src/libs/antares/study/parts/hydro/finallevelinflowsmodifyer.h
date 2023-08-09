/*
** Copyright 2007-2023 RTE
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
#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDRO_FINAL_LEVEL_INFLOWS_MODIFYER_H__
#define __ANTARES_LIBS_STUDY_PARTS_HYDRO_FINAL_LEVEL_INFLOWS_MODIFYER_H__

#include <vector>
#include <yuni/yuni.h>
#include <antares/study/parameters.h>
#include <antares/array/matrix.h>

namespace Antares::Data
{
class PartHydro;
/*!
 ** \brief Final Reservoir Level data for a single area
 */
class FinalLevelInflowsModifier
{
public:
    FinalLevelInflowsModifier() = delete;
    FinalLevelInflowsModifier(const PartHydro& hydro,
                              const unsigned int& areaIndex,
                              const AreaName& areaName);

private:
    // Simulation Data
    uint simEndDay;

    // simulation year
    // overwritten after each MC year
    uint yearIndex;

    // data per area
    // data overwritten after each MC year
    double initialReservoirLevel = -1.;
    double finalReservoirLevel = -1.;
    double deltaReservoirLevel;
    double reservoirCapacity;
    double lowLevelLastDay;
    double highLevelLastDay;
    int initReservoirLvlMonth;

    const PartHydro& hydro;
    const unsigned int& areaIndex;
    const AreaName& areaName;
public:
    // vectors containing data necessary for final reservoir level calculation
    // for one area and all MC years
    // vector indexes correspond to the MC years
    std::vector<bool> includeFinalReservoirLevel;
    std::vector<double> endLevel;
    std::vector<double> deltaLevel;

private:
    // methods:

    void fillEmpty();

    double calculateTotalInflows() const;

    bool preCheckStartAndEndSim() const;

    bool preCheckYearlyInflow(double totalYearInflows) const;

    bool preCheckRuleCurves() const;

    void initializeGeneralData(const Data::Parameters& parameters, uint year);

    void initializePerAreaData(const Matrix<double>& scenarioInitialHydroLevels,
                               const Matrix<double>& scenarioFinalHydroLevels);
    void initializePreCheckData();

    void ruleCurveForSimEndReal();

public:
    void initializeData(const Matrix<double>& scenarioInitialHydroLevels,
                        const Matrix<double>& scenarioFinalHydroLevels,
                        const Data::Parameters& parameters,
                        uint year);
    bool isActive();

    void updateInflows();

    bool makeChecks();
};
} // namespace Antares::Data

#endif /*__ANTARES_LIBS_STUDY_PARTS_HYDRO_FINAL_LEVEL_INFLOWS_MODIFYER_H__*/