#ifndef __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__
#define __ANTARES_LIBS_STUDY_PARTS_COMMON_CLUSTER_LIST_H__

#include <antares/logs/logs.h>
#include "../../fwd.h"

#include <antares/writer/i_writer.h>

#include <algorithm>
#include <vector>
#include <memory>

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
    // Shared pointer
    using SharedPtr = typename std::shared_ptr<ClusterT>;
    // Vector container
    using Vect = typename std::vector<SharedPtr>;
    //! iterator
    using iterator = typename Vect::iterator;
    //! const iterator
    using const_iterator = typename Vect::const_iterator;

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
    virtual ~ClusterList();
    //@}

    //! \name Iterating
    //@{
    /*!
    ** \brief Iterate through all clusters
    */
    template<class PredicateT>
    void each(const PredicateT& predicate)
    {
        for (auto& c : cluster)
            predicate(*c);
    }
    /*!
    ** \brief Iterate through all clusters (const)
    */
    template<class PredicateT>
    void each(const PredicateT& predicate) const
    {
        for (const auto& c : cluster)
            predicate(*c);
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

    SharedPtr add(const SharedPtr& t);
    /*!
    ** \brief Detach a cluster represented by an iterator
    **
    ** The cluster will be removed from the list but _not_
    ** destroyed.
    ** The iterator should considered as invalid after using this method.
    ** \return A pointer to the cluster, NULL if an error has occured
    */
    SharedPtr detach(iterator i);

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
    SharedPtr find(const ClusterList<ClusterT>::SharedPtr& p);


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

    void storeTimeseriesNumbers(Solver::IResultWriter& writer) const;

    //@}

    //! \name Memory management
    //@{
    /*!
    ** \brief Invalidate all clusters
    */
    bool forceReload(bool reload = false) const;

    /*!
    ** \brief Mark the cluster as modified
    */
    void markAsModified() const;

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
    uint64_t memoryUsage() const;

public:
    //! All clusters
    Vect cluster;

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

    bool loadDataSeriesFromFolder(Study& study,
                                 const StudyLoadOptions& options,
                                 const AnyString& folder);

    bool saveDataSeriesToFolder(const AnyString& folder) const;

    bool saveDataSeriesToFolder(const AnyString& folder, const YString& msg) const;

    virtual bool saveToFolder(const AnyString& folder) const = 0;

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
