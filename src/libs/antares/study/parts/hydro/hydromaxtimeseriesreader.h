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

#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDRO_MAX_TIME_SERIES_READER_H__
#define __ANTARES_LIBS_STUDY_PARTS_HYDRO_MAX_TIME_SERIES_READER_H__

#include "../../../array/antares//array/matrix.h"

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
    HydroMaxTimeSeriesReader();

    bool operator()(const AnyString& folder, Area& area, bool usedBySolver);

    Matrix<double, double> dailyMaxPumpAndGen;

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
    /**
     * \brief Loading deprecated files
     *  This function provides reading from deprecated files which
     *  contains daily maximum generation/pumping power and energy data.
     */
    bool loadDailyMaxPowersAndEnergies(const AnyString& folder,
                                       const Area& area,
                                       bool usedBySolver);

    /**
     * \brief Copy energy functions
     *  These functions provides coping of energy data loaded
     *  from deprecated file.
     */
    void copyDailyMaxEnergy(Area& area) const;
    void copyDailyMaxGenerationEnergy(Area& area) const;
    void copyDailyMaxPumpingEnergy(Area& area) const;
};
} // namespace Antares::Data

#endif /*__ANTARES_LIBS_STUDY_PARTS_HYDRO_MAX_TIME_SERIES_READER_H__*/