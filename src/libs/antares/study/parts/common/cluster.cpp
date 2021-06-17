#include <yuni/yuni.h>
#include <yuni/io/file.h>
#include <yuni/io/directory.h>

#include "cluster.h"
#include "../../study.h"
#include "../../../utils.h"

namespace Antares
{
namespace Data
{
Cluster::Cluster(Area* parent) :
 unitCount(0),
 enabled(true),
 parentArea(parent),
 index(0),
 nominalCapacity(0.),
 areaWideIndex((uint)-1)
{
}

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

Yuni::String Cluster::getFullName() const
{
    Yuni::String s;
    s << parentArea->name << "." << pID;
    return s;
}

void Cluster::setName(const AnyString& newname)
{
    pName = newname;
    pID.clear();
    TransformNameIntoID(pName, pID);
}

void Cluster::invalidateArea()
{
    if (parentArea)
        parentArea->invalidate();
}

bool Cluster::isVisibleOnLayer(const size_t& layerID) const
{
    return parentArea ? parentArea->isVisibleOnLayer(layerID) : false;
}

bool CompareClusterName::operator()(const Cluster* s1, const Cluster* s2) const
{
    return (s1->getFullName() < s2->getFullName());
}

} // namespace Data
} // namespace Antares
