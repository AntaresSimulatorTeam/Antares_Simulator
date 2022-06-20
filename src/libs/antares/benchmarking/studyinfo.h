#pragma once

#include <antares/study.h>

namespace Benchmarking
{
/*
    === class StudyInfoContainer ===
*/
class StudyInfoContainer
{
public:
    StudyInfoContainer(const Antares::Data::Study& study);

    void collect();

    unsigned int getAreasCount();
    unsigned int getLinksCount();
    unsigned int getPerformedYearsCount();
    unsigned int getEnabledThermalClustersCount();
    const char* getUnitCommitmentMode();
    unsigned int getMaxNbYearsInParallel();

    unsigned int getEnabledBCcount();
    unsigned int getEnabledHourlyBCcount();
    unsigned int getEnabledDailyBCcount();
    unsigned int getEnabledWeeklyBCcount();

private:
    // Methods
    void collectAreasCount();
    void collectLinksCount();
    void collectPerformedYearsCount();
    void collectEnabledThermalClustersCount();
    void collectEnabledBindingConstraintsCount();
    void collectUnitCommitmentMode();
    void collectMaxNbYearsInParallel();


    // Member data
    const Antares::Data::Study& study_;

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

/*
    === class StudyInfoWriter ===
*/

class StudyInfoWriter
{
public:   
    StudyInfoWriter(Yuni::String& filePath, StudyInfoContainer& fileContent);
    void flush();

private:
    // Member data
    Yuni::IO::File::Stream outputFile_;
    Yuni::String& filePath_;
    StudyInfoContainer& fileContent_;
};

} // namespace Benchmarking
