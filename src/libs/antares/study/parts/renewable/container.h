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



    /*!
    ** \brief Create and initialize the list of all clusters (with the mustrun flag or not)
    **
    ** This method must be called before prepareClustersInMustRunMode()
    ** to ensure the same order whatever the value of the 'mustrun' flag is.
    */
    void prepareAreaWideIndexes();

    /*!
    ** \brief Invalidate all JIT data
    */
    bool forceReload(bool reload) const;

    /*!
    ** \brief Mark the renewable cluster as modified
    */
    void markAsModified() const;

public:
    //! List of all renewable clusters (enabled and disabled)
    RenewableClusterList list;

}; // class PartRenewable

} // namespace Data
} // namespace Antares

#include "container.hxx"

#endif /* __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CONTAINER_H__ */
