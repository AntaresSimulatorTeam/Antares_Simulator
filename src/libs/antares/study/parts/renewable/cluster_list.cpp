#include "cluster_list.h"
#include "../../../inifile.h"
#include "../../study.h"
#include "../../area.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
RenewableClusterList::~RenewableClusterList()
{
}

RenewableClusterList::RenewableClusterList()
{
}

YString RenewableClusterList::typeID() const
{
    return "renewables";
}

void RenewableClusterList::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += (sizeof(void*) * 4 /*overhead map*/) * cluster.size();

    each([&](const Cluster& cluster) {
        u.requiredMemoryForInput += sizeof(RenewableCluster);
        u.requiredMemoryForInput += sizeof(void*);
        if (cluster.series)
            cluster.series->estimateMemoryUsage(u, timeSeriesRenewable /* FIXME */);

        // From the solver
        u.requiredMemoryForInput += 70 * 1024;
    });
}

#define SEP IO::Separator

bool RenewableClusterList::saveToFolder(const AnyString& folder) const
{
    // Make sure the folder is created
    if (IO::Directory::Create(folder))
    {
        Clob buffer;
        bool ret = true;

        // Allocate the inifile structure
        IniFile ini;

        // Browse all clusters
        each([&](const Data::RenewableCluster& c) {
            // Adding a section to the inifile
            IniFile::Section* s = ini.addSection(c.name());

            // The section must not be empty
            // This key will be silently ignored the next time
            s->add("name", c.name());

            if (not c.group().empty())
                s->add("group", c.group());
            if (not c.enabled)
                s->add("enabled", "false");

            if (not Math::Zero(c.nominalCapacity))
                s->add("nominalCapacity", c.nominalCapacity);

            if (not Math::Zero(c.unitCount))
                s->add("unitCount", c.unitCount);

            s->add("ts-interpretation", c.getTimeSeriesModeAsString());
        });

        // Write the ini file
        buffer.clear() << folder << SEP << "list.ini";
        ret = ini.save(buffer) and ret;
    }
    else
    {
        logs.error() << "I/O Error: impossible to create '" << folder << "'";
        return false;
    }

    return true;
}

static bool ClusterLoadFromProperty(RenewableCluster& cluster, const IniFile::Property* p)
{
    if (p->key.empty())
        return false;

    if (p->key == "group")
    {
        cluster.setGroup(p->value);
        return true;
    }

    if (p->key == "name")
        return true;

    if (p->key == "enabled")
        return p->value.to<bool>(cluster.enabled);

    if (p->key == "unitcount")
        return p->value.to<uint>(cluster.unitCount);

    if (p->key == "nominalcapacity")
        return p->value.to<double>(cluster.nominalCapacity);

    if (p->key == "ts-interpretation")
        return cluster.setTimeSeriesModeFromString(p->value);

    // The property is unknown
    return false;
}

static bool ClusterLoadFromSection(const AnyString& filename,
                                   RenewableCluster& cluster,
                                   const IniFile::Section& section)
{
    if (section.name.empty())
        return false;

    cluster.setName(section.name);

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
            if (not ClusterLoadFromProperty(cluster, property))
            {
                logs.warning() << '`' << filename << "`: `" << section.name << "`/`"
                               << property->key << "`: The property is unknown and ignored";
            }
        }
        // update the minUpDownTime
    }
    return true;
}

bool RenewableClusterList::loadFromFolder(const AnyString& folder, Area* area)
{
    assert(area and "A parent area is required");

    // logs
    logs.info() << "Loading renewable configuration for the area " << area->name;

    // Open the ini file
    YString buffer;
    buffer << folder << SEP << "list.ini";

    IniFile ini;
    if (ini.open(buffer))
    {
        bool ret = true;

        if (ini.firstSection)
        {
            for (auto* section = ini.firstSection; section; section = section->next)
            {
                if (section->name.empty())
                    continue;

                auto cluster = std::make_shared<RenewableCluster>(area);

                // Load data of a renewable cluster from a ini file section
                if (not ClusterLoadFromSection(buffer, *cluster, *section))
                {
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
                    continue;
                }

                cluster->flush();
            }
        }

        return ret;
    }
    return false;
}

#undef SEP

} // namespace Data
} // namespace Antares
