#include "cluster_list.h"
#include "../../memory-usage.h"
#include "../../../logs.h"
#include "../../study.h"

using namespace Yuni;
using namespace Antares;

#define SEP IO::Separator

namespace // anonymous
{
struct TSNumbersPredicate
{
    uint32 operator()(uint32 value) const
    {
        return value + 1;
    }
};
} // namespace

namespace Antares
{
namespace Data
{
RenewableCluster* RenewableClusterList::detach(iterator i)
{
    auto* c = i->second;
    cluster.erase(i);
    return c;
}

void RenewableClusterList::remove(iterator i)
{
    cluster.erase(i);
}

bool RenewableClusterList::exists(const Data::ClusterName& id) const
{
    if (not cluster.empty())
    {
        auto element = cluster.find(id);
        return (element != cluster.end());
    }
    return false;
}
Data::RenewableClusterList::RenewableClusterList() : byIndex(nullptr)
{
    (void)::memset(&groupCount, 0, sizeof(groupCount));
}

Data::RenewableClusterList::~RenewableClusterList()
{
    // deleting all renewable clusters
    clear();
}

void RenewableClusterList::clear()
{
    if (byIndex)
    {
        delete[] byIndex;
        byIndex = nullptr;
    }

    if (not cluster.empty())
        cluster.clear();
}

const RenewableCluster* RenewableClusterList::find(const RenewableCluster* p) const
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        if (p == i->second)
            return i->second;
    }
    return nullptr;
}

Data::RenewableCluster* RenewableClusterList::find(const RenewableCluster* p)
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        if (p == i->second)
            return i->second;
    }
    return nullptr;
}

void RenewableClusterList::resizeAllTimeseriesNumbers(uint n)
{
    assert(n < 200000); // arbitrary number
    if (not cluster.empty())
    {
        if (0 == n)
        {
            each([&](RenewableCluster& cluster) { cluster.series->timeseriesNumbers.clear(); });
        }
        else
        {
            each(
              [&](RenewableCluster& cluster) { cluster.series->timeseriesNumbers.resize(1, n); });
        }
    }
}

void RenewableClusterList::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += (sizeof(void*) * 4 /*overhead map*/) * cluster.size();

    each([&](const RenewableCluster& cluster) {
        u.requiredMemoryForInput += sizeof(RenewableCluster);
        u.requiredMemoryForInput += sizeof(void*);
        if (cluster.series)
            cluster.series->estimateMemoryUsage(u, timeSeriesRenewable);

        // From the solver
        u.requiredMemoryForInput += 70 * 1024;
    });
}

void RenewableClusterList::rebuildIndex()
{
    delete[] byIndex;

    if (not empty())
    {
        uint indx = 0;
        typedef RenewableCluster* RenewableClusterWeakPtr;
        byIndex = new RenewableClusterWeakPtr[size()];

        auto end = cluster.end();
        for (auto i = cluster.begin(); i != end; ++i)
        {
            auto* cluster = i->second;
            byIndex[indx] = cluster;
            cluster->index = indx;
            ++indx;
        }
    }
    else
        byIndex = nullptr;
}

bool RenewableClusterList::add(RenewableCluster* newcluster)
{
    if (newcluster)
    {
        if (exists(newcluster->id()))
            return true;

        newcluster->index = (uint)size();
        cluster[newcluster->id()] = newcluster;
        ++(groupCount[newcluster->groupID]);
        rebuildIndex();
        return true;
    }
    return false;
}

static bool RenewableClusterLoadFromProperty(RenewableCluster& cluster, const IniFile::Property* p)
{
    if (p->key.empty())
        return false;

    if (p->key == "group")
    {
        cluster.group(p->value);
        return true;
    }

    if (p->key == "name")
        return true;

    if (p->key == "enabled")
        return p->value.to<bool>(cluster.enabled);

    // The property is unknown
    return false;
}

static bool RenewableClusterLoadFromSection(const AnyString& filename,
                                            RenewableCluster& cluster,
                                            const IniFile::Section& section)
{
    if (section.name.empty())
        return false;

    cluster.name(section.name);

    if (section.firstProperty)
    {
        // Browse all properties
        for (auto* property = section.firstProperty; property; property = property->next)
        {
            if (property->key.empty())
            {
                logs.warning() << '`' << filename << "`: `" << section.name
                               << "`: Invalid key/value";
                continue;
            }
            if (not RenewableClusterLoadFromProperty(cluster, property))
            {
                logs.warning() << '`' << filename << "`: `" << section.name << "`/`"
                               << property->value << "`: The property is unknown and ignored";
            }
        }
        // update the minUpDownTime
    }
    return true;
}

bool RenewableClusterList::loadFromFolder(Study& study, const AnyString& folder, Area* area)
{
    assert(area and "A parent area is required");

    // logs
    logs.info() << "Loading renewable configuration for the area " << area->name;

    // Open the ini file
    study.buffer.clear() << folder << SEP << "list.ini";
    IniFile ini;
    if (ini.open(study.buffer))
    {
        bool ret = true;

        if (ini.firstSection)
        {
            for (auto* section = ini.firstSection; section; section = section->next)
            {
                if (section->name.empty())
                    continue;

                auto* cluster = new RenewableCluster(area, study.maxNbYearsInParallel);

                // Load data of a renewable cluster from a ini file section
                if (not RenewableClusterLoadFromSection(study.buffer, *cluster, *section))
                {
                    delete cluster;
                    continue;
                }

                // Check the data integrity of the cluster
                cluster->integrityCheck();

                // adding the renewable cluster
                if (not add(cluster))
                {
                    // This error should never happen
                    logs.error() << "Impossible to add the renewable cluster '" << cluster->name()
                                 << "'";
                    delete cluster;
                    continue;
                }

                cluster->flush();
            }
        }

        return ret;
    }
    return false;
}

Yuni::uint64 RenewableClusterList::memoryUsage() const
{
    uint64 ret = sizeof(RenewableClusterList) + (2 * sizeof(void*)) * this->size();

    each([&](const Data::RenewableCluster& cluster) { ret += cluster.memoryUsage(); });
    return ret;
}

bool RenewableClusterList::rename(Data::ClusterName idToFind, Data::ClusterName newName)
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
    TransformNameIntoID(newName, newID);

    // Looking for the renewable cluster in the list
    auto it = cluster.find(idToFind);
    if (it == cluster.end())
        return true;

    Data::RenewableCluster* p = it->second;

    if (idToFind == newID)
    {
        p->name(newName);
        return true;
    }

    // The name is the same. Aborting nicely.
    if (p->name() == newName)
        return true;

    // Already exist
    if (this->exists(newID))
        return false;

    cluster.erase(it);

    p->name(newName);
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

bool Data::RenewableClusterList::invalidate(bool reload) const
{
    bool ret = true;
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
        ret = (i->second)->invalidate(reload) and ret;
    return ret;
}

void Data::RenewableClusterList::markAsModified() const
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
        (i->second)->markAsModified();
}

bool RenewableClusterList::storeTimeseriesNumbers(Study& study)
{
    if (cluster.empty())
        return true;

    bool ret = true;
    TSNumbersPredicate predicate;

    each([&](const Data::RenewableCluster& cluster) {
        study.buffer = study.folderOutput;
        study.buffer << SEP << "ts-numbers" << SEP << "renewable" << SEP << cluster.parentArea->id
                     << SEP << cluster.id() << ".txt";
        ret = cluster.series->timeseriesNumbers.saveToCSVFile(study.buffer, 0, true, predicate)
              and ret;
    });
    return ret;
}

void RenewableClusterList::retrieveTotalCapacity(double& total) const
{
    total = 0.;

    if (not cluster.empty())
    {
        auto end = cluster.cend();
        for (auto i = cluster.cbegin(); i != end; ++i)
        {
            if (not i->second)
                return;

            // Reference to the renewable cluster
            auto& cluster = *(i->second);
            total += cluster.nominalCapacity;
        }
    }
}

bool RenewableClusterList::remove(const Data::ClusterName& id)
{
    auto i = cluster.find(id);
    if (i == cluster.end())
        return false;

    // Getting the pointer on the cluster
    auto* c = i->second;

    // Removing it from the list
    cluster.erase(i);
    // Invalidating the parent area
    c->parentArea->invalidate();

    // delete the cluster
    delete c;

    // Rebuilding the index
    rebuildIndex();
    return true;
}

} // namespace Data
} // namespace Antares
