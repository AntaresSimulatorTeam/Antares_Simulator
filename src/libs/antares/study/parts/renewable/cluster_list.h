#ifndef __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_LIST_H__
#define __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_LIST_H__

#include "../../fwd.h"
#include "cluster.h"
#include "../common/cluster_list.h"

namespace Antares
{
namespace Data
{
/*!
** \brief List of renewable clusters
** \ingroup renewableclusters
*/
class RenewableClusterList : public ClusterList
{
public:
    RenewableClusterList(uint sizeGroup);
    ~RenewableClusterList();
    Cluster* clusterFactory(Area*, uint) override;
}; // class RenewableClusterList
} // namespace Data
} // namespace Antares

#endif /* __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_LIST_H__ */
