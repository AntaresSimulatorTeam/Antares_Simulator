#include <boost/algorithm/string/case_conv.hpp>
#include <numeric>
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
ClusterT* ClusterList<ClusterT>::find(const Data::ClusterName& id) const
{
    for (const auto& c : cluster)
        if (c->id() == id)
            return c.get();

    return nullptr;
}

template<class ClusterT>
typename std::shared_ptr<ClusterT> ClusterList<ClusterT>::detach(iterator i)
{
    SharedPtr c = *i;
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
    return std::any_of(cluster.begin(), cluster.end(), [&id](const auto& c){
        return c->id() == id;
    });
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
    cluster.clear();
}

template<class ClusterT>
const ClusterT* ClusterList<ClusterT>::find(const ClusterT* p) const
{
    for (const auto& c : cluster)
        if (c.get() == p)
            return c.get();

    return nullptr;
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
    std::sort(cluster.begin(), cluster.end(), [](const auto& a, const auto& b){
        return a->id() < b->id();
    });

    uint indx = 0;
    for (auto& c : cluster)
        c->index = indx++;
}



template<class ClusterT>
typename ClusterList<ClusterT>::SharedPtr ClusterList<ClusterT>::add(
    const ClusterList<ClusterT>::SharedPtr newcluster)
{
    if (!newcluster)
        return nullptr;

    if (exists(newcluster->id()))
        return newcluster;

    cluster.push_back(newcluster);
    ++(groupCount[newcluster->groupId()]);
    rebuildIndex();
    return newcluster;
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
    auto* cluster_ptr = this->find(idToFind);
    if (!cluster_ptr)
        return true;

    if (idToFind == newID)
    {
        cluster_ptr->setName(newName);
        return true;
    }

    // The name is the same. Aborting nicely.
    if (cluster_ptr->name() == newName)
        return true;

    // Already exist
    if (this->exists(newID))
        return false;

    cluster_ptr->setName(newName);

    // Invalidate matrices attached to the area
    // It is a bit excessive (all matrices not only those related to the renewable cluster)
    // will be rewritten but currently it is the less error-prone.
    if (cluster_ptr->parentArea)
        (cluster_ptr->parentArea)->invalidateJIT = true;

    // Rebuilding the index
    rebuildIndex();
    return true;
}

template<class ClusterT>
bool ClusterList<ClusterT>::forceReload(bool reload) const
{
    return std::all_of(cluster.begin(), cluster.end(), [&reload](const auto& c){
        return c->forceReload(reload);
    });

}

template<class ClusterT>
void ClusterList<ClusterT>::markAsModified() const
{
    for (const auto& c : cluster)
        c->markAsModified();
}

template<class ClusterT>
bool ClusterList<ClusterT>::remove(const Data::ClusterName& id)
{
    // using find_if to get an iterator and use erase later
    const auto& it = find_if(cluster.begin(), cluster.end(),
        [&id](const SharedPtr& c) { return c->id() == id; });

    if (it == cluster.end())
        return false;

    // Getting the pointer on the cluster
    SharedPtr c = *it;

    // Removing it from the list
    cluster.erase(it);
    // Invalidating the parent area
    c->parentArea->forceReload();

    // Rebuilding the index
    rebuildIndex();
    return true;
}

template<class ClusterT>
bool ClusterList<ClusterT>::saveDataSeriesToFolder(const AnyString& folder) const
{
    if (empty())
        return true;

    return std::all_of(cluster.begin(), cluster.end(), [&folder](const auto& c){
        return c->saveDataSeriesToFolder(folder);
    });
}

template<class ClusterT>
bool ClusterList<ClusterT>::saveDataSeriesToFolder(const AnyString& folder, const String& msg) const
{
    if (empty())
        return true;

    bool ret = true;
    uint ticks = 0;

    for (const auto& c : cluster)
    {
        logs.info() << msg << "  " << (ticks * 100 / (1 + this->cluster.size()))
            << "% complete";
        ret = c->saveDataSeriesToFolder(folder) && ret;
        ++ticks;
    }
    return ret;
}

template<class ClusterT>
bool ClusterList<ClusterT>::loadDataSeriesFromFolder(Study& s,
                                                    const StudyLoadOptions& options,
                                                    const AnyString& folder)
{
    if (empty())
        return true;

    bool ret = true;

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

    for (const auto& c : cluster)
    {
        unitCount += c->unitCount;
        total += c->unitCount * c->nominalCapacity;
    }
}

template<class ClusterT>
uint ClusterList<ClusterT>::removeDisabledClusters()
{
    // nothing to do if there is no cluster available
    if (empty())
        return 0;

    auto firstClusterToRemove = std::remove_if(cluster.begin(), cluster.end(), [] (auto& c) {
        return !c->enabled;
    });

    cluster.erase(firstClusterToRemove , cluster.end()); // Actually remove the disabled clusters

    rebuildIndex();

    return size();
}
// Force template instantiation
template class ClusterList<ThermalCluster>;
template class ClusterList<RenewableCluster>;

} // namespace Antares::Data

