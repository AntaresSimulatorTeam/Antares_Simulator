#include "cluster_list.h"
#include "../../../utils.h"
#include "../../study.h"
#include "../../area.h"

namespace Antares
{
namespace Data
{
using namespace Antares;

template<class ClusterT>
inline void ClusterList<ClusterT>::flush()
{
#ifndef ANTARES_SWAP_SUPPORT
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        Cluster& it = *(i->second);
        it.flush();
    }
#endif
}

template<class ClusterT>
inline uint ClusterList<ClusterT>::size() const
{
    return (uint)cluster.size();
}

template<class ClusterT>
inline bool ClusterList<ClusterT>::empty() const
{
    return cluster.empty();
}

template<class ClusterT>
typename ClusterList<ClusterT>::iterator ClusterList<ClusterT>::begin()
{
    return std::begin(cluster);
}

template<class ClusterT>
typename ClusterList<ClusterT>::const_iterator ClusterList<ClusterT>::begin() const
{
    return std::begin(cluster);
}

template<class ClusterT>
typename ClusterList<ClusterT>::iterator ClusterList<ClusterT>::end()
{
    return std::end(cluster);
}

template<class ClusterT>
typename ClusterList<ClusterT>::const_iterator ClusterList<ClusterT>::end() const
{
    return std::end(cluster);
}

template<class ClusterT>
const ClusterT* ClusterList<ClusterT>::find(const Data::ClusterName& id) const
{
    auto i = cluster.find(id);
    return (i != cluster.end()) ? i->second.get() : nullptr;
}

template<class ClusterT>
ClusterT* ClusterList<ClusterT>::find(const Data::ClusterName& id)
{
    auto i = cluster.find(id);
    return (i != cluster.end()) ? i->second.get() : nullptr;
}

template<class ClusterT>
ClusterT* ClusterList<ClusterT>::detach(iterator i)
{
    SharedPtr c = i->second;
    cluster.erase(i);
    return c.get();
}

template<class ClusterT>
void ClusterList<ClusterT>::remove(iterator i)
{
    cluster.erase(i);
}

template<class ClusterT>
bool ClusterList<ClusterT>::exists(const Data::ClusterName& id) const
{
    if (not cluster.empty())
    {
        auto element = cluster.find(id);
        return (element != cluster.end());
    }
    return false;
}

template<class ClusterT>
Data::ClusterList<ClusterT>::ClusterList() : byIndex(nullptr), groupCount(ClusterT::groupMax, 0)
{
}

template<class ClusterT>
Data::ClusterList<ClusterT>::~ClusterList()
{
    // deleting all renewable clusters
    clear();
}

template<class ClusterT>
void ClusterList<ClusterT>::clear()
{
    if (byIndex)
    {
        delete[] byIndex;
        byIndex = nullptr;
    }

    if (not cluster.empty())
        cluster.clear();
}

template<class ClusterT>
const ClusterT* ClusterList<ClusterT>::find(const ClusterT* p) const
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        if (p == i->second.get())
            return i->second.get();
    }
    return nullptr;
}

template<class ClusterT>
ClusterT* ClusterList<ClusterT>::find(const ClusterT* p)
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        if (p == i->second.get())
            return i->second.get();
    }
    return nullptr;
}

#define SEP IO::Separator

template<class ClusterT>
void ClusterList<ClusterT>::rebuildIndex()
{
    delete[] byIndex;

    if (not empty())
    {
        uint indx = 0;
        typedef ClusterT* ClusterWeakPtr;
        byIndex = new ClusterWeakPtr[size()];

        auto end = cluster.end();
        for (auto i = cluster.begin(); i != end; ++i)
        {
            auto cluster = i->second.get();
            byIndex[indx] = cluster;
            cluster->index = indx;
            ++indx;
        }
    }
    else
        byIndex = nullptr;
}

template<class ClusterT>
typename ClusterList<ClusterT>::SharedPtr ClusterList<ClusterT>::add(ClusterT* newcluster)
{
    if (newcluster)
    {
        if (exists(newcluster->id()))
            return cluster[newcluster->id()];

        newcluster->index = (uint)size();
        cluster[newcluster->id()] = SharedPtr(newcluster);
        ++(groupCount[newcluster->groupId()]);
        rebuildIndex();
        return cluster[newcluster->id()];
    }
    return nullptr;
}

template<class ClusterT>
Yuni::uint64 ClusterList<ClusterT>::memoryUsage() const
{
    uint64 ret = sizeof(ClusterList) + (2 * sizeof(void*)) * this->size();

    each([&](const ClusterT& cluster) { ret += cluster.memoryUsage(); });
    return ret;
}

template<class ClusterT>
bool ClusterList<ClusterT>::rename(Data::ClusterName idToFind, Data::ClusterName newName)
{
    if (not idToFind or newName.empty())
        return false;

    // Internal:
    // It is vital to make copy of these strings. We can not make assumption that these
    // CString are not from the same buffer (name, id) than ours.
    // It may have an undefined behavior.
    // Consequently, the parameters `idToFind` and `newName` shall not be `const &`.

    // Making sure that the id is lowercase
    idToFind.toLower();

    // The new ID
    Data::ClusterName newID;
    Antares::TransformNameIntoID(newName, newID);

    // Looking for the renewable cluster in the list
    auto it = cluster.find(idToFind);
    if (it == cluster.end())
        return true;

    SharedPtr p = it->second;

    if (idToFind == newID)
    {
        p->setName(newName);
        return true;
    }

    // The name is the same. Aborting nicely.
    if (p->name() == newName)
        return true;

    // Already exist
    if (this->exists(newID))
        return false;

    cluster.erase(it);

    p->setName(newName);
    cluster[newID] = p;

    // Invalidate matrices attached to the area
    // It is a bit excessive (all matrices not only those related to the renewable cluster)
    // will be rewritten but currently it is the less error-prone.
    if (p->parentArea)
        (p->parentArea)->invalidateJIT = true;

    // Rebuilding the index
    rebuildIndex();
    return true;
}

template<class ClusterT>
bool ClusterList<ClusterT>::invalidate(bool reload) const
{
    bool ret = true;
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
        ret = (i->second)->invalidate(reload) and ret;
    return ret;
}

template<class ClusterT>
void ClusterList<ClusterT>::markAsModified() const
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
        (i->second)->markAsModified();
}

template<class ClusterT>
bool ClusterList<ClusterT>::remove(const Data::ClusterName& id)
{
    auto i = cluster.find(id);
    if (i == cluster.end())
        return false;

    // Getting the pointer on the cluster
    SharedPtr c = i->second;

    // Removing it from the list
    cluster.erase(i);
    // Invalidating the parent area
    c->parentArea->invalidate();

    // Rebuilding the index
    rebuildIndex();
    return true;
}


template<class ClusterT>
void ClusterList<ClusterT>::retrieveTotalCapacityAndUnitCount(double& total, uint& unitCount) const
{
    total = 0.;
    unitCount = 0;

    if (not cluster.empty())
    {
        auto end = cluster.cend();
        for (auto i = cluster.cbegin(); i != end; ++i)
        {
            if (not i->second)
                return;

            // Reference to the thermal cluster
            auto& cluster = *(i->second);
            unitCount += cluster.unitCount;
            total += cluster.unitCount * cluster.nominalCapacity;
        }
    }
}

// Force template instantiation
template class ClusterList<ThermalCluster>;
template class ClusterList<RenewableCluster>;

} // namespace Data
} // namespace Antares
