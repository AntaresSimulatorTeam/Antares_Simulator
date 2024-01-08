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
#ifndef __ANTARES_LIBS_STUDY_PARTS_THERMAL_CONTAINER_H__
#define __ANTARES_LIBS_STUDY_PARTS_THERMAL_CONTAINER_H__

#include <yuni/yuni.h>
#include "cluster_list.h"

namespace Antares
{
namespace Data
{
class PartThermal
{
public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Default constructor
    */
    PartThermal();
    //! Destructor
    ~PartThermal() = default;
    //@}

    /*!
    ** \brief Reset internal data
    */
    void reset();

    /*!
    ** \brief Resize all matrices dedicated to the sampled timeseries numbers
    **
    ** \param n A number of years
    */
    void resizeAllTimeseriesNumbers(uint n);

    /*!
    ** \brief Invalidate all JIT data
    */
    bool forceReload(bool reload) const;

    /*!
    ** \brief Mark the thermal cluster as modified
    */
    void markAsModified() const;

    /*!
    ** \brief Some clusters force the generation of timeseries,
        overriding the global (study) parameter.
    */
    bool hasForcedTimeseriesGeneration() const;

    /*!
    ** \brief Some clusters prevent the generation of timeseries,
        overriding the global (study) parameter.
    */
    bool hasForcedNoTimeseriesGeneration() const;

    // Check & correct availability timeseries for thermal availability
    // Only applies if time-series are ready-made
    void checkAndCorrectAvailability();

public:
    //! The unsupplied energy cost (Euros/MWh)
    double unsuppliedEnergyCost;
    //! The spilled energy cost
    double spilledEnergyCost;

    //! List of all thermal clusters (enabled and disabled) except must-run clusters
    ThermalClusterList list;
    //! List of all thermal clusters in 'must-run' mode
    //! \warning This list must only be used from the solver
    ThermalClusterList mustrunList;

}; // class PartThermal

} // namespace Data
} // namespace Antares

#endif /* __ANTARES_LIBS_STUDY_PARTS_THERMAL_CONTAINER_H__ */
