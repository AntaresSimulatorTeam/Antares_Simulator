#ifndef __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__
#define __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__

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
class ClusterList
{
public:
    //! iterator
    typedef Cluster::Map::iterator iterator;
    //! const iterator
    typedef Cluster::Map::const_iterator const_iterator;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    ClusterList(uint sizeGroup);
    /*!
    ** \brief Destructor
    */
    ~ClusterList();
    //@}

    //! \name Iterating
    //@{
    /*!
    ** \brief Iterate through all clusters
    */
    template<class PredicateT>
    void each(const PredicateT& predicate);
    /*!
    ** \brief Iterate through all clusters (const)
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
    ** \brief Destroy all clusters
    */
    void clear();

    /*!
    ** \brief Add a cluster in the list
    **
    ** \param t The cluster to add
    ** \return True if the cluster has been added, false otherwise
    */
    bool add(Cluster* t);

    /*!
    ** \brief Detach a cluster represented by an iterator
    **
    ** The cluster will be removed from the list but _not_
    ** destroyed.
    ** The iterator should considered as invalid after using this method.
    ** \return A pointer to the cluster, NULL if an error has occured
    */
    Cluster* detach(iterator i);

    /*!
    ** \brief Remove a cluster represented by an iterator
    **
    ** The cluster will be removed from the list but _not_
    ** destroyed.
    ** The iterator should considered as invalid after using this method.
    ** \return void
    */
    void remove(iterator i);

    /*!
    ** \brief Load a list of cluster from a folder
    **
    ** \param folder The folder where the data are located (ex: `input/renewable/clusters/[area]/`)
    ** \param area The associate area
    ** \return True if the operation succeeded, false otherwise
    */
    bool loadFromFolder(Study& s, const AnyString& folder, Area* area);

    /*!
    ** \brief Try to find a cluster from its id
    **
    ** \param id ID of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    Cluster* find(const Data::ClusterName& id);
    /*!
    ** \brief Try to find a cluster from its id (const)
    **
    ** \param id ID of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    const Cluster* find(const Data::ClusterName& id) const;

    /*!
    ** \brief Try to find a cluster from its pointer
    **
    ** \param  p Pointer of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    Cluster* find(const Cluster* p);
    /*!
    ** \brief Try to find a cluster from its pointer (const)
    **
    ** \param  p Pointer of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    const Cluster* find(const Cluster* p) const;

    /*!
    ** \brief Get if a cluster exists
    **
    ** \param id ID of the cluster to find
    ** \return True if the cluster exists
    */
    bool exists(const Data::ClusterName& id) const;

    /*!
    ** \brief Rename a cluster
    **
    ** \param idToFind ID of the cluster to rename
    ** \param newName The new name for the cluster
    ** \return True if the operation succeeded (the cluster has been renammed)
    **   false otherwise (not found or if another cluster has the same name)
    **
    ** The indexes for clusters will be rebuilt.
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
    ** \brief Invalidate all clusters
    */
    bool invalidate(bool reload = false) const;

    /*!
    ** \brief Mark the cluster as modified
    */
    void markAsModified() const;

    /*!
    ** \brief Flush memory to swap files
    */
    void flush();

    /*!
    ** \brief Rebuild the index of clusters
    **
    ** As a list of clusters is a hash table, it is not
    ** possible to directly accees to a cluster from its index.
    ** However an index can be built but it must be re-built when
    ** the hash table is modified.
    */
    void rebuildIndex();

    /*!
    ** \brief Get the size (bytes) occupied in memory by a `ClusterList` structure
    ** \return A size (in bytes)
    */
    Yuni::uint64 memoryUsage() const;

    /*!
    ** \brief Try to estimate the amount of memory which will be used by the solver
    */
    void estimateMemoryUsage(StudyMemoryUsage&) const;
    //@}

public:
    //! All clusters by their index
    Cluster** byIndex;
    //! All clusters
    Cluster::Map cluster;

    virtual Cluster* clusterFactory(Area*, uint) = 0;

    /*!
    ** \brief Number of dispatchable cluster per group
    **
    ** You should rely on these values only after the loading of the study
    ** and until the study is not modified.
    ** These values are modified by 'ClusterListAdd()'
    */
    uint* groupCount;

    int loadDataSeriesFromFolder(Study& study,
                                 const StudyLoadOptions& options,
                                 const AnyString& folder);

    int saveDataSeriesToFolder(const AnyString& folder) const;

    int saveDataSeriesToFolder(const AnyString& folder, const YString& msg) const;

    bool saveToFolder(const AnyString& folder) const;

    void ensureDataTimeSeries();
}; // class ClusterList
} // namespace Data
} // namespace Antares

#include "cluster_list.hxx"

#endif /* __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__ */
