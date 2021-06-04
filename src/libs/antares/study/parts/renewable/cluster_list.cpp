#include "cluster_list.h"

namespace Antares
{
namespace Data
{
Cluster* RenewableClusterList::clusterFactory(Area* area, uint nbParallelYears)
{
    Cluster* cluster = new RenewableCluster(area, nbParallelYears);
    return cluster;
}

RenewableClusterList::~RenewableClusterList()
{
    for (auto& it : cluster)
    {
        delete it.second;
    }
}

RenewableClusterList::RenewableClusterList(uint groupSize) : ClusterList(groupSize)
{
}

} // namespace Data
} // namespace Antares
