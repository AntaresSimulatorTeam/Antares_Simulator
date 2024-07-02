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
#ifndef __ANTARES_LIBS_STUDY_AREA_SCRATCHPAD_H__
#define __ANTARES_LIBS_STUDY_AREA_SCRATCHPAD_H__

#include <numeric>
#include <set>
#include <vector>

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>
#include <yuni/core/string.h>

#include <antares/array/matrix.h>
#include <antares/series/series.h>

#include "../fwd.h"

namespace Antares::Data
{
/*!
** \brief Scratchpad for temporary data performed by the solver
*/
class AreaScratchpad final
{
public:
    //! Matrix used for time-series
    using TSMatrix = Matrix<double, int32_t>;

    //! \name Constructor
    //@{
    /*!
    ** \brief Constructor
    */
    AreaScratchpad(const StudyRuntimeInfos& rinfos, Area& area);
    //! Destructor
    ~AreaScratchpad() = default;
    //@}

    //! Sum of all fatal hors hydro
    double miscGenSum[HOURS_PER_YEAR];

    bool hydroHasMod;

    //! if sum(365)[pumpMaxP * pumpMaxE] > 0. then pumpHasMod = true
    //	else pumpHasMod = false
    bool pumpHasMod;

    //! Sum of all 'must-run' clusters
    // This variable is initialized every MC-year
    std::array<double, HOURS_PER_YEAR> mustrunSum;

    //! Sum of all original 'must-run' clusters (adequacy only)
    // This variable is initialized every MC-year
    std::array<double, HOURS_PER_YEAR> originalMustrunSum;

    /*!
     ** \brief Dispatchable Generation Margin
     **
     ** Those values, written by the output, must be calculated before
     ** running the hydro remix.
     */
    double dispatchableGenerationMargin[168];

    /*!
    ** \brief Daily mean maximum power matrices
    **
    ** These matrices will be calculated based on maximum
    ** hourly generation/pumping matrices
    */
    TimeSeries meanMaxDailyGenPower;
    TimeSeries meanMaxDailyPumpPower;

private:
    /*!
    ** \brief Caluclation of daily mean maximum power matrices
    **
    ** Calculates daily mean maximum generation/pumping power
    ** power matrices meanMaxDailyGenPower/meanMaxDailyPumpPower
    */
    void CalculateMeanDailyMaxPowerMatrices(const Matrix<double>& hourlyMaxGenMatrix,
                                            const Matrix<double>& hourlyMaxPumpMatrix);

}; // class AreaScratchpad

} // namespace Antares::Data

#endif // __ANTARES_LIBS_STUDY_AREA_SCRATCHPAD_H__
