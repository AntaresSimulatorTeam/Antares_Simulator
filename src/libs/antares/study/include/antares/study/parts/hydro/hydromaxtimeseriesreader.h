// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDRO_MAX_TIME_SERIES_READER_H__
#define __ANTARES_LIBS_STUDY_PARTS_HYDRO_MAX_TIME_SERIES_READER_H__

#include "antares/study/parts/hydro/container.h"

namespace Antares::Data
{
/*!
**  This class provides support for old studies, reading from deprecated files,
**  fils matrix dailyMaxPumpAndGen and transfers data to the corresponding data
**  class members of class PartHydro. Just versions below 8.7 will use instance
**  of this class to be compatible with current implementation.
*/
class HydroMaxTimeSeriesReader
{
public:
    HydroMaxTimeSeriesReader(PartHydro& hydro, std::string areaID, std::string areaName);

    bool read(const AnyString& folder);

    enum powerDailyE
    {
        //! Generated max power
        genMaxP = 0,
        //! Generated max energy
        genMaxE,
        //! Pumping max Power
        pumpMaxP,
        // Pumping max Energy
        pumpMaxE,
    };

private:
    PartHydro& hydro_;
    std::string areaID_;
    std::string areaName_;

    /**
     * \brief Loading deprecated files
     *  This function provides reading from deprecated files which
     *  contains daily maximum generation/pumping power and energy data.
     */
    bool loadDailyMaxPowersAndEnergies(const AnyString& folder);

    /**
     * \brief Copy energy functions
     *  These functions provides coping of energy data loaded
     *  from deprecated file.
     */
    void copyDailyMaxEnergy() const;
    void copyDailyMaxGenerationEnergy() const;
    void copyDailyMaxPumpingEnergy() const;
};
} // namespace Antares::Data

#endif /*__ANTARES_LIBS_STUDY_PARTS_HYDRO_MAX_TIME_SERIES_READER_H__*/
