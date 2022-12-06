#include "cluster_list.h"
#include "../../../inifile.h"
#include "../../study.h"
#include "../../area.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
HydroclusterClusterList::~HydroclusterClusterList()
{
}

HydroclusterClusterList::HydroclusterClusterList()
{
}

YString HydroclusterClusterList::typeID() const
{
    return "hydrocluster";
}

void HydroclusterClusterList::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += (sizeof(void*) * 4 /*overhead map*/) * cluster.size();

    each([&](const Cluster& cluster) {
        u.requiredMemoryForInput += sizeof(HydroclusterCluster);
        u.requiredMemoryForInput += sizeof(void*);
        if (cluster.series)
            cluster.series->estimateMemoryUsage(u, timeSeriesHydro /* FIXME */); //cr13
            // cluster.series->estimateMemoryUsage(u, timeSeriesHydrocluster /* FIXME */); //cr13

        // From the solver
        u.requiredMemoryForInput += 70 * 1024;
    });
}

#define SEP IO::Separator


bool HydroclusterClusterList::saveToFolder(const AnyString& folder) const
{

    // Make sure the folder is created
    if (IO::Directory::Create(folder))
    {
        Clob buffer;
        bool ret = true;
        bool hasCoupling = false;

        // Allocate the inifile structure
        IniFile ini;

        // Browse all clusters
        each([&](const Data::HydroclusterCluster& c) {


            // Adding a section to the inifile
            IniFile::Section* s = ini.addSection(c.name());

            // The section must not be empty
            // This key will be silently ignored the next time
            s->add("name", c.name());

            if (not c.group().empty())
                s->add("group", c.group());
            if (not c.enabled)
                s->add("enabled", "false");
            if (not Math::Zero(c.unitCount))
                s->add("unitCount", c.unitCount);
            if (not Math::Zero(c.nominalCapacity))
                s->add("nominalCapacity", c.nominalCapacity);
            // // costs
            // if (c.costgeneration != setManually)
            //     s->add("costgeneration", c.costgeneration);            
            // if (not Math::Zero(c.marginalCost))
            //     s->add("marginal-cost", Math::Round(c.marginalCost, 3));



            buffer.clear() << folder << SEP << c.id();
            if (IO::Directory::Create(buffer))
            {
                buffer.clear() << folder << SEP << c.id() << SEP << "reservoir.txt";
                ret = c.reservoirLevel.saveToCSVFile(buffer) and ret;

                buffer.clear() << folder << SEP << c.id() << SEP << "waterValues.txt";
                ret = c.waterValues.saveToCSVFile(buffer) and ret;                
            }
            else
                ret = 0;
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

static bool ClusterLoadFromProperty(HydroclusterCluster& cluster, const IniFile::Property* p) //CF13
{
    // if (p->key.empty())
    //     return false;

    // if (p->key == "group")
    // {
    //     cluster.setGroup(p->value);
    //     return true;
    // }

    // if (p->key == "name")
    //     return true;

    // if (p->key == "enabled")
    //     return p->value.to<bool>(cluster.enabled);

    // if (p->key == "unitcount")
    //     return p->value.to<uint>(cluster.unitCount);

    // if (p->key == "nominalcapacity")
    //     return p->value.to<double>(cluster.nominalCapacity);

    // The property is unknown
    return false;
}

static bool ClusterLoadFromSection(const AnyString& filename,
                                   HydroclusterCluster& cluster,
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
            // if (not ClusterLoadFromProperty(cluster, property))
            // {
            //     logs.warning() << '`' << filename << "`: `" << section.name << "`/`"
            //                    << property->key << "`: The property is unknown and ignored";
            // }
        }
        // update the minUpDownTime
    }
    return true;
}


bool HydroclusterClusterList::loadHydroclusterClusterDataFromFolder(Study& study,
                                              const StudyLoadOptions& options,
                                              const AnyString& folder)
{
    if (empty())
        return true;


    Clob buffer;
    bool ret = true;

    for (auto it = begin(); it != end(); ++it)
    {
        auto& c = *(it->second);

        buffer.clear() << folder << SEP << c.parentArea->id << SEP << c.id() << SEP  << "reservoir" <<".txt";
        ret = c.reservoirLevel.loadFromCSVFile(buffer, 3, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret;

        buffer.clear() << folder << SEP << c.parentArea->id << SEP << c.id() << SEP  << "waterValues" <<".txt";
        ret = c.waterValues.loadFromCSVFile(buffer, 101, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret; 

    }
    return ret;
}


bool HydroclusterClusterList::loadFromFolder(Study& study, const AnyString& folder, Area* area)
{
    assert(area and "A parent area is required");

    // logs
    logs.info() << "Loading hydrocluster configuration for the area " << area->name;

    // Open the ini file
    YString buffer;
    buffer << folder << SEP << "list.ini";

    IniFile ini;
    if (ini.open(buffer, false))
    {
        bool ret = true;

        if (ini.firstSection)
        {
            for (auto* section = ini.firstSection; section; section = section->next)
            {
                if (section->name.empty())
                    continue;

                auto cluster = std::make_shared<HydroclusterCluster>(area);

                // Load data of a hydrocluster cluster from a ini file section
                if (not ClusterLoadFromSection(buffer, *cluster, *section))
                {
                    continue;
                }


                buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "reservoir" <<".txt";
                ret = cluster->reservoirLevel.loadFromCSVFile(buffer, 3, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret;

                buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "waterValues" <<".txt";
                ret = cluster->waterValues.loadFromCSVFile(buffer, 101, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret; 



                // Check the data integrity of the cluster
                cluster->integrityCheck();

                // adding the hydrocluster cluster
                if (not add(cluster))
                {
                    // This error should never happen
                    logs.error() << "Impossible to add the hydrocluster cluster '" << cluster->name()
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
