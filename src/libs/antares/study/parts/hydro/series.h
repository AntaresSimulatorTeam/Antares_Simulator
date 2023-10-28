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

#include <antares/series/series.h>
#include <antares/array/matrix.h>
#include "../../fwd.h"
#include <antares/exception/antares/exception/LoadingError.hpp>


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
    void copyMaxPowerTS(const DataSeriesHydro& source);

    //! \name Data
    //@{
    /*!
    ** \brief Reset all data, as if it were a new area
    */
    void reset();

    void resizeRORandSTORAGE(unsigned int width);
    void resizeGenerationTS(unsigned int w, unsigned int h);
    void resizeMaxPowerTS(unsigned int w, unsigned int h);


    /*!
    ** \brief Load all data not already loaded
    **
    ** If the load-on-demand is enabled, some data might not be loaded (see `Matrix`)
    */
    bool forceReload(bool reload = false) const;

    void markAsModified() const;
    //@}

    void EqualizeGenerationTSsizes(Area& area, bool usedByTheSolver, bool& fatalError);

    // Loading hydro time series collection
    // Returned boolean : reading from file failed
    bool loadGenerationTS(const AreaName& areaID, const AnyString& folder, unsigned int studyVersion);

    // Loading hydro max generation and mqx pumping TS's
    bool LoadMaxPower(const AreaName& areaID, const AnyString& folder);

    void buildMaxPowerFromDailyTS(const Matrix<double>::ColumnType& DailyMaxGenPower,
                                  const Matrix<double>::ColumnType& DailyMaxPumpPower);

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

    /*!
    ** \brief Maximum Generation (MW)
    **
    ** Merely a matrix of TimeSeriesCount * HOURS_PER_YEAR values
    */

    TimeSeries maxHourlyGenPower;

    /*!
    ** \brief Maximum Pumping (MW)
    **
    ** Merely a matrix of TimeSeriesCount * HOURS_PER_YEAR values
    */
    TimeSeries maxHourlyPumpPower;

    // TS's number matrices for Generation and Maximum Power
    Matrix<uint32_t> timeseriesNumbers;
    Matrix<uint32_t> timeseriesNumbersHydroMaxPower;

    // Equalizing max generation and max pumping numbers of TS's    
    void EqualizeMaxPowerTSsizes(Area& area, bool& fatalError);

    void setHydroModulability(Area& area) const;

    // Getters for generation (ror, storage and mingen) and 
    // max power (generation and pumping) number of TS
    uint TScount() const;
    uint maxPowerTScount() const;

    // Setting TS's when derated mode is on
    void resizeTSinDeratedMode(bool derated, unsigned int studyVersion, bool useBySolver);

private:
    
    // The number of time-series about generation (ror, inflows (=storage), mingen)
    // They all should have the same number of columns (width), as they each year receives a common
    // TS number for all three.
    uint generationTScount_ = 0;

    // The number of time-series about max power (maxHourlyGenPower and maxHourlyPumpPower)
    // They both should have the same number of columns (width), as they each year receives a common
    // TS number for all three.
    uint maxPowerTScount_ = 0;


}; // class DataSeriesHydro

void resizeMatrixNoDataLoss(Matrix<double, int32_t>& matrixToResize, uint width);
bool loadTSfromFile(Matrix<double, int32_t>& ts,
                    const AreaName& areaID,
                    const AnyString& folder,
                    const std::string& filename,
                    unsigned int height);
} // namespace Data
} // namespace Antares

#endif /* __ANTARES_LIBS_STUDY_PARTS_HYDRO_TIMESERIES_H__ */
