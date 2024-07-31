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
#ifndef __LIBS_ANTARES_STUDY_PARTS_COMMON_H__
#define __LIBS_ANTARES_STUDY_PARTS_COMMON_H__

#include <set>
#include <map>
#include <vector>

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>

#include <antares/array/matrix.h>
#include <antares/series/series.h>
#include "../../fwd.h"

#include <antares/study/area/capacityReservation.h>



namespace Antares
{
namespace Data
{
class Cluster;

struct CompareClusterName final
{
    bool operator()(const Cluster* s1, const Cluster* s2) const;
};

class Cluster
{
public:
    using Set = std::set<Cluster*, CompareClusterName>;

public:
    Cluster(Area* parent);

    virtual ~Cluster() = default;

    const ClusterName& id() const;
    const ClusterName& group() const;
    const ClusterName& name() const;
    void setName(const AnyString& newname);
    Yuni::String getFullName() const;

    virtual uint groupId() const = 0;
    virtual void setGroup(Data::ClusterName newgrp) = 0;

    /*!
    ** \brief Check and fix all values of a renewable cluster
    **
    ** \return False if an error has been detected and fixed with a default value
    */
    virtual bool integrityCheck() = 0;

    /*!
    ** \brief Get the memory consummed by the cluster (in bytes)
    */
    virtual uint64_t memoryUsage() const = 0;
    //@}

    /*!
    ** \brief Invalidate all data associated to the cluster
    */
    virtual bool forceReload(bool reload) const = 0;

    /*!
    ** \brief Invalidate the whole attached area
    */
    void invalidateArea();

    /*!
    ** \brief Mark the cluster as modified
    */
    virtual void markAsModified() const = 0;

    /*!
    ** \brief Check wether the cluster is visible in a layer (it's parent area is visible in the
    *layer)
    */
    bool isVisibleOnLayer(const size_t& layerID) const;

    /*!
    ** \brief Reset to default values
    **
    ** This method should only be called from the GUI
    */
    virtual void reset();

    bool saveDataSeriesToFolder(const AnyString& folder) const;
    bool loadDataSeriesFromFolder(Study& s, const AnyString& folder);

    //! @brief Add the reserve participation to the current clusterReservesParticipations map
    //! @param name the name of the reserve to add
    //! @param reserveParticipation the reserve participation to add
    void addReserveParticipation(std::string name,
                                 ClusterReserveParticipation reserveParticipation);

    uint unitCount = 0;

    bool isEnabled() const
    {
        return enabled;
    }

    bool enabled = true;

    //! The associate area (alias)
    Area* parentArea;

    //! Capacity of reference per unit (MW) (pMax)
    double nominalCapacity = 0.;

    //! The index of the cluster from the area's point of view
    //! \warning this variable is only valid when used by the solver
    // (initialized in the same time that the runtime data)
    uint areaWideIndex = (uint)-1;

    //! tsNumbers must be constructed before series
    TimeSeriesNumbers tsNumbers;

    //! Series
    TimeSeries series;

    /*!
    ** \brief Modulation matrix
    **
    ** It is merely a 3x8760 matrix
    ** [modulation cost, modulation capacity, market bid modulation] per hour
    */
    Matrix<> modulation;

    //! \brief Returns true if cluster participates in a reserve with this name
    bool isParticipatingInReserve(Data::ReserveName name);

    //! \brief Returns an array of all reserves the cluster is participating in
    std::vector<Data::ReserveName> listOfParticipatingReserves();

    //! \brief Returns max power for a reserve if participating, -1 otherwise
    float reserveMaxPower(Data::ReserveName name);

    //! \brief Returns participating cost for a reserve if participating, -1 otherwise
    float reserveCost(Data::ReserveName name);

    //! \brief Returns the number of reserves linked to this cluster
    unsigned int reserveParticipationsCount();


protected:
    Data::ClusterName pName;
    Data::ClusterName pID;
    Data::ClusterName pGroup;
    //! reserve
    std::map<Data::ReserveName, ClusterReserveParticipation> clusterReservesParticipations;

private:
    virtual unsigned int precision() const = 0;
    //! @brief Stores the reserves Participations for each reserve, the key is the name of the
    

};
} // namespace Data
} // namespace Antares
#endif /* __LIBS_ANTARES_STUDY_PARTS_COMMON_H__ */
