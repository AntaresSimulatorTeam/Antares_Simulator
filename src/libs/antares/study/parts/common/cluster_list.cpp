#include <boost/algorithm/string/case_conv.hpp>
#include "cluster_list.h"
#include <antares/utils/utils.h>
#include "../../study.h"

using namespace Yuni;
namespace // anonymous
{
struct TSNumbersPredicate
{
    uint32_t operator()(uint32_t value) const
    {
        return value + 1;
    }
};
} // namespace

namespace Antares::Data
{
using namespace Antares;

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
typename std::shared_ptr<ClusterT> ClusterList<ClusterT>::detach(iterator i)
{
    SharedPtr c = i->second;
    cluster.erase(i);
    return c;
}

template<class ClusterT>
void ClusterList<ClusterT>::remove(iterator i)
{
    cluster.erase(i);
}

template<class ClusterT>
bool ClusterList<ClusterT>::exists(const Data::ClusterName& id) const
{
    auto& it = find_if(cluster.begin(), cluster.end(),
        [&id](const ClusterT& c) { return c.id == id; });

    return it != cluster.end();
}

template<class ClusterT>
Data::ClusterList<ClusterT>::ClusterList() : groupCount(ClusterT::groupMax, 0)
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
    byIndex.clear();
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

template<class ClusterT>
typename ClusterList<ClusterT>::SharedPtr ClusterList<ClusterT>::find(
  const ClusterList<ClusterT>::SharedPtr& p)
{
    auto& it = find_if(cluster.begin(), cluster.end(),
        [&id](const ClusterT& c) { return c.id == id; });

    return (it != cluster.end()) ? *it : nullptr;
}

template<class ClusterT>
void ClusterList<ClusterT>::resizeAllTimeseriesNumbers(uint n)
{
    assert(n < 200000); // arbitrary number
    if (not cluster.empty())
    {
        if (0 == n)
        {
            each([&](Cluster& cluster) { cluster.series.timeseriesNumbers.clear(); });
        }
        else
        {
            each([&](Cluster& cluster) { cluster.series.timeseriesNumbers.resize(1, n); });
        }
    }
}

#define SEP IO::Separator

template<class ClusterT>
void ClusterList<ClusterT>::storeTimeseriesNumbers(Solver::IResultWriter& writer) const
{
    if (cluster.empty())
        return;

    TSNumbersPredicate predicate;
    Clob path;
    std::string ts_content;

    each([&](const Cluster& cluster) {
        path.clear() << "ts-numbers" << SEP << typeID() << SEP << cluster.parentArea->id << SEP
                     << cluster.id() << ".txt";
        ts_content.clear(); // We must clear ts_content here, since saveToBuffer does not do it.
        cluster.series.timeseriesNumbers.saveToBuffer(ts_content, 0, true, predicate, true);
        writer.addEntryFromBuffer(path.c_str(), ts_content);
    });
}

template<class ClusterT>
void ClusterList<ClusterT>::rebuildIndex()
{
    byIndex.clear();

    if (not empty())
    {
        uint indx = 0;
        byIndex.resize(size());
        for (auto i = cluster.begin(); i != cluster.end(); ++i)
        {
            auto cluster = i->second.get();
            byIndex[indx] = cluster;
            cluster->index = indx;
            ++indx;
        }
    }
}



template<class ClusterT>
typename ClusterList<ClusterT>::SharedPtr ClusterList<ClusterT>::add(
  const ClusterList<ClusterT>::SharedPtr& newcluster)
{
    if (newcluster)
    {
        if (exists(newcluster->id()))
            return this->find(*newcluster);

        newcluster->index = (uint)size();
        cluster[newcluster->id()] = newcluster;
        ++(groupCount[newcluster->groupId()]);
        rebuildIndex();
        return cluster[newcluster->id()];
    }
    return nullptr;
}

template<class ClusterT>
uint64_t ClusterList<ClusterT>::memoryUsage() const
{
    uint64_t ret = sizeof(ClusterList) + (2 * sizeof(void*)) * this->size();

    each([&](const ClusterT& cluster) { ret += cluster.memoryUsage(); });
    return ret;
}

template<class ClusterT>
bool ClusterList<ClusterT>::rename(Data::ClusterName idToFind, Data::ClusterName newName)
{
    if (idToFind.empty() or newName.empty())
        return false;

    // Internal:
    // It is vital to make copy of these strings. We can not make assumption that these
    // CString are not from the same buffer (name, id) than ours.
    // It may have an undefined behavior.
    // Consequently, the parameters `idToFind` and `newName` shall not be `const &`.

    // Making sure that the id is lowercase
    boost::to_lower(idToFind);

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
bool ClusterList<ClusterT>::forceReload(bool reload) const
{
    bool ret = true;
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
        ret = (i->second)->forceReload(reload) and ret;
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
    c->parentArea->forceReload();

    // Rebuilding the index
    rebuildIndex();
    return true;
}

template<class ClusterT>
int ClusterList<ClusterT>::saveDataSeriesToFolder(const AnyString& folder) const
{
    if (empty())
        return 1;

    int ret = 1;

    auto end = cluster.end();
    for (auto it = cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        ret = cluster.saveDataSeriesToFolder(folder) and ret;
    }
    return ret;
}

template<class ClusterT>
int ClusterList<ClusterT>::saveDataSeriesToFolder(const AnyString& folder, const String& msg) const
{
    if (empty())
        return 1;

    int ret = 1;
    uint ticks = 0;

    auto end = cluster.end();
    for (auto it = cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        logs.info() << msg << "  " << (ticks * 100 / (1 + this->cluster.size()))
            << "% complete";
        ret = cluster.saveDataSeriesToFolder(folder) and ret;
        ++ticks;
    }
    return ret;
}

template<class ClusterT>
int ClusterList<ClusterT>::loadDataSeriesFromFolder(Study& s,
                                                    const StudyLoadOptions& options,
                                                    const AnyString& folder)
{
    if (empty())
        return 1;

    int ret = 1;

    each([&](ClusterT& c) {
        ret = c.loadDataSeriesFromFolder(s, folder) and ret;

        ++options.progressTicks;
        options.pushProgressLogs();
    });
    return ret;
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

} // namespace Antares::Data

