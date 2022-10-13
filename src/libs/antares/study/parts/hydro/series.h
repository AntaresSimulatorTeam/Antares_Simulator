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
#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDRO_TIMESERIES_H__
#define __ANTARES_LIBS_STUDY_PARTS_HYDRO_TIMESERIES_H__

#include "../../../array/matrix.h"
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
    bool invalidate(bool reload = false) const;

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
    Yuni::uint64 memoryUsage() const;
    /*!
    ** \brief Try to estimate the amount of memory required for launching a simulation
    */
    void estimateMemoryUsage(StudyMemoryUsage&) const;
    //@}

public:
    /*!
    ** \brief Run-of-the-river - ROR (MW)
    **
    ** Merely a matrix of TimeSeriesCount * HOURS_PER_YEAR values
    ** (it was DAYS_PER_YEAR before 3.9)
    */
    Matrix<double, Yuni::sint32> ror;

    /*!
    ** \brief Mod (MW)
    **
    ** Merely a matrix of TimeSeriesCount * 365 values
    ** This matrix is not used in `adequation` mode.
    */
    Matrix<double, Yuni::sint32> storage;

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
    ** \brief Monte-Carlo
    */
    Matrix<Yuni::uint32> timeseriesNumbers;

}; // class DataSeriesHydro

} // namespace Data
} // namespace Antares

#include "series.hxx"

#endif /* __ANTARES_LIBS_STUDY_PARTS_HYDRO_TIMESERIES_H__ */
