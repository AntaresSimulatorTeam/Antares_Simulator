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
#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDRO_TIMESERIES_H__
#define __ANTARES_LIBS_STUDY_PARTS_HYDRO_TIMESERIES_H__

#include <antares/array/matrix.h>
#include "../../fwd.h"

namespace Antares
{
namespace Data
{
/*!
** \brief Data series (Hydro)
*/
class DataSeriesHydro
{
public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Default constructor
    */
    DataSeriesHydro();
    //@}

    //! \name Data
    //@{
    /*!
    ** \brief Reset all data, as if it were a new area
    */
    void reset();

    /*!
    ** \brief Load all data not already loaded
    **
    ** If the load-on-demand is enabled, some data might not be loaded (see `Matrix`)
    */
    bool forceReload(bool reload = false) const;

    void markAsModified() const;
    //@}

    //! \name Save / Load
    //@{
    /*!
    ** \brief Load data series for hydro from a folder
    **
    ** \param d The data series for hydro
    ** \param folder The source folder
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    bool loadFromFolder(Study& s, const AreaName& areaID, const AnyString& folder);

    /*!
    ** \brief Save data series for hydro into a folder (`input/hydro/series`)
    **
    ** Nothing will be done if the pointer to the structure is null.
    **
    ** \param d The data series for hydro
    ** \param folder The target folder
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    bool saveToFolder(const AreaName& areaID, const AnyString& folder) const;
    //@}

    //! \name Memory
    //@{
    /*!
    ** \brief Get the size (bytes) in memory occupied by a `DataSeriesHydro` structure
    */
    uint64_t memoryUsage() const;
    /*!
    ** \brief Try to estimate the amount of memory required for launching a simulation
    */

    //@}

    /*!
    ** \brief Check TS number for Minimum Generation and logs error if necessary
    */
    void checkMinGenTsNumber(Study& s, const AreaName& areaID);

public:
    /*!
    ** \brief Run-of-the-river - ROR (MW)
    **

    ** (it was DAYS_PER_YEAR before 3.9)
    */
    Matrix<double, int32_t> ror;

    /*!
    ** \brief Mod (MW)
    **
    ** Merely a matrix of TimeSeriesCount * 365 values
    ** This matrix is not used in `adequation` mode.
    */
    Matrix<double, int32_t> storage;

    /*!
    ** \brief Minimum Generation (MW)
    **
    ** Merely a matrix of TimeSeriesCount * HOURS_PER_YEAR values
    */
    Matrix<double, int32_t> mingen;

    /*!
    ** \brief Maximum Generation (MW)
    **
    ** Merely a matrix of TimeSeriesCount * HOURS_PER_YEAR values
    */
    Matrix<double, int32_t> maxgen;

    /*!
    ** \brief Maximum Pumping (MW)
    **
    ** Merely a matrix of TimeSeriesCount * HOURS_PER_YEAR values
    */
    Matrix<double, int32_t> maxpump;

    /*!
    ** \brief The number of time-series
    **
    ** This value must be the same as the width of the matrices `mod` and `fatal`.
    ** It is only provided for convenience to avoid same strange and ambiguous code
    ** (for example using `fatal.width` and `mod.width` in the same routine, it might
    ** indicate that the two values are not strictly equal)
    */
    uint count;

    /*!
    ** \brief The number of time-series for maxgen and maxpump matrices
    **
    ** This value must be the same as the width of the matrices `maxgen` and `maxpump`.
    ** It is only provided for convenience to avoid same strange and ambiguous code
    ** (for example using `maxgen.width` and `maxpump.width` in the same routine, it might
    ** indicate that the two values are not strictly equal)
    */
    uint countpowercredits = 0;

    /*!
    ** \brief Monte-Carlo
    */
    Matrix<uint32_t> timeseriesNumbers;
    Matrix<uint32_t> timeseriesNumbersPowerCredits;
    bool LoadHydroPowerCredits(Study& study, const AreaName& areaID, const AnyString& folder);

}; // class DataSeriesHydro

} // namespace Data
} // namespace Antares

#include "series.hxx"

#endif /* __ANTARES_LIBS_STUDY_PARTS_HYDRO_TIMESERIES_H__ */
