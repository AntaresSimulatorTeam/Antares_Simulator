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
#ifndef __ANTARES_LIBS_STUDY_AREA_SCRATCHPAD_H__
#define __ANTARES_LIBS_STUDY_AREA_SCRATCHPAD_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/noncopyable.h>
#include "../fwd.h"
#include <antares/array/matrix.h>
#include <vector>
#include <set>
#include <numeric>

namespace Antares
{
namespace Data
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
    ~AreaScratchpad();
    //@}

    //! Sum of all fatal hors hydro
    double miscGenSum[HOURS_PER_YEAR];

    //! Timeseries
    struct TimeseriesData
    {
        TimeseriesData(Area& area);
        //! Alias to the load time-series
        const TSMatrix& load;
        //! Alias to the solar time-series
        const TSMatrix& solar;
        //! Alias to the wind time-series
        const TSMatrix& wind;
    };

    //! Timeseries
    TimeseriesData ts;

    bool hydroHasMod;

    //! if sum(365)[pumpMaxP * pumpMaxE] > 0. then pumpHasMod = true
    //	else pumpHasMod = false
    bool pumpHasMod;

    //! Sum of all 'must-run' clusters
    // This variable is initialized every MC-year
    double mustrunSum[HOURS_PER_YEAR];

    //! Sum of all original 'must-run' clusters (adequacy only)
    // This variable is initialized every MC-year
    double originalMustrunSum[HOURS_PER_YEAR];

       /*!
    ** \brief Dispatchable Generation Margin
    **
    ** Those values, written by the output, must be calculated before
    ** running the hydro remix.
    */
    double dispatchableGenerationMargin[168];

private:
    bool CheckForPositiveEnergy(const Matrix<double, int32_t>& matrix,
                                const Matrix<double>::ColumnType& hours);
}; // class AreaScratchpad

double CalculateDailyMeanPower(uint dYear, const Matrix<double>::ColumnType&  maxPower);

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_AREA_SCRATCHPAD_H__
