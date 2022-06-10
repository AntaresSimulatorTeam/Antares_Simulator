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
    void getNbPerformedYears();
    void getEnabledThermalClusters();

    // Member data
    const Antares::Data::Study& study_;
    Yuni::IO::File::Stream outputFile_;

    unsigned int nbPerformedYears_ = 0;
    unsigned int nbEnabledThermalClusters_ = 0;
    unsigned int nbEnabledBindingConstraints_ = 0;
    unsigned int nbAreas_ = 0;
    unsigned int nbLinks_ = 0;
    unsigned int UnitComitmentMode_;

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
