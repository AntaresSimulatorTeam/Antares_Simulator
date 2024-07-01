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
#include "antares/solver/hydro/management/HydroErrorsCollector.h"
#include "antares/solver/hydro/management/MinGenerationScaling.h"
#include "antares/solver/hydro/management/PrepareInflows.h"
#include "antares/study/study.h"

namespace Antares
{

class HydroInputsChecker
{
public:
    explicit HydroInputsChecker(Antares::Data::Study& study);
    void Execute(uint year);
    void CheckForErrors() const;
    void CheckFinalReservoirLevelsConfiguration(uint year);

private:
    Data::AreaList& areas_;
    const Data::Parameters& parameters_;
    const Date::Calendar& calendar_;
    PrepareInflows prepareInflows_;
    MinGenerationScaling minGenerationScaling_;
    const Data::TimeSeries::TS& scenarioInitialHydroLevels_;
    const Data::TimeSeries::TS& scenarioFinalHydroLevels_;
    HydroErrorsCollector errorCollector_;

    //! return false if checkGenerationPowerConsistency or checkMinGeneration returns false
    bool checkMonthlyMinGeneration(uint year, const Data::Area& area);
    //! check Yearly minimum generation is lower than available inflows
    bool checkYearlyMinGeneration(uint year, const Data::Area& area);
    //! check Weekly minimum generation is lower than available inflows
    bool checkWeeklyMinGeneration(uint year, const Data::Area& area);
    //! check Hourly minimum generation is lower than available inflows
    bool checkGenerationPowerConsistency(uint year);
    //! return false if checkGenerationPowerConsistency or checkMinGeneration returns false
    bool checksOnGenerationPowerBounds(uint year);
    //! check minimum generation is lower than available inflows
    bool checkMinGeneration(uint year);
};

} // namespace Antares
