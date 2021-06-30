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
#ifndef __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_H__
#define __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_H__

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>
#include "../../../array/matrix.h"
#include "defines.h"
#include "../common/cluster.h"
#include "../../fwd.h"

namespace Antares
{
namespace Data
{
/*!
** \brief A single renewable cluster
*/
class RenewableCluster final : public Cluster
{
public:
    enum RenewableGroup
    {
        //! Wind offshore
        windOffShore = 0,
        //! Wind onshore
        windOnShore,
        //! Concentration solar
        concentrationSolar,
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
    typedef std::set<Cluster*, CompareClusterName> Set;
    //! Set of renewable clusters (pointer)
    typedef std::set<Cluster*> SetPointer;
    //! Vector of renewable clusters
    typedef std::vector<Data::Cluster*> Vector;

    /*!
    ** \brief Get the group name string
    ** \return A valid CString
    */
    static const char* GroupName(enum RenewableGroup grp);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor, with a parent area
    */
    explicit RenewableCluster(Data::Area* parent);
    //! Destructor
    ~RenewableCluster();
    //@}

    /*!
    ** \brief Invalidate all data associated to the renewable cluster
    */
    bool invalidate(bool reload) const override;

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

    //! \name Memory management
    //@{
    /*!
    ** \brief Flush the memory to swap files (if swap support enabled)
    */
    void flush() override;

    /*!
    ** \brief Group ID as an uint
    */
    uint groupId() const override;

    /*!
    ** \brief Get the memory consummed by the renewable cluster (in bytes)
    */
    Yuni::uint64 memoryUsage() const override;
    //@}

    bool setTimeSeriesModeFromString(const YString& value);

    const YString& getTimeSeriesModeAsString() const;

    /* !
    ** Get production value at time-step ts
    */
    double valueAtTimeStep(uint timeSeriesIndex, uint timeStepIndex) const;

public:
    /*!
    ** \brief The group ID
    **
    ** This value is computed from the field 'group' in 'group()
    ** \see group()
    */
    enum RenewableGroup groupID;

    enum TimeSeriesMode tsMode;

    friend class RenewableClusterList;

    private:
    unsigned int precision() const override;
}; // class RenewableCluster

} // namespace Data
} // namespace Antares

#endif //__ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_H__
