#pragma once

namespace Antares {
  namespace Data {
    class Study;
  }
}

namespace Benchmarking
{
struct StudyInfo
{
    StudyInfo();
    void initializeFromStudy(const Antares::Data::Study&);
    // Business problem
    unsigned int nbEnabledYears;
    unsigned int nbEnabledClusters;
    unsigned int nbEnabledBindingConstraints;
    unsigned int nbAreas;
    unsigned int nbLinks;
    unsigned int UnitComitmentMode;

    // Parallel execution
    unsigned int maxNbYearsInParallel;
    unsigned int minNbYearsInParallel;
    unsigned int nbCoreMode;

    // Optimization problem
    unsigned int nbNonZeroTermsInConstraintMatrix;
    unsigned int nbVariables;
    unsigned int nbConstraints;
};
} // namespace Benchmarking
