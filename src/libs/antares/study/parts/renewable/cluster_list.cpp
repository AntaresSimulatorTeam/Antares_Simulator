#include "cluster_list.h"

namespace Antares
{
namespace Data
{
RenewableClusterList::~RenewableClusterList()
{
    for (auto& it : cluster)
    {
        delete it.second;
    }
}

RenewableClusterList::RenewableClusterList()
{
}

YString RenewableClusterList::typeID() const
{
    return "renewable";
}

void RenewableClusterList::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += (sizeof(void*) * 4 /*overhead map*/) * cluster.size();

    each([&](const Cluster& cluster) {
        u.requiredMemoryForInput += sizeof(RenewableCluster);
        u.requiredMemoryForInput += sizeof(void*);
        if (cluster.series)
            cluster.series->estimateMemoryUsage(u, timeSeriesRenewable /* FIXME */);

        // From the solver
        u.requiredMemoryForInput += 70 * 1024;
    });
}

} // namespace Data
} // namespace Antares
