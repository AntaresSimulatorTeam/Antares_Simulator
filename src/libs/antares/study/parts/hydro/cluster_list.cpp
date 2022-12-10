#include "cluster_list.h"
#include "../../../inifile.h"
#include "../../study.h"
#include "../../area.h"
#include "../parts.h"

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


void HydroclusterClusterList::ensureDataPrepro()
{
    auto end = cluster.end();
    for (auto it = cluster.begin(); it != end; ++it)
    {
        auto c = it->second;
        if (not c->prepro)
            c->prepro = new PreproHydro();
    }
}

#define SEP IO::Separator
bool HydroclusterClusterList::saveToFolder(const AnyString& folder) const
{
    // Make sure the folder is created
    if (IO::Directory::Create(folder))
    {
        Clob buffer;
        bool ret = true;

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
        });
        // Write the ini file
        buffer.clear() << folder << SEP << "list.ini";
        ret = ini.save(buffer) and ret;

        //Hydrocluster data
        each([&](const Data::HydroclusterCluster& c){

            buffer.clear() << folder << SEP << c.id();
            if (IO::Directory::Create(buffer))
            {
                // Allocation
                buffer.clear() << folder << SEP << c.id() << SEP << "allocation" << ".ini";
                ret = c.allocation.saveToFile(buffer) and ret;

                // prepro
                if(c.prepro)
                {
                    buffer.clear() << folder << SEP << c.id() << SEP << "prepro";
                    ret = c.prepro->saveToFolder(c.id(), buffer.c_str()) and ret;
                }


                buffer.clear() << folder << SEP << c.id() << SEP << "reservoir.txt";
                ret = c.reservoirLevel.saveToCSVFile(buffer, 3) and ret;

                buffer.clear() << folder << SEP << c.id() << SEP << "waterValues.txt";
                ret = c.waterValues.saveToCSVFile(buffer, 2) and ret;   

                buffer.clear() << folder << SEP << c.id() << SEP << "maxPower.txt";
                ret = c.maxPower.saveToCSVFile(buffer, 2) and ret;   

                buffer.clear() << folder << SEP << c.id() << SEP << "creditmodulations.txt";
                ret = c.creditModulation.saveToCSVFile(buffer, 2) and ret;                                                
        
                // inflow pattern
                buffer.clear() << folder << SEP << c.id() << SEP << "inflowPattern.txt";
                ret = c.inflowPattern.saveToCSVFile(buffer, 3) and ret;

            }
            else
                ret = 0;
        });

        // Allocate the inifile structure Hydro management options
        IniFile inimanagement;
        auto* s = inimanagement.addSection("inter-daily-breakdown");
        auto* smod = inimanagement.addSection("intra-daily-modulation");
        auto* sIMB = inimanagement.addSection("inter-monthly-breakdown");
        auto* sreservoir = inimanagement.addSection("reservoir");
        auto* sreservoirCapacity = inimanagement.addSection("reservoir capacity");
        auto* sFollowLoad = inimanagement.addSection("follow load");
        auto* sUseWater = inimanagement.addSection("use water");
        auto* sHardBounds = inimanagement.addSection("hard bounds");
        auto* sInitializeReservoirDate = inimanagement.addSection("initialize reservoir date");
        auto* sUseHeuristic = inimanagement.addSection("use heuristic");
        auto* sUseLeeway = inimanagement.addSection("use leeway");
        auto* sPowerToLevel = inimanagement.addSection("power to level");
        auto* sLeewayLow = inimanagement.addSection("leeway low");
        auto* sLeewayUp = inimanagement.addSection("leeway up");
        auto* spumpingEfficiency = inimanagement.addSection("pumping efficiency");

        // Browse all clusters
        each([&](const Data::HydroclusterCluster& cluster) {
            s->add(cluster.id(), cluster.interDailyBreakdown);
            smod->add(cluster.id(), cluster.intraDailyModulation);
            sIMB->add(cluster.id(), cluster.intermonthlyBreakdown);
            sInitializeReservoirDate->add(cluster.id(), cluster.initializeReservoirLevelDate);
            sLeewayLow->add(cluster.id(), cluster.leewayLowerBound);
            sLeewayUp->add(cluster.id(), cluster.leewayUpperBound);
            spumpingEfficiency->add(cluster.id(), cluster.pumpingEfficiency);
            if (cluster.reservoirCapacity > 1e-6)
                sreservoirCapacity->add(cluster.id(), cluster.reservoirCapacity);
            if (cluster.reservoirManagement)
                sreservoir->add(cluster.id(), true);
            if (!cluster.followLoadModulations)
                sFollowLoad->add(cluster.id(), false);
            if (cluster.useWaterValue)
                sUseWater->add(cluster.id(), true);
            if (cluster.hardBoundsOnRuleCurves)
                sHardBounds->add(cluster.id(), true);
            if (!cluster.useHeuristicTarget)
                sUseHeuristic->add(cluster.id(), false);
            if (cluster.useLeeway)
                sUseLeeway->add(cluster.id(), true);
            if (cluster.powerToLevel)
                sPowerToLevel->add(cluster.id(), true);

        });
        // Write the ini file
        buffer.clear() << folder << SEP << "managementoptions.ini";
        ret = inimanagement.save(buffer) and ret;
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
        // allocation
        buffer.clear() << folder << SEP << c.parentArea->id << SEP << c.id() << SEP  << "allocation" <<".ini";
        ret = c.allocation.loadFromFile(c.id(), buffer) && ret;

        if(c.prepro)
        {
            buffer.clear() << folder << SEP << c.parentArea->id << SEP << c.id() << SEP  << "prepro";
            ret = c.prepro->loadFromFolder(study, c.id(), buffer.c_str()) and ret;
        }

        //CR13 todo see reservoirLevel.loadFromCSVFile, note. need to check enabledModeIsChanged, and post processing
        buffer.clear() << folder << SEP << c.parentArea->id << SEP << c.id() << SEP  << "reservoir" <<".txt";
        ret = c.reservoirLevel.loadFromCSVFile(buffer, 3, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret;

        buffer.clear() << folder << SEP << c.parentArea->id << SEP << c.id() << SEP  << "waterValues" <<".txt";
        ret = c.waterValues.loadFromCSVFile(buffer, 101, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret; 

        //CR13 todo see maxPower.loadFromCSVFile, note. need to check enabledModeIsChanged, and post processing
        buffer.clear() << folder << SEP << c.parentArea->id << SEP << c.id() << SEP  << "maxPower" <<".txt";
        ret = c.maxPower.loadFromCSVFile(buffer, 4, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret; 

        buffer.clear() << folder << SEP << c.parentArea->id << SEP << c.id() << SEP  << "creditmodulations" <<".txt";
        ret = c.creditModulation.loadFromCSVFile(buffer, 101, 2, Matrix<>::optFixedSize, &study.dataBuffer) && ret;

        buffer.clear() << folder << SEP << c.parentArea->id << SEP << c.id() << SEP  << "inflowPattern" <<".txt";
        ret = c.inflowPattern.loadFromCSVFile(buffer, 1, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret;

    }

    for (auto it = begin(); it != end(); ++it)
    {
        auto& c = *(it->second);

        //###221210

        IniFile ini;
        if (not ini.open(buffer.clear() << folder << SEP << c.parentArea->id << SEP << "managementoptions.ini"))
        {
            continue;
        }

        IniFile::Section* section;
        IniFile::Property* property;

        if ((section = ini.find("inter-daily-breakdown")))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                        ret = property->value.to<double>(c.interDailyBreakdown) && ret;
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown cluster";
                }
            }
        }

        if (section = ini.find("intra-daily-modulation"))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                    {
                        ret = property->value.to<double>(c.intraDailyModulation) && ret;
                        if (c.intraDailyModulation < 1.)
                        {
                            logs.error()
                            << c.id() << ": Invalid intra-daily modulation. It must be >= 1.0, Got "
                            << c.intraDailyModulation << " (truncated to 1)";
                            c.intraDailyModulation = 1.;
                        }
                    }
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
                }
            }
        }

        if (section = ini.find("reservoir"))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                        ret = property->value.to<bool>(c.reservoirManagement) && ret;
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
                }
            }
        }

        if (section = ini.find("reservoir capacity"))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                    {
                        ret = property->value.to<double>(c.reservoirCapacity) && ret;
                        if (c.reservoirCapacity < 1e-6)
                        {
                            logs.error() << c.id() << ": Invalid reservoir capacity.";
                            c.reservoirCapacity = 0.;
                        }
                    }
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
                }
            }
        }

        // Check on reservoir capacity (has to be done after reservoir management and capacity reading,
        // not before). Some areas reservoir capacities may not be printed in hydro ini file when saving
        // the study, because they are too small (< 1e-6). We cannot have reservoir management = yes and
        // capacity = 0 because of further division by capacity. reservoir management = no and capacity
        // = 0 is possible (no use of capacity further)
        if (c.reservoirCapacity < 1e-3 && c.reservoirManagement)
        {
            logs.error() << c.id() << ": reservoir capacity not defined. Impossible to manage.";
            ret = false && ret;
        }


        if (section = ini.find("inter-monthly-breakdown"))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                    {
                        ret = property->value.to<double>(c.intermonthlyBreakdown) && ret;
                        if (c.intermonthlyBreakdown < 0)
                        {
                            logs.error() << c.id() << ": Invalid intermonthly breakdown";
                            c.intermonthlyBreakdown = 0.;
                        }
                    }
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
                }
            }
        }
        if (section = ini.find("follow load"))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                        ret = property->value.to<bool>(c.followLoadModulations) && ret;
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
                }
            }
        }
        if (section = ini.find("use water"))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                        ret = property->value.to<bool>(c.useWaterValue) && ret;
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
                }
            }
        }
        if (section = ini.find("hard bounds"))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                        ret = property->value.to<bool>(c.hardBoundsOnRuleCurves) && ret;
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
                }
            }
        }
        if (section = ini.find("use heuristic"))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                        ret = property->value.to<bool>(c.useHeuristicTarget) && ret;
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
                }
            }
        }
        if (section = ini.find("power to level"))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                        ret = property->value.to<bool>(c.powerToLevel) && ret;
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
                }
            }
        }
        if (section = ini.find("initialize reservoir date"))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                    {
                        ret = property->value.to<int>(c.initializeReservoirLevelDate) && ret;
                        if (c.initializeReservoirLevelDate < 0)
                        {
                            logs.error() << c.id() << ": Invalid initialize reservoir date";
                            c.initializeReservoirLevelDate = 0;
                        }
                    }
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
                }
            }
        }
        // Leeways : use leeway bounds (upper and lower)
        if (section = ini.find("use leeway"))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                        ret = property->value.to<bool>(c.useLeeway) && ret;
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
                }
            }
        }
        if (section = ini.find("leeway low"))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                    {
                        ret = property->value.to<double>(c.leewayLowerBound) && ret;
                        if (c.leewayLowerBound < 0.)
                        {
                            logs.error()
                            << c.id() << ": Invalid leeway lower bound. It must be >= 0.0, Got "
                            << c.leewayLowerBound;
                            c.leewayLowerBound = 0.;
                        }
                    }
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
                }
            }
        }
        if (section = ini.find("leeway up"))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                    {
                        ret = property->value.to<double>(c.leewayUpperBound) && ret;
                        if (c.leewayUpperBound < 0.)
                        {
                            logs.error()
                            << c.id() << ": Invalid leeway upper bound. It must be >= 0.0, Got "
                            << c.leewayUpperBound;
                            c.leewayUpperBound = 0.;
                        }
                    }
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
                }
            }
        }

        // they are too small (< 1e-6). We cannot allow these areas to have reservoir management =
        // true.
        if (c.leewayLowerBound > c.leewayUpperBound)
            logs.error() << c.id() << ": Leeway lower bound greater than leeway upper bound.";


        if (section = ini.find("pumping efficiency"))
        {
            if ((property = section->firstProperty))
            {
                // Browse all properties
                for (; property; property = property->next)
                {
                    if (c.id().equals(property->key.toLower()))
                    {
                        ret = property->value.to<double>(c.pumpingEfficiency) && ret;
                        if (c.pumpingEfficiency < 0)
                        {
                            logs.error() << c.id() << ": Invalid pumping efficiency";
                            c.pumpingEfficiency = 0.;
                        }
                    }
                    else
                        logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
                }
            }
        }

        if (not c.useHeuristicTarget && not c.useWaterValue)
        {
            logs.error() << c.id()
                        << " : use water value = no conflicts with use heuristic target = no";
            ret = false && ret;
        }

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
                //###221210

                // // allocation
                // buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "allocation" <<".ini";
                // ret = cluster->allocation.loadFromFile(cluster->id(), buffer) && ret;

                // if(cluster->prepro)
                // {
                //     buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "prepro";
                //     ret = cluster->prepro->loadFromFolder(study, cluster->id(), buffer.c_str()) and ret;
                // }

                // //CR13 todo see reservoirLevel.loadFromCSVFile, note. need to check enabledModeIsChanged, and post processing
                // buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "reservoir" <<".txt";
                // ret = cluster->reservoirLevel.loadFromCSVFile(buffer, 3, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret;

                // buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "waterValues" <<".txt";
                // ret = cluster->waterValues.loadFromCSVFile(buffer, 101, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret; 

                // //CR13 todo see maxPower.loadFromCSVFile, note. need to check enabledModeIsChanged, and post processing
                // buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "maxPower" <<".txt";
                // ret = cluster->maxPower.loadFromCSVFile(buffer, 4, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret; 

                // buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "creditmodulations" <<".txt";
                // ret = cluster->creditModulation.loadFromCSVFile(buffer, 101, 2, Matrix<>::optFixedSize, &study.dataBuffer) && ret; 

                // buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "inflowPattern" <<".txt";
                // ret = cluster->inflowPattern.loadFromCSVFile(buffer, 1, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret;


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
