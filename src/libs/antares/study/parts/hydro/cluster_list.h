#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDROCLUSTER_CLUSTER_LIST_H__
#define __ANTARES_LIBS_STUDY_PARTS_HYDROCLUSTER_CLUSTER_LIST_H__

#include "../../fwd.h"
#include "cluster.h"
#include "../common/cluster_list.h"

namespace Antares
{
namespace Data
{
/*!
** \brief List of hydrocluster clusters
** \ingroup hydroclusterclusters
*/
class HydroclusterClusterList : public ClusterList<HydroclusterCluster>
{
public:
    HydroclusterClusterList();
    ~HydroclusterClusterList();
    // Overriden virtual methods
    YString typeID() const override;
    void estimateMemoryUsage(StudyMemoryUsage&) const override;
    bool loadFromFolder(const AnyString& folder, Area* area);
    bool saveToFolder(const AnyString& folder) const override;
}; // class HydroclusterClusterList
} // namespace Data
} // namespace Antares

#endif /* __ANTARES_LIBS_STUDY_PARTS_HYDROCLUSTER_CLUSTER_LIST_H__ */
