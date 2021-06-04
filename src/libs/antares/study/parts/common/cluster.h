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
struct CompareClusterName;

class Cluster
{
public:
    //! Set of renewable clusters
    typedef std::set<Cluster*, CompareClusterName> Set;
    //! Set of renewable clusters (pointer)
    typedef std::set<Cluster*> SetPointer;
    //! Map of renewable clusters
    typedef std::map<ClusterName, Cluster*> Map;
    //! Vector of renewable clusters
    typedef std::vector<Data::Cluster*> Vector;

public:
    Cluster(Area* parent);

    //! Get the thermal cluster ID
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

    //! Get the full thermal cluster name
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
    ** \brief Mark the renewable cluster as modified
    */
    virtual void markAsModified() const = 0;

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

    //! The index of the thermal cluster from the area's point of view
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
    int saveDataSeriesToFolder(const AnyString& folder) const;
    int loadDataSeriesFromFolder(Study& s, const AnyString& folder);
};

struct CompareClusterName final
{
    inline bool operator()(const Cluster* s1, const Cluster* s2) const
    {
        return (s1->getFullName() < s2->getFullName());
    }
};

} // namespace Data
} // namespace Antares
#endif /* __LIBS_ANTARES_STUDY_PARTS_COMMON_H__ */
