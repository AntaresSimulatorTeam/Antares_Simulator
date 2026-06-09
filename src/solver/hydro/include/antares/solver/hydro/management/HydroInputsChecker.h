// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <antares/study/area/area.h>
#include "antares/date/date.h"
#include "antares/solver/hydro/management/HydroErrorsCollector.h"
#include "antares/solver/hydro/management/MinGenerationScaling.h"
#include "antares/solver/hydro/management/PrepareInflows.h"
#include "antares/study/study.h"

namespace Antares
{

class HydroInputsChecker final
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
    //! check reservoir levels
    bool checkRuleCurves(uint year);
};

} // namespace Antares
