#ifndef __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__
#define __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__

#include <antares/logs/logs.h>
#include "../../fwd.h"

#include <antares/writer/i_writer.h>

#include <algorithm>
#include <vector>
#include <memory>
#include <ranges>


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
    using Vect = typename std::vector<SharedPtr>;
    using iterator = typename Vect::iterator;
    using const_iterator = typename Vect::const_iterator;

    /*!
    ** \brief Iterate through all clusters (const)
    */
    template<class PredicateT>
    void each(const PredicateT& predicate) const
    {
        std::ranges::for_each(clusters, [&predicate](const auto& c) { predicate(*c); });
    }

    //! \name clusters management
    //@{
    /*!
    ** \brief Destroy all clusters
    */
    void clear();

    /*!
    ** \brief Add a cluster in the list
    **
    ** \param t The cluster to add
    ** \return True if the cluster has been added, false otherwise
    */
    void add(const SharedPtr clusters);

    /*!
    ** \brief Try to find a cluster from its id (const)
    **
    ** \param id ID of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    ClusterT* findInAll(const Data::ClusterName& id) const;

    /*!
    ** \brief Get if a cluster exists
    **
    ** \param id ID of the cluster to find
    ** \return True if the cluster exists
    */
    bool exists(const Data::ClusterName& id) const;

    auto each_enabled() const { return allClusters | std::views::filter(&ClusterT::isEnabled); }

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

    //! Return true if the list is empty
    bool empty() const;
    //@}

    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

    SharedPtr operator[](std::size_t idx) { return clusters[idx]; }
    const SharedPtr operator[](std::size_t idx) const { return clusters[idx]; }
    /*!
    ** \brief Resize all matrices dedicated to the sampled timeseries numbers
    **
    ** \param n A number of years
    */
    void resizeAllTimeseriesNumbers(uint n);

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


    /*!
    ** \brief Get the size (bytes) occupied in memory by a `ClusterList` structure
    ** \return A size (in bytes)
    */
    virtual uint64_t memoryUsage() const = 0;

    /// \name IO functions
    /// @{
    bool loadDataSeriesFromFolder(Study& study,
                                 const StudyLoadOptions& options,
                                 const AnyString& folder);

    bool saveDataSeriesToFolder(const AnyString& folder) const;

    bool saveDataSeriesToFolder(const AnyString& folder, const YString& msg) const;

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

    /*!
    ** \brief Removes disabled clusters
    **
    ** All clusters with the flag 'enabled' turned to false will be removed from 'list'.
    ** As a consequence, they will no longer be seen as thermal clusters
    ** from the solver's point of view.
    ** \warning This method should only be used from the solver
    **
    ** \return The number of disabled clusters found
    */
    uint removeDisabledClusters();
    unsigned int enabledCount() const;
    unsigned int allClustersCount() const;
    void addToCompleteList(std::shared_ptr<ClusterT> cluster);

protected:
    // The vector containing the clusters
    Vect clusters;
    std::vector<std::shared_ptr<ClusterT>> allClusters;

    /// thermal, renewable, etc.
    virtual std::string typeID() const = 0;

    /// Sort the vector, set index value for each cluster
    void rebuildIndex();
private:
    bool alreadyInAllClusters(std::string clusterName);

}; // class ClusterList
} // namespace Data
} // namespace Antares
#endif /* __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__ */
