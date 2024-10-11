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
#ifndef __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_H__
#define __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_H__

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>

#include <antares/array/matrix.h>

#include "../../fwd.h"
#include "../common/cluster.h"
#include "defines.h"

namespace Antares
{
namespace Data
{

/*
** \brief A single renewable cluster
*/
class RenewableCluster final: public Cluster
{
public:
    enum RenewableGroup
    {
        //! Wind offshore
        windOffShore = 0,
        //! Wind onshore
        windOnShore,
        //! Concentration solar
        thermalSolar,
        //! PV solar
        PVSolar,
        //! Rooftop solar
        rooftopSolar,
        //! Other 1
        renewableOther1,
        //! Other 2
        renewableOther2,
        //! Other 3
        renewableOther3,
        //! Other 4
        renewableOther4,

        //! The highest value
        groupMax
    };

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

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor, with a parent area
    */
    explicit RenewableCluster(Data::Area* parent);
    //@}

    /*!
    ** \brief Invalidate all data associated to the renewable cluster
    */
    bool forceReload(bool reload) const override;

    /*!
    ** \brief Mark the renewable cluster as modified
    */
    void markAsModified() const override;

    /*!
    ** \brief Reset to default values
    **
    ** This method should only be called from the GUI
    */
    void reset() override;

    //! Set the group
    void setGroup(Data::ClusterName newgrp) override;
    //@}

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

    /*!
    ** \brief Group ID as an uint
    */
    uint groupId() const override;

    bool setTimeSeriesModeFromString(const YString& value);

    YString getTimeSeriesModeAsString() const;

    /* !
    ** Get production value at time-step ts
    */
    double valueAtTimeStep(uint year, uint hourInYear) const;

public:
    /*!
    ** \brief The group ID
    **
    ** This value is computed from the field 'group' in 'group()
    ** \see group()
    */
    enum RenewableGroup groupID = renewableOther1;

    enum TimeSeriesMode tsMode = powerGeneration;

    friend class RenewableClusterList;

private:
    unsigned int precision() const override;
}; // class RenewableCluster

} // namespace Data
} // namespace Antares

#endif //__ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_H__
