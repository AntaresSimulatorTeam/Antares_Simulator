/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#ifndef __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_LIST_H__
#define __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_LIST_H__

#include "../../fwd.h"
#include "../common/cluster_list.h"
#include "cluster.h"

namespace Antares::Data
{
/*!
** \brief List of renewable clusters
** \ingroup renewableclusters
*/
class RenewableClusterList: public ClusterList<RenewableCluster>
{
public:
    std::string typeID() const override;
    uint64_t memoryUsage() const override;

    bool loadFromFolder(const AnyString& folder, Area* area);
    bool validateClusters() const;

    bool saveToFolder(const AnyString& folder) const override;
}; // class RenewableClusterList
} // namespace Antares::Data

#endif /* __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_LIST_H__ */
