/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
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
    ~PartThermal();
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
    ** \brief Create and initialize the list of all clusters (with the mustrun flag or not)
    **
    ** This method must be called before prepareClustersInMustRunMode()
    ** to ensure the same order whatever the value of the 'mustrun' flag is.
    */
    void prepareAreaWideIndexes();

    /*!
    ** \brief Prepare all thermal clusters in the 'mustrun' mode
    **
    ** All clusters with the flag will be moved to the container 'mustrunList'.
    ** As a consequence, they will no longer be seen as thermal clusters
    ** from the solver's point of view.
    ** \warning This method should only be used from the solver
    **
    ** \return The number of clusters found in 'must-run' mode
    */
    uint prepareClustersInMustRunMode();

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

    /*!
    ** \brief All clusters for the area
    **
    ** This variable is only available at runtime from the solver.
    ** It is initialized in the same time that the runtime data.
    **
    ** This list is mainly used to ensure the same order of the
    ** thermal clusters in the outputs.
    */
    std::vector<ThermalCluster*> clusters;

    // Return unit count
    inline size_t clusterCount() const
    {
        return clusters.size();
    }
}; // class PartThermal

} // namespace Data
} // namespace Antares

#include "container.hxx"

#endif /* __ANTARES_LIBS_STUDY_PARTS_THERMAL_CONTAINER_H__ */
