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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDRO_TIMESERIES_H__
#define __ANTARES_LIBS_STUDY_PARTS_HYDRO_TIMESERIES_H__

#include <antares/series/series.h>
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

    void copyGenerationTS(const DataSeriesHydro& source);

    //! \name Data
    //@{
    /*!
    ** \brief Reset all data, as if it were a new area
    */
    void reset();

    void resize_ROR_STORAGE_MINGEN_whenGeneratedTS(unsigned int width);
    void resizeGenerationTS(unsigned int w, unsigned int h);

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
    TimeSeries ror;

    /*!
    ** \brief Mod (MW)
    **
    ** Merely a matrix of TimeSeriesCount * 365 values
    ** This matrix is not used in `adequation` mode.
    */
    TimeSeries storage;

    /*!
    ** \brief Minimum Generation (MW)
    **
    ** Merely a matrix of TimeSeriesCount * HOURS_PER_YEAR values
    */
    TimeSeries mingen;

    unsigned int TScount() const { return count; };

    /*!
    ** \brief Monte-Carlo
    */
    Matrix<uint32_t> timeseriesNumbers;

    /*!
    ** \brief The number of time-series
    **
    ** This value must be the same as the width of the matrices `mod` and `fatal`.
    ** It is only provided for convenience to avoid same strange and ambiguous code
    ** (for example using `fatal.width` and `mod.width` in the same routine, it might
    ** indicate that the two values are not strictly equal)
    */
private:
    uint count = 0;

}; // class DataSeriesHydro

} // namespace Data
} // namespace Antares

#endif /* __ANTARES_LIBS_STUDY_PARTS_HYDRO_TIMESERIES_H__ */
