#include "cluster.h"
#include "../../../utils.h"

namespace Antares
{
namespace Data
{
  Cluster::Cluster(Area* parent) : unitCount(0),
                                   enabled(true),
                                   parentArea(parent),
                                   index(0),
                                   nominalCapacity(0.),
                                   series(nullptr)

  {}

const ClusterName& Cluster::group() const
{
    return pGroup;
}

const ClusterName& Cluster::name() const
{
    return pName;
}

const ClusterName& Cluster::id() const
{
    return pID;
}

void Cluster::setName(const AnyString& newname)
{
    pName = newname;
    pID.clear();
    TransformNameIntoID(pName, pID);
}

}
}
