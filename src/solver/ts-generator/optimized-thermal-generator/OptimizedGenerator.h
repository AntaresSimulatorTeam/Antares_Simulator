//
// Created by milos on 10/11/23.
//

#pragma once

#include "../randomized-thermal-generator/RandomizedGenerator.h"
#include "../../../libs/antares/study/maintenance_planning/MaintenanceGroupRepository.h"
#include "../../../libs/antares/study/maintenance_planning/MaintenanceGroup.h"

namespace Antares::Solver::TSGenerator
{

class OptimizedThermalGenerator : public GeneratorTempData
{
private:
    /*
    in adq-patch we re-used existing structure(s) for helping us define an optimization problem ->
    like: struct CORRESPONDANCES_DES_VARIABLES.
    unfortunately here we have to create our own help-structure if we need one
    Question: can we still use struct PROBLEME_ANTARES_A_RESOUDRE ?! - no we use MPSolver class
    */

    // define here variables/structures that will help us build optimization problem

    // optimization problem construction methods
    void calculateParameters();
    void buildProblemVariables();
    void setVariableBounds();
    void buildProblemConstraintsLHS();
    void buildProblemConstraintsRHS();
    void setProblemCost();
    void solveProblem();
    void allocateProblem(); // this one should be called in constructor. It basically resets all the
                            // vectors in PROBLEME_ANTARES_A_RESOUDRE for new opt problem.

    // some methods
    void GenerateOptimizedThermalTimeSeriesPerOneMaintenanceGroup(Data::MaintenanceGroup& group);
    void calculateResidualLoad(Data::MaintenanceGroup& group);
    void createOptimizationProblemPerCluster(const Data::Area& area, Data::ThermalCluster& cluster);

    // variables
    Data::MaintenanceGroupRepository& maintenanceGroupRepo;
    bool globalThermalTSgeneration_;

public:
    void run(); // calls private optimization problem construction methods

    explicit OptimizedThermalGenerator(Data::Study& study,
                                       uint year,
                                       bool globalThermalTSgeneration,
                                       Solver::Progression::Task& progr,
                                       IResultWriter& writer) :
     GeneratorTempData(study, progr, writer), maintenanceGroupRepo(study.maintenanceGroups)
    {
        currentYear = year;
        globalThermalTSgeneration_ = globalThermalTSgeneration;
        nbThermalTimeseries = study.parameters.maintenancePlanning.getScenarioLength()
                              * study.parameters.maintenancePlanning.getScenarioNumber();
        // allocateProblem();
    }

    ~OptimizedThermalGenerator() = default;

    void GenerateOptimizedThermalTimeSeriesPerAllMaintenanceGroups();
};

} // namespace Antares::Solver::TSGenerator