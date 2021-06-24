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
class RenewableClusterList : public ClusterList<RenewableCluster>
{
public:
    RenewableClusterList();
    ~RenewableClusterList();
    // Overriden virtual methods
    YString typeID() const override;
    void estimateMemoryUsage(StudyMemoryUsage&) const override;
    bool loadFromFolder(const AnyString& folder, Area* area);
    bool saveToFolder(const AnyString& folder) const override;

    /*!
    ** \brief Resize all matrices dedicated to the sampled timeseries numbers
    **
    ** \param n A number of years
    */
    void resizeAllTimeseriesNumbers(uint n);

    bool storeTimeseriesNumbers(Study& study);

    int loadDataSeriesFromFolder(Study& study,
        const StudyLoadOptions& options,
        const AnyString& folder,
        bool fast);

    int saveDataSeriesToFolder(const AnyString& folder) const;

    int saveDataSeriesToFolder(const AnyString& folder, const YString& msg) const;

    void ensureDataTimeSeries();

}; // class RenewableClusterList
} // namespace Data
} // namespace Antares

#endif /* __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_LIST_H__ */
