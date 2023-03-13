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

static bool ThermalClusterLoadCouplingSection(const AnyString& filename,
                                              ThermalClusterList& list,
                                              const IniFile::Section* s);

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
    if (ini.open(study.buffer))
    {
        bool ret = true;

        if (ini.firstSection)
        {
            String modulationFile;

            for (auto* section = ini.firstSection; section; section = section->next)
            {
                if (section->name.empty())
                    continue;

                if (section->name == "~_-_coupling_-_~")
                {
                    ThermalClusterLoadCouplingSection(study.buffer, *this, section);

                    // ignoring all other sections
                    section = section->next;
                    for (; section; section = section->next)
                        logs.warning() << "Ignoring the section " << section->name;
                    break;
                }

                auto cluster = std::make_shared<ThermalCluster>(area, study.maxNbYearsInParallel);

                // Load data of a thermal cluster from a ini file section
                if (not ThermalClusterLoadFromSection(study.buffer, *cluster, *section))
                {
                    continue;
                }

                if (study.header.version < 390)
                {
                    // We may have some strange name/id in older studies
                    // temporary reverting to the old naming convention
                    cluster->pID = cluster->name();
                    cluster->pID.toLower();
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

                if (study.header.version <= 500)
                {
                    // cluster->marketBidCost = Math::Round(cluster->marketBidCost, 3);

                    // Market bid cost
                    if (Math::Abs(cluster->marketBidCost) < 5.e-3)
                        cluster->marketBidCost = 0.;
                    else
                    {
                        if (Math::Abs(cluster->marketBidCost) > 5.e4)
                            (cluster->marketBidCost > 0) ? cluster->marketBidCost = 5.e4
                                                         : cluster->marketBidCost = -5.e4;
                        else
                            cluster->marketBidCost = Math::Round(cluster->marketBidCost, 3);
                    }

                    // Marginal cost
                    if (Math::Abs(cluster->marginalCost) < 5.e-3)
                        cluster->marginalCost = 0.;
                    else
                    {
                        if (Math::Abs(cluster->marginalCost) > 5.e4)
                            (cluster->marginalCost > 0) ? cluster->marginalCost = 5.e4
                                                        : cluster->marginalCost = -5.e4;
                        else
                            cluster->marginalCost = Math::Round(cluster->marginalCost, 3);
                    }

                    // Spread cost - no negative values, must be 0 or >= 0.005
                    if (cluster->spreadCost < 5.e-3)
                        cluster->spreadCost = 0.;
                    else
                    {
                        if (cluster->spreadCost > 5.e4)
                            cluster->spreadCost = 5.e4;
                        else
                            cluster->spreadCost = Math::Round(cluster->spreadCost, 3);
                    }

                    // Fixed cost
                    if (Math::Abs(cluster->fixedCost) < 5.e-3)
                        cluster->fixedCost = 0.;
                    else
                    {
                        if (Math::Abs(cluster->fixedCost) > 5.e4)
                            (cluster->fixedCost > 0) ? cluster->fixedCost = 5.e4
                                                     : cluster->fixedCost = -5.e4;
                        else
                            cluster->fixedCost = Math::Round(cluster->fixedCost, 3);
                    }

                    // Startup cost
                    if (Math::Abs(cluster->startupCost) < 5.e-3)
                        cluster->startupCost = 0.;
                    else
                    {
                        if (Math::Abs(cluster->startupCost) > 5.e6)
                            (cluster->startupCost > 0) ? cluster->startupCost = 5.e6
                                                       : cluster->startupCost = -5.e6;
                        else
                            cluster->startupCost = Math::Round(cluster->startupCost, 3);
                    }

                    // Before v3.5, the marginal cost and the market bid cost were the same
                    // (and was named 'operatingCost')
                    // Rounding to 3 decimal places
                    if (study.header.version < 350)
                        cluster->marginalCost = cluster->marketBidCost;

                    //	if (not Math::Zero(it->weeklyMinimumCapacity))
                    //		it->minUpDownTime = 168;
                    //	else
                    //	{
                    //		if (not Math::Zero(it->dailyMinimumCapacity))
                    //			it->minUpDownTime = 24;
                    //		else
                    //			it->minUpDownTime = 1;
                    //	}
                    //	it->minStablePower =
                    //		Math::Max(it->hourlyMinimumCapacity,
                    //			Math::Max(it->dailyMinimumCapacity,
                    // it->weeklyMinimumCapacity));
                }
                // Backward compatibility
                // switch (it->minUpDownTime)
                // {
                //	case 1:
                //		it->hourlyMinimumCapacity = it->minStablePower;
                //		it->dailyMinimumCapacity  = 0;
                //		it->weeklyMinimumCapacity = 0;
                //		break;
                //	case 24:
                //		it->hourlyMinimumCapacity = 0;//it->minStablePower;
                //		it->dailyMinimumCapacity  = it->minStablePower;
                //		it->weeklyMinimumCapacity = 0;
                //		break;
                //	case 168:
                //		it->hourlyMinimumCapacity = 0;//it->minStablePower;
                //		it->dailyMinimumCapacity  = 0;//it->minStablePower;
                //		it->weeklyMinimumCapacity = it->minStablePower;
                //		break;
                //	default:
                //		logs.error() << "Invalid 'Min. Up/Down time' for the cluster '" <<
                // it->name() << "'"; 		it->hourlyMinimumCapacity = it->minStablePower;
                //		it->dailyMinimumCapacity  = 0;
                //		it->weeklyMinimumCapacity = 0;
                // }

                // Modulation
                modulationFile.clear() << folder << SEP << ".." << SEP << ".." << SEP << "prepro"
                                       << SEP << cluster->parentArea->id << SEP << cluster->id()
                                       << SEP << "modulation." << study.inputExtension;

                if (study.header.version < 350)
                {
                    auto& modulation = cluster->modulation;
                    // Before v3.5, the market bid modulation is missing
                    bool r = modulation.loadFromCSVFile(
                      modulationFile,
                      2,
                      HOURS_PER_YEAR,
                      Matrix<>::optImmediate | Matrix<>::optMarkAsModified);
                    if (r and modulation.width == 2)
                    {
                        modulation.resizeWithoutDataLost(thermalModulationMax, modulation.height);
                        // Copy of the modulation cost into the market bid modulation
                        // modulation.fillColumn(2, 1.);
                        modulation.pasteToColumn(thermalModulationMarketBid,
                                                 modulation[thermalModulationCost]);
                        modulation.fillColumn(thermalMinGenModulation, 0.);
                    }
                    else
                    {
                        modulation.reset(thermalModulationMax, HOURS_PER_YEAR);
                        modulation.fill(1.);
                    }
                    modulation.markAsModified();
                    ret = ret and r;
                }
                else if (study.header.version <= 450)
                {
                    auto& modulation = cluster->modulation;
                    // Before v4.5, the modulation generation relative is missing
                    bool r = cluster->modulation.loadFromCSVFile(
                      modulationFile,
                      3,
                      HOURS_PER_YEAR,
                      Matrix<>::optImmediate | Matrix<>::optMarkAsModified);

                    if (r and modulation.width == 3)
                    {
                        modulation.resizeWithoutDataLost(thermalModulationMax, modulation.height);
                    }
                    else
                    {
                        modulation.reset(thermalModulationMax, HOURS_PER_YEAR);
                        modulation.fill(1.);
                    }
                    // switch thermalModulationMarketBid and thermalModulationCapacity
                    // They have a wrong order
                    modulation.pasteToColumn(thermalMinGenModulation,
                                             modulation[thermalModulationMarketBid]);
                    modulation.pasteToColumn(thermalModulationMarketBid,
                                             modulation[thermalModulationCapacity]);
                    modulation.pasteToColumn(thermalModulationCapacity,
                                             modulation[thermalMinGenModulation]);
                    modulation.fillColumn(thermalMinGenModulation, 0.);
                    modulation.markAsModified();
                    ret = ret and r;
                }
                else
                {
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
                }

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
        }

        return ret;
    }
    return false;
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
    if (p->key == "costgeneration")
        return p->value.to(cluster.costgeneration);
    if (p->key == "dailyminimumcapacity")
    {
        double d = p->value.to<double>();
        if (cluster.minUpTime < 24)
            cluster.minUpTime = 24;
        if (cluster.minDownTime < 24)
            cluster.minDownTime = 24;
        cluster.minStablePower = std::max(cluster.minStablePower, d);
        return true; // ignored since 3.7
    }
    if (p->key == "enabled")
        return p->value.to<bool>(cluster.enabled);
    if (p->key == "efficiency")
        return p->value.to<double>(cluster.efficiency);
    if (p->key == "fixed-cost")
        return p->value.to<double>(cluster.fixedCost);
    if (p->key == "flexibility")
    {
        // The flexibility is now ignored since v3.5
        return true;
    }
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
    if (p->key == "hourlyminimumcapacity")
    {
        double d = p->value.to<double>();
        cluster.minStablePower = std::max(cluster.minStablePower, d);
        return true; // ignored since 3.7
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
    // for compatibility < 5.0
    if (p->key == "min-updown-time")
    {
        uint val;
        p->value.to<uint>(val);
        if (val)
        {
            if (val < 1)
                val = 1;
            if (val > 168)
                val = 168;
            cluster.minUpTime = val;
            cluster.minDownTime = val;
            return true;
        }
        return false;
    }
    if (p->key == "name")
        return true; // silently ignore it
    if (p->key == "nominalcapacity")
        return p->value.to<double>(cluster.nominalCapacity);

    // for compatibility <3.5
    if (p->key == "operatingcost")
        return p->value.to<double>(cluster.marketBidCost);

    if (p->key == "spread-cost")
        return p->value.to<double>(cluster.spreadCost);
    if (p->key == "spinning")
        return p->value.to<double>(cluster.spinning);
    if (p->key == "startup-cost")
        return p->value.to<double>(cluster.startupCost);
    // for compatibility <3.5
    if (p->key == "stddeviationannualcost")
        return p->value.to<double>(cluster.spreadCost);

    if (p->key == "unitcount")
        return p->value.to<uint>(cluster.unitCount);
    if (p->key == "volatility.planned")
        return p->value.to(cluster.plannedVolatility);
    if (p->key == "volatility.forced")
        return p->value.to(cluster.forcedVolatility);
    if (p->key == "weeklyminimumcapacity")
    {
        double d = p->value.to<double>();
        if (cluster.minUpTime < 168)
            cluster.minUpTime = 168;
        if (cluster.minDownTime < 168)
            cluster.minDownTime = 168;
        cluster.minStablePower = std::max(cluster.minStablePower, d);
        return true; // ignored since 3.7
    }

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

bool ThermalClusterLoadCouplingSection(const AnyString& filename,
                                       ThermalClusterList& list,
                                       const IniFile::Section* s)
{
    if (s->firstProperty)
    {
        ClusterName from;
        ClusterName with;
        ThermalCluster* clusterFrom;
        ThermalCluster* clusterWith;

        // Browse all properties
        for (const IniFile::Property* p = s->firstProperty; p; p = p->next)
        {
            from = p->key;
            with = p->value;
            if (not from or !with)
            {
                logs.warning() << '`' << filename << "`: `" << s->name << "`: Invalid key/value";
                continue;
            }
            from.toLower();
            with.toLower();
            clusterFrom = list.find(from);
            if (not clusterFrom)
            {
                logs.error() << filename << ": impossible to find the cluster '" << from << "'";
                continue;
            }
            clusterWith = list.find(with);
            if (not clusterWith)
            {
                logs.error() << filename << ": impossible to find the cluster '" << with << "'";
                continue;
            }

            if (clusterFrom->coupling.end() != clusterFrom->coupling.find(clusterWith))
                // already referenced
                continue;

            // Adding the reference in both clusters
            clusterFrom->coupling.insert(clusterWith);
            clusterWith->coupling.insert(clusterFrom);
            logs.info() << "  cluster coupling : " << clusterFrom->name() << " <-> "
                        << clusterWith->name();
        }
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

    // Remove all cluster coupling
    if (not c->coupling.empty())
    {
        auto end = c->coupling.end();
        for (auto j = c->coupling.begin(); j != end; ++j)
        {
            auto* link = *j;
            link->parentArea->forceReload();
            link->coupling.erase(c.get());
        }
    }

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
        bool hasCoupling = false;

        // Allocate the inifile structure
        IniFile ini;

        // Browse all clusters
        each([&](const Data::ThermalCluster& c) {
            // Coupling
            if (not c.coupling.empty())
                hasCoupling = true;

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

            // efficiency
            if (c.efficiency != 100.0)
                s->add("efficiency", c.efficiency);

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
            if (c.costgeneration != setManually)
                s->add("costgeneration", c.costgeneration);            
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

        if (hasCoupling)
        {
            IniFile::Section* s = ini.addSection("~_-_coupling_-_~");
            each([&](const Data::ThermalCluster& c) {
                if (c.coupling.empty())
                    return;
                auto send = c.coupling.end();
                for (auto j = c.coupling.begin(); j != send; ++j)
                    s->add(c.id(), (*j)->id());
            });
        }

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
