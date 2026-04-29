// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_LIST_H__
#define __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_LIST_H__

#include <filesystem>

#include "../../fwd.h"
#include "../common/cluster_list.h"
#include "cluster.h"

namespace Antares::Data
{
/*!
** \brief List of renewable clusters
** \ingroup renewableclusters
*/
class RenewableClusterList final: public ClusterList<RenewableCluster>
{
public:
    std::string typeID() const override;

    bool loadFromFolder(const std::filesystem::path& folder, Area* area);
    bool validateClusters() const;

}; // class RenewableClusterList
} // namespace Antares::Data

#endif /* __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_LIST_H__ */
