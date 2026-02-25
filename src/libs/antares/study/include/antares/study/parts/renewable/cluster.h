// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_H__
#define __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_H__

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>

#include <antares/array/matrix.h>

#include "../../fwd.h"
#include "../common/cluster.h"
#include "defines.h"

namespace Antares::Data
{

/*
** \brief A single renewable cluster
*/
class RenewableCluster final: public Cluster
{
public:
    enum TimeSeriesMode
    {
        //! TS contain power generation for each unit
        //! Nominal capacity is *ignored*
        powerGeneration = 0,
        //! TS contain production factor for each unit
        //! Nominal capacity is used as a multiplicative factor
        productionFactor
    };

    //! Set of renewable clusters
    using Set = std::set<RenewableCluster*, CompareClusterName>;

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor, with a parent area
    */
    explicit RenewableCluster(Data::Area* parent);
    //@}

    /*!
    ** \brief Reset to default values
    **
    ** This method should only be called from the GUI
    */
    void reset() override;

    /*!
    ** \brief Check and fix all values of a renewable cluster
    **
    ** \return False if an error has been detected and fixed with a default value
    */
    bool integrityCheck() override;

    /*!
    ** \brief Copy data from another cluster
    **
    ** The name and the id will remain untouched.
    */
    void copyFrom(const RenewableCluster& cluster);

    bool setTimeSeriesModeFromString(const YString& value);

    YString getTimeSeriesModeAsString() const;

    /* !
    ** Get production value at time-step ts
    */
    double valueAtTimeStep(uint year, uint hourInYear) const;

    enum TimeSeriesMode tsMode = powerGeneration;

    friend class RenewableClusterList;

private:
    unsigned int precision() const override;
}; // class RenewableCluster

} // namespace Antares::Data

#endif //__ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_H__
