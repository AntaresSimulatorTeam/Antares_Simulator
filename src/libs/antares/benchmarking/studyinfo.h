#pragma once

#include <antares/study.h>

namespace Benchmarking
{
class StudyInfo
{
public:   
    StudyInfo(const Antares::Data::Study& study);
    void collect();
    void flush(Yuni::String& filePath);

private:
    // Methods
    void getAreasCount();
    void getLinksCount();
    void getPerformedYearsCount();
    void getEnabledThermalClustersCount();
    void getEnabledBindingConstraintsCount();
    void getUnitCommitmentMode();
    void getMaxNbYearsInParallel();

    // Member data
    const Antares::Data::Study& study_;
    Yuni::IO::File::Stream outputFile_;

    unsigned int nbPerformedYears_ = 0;
    unsigned int nbEnabledThermalClusters_ = 0;
    unsigned int nbEnabledBC_ = 0;
    unsigned int nbEnabledHourlyBC_ = 0;
    unsigned int nbEnabledDailyBC_ = 0;
    unsigned int nbEnabledWeeklyBC_ = 0;
    unsigned int nbAreas_ = 0;
    unsigned int nbLinks_ = 0;
    const char* UnitComitmentMode_ = "";

    // Parallel execution
    unsigned int maxNbYearsInParallel_ = 0;
    unsigned int minNbYearsInParallel_ = 0;
    unsigned int nbCoreMode_;

    // Optimization problem
    unsigned int nbNonZeroTermsInConstraintMatrix_ = 0;
    unsigned int nbVariables_ = 0;
    unsigned int nbConstraints_ = 0;
};
} // namespace Benchmarking
