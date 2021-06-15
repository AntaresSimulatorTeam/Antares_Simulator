#ifndef __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__
#define __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__

#include "../../memory-usage.h"
#include "../../../logs.h"
#include "../../fwd.h"

#include <vector>

namespace Antares
{
namespace Data
{
/*!
** \brief List of clusters
** \ingroup renewableclusters
*/
template<class ClusterT>
class ClusterList
{
public:
    typedef typename std::map<ClusterName, ClusterT*> Map;
    //! iterator
    typedef typename Map::iterator iterator;
    //! const iterator
    typedef typename Map::const_iterator const_iterator;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    ClusterList();
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
    void each(const PredicateT& predicate)
    {
        auto end = cluster.cend();
        for (auto i = cluster.cbegin(); i != end; ++i)
        {
            auto& it = *(i->second);
            predicate(it);
        }
    }
    /*!
    ** \brief Iterate through all clusters (const)
    */
    template<class PredicateT>
    void each(const PredicateT& predicate) const
    {
        auto end = cluster.end();
        for (auto i = cluster.begin(); i != end; ++i)
        {
            const auto& it = *(i->second);
            predicate(it);
        }
    }

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
    bool add(ClusterT* t);

    /*!
    ** \brief Detach a cluster represented by an iterator
    **
    ** The cluster will be removed from the list but _not_
    ** destroyed.
    ** The iterator should considered as invalid after using this method.
    ** \return A pointer to the cluster, NULL if an error has occured
    */
    ClusterT* detach(iterator i);

    /*!
    ** \brief Remove a cluster represented by an iterator
    **
    ** The cluster will be removed from the list but _not_
    ** destroyed.
    ** The iterator should considered as invalid after using this method.
    ** \return void
    */
    virtual void remove(iterator i);

    /*!
    ** \brief Load a list of cluster from a folder
    **
    ** \param folder The folder where the data are located (ex: `input/renewable/clusters/[area]/`)
    ** \param area The associate area
    ** \return True if the operation succeeded, false otherwise
    */
    virtual bool loadFromFolder(Study& s, const AnyString& folder, Area* area) = 0;

    /*!
    ** \brief Try to find a cluster from its id
    **
    ** \param id ID of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    ClusterT* find(const Data::ClusterName& id);
    /*!
    ** \brief Try to find a cluster from its id (const)
    **
    ** \param id ID of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    const ClusterT* find(const Data::ClusterName& id) const;

    /*!
    ** \brief Try to find a cluster from its pointer
    **
    ** \param  p Pointer of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    ClusterT* find(const ClusterT* p);
    /*!
    ** \brief Try to find a cluster from its pointer (const)
    **
    ** \param  p Pointer of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    const ClusterT* find(const ClusterT* p) const;

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
    virtual bool remove(const Data::ClusterName& id);

    //! Get the number of items in the list
    uint size() const;

    //! Get if the list is empty
    bool empty() const;
    //@}

    //! iterator to the begining of the list
    iterator begin();
    //! iterator to the begining of the list
    const_iterator begin() const;

    //! iterator to the end of the list
    iterator end();
    //! iterator to the end of the list
    const_iterator end() const;

    /*!
    ** \brief Resize all matrices dedicated to the sampled timeseries numbers
    **
    ** \param n A number of years
    */
    void resizeAllTimeseriesNumbers(uint n);

    bool storeTimeseriesNumbers(Study& study);

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
    virtual void estimateMemoryUsage(StudyMemoryUsage&) const = 0;
    //@}

public:
    //! All clusters by their index
    ClusterT** byIndex;
    //! All clusters
    Map cluster;

    // thermal, renewable, etc.
    virtual YString typeID() const = 0;

    /*!
    ** \brief Number of dispatchable cluster per group
    **
    ** You should rely on these values only after the loading of the study
    ** and until the study is not modified.
    ** These values are modified by 'ClusterListAdd()'
    */
    std::vector<uint> groupCount;

    int loadDataSeriesFromFolder(Study& study,
                                 const StudyLoadOptions& options,
                                 const AnyString& folder,
                                 bool fast);

    int saveDataSeriesToFolder(const AnyString& folder) const;

    int saveDataSeriesToFolder(const AnyString& folder, const YString& msg) const;

    virtual bool saveToFolder(const AnyString& folder) const = 0;

    void ensureDataTimeSeries();

    //! \name Informations
    //@{
    /*!
    ** \brief Retrieve the total capacity and the total unit count
    **
    ** Pseudo code:
    ** \code
    ** each thermal cluster do
    ** 	total += cluster{unit count} * cluster{nominal capacity}
    **	unit  += cluster{unit count}
    ** \endcode
    */
    void retrieveTotalCapacityAndUnitCount(double& total, uint& unitCount) const;
    //@}
}; // class ClusterList
} // namespace Data
} // namespace Antares
#endif /* __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__ */
