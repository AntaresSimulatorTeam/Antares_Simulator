// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CONTAINER_H__
#define __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CONTAINER_H__

#include <yuni/yuni.h>

#include "../../fwd.h"
#include "cluster.h"
#include "cluster_list.h"

namespace Antares::Data
{
class PartRenewable final
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

public:
    //! List of all renewable clusters (enabled and disabled)
    RenewableClusterList list;

}; // class PartRenewable

} // namespace Antares::Data

#include "container.hxx"

#endif /* __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CONTAINER_H__ */
