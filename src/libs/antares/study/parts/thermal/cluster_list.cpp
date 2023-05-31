#include "cluster_list.h"
#include "cluster.h"
#include "../../study.h"

namespace // anonymous
{
struct TSNumbersPredicate
{
    Yuni::uint32 operator()(Yuni::uint32 value) const
    {
        return value + 1;
    }
};

} // anonymous namespace

namespace Antares
{
namespace Data
{
using namespace Yuni;

ThermalClusterList::ThermalClusterList()
{
}

ThermalClusterList::~ThermalClusterList()
{
    // deleting all thermal clusters
    clear();
}

void ThermalClusterList::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += (sizeof(void*) * 4 /*overhead map*/) * cluster.size();

    each([&](const ThermalCluster& cluster) {
        u.requiredMemoryForInput += sizeof(ThermalCluster);
        u.requiredMemoryForInput += sizeof(void*);
        u.requiredMemoryForInput += sizeof(double) * HOURS_PER_YEAR; // productionCost
        u.requiredMemoryForInput += sizeof(double) * HOURS_PER_YEAR; // PthetaInf
        u.requiredMemoryForInput += sizeof(double) * HOURS_PER_YEAR; // dispatchedUnitsCount
        cluster.modulation.estimateMemoryUsage(u, true, thermalModulationMax, HOURS_PER_YEAR);

        if (cluster.series)
            cluster.series->estimateMemoryUsage(u, timeSeriesThermal);
        if (cluster.prepro)
            cluster.prepro->estimateMemoryUsage(u);

        // From the solver
        u.requiredMemoryForInput += 70 * 1024;
    });
}

#define SEP IO::Separator

static bool ThermalClusterLoadFromSection(const AnyString& filename,
                                          ThermalCluster& cluster,
                                          const IniFile::Section& section);

bool ThermalClusterList::loadFromFolder(Study& study, const AnyString& folder, Area* area)
{
    assert(area and "A parent area is required");

    // logs
    logs.info() << "Loading thermal configuration for the area " << area->name;

    // Open the ini file
    study.buffer.clear() << folder << SEP << "list.ini";
    IniFile ini;
    if (!ini.open(study.buffer))
        return false;

    bool ret = true;

    if (!ini.firstSection)
        return ret;

    String modulationFile;

    for (auto* section = ini.firstSection; section; section = section->next)
    {
        if (section->name.empty())
            continue;

        auto cluster = std::make_shared<ThermalCluster>(area);

        // Load data of a thermal cluster from a ini file section
        if (not ThermalClusterLoadFromSection(study.buffer, *cluster, *section))
        {
            continue;
        }

        // Keeping the current value of 'mustrun' somewhere else
        cluster->mustrunOrigin = cluster->mustrun;

        // MBO 15/04/2014
        // new rounding scheme starting version 450
        // if abs(value) < 1.e-3 => 0 ; if abs(value) > 5.e-4 => 5.e-4
        // applies to
        //	- Market Bid cost
        //	- Marginal cost
        //	- Spread cost
        //	- Fixed cost
        //	- Startup cost
        // MBO 23/12/2015
        // v5.0 format
        // allow startup cost between [-5 000 000 ;-5 000 000] (was [-50 000;50 000])

        // Modulation
        modulationFile.clear() << folder << SEP << ".." << SEP << ".." << SEP << "prepro"
                               << SEP << cluster->parentArea->id << SEP << cluster->id()
                               << SEP << "modulation." << study.inputExtension;


        enum
        {
            options = Matrix<>::optFixedSize,
        };
        bool r = cluster->modulation.loadFromCSVFile(
                modulationFile, thermalModulationMax, HOURS_PER_YEAR, options);
        if (not r and study.usedByTheSolver)
        {
            cluster->modulation.reset(thermalModulationMax, HOURS_PER_YEAR);
            cluster->modulation.fill(1.);
            cluster->modulation.fillColumn(thermalMinGenModulation, 0.);
        }
        ret = ret and r;

        // Special operations when not ran from the interface (aka solver)
        if (study.usedByTheSolver)
        {
            if (not cluster->productionCost)
                cluster->productionCost = new double[HOURS_PER_YEAR];

            // alias to the production cost
            double* prodCost = cluster->productionCost;
            // alias to the marginal cost
            double marginalCost = cluster->marginalCost;
            // Production cost
            auto& modulation = cluster->modulation[thermalModulationCost];
            for (uint h = 0; h != cluster->modulation.height; ++h)
                prodCost[h] = marginalCost * modulation[h];

            if (not study.parameters.include.thermal.minStablePower)
                cluster->minStablePower = 0.;
            if (not study.parameters.include.thermal.minUPTime)
            {
                cluster->minUpDownTime = 1;
                cluster->minUpTime = 1;
                cluster->minDownTime = 1;
            }
            else
                cluster->minUpDownTime
                  = Math::Max(cluster->minUpTime, cluster->minDownTime);

            if (not study.parameters.include.reserve.spinning)
                cluster->spinning = 0;

            cluster->nominalCapacityWithSpinning = cluster->nominalCapacity;
        }

        // Check the data integrity of the cluster
        cluster->integrityCheck();

        // adding the thermal cluster
        auto added = add(cluster);
        if (not added)
        {
            // This error should never happen
            logs.error() << "Impossible to add the thermal cluster '" << cluster->name()
                         << "'";
            continue;
        }
        // keeping track of the cluster
        mapping[cluster->id()] = added;
    }

    return ret;
}

YString ThermalClusterList::typeID() const
{
    return "thermal";
}

static bool ThermalClusterLoadFromProperty(ThermalCluster& cluster, const IniFile::Property* p)
{
    if (p->key.empty())
        return false;

    if (p->key == "annuityinvestment")
        return p->value.to<uint>(cluster.annuityInvestment);

    if (p->key == "enabled")
        return p->value.to<bool>(cluster.enabled);
    if (p->key == "fixed-cost")
        return p->value.to<double>(cluster.fixedCost);

    if (p->key == "groupmincount")
        return p->value.to<uint>(cluster.groupMinCount);
    if (p->key == "groupmaxcount")
        return p->value.to<uint>(cluster.groupMaxCount);
    if (p->key == "group")
    {
        cluster.setGroup(p->value);
        return true;
    }
    if (p->key == "gen-ts")
    {
        return p->value.to(cluster.tsGenBehavior);
    }
    if (p->key == "law.planned")
        return p->value.to(cluster.plannedLaw);
    if (p->key == "law.forced")
        return p->value.to(cluster.forcedLaw);
    if (p->key == "market-bid-cost")
        return p->value.to<double>(cluster.marketBidCost);
    if (p->key == "marginal-cost")
        return p->value.to<double>(cluster.marginalCost);
    if (p->key == "must-run")
        // mustrunOrigin will be initialized later, after LoadFromSection
        return p->value.to<bool>(cluster.mustrun);
    if (p->key == "min-stable-power")
        return p->value.to<double>(cluster.minStablePower);

    if (p->key == "min-up-time")
    {
        if (p->value.to<uint>(cluster.minUpTime))
        {
            if (cluster.minUpTime < 1)
                cluster.minUpTime = 1;
            if (cluster.minUpTime > 168)
                cluster.minUpTime = 168;
            return true;
        }
        return false;
    }
    if (p->key == "min-down-time")
    {
        if (p->value.to<uint>(cluster.minDownTime))
        {
            if (cluster.minDownTime < 1)
                cluster.minDownTime = 1;
            if (cluster.minDownTime > 168)
                cluster.minDownTime = 168;
            return true;
        }
        return false;
    }
    if (p->key == "name")
        return true; // silently ignore it
    if (p->key == "nominalcapacity")
        return p->value.to<double>(cluster.nominalCapacity);

    if (p->key == "spread-cost")
        return p->value.to<double>(cluster.spreadCost);
    if (p->key == "spinning")
        return p->value.to<double>(cluster.spinning);
    if (p->key == "startup-cost")
        return p->value.to<double>(cluster.startupCost);

    if (p->key == "unitcount")
        return p->value.to<uint>(cluster.unitCount);
    if (p->key == "volatility.planned")
        return p->value.to(cluster.plannedVolatility);
    if (p->key == "volatility.forced")
        return p->value.to(cluster.forcedVolatility);

    //pollutant
    if (auto it = Pollutant::namesToEnum.find(p->key.c_str()); it != Pollutant::namesToEnum.end())
        return p->value.to<double> (cluster.emissions.factors[it->second]);

    // The property is unknown
    return false;
}

bool ThermalClusterLoadFromSection(const AnyString& filename,
                                   ThermalCluster& cluster,
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
            if (!ThermalClusterLoadFromProperty(cluster, property))
            {
                logs.warning() << '`' << filename << "`: `" << section.name << "`/`"
                               << property->key << "`: The property is unknown and ignored";
            }
        }
        // update the minUpDownTime
        cluster.minUpDownTime = Math::Max(cluster.minUpTime, cluster.minDownTime);
    }
    return true;
}

void ThermalClusterList::calculationOfSpinning()
{
    each([&](ThermalCluster& cluster) { cluster.calculationOfSpinning(); });
}

void ThermalClusterList::reverseCalculationOfSpinning()
{
    auto end = cluster.end();
    for (auto it = cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        cluster.reverseCalculationOfSpinning();
    }
}

bool ThermalClusterList::remove(const ClusterName& id)
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

void ThermalClusterList::enableMustrunForEveryone()
{
    // enabling the mustrun mode
    each([&](ThermalCluster& cluster) { cluster.mustrun = true; });
}

void ThermalClusterList::ensureDataPrepro()
{
    auto end = cluster.end();
    for (auto it = cluster.begin(); it != end; ++it)
    {
        auto c = it->second;
        if (not c->prepro)
            c->prepro = new PreproThermal(c);
    }
}

bool ThermalClusterList::saveToFolder(const AnyString& folder) const
{
    // Make sure the folder is created
    if (IO::Directory::Create(folder))
    {
        Clob buffer;
        bool ret = true;

        // Allocate the inifile structure
        IniFile ini;

        // Browse all clusters
        each([&](const Data::ThermalCluster& c) {
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
            // TS generation
            if (c.tsGenBehavior != LocalTSGenerationBehavior::useGlobalParameter)
            {
                s->add("gen-ts", c.tsGenBehavior);
            }
            // Min. Stable Power
            if (not Math::Zero(c.minStablePower))
                s->add("min-stable-power", c.minStablePower);

            // Min up and min down time
            if (c.minUpTime != 1)
                s->add("min-up-time", c.minUpTime);
            if (c.minDownTime != 1)
                s->add("min-down-time", c.minDownTime);

            // must-run
            if (c.mustrun)
                s->add("must-run", "true");

            // spinning
            if (not Math::Zero(c.spinning))
                s->add("spinning", c.spinning);

            // volatility
            if (not Math::Zero(c.forcedVolatility))
                s->add("volatility.forced", Math::Round(c.forcedVolatility, 3));
            if (not Math::Zero(c.plannedVolatility))
                s->add("volatility.planned", Math::Round(c.plannedVolatility, 3));

            // laws
            if (c.forcedLaw != thermalLawUniform)
                s->add("law.forced", c.forcedLaw);
            if (c.plannedLaw != thermalLawUniform)
                s->add("law.planned", c.plannedLaw);

            // costs
            if (not Math::Zero(c.marginalCost))
                s->add("marginal-cost", Math::Round(c.marginalCost, 3));
            if (not Math::Zero(c.spreadCost))
                s->add("spread-cost", c.spreadCost);
            if (not Math::Zero(c.fixedCost))
                s->add("fixed-cost", Math::Round(c.fixedCost, 3));
            if (not Math::Zero(c.startupCost))
                s->add("startup-cost", Math::Round(c.startupCost, 3));
            if (not Math::Zero(c.marketBidCost))
                s->add("market-bid-cost", Math::Round(c.marketBidCost, 3));

            // groun{min,max}
            if (not Math::Zero(c.groupMinCount))
                s->add("groupMinCount", c.groupMinCount);
            if (not Math::Zero(c.groupMaxCount))
                s->add("groupMaxCount", c.groupMaxCount);
            if (not Math::Zero(c.annuityInvestment))
                s->add("annuityInvestment", c.annuityInvestment);

            //pollutant factor
            for (auto const& [key, val] : Pollutant::namesToEnum)
                s->add(key, c.emissions.factors[val]);


            buffer.clear() << folder << SEP << ".." << SEP << ".." << SEP << "prepro" << SEP
                           << c.parentArea->id << SEP << c.id();
            if (IO::Directory::Create(buffer))
            {
                buffer.clear() << folder << SEP << ".." << SEP << ".." << SEP << "prepro" << SEP
                               << c.parentArea->id << SEP << c.id() << SEP << "modulation.txt";

                ret = c.modulation.saveToCSVFile(buffer) and ret;
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

bool ThermalClusterList::savePreproToFolder(const AnyString& folder) const
{
    if (empty())
        return true;

    Clob buffer;
    bool ret = true;

    each([&](const Data::ThermalCluster& c) {
        if (c.prepro)
        {
            assert(c.parentArea and "cluster: invalid parent area");
            buffer.clear() << folder << SEP << c.parentArea->id << SEP << c.id();
            ret = c.prepro->saveToFolder(buffer) and ret;
        }
    });
    return ret;
}

bool ThermalClusterList::loadPreproFromFolder(Study& study,
                                              const StudyLoadOptions& options,
                                              const AnyString& folder)
{
    if (empty())
        return true;

    const bool globalThermalTSgeneration
      = study.parameters.timeSeriesToGenerate & timeSeriesThermal;

    Clob buffer;
    bool ret = true;

    for (auto it = begin(); it != end(); ++it)
    {
        auto& c = *(it->second);
        if (c.prepro)
        {
            assert(c.parentArea and "cluster: invalid parent area");
            buffer.clear() << folder << SEP << c.parentArea->id << SEP << c.id();

            bool result = c.prepro->loadFromFolder(study, buffer);

            if (result && study.usedByTheSolver && c.doWeGenerateTS(globalThermalTSgeneration))
            {
                // checking NPO max
                result = c.prepro->normalizeAndCheckNPO();
            }

            ret = result and ret;
        }
        ++options.progressTicks;
        options.pushProgressLogs();
    }
    return ret;
}

} // namespace Data
} // namespace Antares
