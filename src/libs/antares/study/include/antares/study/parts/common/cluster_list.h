// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__
#define __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__

#include <algorithm>
#include <memory>
#include <ranges>
#include <vector>

#include <antares/inifile/inifile.h>
#include <antares/logs/logs.h>
#include <antares/study/area/capacityReservation.h>
#include <antares/study/parts/common/cluster.h>
#include <antares/study/parts/thermal/cluster.h>
#include <antares/writer/i_writer.h>

#include "../../fwd.h"

namespace Antares::Data
{

// TODO VP: remove template, we can have a regular class in this use case
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
    bool exists(const std::string& id) const;

    auto each_enabled() const
    {
        return allClusters_ | std::views::filter(&ClusterT::isEnabled);
    }

    std::vector<std::shared_ptr<ClusterT>> all() const;

    /*!
    ** \brief Remove properly a cluster
    */
    virtual bool remove(const std::string& id);

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
    ** @brief Get the cluster and reserve names for a given index of reserveParticipation
    ** @param area The area where to look for the reserveParticipation
    ** @param index Global index of the reserveParicipation
    ** @return the cluster and reserve names
    */
    std::pair<std::string, ReserveID> reserveParticipationClusterAt(const Area* area,
                                                                    unsigned int index) const;

    /*!
    ** @brief Get the group and reserve names for a given index of reserveParticipation
    ** @param area The area where to look for the reserveParticipation
    ** @param index Global index of the reserveParicipation
    ** @return the group and reserve names
    */
    std::pair<std::string, ReserveID> reserveParticipationGroupAt(const Area* area,
                                                                  unsigned int index) const;

    /*!
    ** \brief Resize all matrices dedicated to the sampled timeseries numbers
    **
    ** \param n A number of years
    */
    void resizeAllTimeseriesNumbers(uint n) const;

    void storeTimeseriesNumbers(Solver::IResultWriter& writer) const;

    //@}

    /// \name IO functions
    /// @{
    bool loadDataSeriesFromFolder(Study& study, const std::filesystem::path& folder);

    /// @brief Load the reserve participation. For each entry, it checks if the reserve has been
    /// added to area.allCapacityReservations, if not then log the name of the reserve that has not
    /// been found.
    /// @tparam ClusterT Type of the Cluster list
    /// @param area Reference to area
    /// @param file File to read the reserve participations entries
    /// @return false if the file opening failed, true otherwise
    bool loadReserveParticipations(Area& area, const std::filesystem::path& file);

    bool saveDataSeriesToFolder(const AnyString& folder) const;
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
} // namespace Antares::Data
#endif /* __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__ */
