#pragma once

#include <antares/study.h>
#include "file_content.h"

namespace Benchmarking
{

struct OptimizationInfo
{
    unsigned int nbVariables = 0;
    unsigned int nbConstraints = 0;
    unsigned int nbNonZeroCoeffs = 0;
};

/*
    === Info collectors ===
*/

class StudyInfoCollector
{
public:
    StudyInfoCollector(const Antares::Data::Study& study, FileContent& file_content)
        : study_(study), file_content_(file_content)
    {}
    void collect();
private:
    // Methods
    void collectAreasCount();
    void collectLinksCount();
    void collectPerformedYearsCount();
    void collectEnabledThermalClustersCount();
    void collectEnabledBindingConstraintsCount();
    void collectUnitCommitmentMode();
    void collectMaxNbYearsInParallel();
    void collectSolverVersion();

    // Member data
    FileContent& file_content_;
    const Antares::Data::Study& study_;

    // ----------------------------------------------------------------------------------------
    // TODO : add some more pieces of information about study : see following data members.
    // ----------------------------------------------------------------------------------------
    // Parallel execution
    unsigned int minNbYearsInParallel_ = 0;
    unsigned int nbCoreMode_ = 0;
};

class SimulationInfoCollector
{
public:
    SimulationInfoCollector(const OptimizationInfo& optInfo, FileContent& file_content)
        : opt_info_(optInfo), file_content_(file_content)
    {};

    void collect();

private:
    FileContent& file_content_;
    const OptimizationInfo& opt_info_;
};


} // namespace Benchmarking
