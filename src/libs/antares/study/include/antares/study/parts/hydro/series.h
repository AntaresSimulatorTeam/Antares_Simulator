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
#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDRO_TIMESERIES_H__
#define __ANTARES_LIBS_STUDY_PARTS_HYDRO_TIMESERIES_H__

#include <antares/series/series.h>
#include <antares/array/matrix.h>
#include <antares/study/version.h>
#include "antares/study/parts/hydro/lt_storage_reserve_participation.h"

#include "../../fwd.h"


namespace Antares
{
namespace Data
{

//    struct ReserveParams
//{
//    double maxTurbining;
//    double maxPumping;
//    double participationCost;
//};


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

    void resizeGenerationTS(uint nbSeries);
    void resizeMaxPowerTS(uint nbSeries);

    /*!
    ** \brief Load all data not already loaded
    **
    ** If the load-on-demand is enabled, some data might not be loaded (see `Matrix`)
    */
    bool forceReload(bool reload = false) const;

    void markAsModified() const;
    //@}

    void EqualizeGenerationTSsizes(Area& area, bool usedByTheSolver);

    // Loading hydro time series collection
    // Returned boolean : reading from file failed
    bool loadGenerationTS(const AreaName& areaID, const AnyString& folder, StudyVersion version);

    // Loading hydro max generation and mqx pumping TS's
    bool LoadMaxPower(const AreaName& areaID, const AnyString& folder);

    void buildHourlyMaxPowerFromDailyTS(const Matrix<double>::ColumnType& DailyMaxGenPower,
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
    void EqualizeMaxPowerTSsizes(Area& area);

    void setHydroModulability(Area& area) const;

    // Getters for generation (ror, storage and mingen) and 
    // max power (generation and pumping) number of TS
    uint TScount() const;
    uint maxPowerTScount() const;
    void setMaxPowerTScount(uint count) { maxPowerTScount_ = count;}

    // Setting TS's when derated mode is on
    void resizeTSinDeratedMode(bool derated, StudyVersion version, bool useBySolver);

    void addReserveParticipation(const std::string& reserveName,
                                 const LTStorageClusterReserveParticipation& participation);

    struct LTStorageReserves
    {
        std::unordered_map<std::string, std::vector<LTStorageClusterReserveParticipation>> reserves;
    };

    LTStorageReserves ltStorageReserves;



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
} // namespace Data
} // namespace Antares

#endif /* __ANTARES_LIBS_STUDY_PARTS_HYDRO_TIMESERIES_H__ */
