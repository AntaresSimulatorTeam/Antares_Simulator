#include <boost/algorithm/string/case_conv.hpp>
#include <algorithm>
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
    return (uint)clusters.size();
}

template<class ClusterT>
inline bool ClusterList<ClusterT>::empty() const
{
    return clusters.empty();
}

template<class ClusterT>
typename ClusterList<ClusterT>::iterator ClusterList<ClusterT>::begin()
{
    return std::begin(clusters);
}

template<class ClusterT>
typename ClusterList<ClusterT>::const_iterator ClusterList<ClusterT>::begin() const
{
    return std::begin(clusters);
}

template<class ClusterT>
typename ClusterList<ClusterT>::iterator ClusterList<ClusterT>::end()
{
    return std::end(clusters);
}

template<class ClusterT>
typename ClusterList<ClusterT>::const_iterator ClusterList<ClusterT>::end() const
{
    return std::end(clusters);
}

template<class ClusterT>
ClusterT* ClusterList<ClusterT>::find(const Data::ClusterName& id) const
{
    const auto& it = std::ranges::find_if(clusters, [&id](auto& c) { return c->id() == id; });

    return (it != clusters.end()) ? it->get() : nullptr;
}

template<class ClusterT>
bool ClusterList<ClusterT>::exists(const Data::ClusterName& id) const
{
    return std::ranges::any_of(clusters, [&id](const auto& c){ return c->id() == id; });
}

template<class ClusterT>
void ClusterList<ClusterT>::clear()
{
    clusters.clear();
}

template<class ClusterT>
void ClusterList<ClusterT>::resizeAllTimeseriesNumbers(uint n)
{
    each([&](Cluster& cluster) { cluster.series.timeseriesNumbers.reset(1, n); });
}

#define SEP IO::Separator

template<class ClusterT>
void ClusterList<ClusterT>::storeTimeseriesNumbers(Solver::IResultWriter& writer) const
{
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
    std::sort(clusters.begin(), clusters.end(), [](const auto& a, const auto& b){
        return a->id() < b->id();
    });

    uint indx = 0;
    for (auto& c : clusters)
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

    clusters.push_back(newcluster);
    rebuildIndex();
    return newcluster;
}

template<class ClusterT>
uint64_t ClusterList<ClusterT>::memoryUsage() const
{
    uint64_t ret = sizeof(ClusterList) + (2 * sizeof(void*)) * this->size();

    each([&](const ClusterT& clusters) { ret += clusters.memoryUsage(); });
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

    // Looking for the renewable clusters in the list
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
    // It is a bit excessive (all matrices not only those related to the renewable clusters)
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
    return std::ranges::all_of(clusters, [&reload](const auto& c){
        return c->forceReload(reload);
    });

}

template<class ClusterT>
void ClusterList<ClusterT>::markAsModified() const
{
    for (const auto& c : clusters)
        c->markAsModified();
}

template<class ClusterT>
bool ClusterList<ClusterT>::remove(const Data::ClusterName& id)
{
    auto nbDeletion = std::erase_if(clusters, [&id](const SharedPtr& c) { return c->id() == id; });

    // Invalidating the parent area
    forceReload();

    // Rebuilding the index
    rebuildIndex();

    return nbDeletion > 0;
}

template<class ClusterT>
bool ClusterList<ClusterT>::saveDataSeriesToFolder(const AnyString& folder) const
{
    return std::ranges::all_of(clusters, [&folder](const auto& c){
        return c->saveDataSeriesToFolder(folder);
    });
}

template<class ClusterT>
bool ClusterList<ClusterT>::saveDataSeriesToFolder(const AnyString& folder, const String& msg) const
{
    uint ticks = 0;

    return std::ranges::all_of(clusters, [&](const auto& c)
    {
        logs.info() << msg << "  " << (ticks * 100 / (1 + this->clusters.size()))
            << "% complete";
        ++ticks;
        return c->saveDataSeriesToFolder(folder);
    });
}

template<class ClusterT>
bool ClusterList<ClusterT>::loadDataSeriesFromFolder(Study& s,
                                                    const StudyLoadOptions& options,
                                                    const AnyString& folder)
{
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

    for (const auto& c : clusters)
    {
        unitCount += c->unitCount;
        total += c->unitCount * c->nominalCapacity;
    }
}

template<class ClusterT>
uint ClusterList<ClusterT>::removeDisabledClusters()
{
    // nothing to do if there is no clusters available
    if (empty())
        return 0;

    auto count = std::erase_if(clusters, [] (auto& c) { return !c->enabled; });

    rebuildIndex();

    return count;
}
// Force template instantiation
template class ClusterList<ThermalCluster>;
template class ClusterList<RenewableCluster>;

} // namespace Antares::Data

