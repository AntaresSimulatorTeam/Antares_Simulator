#ifndef __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_LIST_H__
#define __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_LIST_H__

#include "../../fwd.h"
#include "cluster.h"

namespace Antares
{
namespace Data
{
/*!
** \brief List of clusters
** \ingroup renewableclusters
*/
class RenewableClusterList
{
public:
    //! iterator
    typedef RenewableCluster::Map::iterator iterator;
    //! const iterator
    typedef RenewableCluster::Map::const_iterator const_iterator;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    RenewableClusterList();
    /*!
    ** \brief Destructor
    */
    ~RenewableClusterList();
    //@}

    //! \name Iterating
    //@{
    /*!
    ** \brief Iterate through all renewable clusters
    */
    template<class PredicateT>
    void each(const PredicateT& predicate);
    /*!
    ** \brief Iterate through all renewable clusters (const)
    */
    template<class PredicateT>
    void each(const PredicateT& predicate) const;

    //! iterator to the begining of the list
    iterator begin();
    //! iterator to the begining of the list
    const_iterator begin() const;

    //! iterator to the end of the list
    iterator end();
    //! iterator to the end of the list
    const_iterator end() const;

    //@}

    //! \name Cluster management
    //@{
    /*!
    ** \brief Destroy all renewable clusters
    */
    void clear();

    /*!
    ** \brief Add a renewable cluster in the list
    **
    ** \param t The renewable cluster to add
    ** \return True if the renewable cluster has been added, false otherwise
    */
    bool add(RenewableCluster* t);

    /*!
    ** \brief Detach a cluster represented by an iterator
    **
    ** The renewable cluster will be removed from the list but _not_
    ** destroyed.
    ** The iterator should considered as invalid after using this method.
    ** \return A pointer to the renewable cluster, NULL if an error has occured
    */
    RenewableCluster* detach(iterator i);

    /*!
    ** \brief Remove a cluster represented by an iterator
    **
    ** The renewable cluster will be removed from the list but _not_
    ** destroyed.
    ** The iterator should considered as invalid after using this method.
    ** \return void
    */
    void remove(iterator i);

    /*!
    ** \brief Load a list of renewable cluster from a folder
    **
    ** \param folder The folder where the data are located (ex: `input/renewable/clusters/[area]/`)
    ** \param area The associate area
    ** \return True if the operation succeeded, false otherwise
    */
    bool loadFromFolder(Study& s, const AnyString& folder, Area* area);

    /*!
    ** \brief Try to find a renewable cluster from its id
    **
    ** \param id ID of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    RenewableCluster* find(const Data::ClusterName& id);
    /*!
    ** \brief Try to find a renewable cluster from its id (const)
    **
    ** \param id ID of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    const RenewableCluster* find(const Data::ClusterName& id) const;

    /*!
    ** \brief Try to find a renewable cluster from its pointer
    **
    ** \param  p Pointer of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    RenewableCluster* find(const RenewableCluster* p);
    /*!
    ** \brief Try to find a renewable cluster from its pointer (const)
    **
    ** \param  p Pointer of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    const RenewableCluster* find(const RenewableCluster* p) const;

    /*!
    ** \brief Get if a renewable cluster exists
    **
    ** \param id ID of the cluster to find
    ** \return True if the renewable cluster exists
    */
    bool exists(const Data::ClusterName& id) const;

    /*!
    ** \brief Rename a renewable cluster
    **
    ** \param idToFind ID of the cluster to rename
    ** \param newName The new name for the cluster
    ** \return True if the operation succeeded (the cluster has been renammed)
    **   false otherwise (not found or if another renewable cluster has the same name)
    **
    ** The indexes for renewable clusters will be rebuilt.
    */
    bool rename(Data::ClusterName idToFind, Data::ClusterName newName);

    /*!
    ** \brief Remove properly a cluster
    */
    bool remove(const Data::ClusterName& id);

    //! Get the number of items in the list
    uint size() const;

    //! Get if the list is empty
    bool empty() const;
    //@}

    /*!
    ** \brief Resize all matrices dedicated to the sampled timeseries numbers
    **
    ** \param n A number of years
    */
    void resizeAllTimeseriesNumbers(uint n);

    bool storeTimeseriesNumbers(Study& study);

    void retrieveTotalCapacity(double& total) const;
    //@}

    //! \name Memory management
    //@{
    /*!
    ** \brief Invalidate all renewable clusters
    */
    bool invalidate(bool reload = false) const;

    /*!
    ** \brief Mark the renewable cluster as modified
    */
    void markAsModified() const;

    /*!
    ** \brief Flush memory to swap files
    */
    void flush();

    /*!
    ** \brief Rebuild the index of renewable clusters
    **
    ** As a list of renewable clusters is a hash table, it is not
    ** possible to directly accees to a renewable cluster from its index.
    ** However an index can be built but it must be re-built when
    ** the hash table is modified.
    */
    void rebuildIndex();

    /*!
    ** \brief Get the size (bytes) occupied in memory by a `RenewableClusterList` structure
    ** \return A size (in bytes)
    */
    Yuni::uint64 memoryUsage() const;

    /*!
    ** \brief Try to estimate the amount of memory which will be used by the solver
    */
    void estimateMemoryUsage(StudyMemoryUsage&) const;
    //@}

public:
    //! All renewable clusters by their index
    RenewableCluster** byIndex;
    //! All renewable clusters
    RenewableCluster::Map cluster;

    /*!
    ** \brief Number of renewable dispatchable cluster per group
    **
    ** You should rely on these values only after the loading of the study
    ** and until the study is not modified.
    ** These values are modified by 'RenewableClusterListAdd()'
    */
    uint groupCount[renewableGroupMax];

    int loadDataSeriesFromFolder(Study& study,
                                 const StudyLoadOptions& options,
                                 const AnyString& folder);

    int saveDataSeriesToFolder(const AnyString& folder) const;

    int saveDataSeriesToFolder(const AnyString& folder, const YString& msg) const;

    bool saveToFolder(const AnyString& folder) const;

    void ensureDataTimeSeries();
}; // class RenewableClusterList
} // namespace Data
} // namespace Antares

#include "cluster_list.hxx"

#endif /* __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_LIST_H__ */
