/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_H__
#define __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_H__

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>
#include "../../../array/matrix.h"
#include "defines.h"
#include "series.h"
#include "../../fwd.h"
#include <set>
#include <map>
#include <vector>

namespace Antares
{
namespace Data
{
enum RenewableGroup
{
    //! Wind offshore
    windOffShore = 0,
    //! Wind onshore
    windOnShore,
    //! Concentration solar
    concentrationSolar,
    //! PV solar
    PVSolar,
    //! Rooftop solar
    rooftopSolar,
    //! Other 1
    renewableOther1,
    //! Other 2
    renewableOther2,
    //! Other 3
    renewableOther3,
    //! Other 4
    renewableOther4,

    //! The highest value
    renewableGroupMax
};

struct CompareRenewableClusterName;

/*!
** \brief A single renewable cluster
*/
class RenewableCluster final : public Yuni::NonCopyable<RenewableCluster>
{
public:
    //! Set of renewable clusters
    typedef std::set<RenewableCluster*, CompareRenewableClusterName> Set;
    //! Set of renewable clusters (pointer)
    typedef std::set<RenewableCluster*> SetPointer;
    //! Map of renewable clusters
    typedef std::map<RenewableClusterName, RenewableCluster*> Map;
    //! Vector of renewable clusters
    typedef std::vector<Data::RenewableCluster*> Vector;

public:
    /*!
    ** \brief Get the group name string
    ** \return A valid CString
    */
    static const char* GroupName(enum RenewableGroup grp);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor, with a parent area
    */
    explicit RenewableCluster(Data::Area* parent);
    explicit RenewableCluster(Data::Area* parent, uint nbParallelYears);
    //! Destructor
    ~RenewableCluster();
    //@}

    /*!
    ** \brief Invalidate all data associated to the renewable cluster
    */
    bool invalidate(bool reload) const;

    /*!
    ** \brief Mark the renewable cluster as modified
    */
    void markAsModified() const;

    /*!
    ** \brief Invalidate the whole attached area
    */
    void invalidateArea();

    /*!
    ** \brief Reset to default values
    **
    ** This method should only be called from the GUI
    */
    void reset();

    //! \name Name and ID
    //@{
    //! Get the renewable cluster ID
    const Data::RenewableClusterName& id() const;

    //! Get the renewable cluster name
    const Data::RenewableClusterName& name() const;

    //! Get the full renewable cluster name
    Yuni::String getFullName() const;

    //! Set the name/ID
    void name(const AnyString& newname);
    //@}

    //! \name Group
    //@{
    //! Get the group of the cluster
    const Data::RenewableClusterName& group() const;
    //! Set the group
    void group(Data::RenewableClusterName newgrp);
    //@}

    /*!
    ** \brief Check and fix all values of a renewable cluster
    **
    ** \return False if an error has been detected and fixed with a default value
    */
    bool integrityCheck();

    /*!
    ** \brief Copy data from another cluster
    **
    ** The name and the id will remain untouched.
    */
    void copyFrom(const RenewableCluster& cluster);

    //! \name Memory management
    //@{
    /*!
    ** \brief Flush the memory to swap files (if swap support enabled)
    */
    void flush();

    /*!
    ** \brief Get the memory consummed by the renewable cluster (in bytes)
    */
    Yuni::uint64 memoryUsage() const;
    //@}

    //! \name validity of Min Stable Power
    //@{
    // bool minStablePowerValidity() const;

    /*!
    ** \brief Calculte the minimum modulation/ceil(modulation) from 8760 capacity modulation
    */
    void calculatMinDivModulation();

    /*!
    ** \brief Check the validity of Min Stable Power
    */
    bool checkMinStablePower();

    /*!
    ** \brief Check the validity of Min Stable Power with a new modulation value
    */
    bool checkMinStablePowerWithNewModulation(uint index, double value);
    //@}

    /*!
    ** \brief Check wether the cluster is visible in a layer (it's parent area is visible in the
    *layer)
    */
    bool isVisibleOnLayer(const size_t& layerID) const;

public:
    /*!
    ** \brief The group ID
    **
    ** This value is computed from the field 'group' in 'group()
    ** \see group()
    */
    enum RenewableGroup groupID;

    //! The index of the renewable cluster (within a list)
    uint index;
    //! The index of the renewable cluster from the area's point of view
    //! \warning this variable is only valid when used by the solver
    // (initialized in the same time that the runtime data)
    uint areaWideIndex;

    //! The associate area (alias)
    Area* parentArea;

    //! Enabled
    bool enabled;

    //! Capacity of reference per unit (MW) (pMax)
    double nominalCapacity;

    //! Series
    DataSeriesRenewable* series;

private:
    //! Name
    Data::RenewableClusterName pName;
    //! ID
    Data::RenewableClusterName pID;
    //! Group
    Data::RenewableClusterName pGroup;

    friend class RenewableClusterList;

}; // class RenewableCluster

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
    RenewableCluster* find(const Data::RenewableClusterName& id);
    /*!
    ** \brief Try to find a renewable cluster from its id (const)
    **
    ** \param id ID of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    const RenewableCluster* find(const Data::RenewableClusterName& id) const;

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
    bool exists(const Data::RenewableClusterName& id) const;

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
    bool rename(Data::RenewableClusterName idToFind, Data::RenewableClusterName newName);

    /*!
    ** \brief Remove properly a cluster
    */
    bool remove(const Data::RenewableClusterName& id);

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

    /*!
    ** \brief Original mapping from the input
    **
    ** Without taking into consideration the enabled flags
    */
    RenewableCluster::Map mapping;

}; // class RenewableClusterList

/*!
** \brief Load data related to time-series from a list of renewable clusters from a folder
** \ingroup renewableclusters
**
** \todo Remaining of old C-library. this routine should be moved into the appropriate class
** \param l A list of renewable clusters
** \param folder The target folder
** \return A non-zero value if the operation succeeded, 0 otherwise
*/
int RenewableClusterListLoadDataSeriesFromFolder(Study& study,
                                                 const StudyLoadOptions& options,
                                                 RenewableClusterList* l,
                                                 const AnyString& folder,
                                                 int fast);

/*!
** \brief Save data related to time-series from a list of renewable clusters to a folder
** \ingroup renewableclusters
**
** \todo Remaining of old C-library. this routine should be moved into the appropriate class
** \param l A list of renewable clusters
** \param folder The target folder
** \param msg Message to display
** \return A non-zero value if the operation succeeded, 0 otherwise
*/
int RenewableClusterListSaveDataSeriesToFolder(const RenewableClusterList* l,
                                               const AnyString& folder);
int RenewableClusterListSaveDataSeriesToFolder(const RenewableClusterList* l,
                                               const AnyString& folder,
                                               const YString& msg);

struct CompareRenewableClusterName final
{
    inline bool operator()(const RenewableCluster* s1, const RenewableCluster* s2) const
    {
        return (s1->getFullName() < s2->getFullName());
    }
};

} // namespace Data
} // namespace Antares

#include "cluster.hxx"

#endif //__ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CLUSTER_H__
