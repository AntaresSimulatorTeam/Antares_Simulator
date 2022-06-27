#ifndef __LIBS_ANTARES_STUDY_PARTS_COMMON_H__
#define __LIBS_ANTARES_STUDY_PARTS_COMMON_H__

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>
#include "series.h"
#include "../../../array/matrix.h"
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
    //! Map of renewable clusters
    using Map = std::map<ClusterName, Cluster*>;

public:
    Cluster(Area* parent);

    virtual ~Cluster() = default;

    //! Get the cluster ID
    const ClusterName& id() const;

    //! \name Group
    //@{
    //! Get the group of the cluster
    const ClusterName& group() const;

    //! Get the renewable cluster name
    const ClusterName& name() const;

    //! Set the name/ID
    void setName(const AnyString& newname);
    //@}

    //! Get the full cluster name
    Yuni::String getFullName() const;

    //! \name Memory management
    //@{
    /*!
    ** \brief Flush the memory to swap files (if swap support enabled)
    */
    virtual void flush() = 0;

    virtual uint groupId() const = 0;

    //! Set the group
    virtual void setGroup(Data::ClusterName newgrp) = 0;
    //@}

    /*!
    ** \brief Check and fix all values of a renewable cluster
    **
    ** \return False if an error has been detected and fixed with a default value
    */
    virtual bool integrityCheck() = 0;

    /*!
    ** \brief Get the memory consummed by the renewable cluster (in bytes)
    */
    virtual Yuni::uint64 memoryUsage() const = 0;
    //@}

    /*!
    ** \brief Invalidate all data associated to the renewable cluster
    */
    virtual bool invalidate(bool reload) const = 0;

    /*!
    ** \brief Invalidate the whole attached area
    */
    void invalidateArea();

    /*!
    ** \brief Mark the renewable cluster as modified
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
    DataSeriesCommon* series;

    /*!
    ** \brief Modulation matrix
    **
    ** It is merely a 3x8760 matrix
    ** [modulation cost, modulation capacity, market bid modulation] per hour
    */
    Matrix<> modulation;

protected:
    //! Name
    Data::ClusterName pName;
    //! ID
    Data::ClusterName pID;
    //! Group
    Data::ClusterName pGroup;

public:
    //! Set of clusters
    using Set = std::set<Cluster*, CompareClusterName>;

    int saveDataSeriesToFolder(const AnyString& folder) const;
    int loadDataSeriesFromFolder(Study& s, const AnyString& folder);

private:
    virtual unsigned int precision() const = 0;
};
} // namespace Data
} // namespace Antares
#endif /* __LIBS_ANTARES_STUDY_PARTS_COMMON_H__ */
