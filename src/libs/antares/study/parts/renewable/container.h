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
#ifndef __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CONTAINER_H__
#define __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CONTAINER_H__

#include <yuni/yuni.h>
#include "../../fwd.h"
#include "cluster.h"
#include "cluster_list.h"

namespace Antares
{
namespace Data
{
class PartRenewable
{
public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Default constructor
    */
    PartRenewable();
    //! Destructor
    ~PartRenewable();
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

    void estimateMemoryUsage(StudyMemoryUsage&) const;

    /*!
    ** \brief Create and initialize the list of all clusters (with the mustrun flag or not)
    **
    ** This method must be called before prepareClustersInMustRunMode()
    ** to ensure the same order whatever the value of the 'mustrun' flag is.
    */
    void prepareAreaWideIndexes();

    /*!
    ** \brief Removes disabled renewable clusters
    **
    ** All clusters with the flag 'enabled' turned to false will be removed from 'list'.
    ** As a consequence, they will no longer be seen as renewable clusters
    ** from the solver's point of view.
    ** \warning This method should only be used from the solver
    **
    ** \return The number of disabled clusters found
    */
    uint removeDisabledClusters();

    /*!
    ** \brief Invalidate all JIT data
    */
    bool invalidate(bool reload) const;

    /*!
    ** \brief Mark the renewable cluster as modified
    */
    void markAsModified() const;

public:
    //! List of all renewable clusters (enabled and disabled)
    RenewableClusterList list;

    /*!
    ** \brief All clusters for the area
    **
    ** This variable is only available at runtime from the solver.
    ** It is initialized in the same time that the runtime data.
    **
    ** This list is mainly used to ensure the same order of the
    ** renewable clusters in the outputs.
    */
    std::vector<RenewableCluster*> clusters;
    //! How many clusters have we got ?
    // Only available from the solver
    inline size_t clusterCount() const
    {
        return clusters.size();
    }
}; // class PartRenewable

} // namespace Data
} // namespace Antares

#include "container.hxx"

#endif /* __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CONTAINER_H__ */
