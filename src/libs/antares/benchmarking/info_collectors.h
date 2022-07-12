#pragma once

#include <vector>
#include <map>
#include <string>

#include <antares/study.h>
#include "file_content.h"

using namespace std;

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


class DurationCollector
{
public:
    DurationCollector() = default;

    void toFileContent(FileContent& file_content);
    void addDuration(std::string name, int64_t duration);
    int64_t getTotalDuration() { return total_duration_; }

private:
    int64_t total_duration_ = 0;
    map<string, vector<int64_t>> duration_items_;
    
    // Durations can be added in a context of multi-threading, so we need to protect
    // these additions from thread concurrency
    Yuni::Mutex mutex_;
};


} // namespace Benchmarking
