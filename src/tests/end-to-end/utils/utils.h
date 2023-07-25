#pragma once
#define WIN32_LEAN_AND_MEAN
#include "antares/study/study.h"
#include "simulation/economy.h"
#include "antares/study/scenario-builder/rules.h"
#include "antares/study/scenario-builder/sets.h"
#include "simulation.h"

using namespace Antares::Solver;
using namespace Antares::Solver::Simulation;


void initializeStudy(Study::Ptr study);
void configureLinkCapacities(AreaLink* link);
void addLoadToArea(Area* area, double loadInArea);

void configureCluster(std::shared_ptr<ThermalCluster> cluster,
                      double nominalCapacity,
                      double availablePower,
                      double cost,
                      unsigned int unitCount);

std::shared_ptr<ThermalCluster> addClusterToArea(Area* area, const std::string& clusterName);
void addScratchpadToEachArea(Study::Ptr study);

// -------------------------------
// Simulation results retrieval
// -------------------------------
class averageResults
{
public:
    averageResults(Variable::R::AllYears::AverageData& averageResults) : averageResults_(averageResults)
    {}

    double hour(unsigned int hour) { return averageResults_.hourly[hour]; }
    double day(unsigned int day) { return averageResults_.daily[day]; }
    double week(unsigned int week) { return averageResults_.weekly[week]; }

private:
    Variable::R::AllYears::AverageData& averageResults_;
};


class OutputRetriever
{
public:
    OutputRetriever(std::shared_ptr<ISimulation<Economy>>& simulation) : simulation_(simulation) {}
    averageResults overallCost(Area* area);
    averageResults load(Area* area);
    averageResults flow(AreaLink* link);
    averageResults thermalGeneration(ThermalCluster* cluster);

private:
    template<class VCard>
    typename Variable::Storage<VCard>::ResultsType* retrieveAreaResults(Area* area);

    template<class VCard>
    typename Variable::Storage<VCard>::ResultsType* retrieveLinkResults(AreaLink* link);

    template<class VCard>
    typename Variable::Storage<VCard>::ResultsType* retrieveResultsForThermalCluster(ThermalCluster* cluster);

    std::shared_ptr<ISimulation<Economy>>& simulation_;
};

template<class VCard>
typename Variable::Storage<VCard>::ResultsType*
OutputRetriever::retrieveAreaResults(Area* area)
{
    typename Variable::Storage<VCard>::ResultsType* result = nullptr;
    simulation_->variables.retrieveResultsForArea<VCard>(&result, area);
    return result;
}

template<class VCard>
typename Variable::Storage<VCard>::ResultsType*
OutputRetriever::retrieveLinkResults(AreaLink* link)
{
    typename Variable::Storage<VCard>::ResultsType* result = nullptr;
    simulation_->variables.retrieveResultsForLink<VCard>(&result, link);
    return result;
}

template<class VCard>
typename Variable::Storage<VCard>::ResultsType*
OutputRetriever::retrieveResultsForThermalCluster(ThermalCluster* cluster)
{
    typename Variable::Storage<VCard>::ResultsType* result = nullptr;
    simulation_->variables.retrieveResultsForThermalCluster<VCard>(&result, cluster);
    return result;
}

// -----------------------
// BC rhs configuration
// -----------------------
class BCrhsConfig
{
public:
    BCrhsConfig() = delete;
    BCrhsConfig(std::shared_ptr<BindingConstraint> BC, unsigned int nbTimeSeries);
    void fillRHStimeSeriesWith(unsigned int TSnumber, double rhsValue);

private:
    std::shared_ptr<BindingConstraint> BC_;
    unsigned int nbOfTimeSeries_ = 0;
};


// --------------------------------------
// BC group TS number configuration
// --------------------------------------
class BCgroupScenarioBuilder
{
public:
    BCgroupScenarioBuilder() = delete;
    BCgroupScenarioBuilder(Study::Ptr study, unsigned int nbYears);
    void yearGetsTSnumber(std::string groupName, unsigned int year, unsigned int TSnumber);

private:
    unsigned int nbYears_ = 0;
    ScenarioBuilder::Rules::Ptr rules_;
};


// =====================
// Simulation handler
// =====================
using namespace Benchmarking;

class SimulationHandler
{
public:
    SimulationHandler(std::shared_ptr<Study> study)
        : study_(study)
    {}
    ~SimulationHandler() = default;
    void create();
    void run() { simulation_->run(); }
    std::shared_ptr<ISimulation<Economy>>& get() { return simulation_; }

private:
    std::shared_ptr<ISimulation<Economy>> simulation_;
    NullDurationCollector nullDurationCollector_;
    Settings settings_;
    std::shared_ptr<Study> study_;
};


// =========================
// Basic study builder
// =========================

struct StudyBuilder
{
    StudyBuilder();

    void simulationBetweenDays(const unsigned int firstDay, const unsigned int lastDay);
    Area* addAreaToStudy(const std::string& areaName);
    void setNumberMCyears(unsigned int nbYears);
    void playOnlyYear(unsigned int year);

    // Data members
    std::shared_ptr<Study> study;
    std::shared_ptr<SimulationHandler> simulation;
    std::shared_ptr<OutputRetriever> output;
};



// ===========================================================

void prepareStudy(Antares::Data::Study::Ptr pStudy, int nbYears);

Antares::Data::Area* addArea(Antares::Data::Study::Ptr pStudy, const std::string& areaName, int nbTS);

std::shared_ptr<Antares::Data::ThermalCluster> addCluster(Antares::Data::Area* pArea, const std::string& clusterName, double maximumPower, double cost, int nbTS, int unitCount = 1);

std::shared_ptr<Antares::Data::BindingConstraint> addBindingConstraints(Antares::Data::Study::Ptr study, std::string name, std::string group);

void cleanSimulation(Antares::Solver::Simulation::ISimulation< Antares::Solver::Simulation::Economy >* simulation);
void cleanStudy(Antares::Data::Study::Ptr pStudy);

float defineYearsWeight(Study::Ptr pStudy, const std::vector<float>& yearsWeight);

ScenarioBuilder::Rules::Ptr createScenarioRules(Study::Ptr pStudy);


class NullResultWriter: public Solver::IResultWriter {
    void addEntryFromBuffer(const std::string &, Clob &) override;

    void addEntryFromBuffer(const std::string &, std::string &) override;

    void addEntryFromFile(const std::string &, const std::string &) override;

    bool needsTheJobQueue() const override;

    void finalize(bool ) override;
};
