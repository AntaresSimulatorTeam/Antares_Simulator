#ifndef __LIBS_ANTARES_STUDY_PARTS_COMMON_H__
#define __LIBS_ANTARES_STUDY_PARTS_COMMON_H__

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>
#include <antares/array/matrix.h>
#include <antares/series/series.h>
#include "../../fwd.h"

#include <set>
#include <map>
#include <vector>

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

    //! Count of unit
    uint unitCount;

    //! Enabled
    bool enabled;

    //! The associate area (alias)
    Area* parentArea;

    //! The index of the cluster (within a list)
    uint index;

    //! Capacity of reference per unit (MW) (pMax)
    double nominalCapacity;

    //! The index of the cluster from the area's point of view
    //! \warning this variable is only valid when used by the solver
    // (initialized in the same time that the runtime data)
    uint areaWideIndex;

    //! Series
    TimeSeries series;

    TimeSeries::numbers tsNumbers;

    /*!
    ** \brief Modulation matrix
    **
    ** It is merely a 3x8760 matrix
    ** [modulation cost, modulation capacity, market bid modulation] per hour
    */
    Matrix<> modulation;

protected:
    Data::ClusterName pName;
    Data::ClusterName pID;
    Data::ClusterName pGroup;

private:
    virtual unsigned int precision() const = 0;
};
} // namespace Data
} // namespace Antares
#endif /* __LIBS_ANTARES_STUDY_PARTS_COMMON_H__ */
