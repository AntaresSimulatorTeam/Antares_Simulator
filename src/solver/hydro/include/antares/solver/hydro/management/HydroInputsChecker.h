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
#pragma once
#include <antares/study/area/area.h>
#include "antares/date/date.h"
#include "antares/solver/hydro/management/MinGenerationScaling.h"
#include "antares/solver/hydro/management/PrepareInflows.h"

namespace Antares
{
class HydroSingleCheck
{
public:
    HydroSingleCheck() = default;
    virtual void Run() = 0;
};

class HydroChecks: public HydroSingleCheck
{
public:
    HydroChecks() = default;
    void Run() override;

private:
    // can we do better than shared_ptr ?
    std::vector<std::shared_ptr<HydroSingleCheck>> checks;
};

class HydroInputsChecker
{
public:
    HydroInputsChecker(Antares::Data::Study& study, Solver::IResultWriter& resultWriter);
    void Execute();

private:
    Antares::Data::Study& study_;
    Data::AreaList& areas_;
    const Date::Calendar& calendar_;
    const uint firstYear_;
    const uint endYear_;
    const Data::Parameters& parameters_;
    PrepareInflows prepareInflows_;
    MinGenerationScaling minGenerationScaling_;
    Data::SimulationMode simulationMode_;
    Solver::IResultWriter& resultWriter_;

    //! return false if checkGenerationPowerConsistency or checkMinGeneration returns false
    bool checkMonthlyMinGeneration(uint year, const Data::Area& area) const;
    //! check Yearly minimum generation is lower than available inflows
    bool checkYearlyMinGeneration(uint year, const Data::Area& area) const;
    //! check Weekly minimum generation is lower than available inflows
    bool checkWeeklyMinGeneration(uint year, const Data::Area& area) const;
    //! check Hourly minimum generation is lower than available inflows
    bool checkGenerationPowerConsistency(uint year) const;
    //! return false if checkGenerationPowerConsistency or checkMinGeneration returns false
    bool checksOnGenerationPowerBounds(uint year) const;
    //! check minimum generation is lower than available inflows
    bool checkMinGeneration(uint year) const;
    // void prepareNetDemand(uint year,
    //                       Data::SimulationMode mode,
    //                       const Antares::Data::Area::ScratchMap& scratchmap);
    // void prepareEffectiveDemand();
    /*void prepareMonthlyOptimalGenerations(double* random_reservoir_level, uint y);
    double prepareMonthlyTargetGenerations(Data::Area& area, Antares::Data::TmpDataByArea& data);*/
    // void PrepareDataFromClustersInMustrunMode(Data::Area::ScratchMap& scratchmap, uint year);
};

} // namespace Antares