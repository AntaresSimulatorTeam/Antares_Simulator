#pragma once

#include <vector>
#include <map>
#include <string>
#include <mutex>

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
    StudyInfoCollector(const Antares::Data::Study& study)
        : study_(study)
    {}
    void toFileContent(FileContent& file_content);
private:
    // Methods
    void areasCountToFileContent(FileContent& file_content);
    void linksCountToFileContent(FileContent& file_content);
    void performedYearsCountToFileContent(FileContent& file_content);
    void enabledThermalClustersCountToFileContent(FileContent& file_content);
    void enabledBindingConstraintsCountToFileContent(FileContent& file_content);
    void unitCommitmentModeToFileContent(FileContent& file_content);
    void maxNbYearsInParallelToFileContent(FileContent& file_content);
    void solverVersionToFileContent(FileContent& file_content);

    void ORToolsUsed(FileContent& file_content);
    void ORToolsSolver(FileContent& file_content);

    // Member data
    const Antares::Data::Study& study_;
};

class SimulationInfoCollector
{
public:
    SimulationInfoCollector(const OptimizationInfo& optInfo)
        : opt_info_(optInfo)
    {};

    void toFileContent(FileContent& file_content);

private:
    const OptimizationInfo& opt_info_;
};



class IDurationCollector
{
public:
    virtual void addDuration(const std::string& name, int64_t duration) = 0;
};

class NullDurationCollector : public IDurationCollector
{
public:
    NullDurationCollector() = default;
    void addDuration(const std::string& /* name */, int64_t /* duration */) override { /* Do nothing */ }
};

class DurationCollector : public IDurationCollector
{
public:
    DurationCollector() = default;

    void toFileContent(FileContent& file_content);
    void addDuration(const std::string& name, int64_t duration) override;

private:
    map<string, vector<int64_t>> duration_items_;

    // Durations can be added in a context of multi-threading, so we need to protect
    // these additions from thread concurrency
    std::mutex mutex_;
};


} // namespace Benchmarking
