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

                auto* cluster = new ThermalCluster(area, study.maxNbYearsInParallel);

                // Load data of a thermal cluster from a ini file section
                if (not ThermalClusterLoadFromSection(study.buffer, *cluster, *section))
                {
                    delete cluster;
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
                if (not add(cluster))
                {
                    // This error should never happen
                    logs.error() << "Impossible to add the thermal cluster '" << cluster->name()
                                 << "'";
                    delete cluster;
                    continue;
                }
                // keeping track of the cluster
                mapping[cluster->id()] = cluster;

                cluster->flush();
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

    switch (p->key[0])
    {
    case 'a':
    {
        if (p->key == "annuityinvestment")
            return p->value.to<uint>(cluster.annuityInvestment);
        break;
    }
    case 'c':
    {
        if (p->key == "co2")
            return p->value.to<double>(cluster.co2);
        break;
    }
    case 'd':
    {
        if (p->key == "dailyminimumcapacity")
        {
            double d = p->value.to<double>();
            if (not Math::Zero(d) and cluster.minUpTime < 24)
                cluster.minUpTime = 24;
            if (not Math::Zero(d) and cluster.minDownTime < 24)
                cluster.minDownTime = 24;
            cluster.minStablePower = Math::Max(cluster.minStablePower, d);
            return true; // ignored since 3.7
        }
        break;
    }
    case 'e':
    {
        if (p->key == "enabled")
            return p->value.to<bool>(cluster.enabled);
        break;
    }

    case 'f':
    {
        if (p->key == "fixed-cost")
            return p->value.to<double>(cluster.fixedCost);
        if (p->key == "flexibility")
        {
            // The flexibility is now ignored since v3.5
            return true;
        }
        break;
    }
    case 'g':
    {
        if (p->key == "groupmincount")
            return p->value.to<uint>(cluster.groupMinCount);
        if (p->key == "groupmaxcount")
            return p->value.to<uint>(cluster.groupMaxCount);
        if (p->key == "group")
        {
            cluster.setGroup(p->value);
            return true;
        }
        break;
    }
    case 'h':
    {
        if (p->key == "hourlyminimumcapacity")
        {
            double d = p->value.to<double>();
            cluster.minStablePower = Math::Max(cluster.minStablePower, d);
            return true; // ignored since 3.7
        }
        break;
    }
    case 'l':
    {
        if (p->key == "law.planned")
            return p->value.to(cluster.plannedLaw);
        if (p->key == "law.forced")
            return p->value.to(cluster.forcedLaw);
        break;
    }
    case 'm':
    {
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
        break;
    }
    case 'n':
    {
        if (p->key == "name")
            return true; // silently ignore it
        if (p->key == "nominalcapacity")
            return p->value.to<double>(cluster.nominalCapacity);
        break;
    }
    case 'o':
    {
        // for compatibility <3.5
        if (p->key == "operatingcost")
            return p->value.to<double>(cluster.marketBidCost);
        break;
    }
    case 's':
    {
        if (p->key == "spread-cost")
            return p->value.to<double>(cluster.spreadCost);
        if (p->key == "spinning")
            return p->value.to<double>(cluster.spinning);
        if (p->key == "startup-cost")
            return p->value.to<double>(cluster.startupCost);
        // for compatibility <3.5
        if (p->key == "stddeviationannualcost")
            return p->value.to<double>(cluster.spreadCost);
        break;
    }
    case 'u':
    {
        if (p->key == "unitcount")
            return p->value.to<uint>(cluster.unitCount);
        break;
    }
    case 'v':
    {
        if (p->key == "volatility.planned")
            return p->value.to(cluster.plannedVolatility);
        if (p->key == "volatility.forced")
            return p->value.to(cluster.forcedVolatility);
        break;
    }
    case 'w':
    {
        if (p->key == "weeklyminimumcapacity")
        {
            double d = p->value.to<double>();
            if (not Math::Zero(d) and cluster.minUpTime < 168)
                cluster.minUpTime = 168;
            if (not Math::Zero(d) and cluster.minDownTime < 168)
                cluster.minDownTime = 168;
            cluster.minStablePower = Math::Max(cluster.minStablePower, d);
            return true; // ignored since 3.7
        }
        break;
    }
    }

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
            if (not ThermalClusterLoadFromProperty(cluster, property))
            {
                logs.warning() << '`' << filename << "`: `" << section.name << "`/`"
                               << property->value << "`: The property is unknown and ignored";
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

void ThermalClusterList::retrieveTotalCapacityAndUnitCount(double& total, uint& unitCount) const
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

bool ThermalClusterList::remove(const ClusterName& id)
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

    // Remove all cluster coupling
    if (not c->coupling.empty())
    {
        auto end = c->coupling.end();
        for (auto j = c->coupling.begin(); j != end; ++j)
        {
            auto* link = *j;
            link->parentArea->invalidate();
            link->coupling.erase(c);
        }
    }

    // delete the cluster
    delete c;

    // Rebuilding the index
    rebuildIndex();
    return true;
}

void ThermalClusterList::enableMustrunForEveryone()
{
    // enabling the mustrun mode
    each([&](ThermalCluster& cluster) { cluster.mustrun = true; });
}

bool ThermalCluster::isVisibleOnLayer(const size_t& layerID) const
{
    return parentArea ? parentArea->isVisibleOnLayer(layerID) : false;
}

// TODO : move to class
void ThermalClusterListEnsureDataPrepro(ThermalClusterList* list)
{
    auto end = list->cluster.end();
    for (auto it = list->cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        if (not cluster.prepro)
            cluster.prepro = new PreproThermal();
    }
}

void ThermalClusterListEnsureDataTimeSeries(ThermalClusterList* list)
{
    auto end = list->cluster.end();
    for (auto it = list->cluster.begin(); it != end; ++it)
    {
        auto& cluster = *(it->second);
        if (not cluster.series)
            cluster.series = new DataSeriesCommon();
    }
}
} // namespace Data
} // namespace Antares
