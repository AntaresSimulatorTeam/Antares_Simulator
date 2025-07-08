/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__
#define __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__

#include <algorithm>
#include <memory>
#include <ranges>
#include <vector>

#include <antares/logs/logs.h>
#include <antares/writer/i_writer.h>

#include "../../fwd.h"

namespace Antares
{
namespace Data
{
/*!
** \brief Generic list of clusters
** \ingroup renewableclusters
** This class implements the base functions for a list of cluster
** It's used for thermal and renewable clusters
*/
template<class ClusterT>
class ClusterList
{
public:
    using SharedPtr = typename std::shared_ptr<ClusterT>;

    void clearAll();
    bool empty() const;

    /*!
    ** \brief Try to find a cluster from its id (const) in the complete cluster list
    **
    ** \param id ID of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    ClusterT* findInAll(std::string_view id) const;

    /*!
    ** \brief Get if a cluster exists
    **
    ** \param id ID of the cluster to find
    ** \return True if the cluster exists
    */
    bool exists(const Data::ClusterName& id) const;

    auto each_enabled() const
    {
        return allClusters_ | std::views::filter(&ClusterT::isEnabled);
    }

    std::vector<std::shared_ptr<ClusterT>> all() const;

    /*!
    ** \brief Rename a cluster
    **
    ** \param idToFind ID of the cluster to rename
    ** \param newName The new name for the cluster
    ** \return True if the operation succeeded (the cluster has been renamed)
    **   false otherwise (not found or if another cluster has the same name)
    **
    ** The indexes for clusters will be rebuilt.
    */
    bool rename(Data::ClusterName idToFind, Data::ClusterName newName);

    /*!
    ** \brief Remove properly a cluster
    */
    virtual bool remove(const Data::ClusterName& id);

    //@}

    SharedPtr operator[](std::size_t idx)
    {
        return allClusters_[idx];
    }

    SharedPtr operator[](std::size_t idx) const
    {
        return allClusters_[idx];
    }

    SharedPtr enabledClusterAt(unsigned int index) const;
    /*!
    ** \brief Resize all matrices dedicated to the sampled timeseries numbers
    **
    ** \param n A number of years
    */
    void resizeAllTimeseriesNumbers(uint n) const;

    void storeTimeseriesNumbers(Solver::IResultWriter& writer) const;

    //@}

    /*!
    ** \brief Invalidate all clusters
    */
    bool forceReload(bool reload = false) const;

    /*!
    ** \brief Mark the clusters as modified
    */
    void markAsModified() const;

    /// \name IO functions
    /// @{
    bool loadDataSeriesFromFolder(Study& study, const std::filesystem::path& folder);

    bool saveDataSeriesToFolder(const AnyString& folder) const;

    virtual bool saveToFolder(const AnyString& folder) const = 0;
    ///@}

    /*!
    ** \brief Retrieve the total capacity and the total unit count
    **
    ** Pseudo code:
    ** \code
    ** each thermal clusters do
    ** 	total += clusters{unit count} * clusters{nominal capacity}
    **	unit  += clusters{unit count}
    ** \endcode
    */
    void retrieveTotalCapacityAndUnitCount(double& total, uint& unitCount) const;

    unsigned int enabledCount() const;
    unsigned int allClustersCount() const;
    void addToCompleteList(std::shared_ptr<ClusterT> cluster);
    void sortCompleteList();

protected:
    std::vector<std::shared_ptr<ClusterT>> allClusters_;

    virtual std::string typeID() const = 0;

    // Give a special index to enabled clusters (thermal / renewable)
    void rebuildIndexes();

private:
    bool alreadyInAllClusters(std::string clusterName);

}; // class ClusterList
} // namespace Data
} // namespace Antares
#endif /* __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__ */
