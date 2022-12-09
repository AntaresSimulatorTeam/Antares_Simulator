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



    // String buffer;
    // buffer.clear() << folder << SEP << "common" << SEP << "capacity";

    // // Init
    // IniFile ini;
    // auto* s = ini.addSection("inter-daily-breakdown");
    // auto* smod = ini.addSection("intra-daily-modulation");
    // auto* sIMB = ini.addSection("inter-monthly-breakdown");
    // auto* sreservoir = ini.addSection("reservoir");
    // auto* sreservoirCapacity = ini.addSection("reservoir capacity");
    // auto* sFollowLoad = ini.addSection("follow load");
    // auto* sUseWater = ini.addSection("use water");
    // auto* sHardBounds = ini.addSection("hard bounds");
    // auto* sInitializeReservoirDate = ini.addSection("initialize reservoir date");
    // auto* sUseHeuristic = ini.addSection("use heuristic");
    // auto* sUseLeeway = ini.addSection("use leeway");
    // auto* sPowerToLevel = ini.addSection("power to level");
    // auto* sLeewayLow = ini.addSection("leeway low");
    // auto* sLeewayUp = ini.addSection("leeway up");
    // auto* spumpingEfficiency = ini.addSection("pumping efficiency");

    // // return status
    // bool ret = true;

    // // Add all alpha values for each area
    // areas.each([&](const Data::Area& area) {
    //     s->add(area.id, area.hydro.interDailyBreakdown);
    //     smod->add(area.id, area.hydro.intraDailyModulation);
    //     sIMB->add(area.id, area.hydro.intermonthlyBreakdown);
    //     sInitializeReservoirDate->add(area.id, area.hydro.initializeReservoirLevelDate);
    //     sLeewayLow->add(area.id, area.hydro.leewayLowerBound);
    //     sLeewayUp->add(area.id, area.hydro.leewayUpperBound);
    //     spumpingEfficiency->add(area.id, area.hydro.pumpingEfficiency);
    //     if (area.hydro.reservoirCapacity > 1e-6)
    //         sreservoirCapacity->add(area.id, area.hydro.reservoirCapacity);
    //     if (area.hydro.reservoirManagement)
    //         sreservoir->add(area.id, true);
    //     if (!area.hydro.followLoadModulations)
    //         sFollowLoad->add(area.id, false);
    //     if (area.hydro.useWaterValue)
    //         sUseWater->add(area.id, true);
    //     if (area.hydro.hardBoundsOnRuleCurves)
    //         sHardBounds->add(area.id, true);
    //     if (!area.hydro.useHeuristicTarget)
    //         sUseHeuristic->add(area.id, false);
    //     if (area.hydro.useLeeway)
    //         sUseLeeway->add(area.id, true);
    //     if (area.hydro.powerToLevel)
    //         sPowerToLevel->add(area.id, true);
    //     // max power
    //     buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "maxpower_"
    //                    << area.id << ".txt";
    //     ret = area.hydro.maxPower.saveToCSVFile(buffer, /*decimal*/ 2) && ret;
    //     // credit modulations
    //     buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
    //                    << "creditmodulations_" << area.id << ".txt";
    //     ret = area.hydro.creditModulation.saveToCSVFile(buffer, /*decimal*/ 2) && ret;
    //     // inflow pattern
    //     buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "inflowPattern_"
    //                    << area.id << ".txt";
    //     ret = area.hydro.inflowPattern.saveToCSVFile(buffer, /*decimal*/ 3) && ret;
    //     // reservoir
    //     buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "reservoir_"
    //                    << area.id << ".txt";
    //     ret = area.hydro.reservoirLevel.saveToCSVFile(buffer, /*decimal*/ 3) && ret;
    //     buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "waterValues_"
    //                    << area.id << ".txt";
    //     ret = area.hydro.waterValues.saveToCSVFile(buffer, /*decimal*/ 2) && ret;
    // });

    // // Write the ini file
    // buffer.clear() << folder << SEP << "hydro.ini";
    // return ini.save(buffer) && ret;









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
        auto* sInitializeReservoirDate = iniinimanagement.addSection("initialize reservoir date");
        auto* sUseHeuristic = inimanagement.addSection("use heuristic");
        auto* sUseLeeway = inimanagement.addSection("use leeway");
        auto* sPowerToLevel = inimanagement.addSection("power to level");
        auto* sLeewayLow = inimanagement.addSection("leeway low");
        auto* sLeewayUp = inimanagement.addSection("leeway up");
        auto* spumpingEfficiency = inimanagement.addSection("pumping efficiency");

        // Browse all clusters
        each([&](const Data::HydroclusterCluster& c) {
            // Adding a section to the inifile
            IniFile::Section* s = inimanagement.addSection(c.name());

            // The section must not be empty
            // This key will be silently ignored the next time
            s->add("name", c.name());

            // if (not c.group().empty())
            //     s->add("group", c.group());
            // if (not c.enabled)
            //     s->add("enabled", "false");
            // if (not Math::Zero(c.unitCount))
            //     s->add("unitCount", c.unitCount);
            // if (not Math::Zero(c.nominalCapacity))
            //     s->add("nominalCapacity", c.nominalCapacity);.

            //todo save management options here:


            //
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


                // allocation
                buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "allocation" <<".ini";
                ret = cluster->allocation.loadFromFile(cluster->id(), buffer) && ret;

                if(cluster->prepro)
                {
                    buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "prepro";
                    ret = cluster->prepro->loadFromFolder(study, cluster->id(), buffer.c_str()) and ret;
                }

                //CR13 todo see reservoirLevel.loadFromCSVFile, note. need to check enabledModeIsChanged, and post processing
                buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "reservoir" <<".txt";
                ret = cluster->reservoirLevel.loadFromCSVFile(buffer, 3, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret;

                buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "waterValues" <<".txt";
                ret = cluster->waterValues.loadFromCSVFile(buffer, 101, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret; 

                //CR13 todo see maxPower.loadFromCSVFile, note. need to check enabledModeIsChanged, and post processing
                buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "maxPower" <<".txt";
                ret = cluster->maxPower.loadFromCSVFile(buffer, 4, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret; 

                buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "creditmodulations" <<".txt";
                ret = cluster->creditModulation.loadFromCSVFile(buffer, 101, 2, Matrix<>::optFixedSize, &study.dataBuffer) && ret; 

                buffer.clear() << folder << SEP << cluster->parentArea->id << SEP << cluster->id() << SEP  << "inflowPattern" <<".txt";
                ret = cluster->inflowPattern.loadFromCSVFile(buffer, 1, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer) && ret;


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
