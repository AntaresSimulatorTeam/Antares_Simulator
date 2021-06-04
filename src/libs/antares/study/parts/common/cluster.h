#ifndef __LIBS_ANTARES_STUDY_PARTS_COMMON_H__
#define __LIBS_ANTARES_STUDY_PARTS_COMMON_H__

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>
#include "series.h"
#include "../../../array/matrix.h"
#include "../../fwd.h"

namespace Antares
{
namespace Data
{

class Cluster
{
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
};
}
}
#endif /* __LIBS_ANTARES_STUDY_PARTS_COMMON_H__ */
