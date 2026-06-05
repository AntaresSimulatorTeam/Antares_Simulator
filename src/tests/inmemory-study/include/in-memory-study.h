// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#define WIN32_LEAN_AND_MEAN
#include <limits>

#include "antares/io/outputs/SimulationTable.h"
#include "antares/solver/simulation/ISimulationObserver.h"
#include "antares/solver/simulation/economy.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/study/scenario-builder/rules.h"
#include "antares/study/scenario-builder/sets.h"
#include "antares/study/study.h"
#include "antares/writer/null_result_writer.h"

using namespace Antares::Solver;
using namespace Antares::Solver::Simulation;
using namespace Antares::Data::ScenarioBuilder;

void initializeStudy(Data::Study* study);
void configureLinkCapacities(AreaLink* link);

class TimeSeriesConfigurer final
{
public:
    TimeSeriesConfigurer() = default;

    TimeSeriesConfigurer(Matrix<>& matrix):
        ts_(&matrix)
    {
    }

    TimeSeriesConfigurer(TimeSeries& ts):
        ts_(&ts.timeSeries)
    {
    }

    TimeSeriesConfigurer& setDimensions(unsigned columnCount, unsigned rowCount = HOURS_PER_YEAR);
    TimeSeriesConfigurer& fillColumnWith(unsigned column, double value);
    TimeSeriesConfigurer& fillColumnWith(unsigned column, const std::vector<double>& values);

private:
    Matrix<>* ts_ = nullptr;
};

class ThermalClusterConfig final
{
public:
    ThermalClusterConfig() = delete;
    explicit ThermalClusterConfig(std::shared_ptr<ThermalCluster> cluster);
    ThermalClusterConfig& setNominalCapacity(double nominalCapacity);
    ThermalClusterConfig& setUnitCount(unsigned unitCount);
    ThermalClusterConfig& setCosts(double cost);
    ThermalClusterConfig& setAvailablePowerNumberOfTS(unsigned columnCount);
    ThermalClusterConfig& setAvailablePower(unsigned column, double value);

private:
    std::shared_ptr<ThermalCluster> cluster_ = nullptr;
    TimeSeriesConfigurer tsAvailablePowerConfig_;
};

class ShortTermStorageAddConstraintConfig final
{
public:
    ShortTermStorageAddConstraintConfig() = delete;

    ShortTermStorageAddConstraintConfig(Antares::Data::ShortTermStorage::STStorageCluster& storage):
        storage(storage),
        constraint(std::make_shared<Antares::Data::ShortTermStorage::AdditionalConstraints>())
    {
    }

    ShortTermStorageAddConstraintConfig& setName(const std::string& name)
    {
        constraint->name = name;
        return *this;
    }

    ShortTermStorageAddConstraintConfig& setVariable(const std::string& variable)
    {
        constraint->variable = variable;
        return *this;
    }

    ShortTermStorageAddConstraintConfig& setOperatorType(const std::string& operatorType)
    {
        constraint->operatorType = operatorType;
        return *this;
    }

    ShortTermStorageAddConstraintConfig& setHours(const std::vector<std::set<int>>& hourSets)
    {
        for (const auto& hourSet: hourSets)
        {
            constraint->constraints.push_back(
              {.hours = hourSet, .globalIndex = 0, .localIndex = 0});
        }
        return *this;
    }

    std::shared_ptr<Antares::Data::ShortTermStorage::AdditionalConstraints> build()
    {
        storage.additionalConstraints.push_back(std::move(constraint));
        // The ShortTermStorageAddConstraintConfig instance may be re-used
        constraint = std::make_shared<Antares::Data::ShortTermStorage::AdditionalConstraints>();
        return storage.additionalConstraints.back();
    }

private:
    Antares::Data::ShortTermStorage::STStorageCluster& storage;
    std::shared_ptr<Antares::Data::ShortTermStorage::AdditionalConstraints> constraint;
};

class ShortTermStorageConfig final

{
public:
    ShortTermStorageConfig() = delete;
    explicit ShortTermStorageConfig(Antares::Data::ShortTermStorage::STStorageCluster& storage);
    ShortTermStorageConfig& setInjectionNominalCapacity(double injectionNominalCapacity);
    ShortTermStorageConfig& setWithdrawalNominalCapacity(double withdrawalNominalCapacity);
    ShortTermStorageConfig& setReservoirCapacity(double reservoirCapacity);
    ShortTermStorageConfig& setInitialLevel(double initialLevel);
    ShortTermStorageConfig& setInitialLevelOptim(bool initialLevelOptim);
    ShortTermStorageConfig& setInjectionEfficiency(double injectionEfficiency);
    ShortTermStorageConfig& setWithdrawalEfficiency(double withdrawalEfficiency);
    ShortTermStorageConfig& setGroupName(const std::string& groupName);
    ShortTermStorageConfig& setName(const std::string& name);
    ShortTermStorageConfig& setPenalizeVariationWithdrawal(bool penalizeVariationWithdrawal);
    ShortTermStorageConfig& setAllowOverflow(bool allowOverflow);

    ShortTermStorageConfig& setPenalizeVariationInjection(bool penalizeVariationInjection);
    ShortTermStorageConfig& setEnabled(bool enabled);

    ShortTermStorageAddConstraintConfig& addConstraint()
    {
        return constraintConfig;
    }

private:
    Antares::Data::ShortTermStorage::STStorageCluster& storage;
    ShortTermStorageAddConstraintConfig constraintConfig;
};

std::shared_ptr<ThermalCluster> addClusterToArea(Area* area, const std::string& clusterName);

Antares::Data::ShortTermStorage::STStorageCluster* addSTSToArea(Area* area,
                                                                const std::string& stsName);

// -------------------------------
// Simulation results retrieval
// -------------------------------
class averageResults final
{
public:
    averageResults(Variable::R::AllYears::AverageData& averageResults):
        averageResults_(averageResults)
    {
    }

    long double* hours()
    {
        return averageResults_.hourly.data();
    }

    double hour(unsigned hour)
    {
        return averageResults_.hourly[hour];
    }

    long double* days()
    {
        return averageResults_.daily.data();
    }

    double day(unsigned day)
    {
        return averageResults_.daily[day];
    }

    long double* weeks()
    {
        return averageResults_.weekly.data();
    }

    double week(unsigned week)
    {
        return averageResults_.weekly[week];
    }

private:
    Variable::R::AllYears::AverageData& averageResults_;
};

class OutputRetriever final
{
public:
    OutputRetriever(ISimulation<Economy>& simulation):
        simulation_(simulation)
    {
    }

    averageResults overallCost(Area* area);
    averageResults levelForSTSgroup(Area* area, unsigned groupNb);
    averageResults withdrawalForSTSgroup(Area* area, unsigned groupNb);
    averageResults load(Area* area);
    averageResults hydroStorage(Area* area);
    averageResults flow(AreaLink* link);
    averageResults thermalGeneration(ThermalCluster* cluster);
    averageResults thermalNbUnitsON(ThermalCluster* cluster);

private:
    template<class VCard>
    typename Variable::Storage<VCard>::ResultsType* retrieveAreaResults(Area* area);

    template<class VCard>
    typename Variable::Storage<VCard>::ResultsType* retrieveLinkResults(AreaLink* link);

    template<class VCard>
    typename Variable::Storage<VCard>::ResultsType* retrieveResultsForThermalCluster(
      ThermalCluster* cluster);

    ISimulation<Economy>& simulation_;
};

template<class VCard>
typename Variable::Storage<VCard>::ResultsType* OutputRetriever::retrieveAreaResults(Area* area)
{
    typename Variable::Storage<VCard>::ResultsType* result = nullptr;
    simulation_.variables.retrieveResultsForArea<VCard>(&result, area);
    return result;
}

template<class VCard>
typename Variable::Storage<VCard>::ResultsType* OutputRetriever::retrieveLinkResults(AreaLink* link)
{
    typename Variable::Storage<VCard>::ResultsType* result = nullptr;
    simulation_.variables.retrieveResultsForLink<VCard>(&result, link);
    return result;
}

template<class VCard>
typename Variable::Storage<VCard>::ResultsType* OutputRetriever::retrieveResultsForThermalCluster(
  ThermalCluster* cluster)
{
    typename Variable::Storage<VCard>::ResultsType* result = nullptr;
    simulation_.variables.retrieveResultsForThermalCluster<VCard>(&result, cluster);
    return result;
}

class ScenarioBuilderRule final
{
public:
    ScenarioBuilderRule(Data::Study& study);

    loadTSNumberData& load()
    {
        return rules_->load;
    }

    BindingConstraintsTSNumberData& bcGroup()
    {
        return rules_->binding_constraints;
    }

    hydroTSNumberData& hydro()
    {
        return rules_->hydro;
    }

    // index = area index
    std::vector<ShortTermAdditionalConstraintsTSNumberData>& stsAdditionalConstraints()
    {
        return rules_->shortTermStorageAdditionalConstraints;
    }

private:
    Rules::Ptr rules_;
};

// =====================
// Simulation handler
// =====================

class TestingSimulationObserver final: public Solver::Simulation::ISimulationObserver
{
public:
    struct Variable
    {
        // All comparisons with NaN return false, except for !=
        // For example (NaN == 4.) => false
        // (NaN == NaN) => false
        // Using any other arbitrary value (infinity, 0, etc.) may result in false positives
        // or false negatives
        double Xmin = std::numeric_limits<double>::quiet_NaN();
        double Xmax = std::numeric_limits<double>::quiet_NaN();
        double objectiveCoefficient = std::numeric_limits<double>::quiet_NaN();
    };

    struct Constraint
    {
        double rhs = std::numeric_limits<double>::quiet_NaN();
        std::map<std::string, double> coefficients;
    };

    struct SingleProblem

    {
        std::map<std::string, Variable> variables;
        std::map<std::string, Constraint> constraints;
    };

    std::map<std::pair<int, std::string>, SingleProblem> problems;

    void notifyHebdoProblem(const PROBLEME_HEBDO& problemeHebdo,
                            int optimizationNumber,
                            std::string_view name) override;
};

class SimulationHandler final
{
public:
    SimulationHandler(Data::Study& study):
        study_(study)
    {
    }

    ~SimulationHandler() = default;

    SimulationHandler(const SimulationHandler&) = delete;
    SimulationHandler& operator=(const SimulationHandler&) = delete;

    void create();

    void run()
    {
        simulation_->run();
    }

    ISimulation<Economy>& rawSimu()
    {
        return *simulation_;
    }

public:
    const TestingSimulationObserver& getObserver() const
    {
        return observer_;
    }

private:
    std::shared_ptr<ISimulation<Economy>> simulation_;
    Benchmarking::DurationCollector durationCollector_;
    Settings settings_;
    Data::Study& study_;
    NullResultWriter resultWriter_;
    TestingSimulationObserver observer_;
};

// =========================
// Basic study builder
// =========================

struct StudyBuilder
{
    StudyBuilder();

    void simulationBetweenDays(const unsigned firstDay, const unsigned lastDay);
    Area* addAreaToStudy(const std::string& areaName);
    void setNumberMCyears(unsigned nbYears);
    void playOnlyYear(unsigned year);
    void giveWeightToYear(float weight, unsigned year);

    // Data members
    std::unique_ptr<Data::Study> study;
    SimulationHandler simulation;
};

std::shared_ptr<Antares::Data::BindingConstraint> addBindingConstraints(Antares::Data::Study& study,
                                                                        std::string name,
                                                                        std::string group);
