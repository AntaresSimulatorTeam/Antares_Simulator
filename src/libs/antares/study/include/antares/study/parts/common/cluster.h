// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __LIBS_ANTARES_STUDY_PARTS_COMMON_H__
#define __LIBS_ANTARES_STUDY_PARTS_COMMON_H__

#include <map>
#include <set>
#include <vector>

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>

#include <antares/array/matrix.h>
#include <antares/series/series.h>
#include <antares/study/area/ReserveOpt.h>
#include <antares/study/area/reserveParticipationContainer.h>

#include "../../fwd.h"

namespace Antares::Data
{
class Cluster;

struct CompareClusterName final
{
    bool operator()(const Cluster* s1, const Cluster* s2) const;
};

class Cluster
{
public:
    Cluster(Area* parent);

    virtual ~Cluster() = default;

    const std::string& id() const;
    const std::string& name() const;
    void setName(const AnyString& newname);
    Yuni::String getFullName() const;

    void setGroup(const std::string& group);
    std::string getGroup() const;

    /*!
    ** \brief Check and fix all values of a renewable cluster
    **
    ** \return False if an error has been detected and fixed with a default value
    */
    virtual bool integrityCheck() = 0;

    bool saveDataSeriesToFolder(const AnyString& folder) const;
    bool loadDataSeriesFromFolder(Study& s, const std::filesystem::path& folder);

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
    unsigned int areaWideIndex = (uint)-1;
    unsigned int enabledIndex = (uint)-1;

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

    //! Reserve participation container to store the participation of the cluster in the reserves
    //! and the symmetries
    ReserveOpt<ReserveParticipationContainer<ThermalClusterReserveParticipation>>
      reserveParticipationContainer;

protected:
    std::string pName;
    std::string pID;

private:
    std::string group_ = "OTHER";

    virtual unsigned int precision() const = 0;
};
} // namespace Antares::Data
#endif /* __LIBS_ANTARES_STUDY_PARTS_COMMON_H__ */
