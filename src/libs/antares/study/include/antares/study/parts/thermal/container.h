// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_THERMAL_CONTAINER_H__
#define __ANTARES_LIBS_STUDY_PARTS_THERMAL_CONTAINER_H__

#include <yuni/yuni.h>

#include "cluster_list.h"

namespace Antares::Data
{
class PartThermal final
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

} // namespace Antares::Data

#endif /* __ANTARES_LIBS_STUDY_PARTS_THERMAL_CONTAINER_H__ */
