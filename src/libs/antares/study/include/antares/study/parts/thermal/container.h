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
    void resizeAllTimeseriesNumbers(uint n) const;

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
    void checkAndCorrectAvailability() const;

public:
    //! The unsupplied energy cost (Euros/MWh)
    double unsuppliedEnergyCost;
    //! The spilled energy cost
    double spilledEnergyCost;

    //! List of all thermal clusters (enabled and disabled) except must-run clusters
    ThermalClusterList list;

}; // class PartThermal

} // namespace Data
} // namespace Antares

#endif /* __ANTARES_LIBS_STUDY_PARTS_THERMAL_CONTAINER_H__ */
