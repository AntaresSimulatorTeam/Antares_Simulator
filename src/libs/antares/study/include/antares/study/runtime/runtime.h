// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_RUNTIME_RUNTIME_INFOS_H__
#define __ANTARES_LIBS_STUDY_RUNTIME_RUNTIME_INFOS_H__

#include <string>
#include <vector>

#include <antares/mersenne-twister/mersenne-twister.h>
#include <antares/study/parameters.h>
#include "antares/study/fwd.h"

namespace Antares::Data
{

class Study;

enum RangeLimitsIndex
{
    rangeBegin = 0,
    rangeEnd,
    rangeCount,
    rangeMax
};

struct StudyRangeLimits
{
public:
    /*!
    ** \brief Dynamic bound-checking (debug only)
    */
    void checkIntegrity() const;

    //! Hours
    unsigned int hour[rangeMax];
    //! Day
    unsigned int day[rangeMax];
    //! Week
    unsigned int week[rangeMax];
    //! Month
    unsigned int month[rangeMax];
    //! Year
    unsigned int year[rangeMax];

}; // class StudyRangeLimits

/*!
** \brief Runtime informations
** \ingroup runtimedata
*/
class StudyRuntimeInfos
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    StudyRuntimeInfos();
    /*!
    ** \brief Destructor
    */
    ~StudyRuntimeInfos();
    //@}

    /*!
    ** \brief Reset internal data according a given study
    */
    bool loadFromStudy(Study& study);

    void initializeRandomNumberGenerators(const Parameters& parameters);

public:
    //! The number of years to process
    unsigned int nbYears;

    //! Range limits
    StudyRangeLimits rangeLimits;

    //! Mode of the simulation
    SimulationMode mode;

    //! The number of interconnections
    unsigned int interconnectionsCount() const;
    //! All interconnections
    std::vector<AreaLink*> areaLink;

    //! Random numbers generators
    MersenneTwister random[seedMax];

    //! Total
    struct Counts
    {
        unsigned int thermalPlants = 0;
        unsigned int thermalPlantsMustRun = 0;
        unsigned int reserveParticipations = 0; //! Total number of reserve participations
        unsigned int capacityReservations = 0;  //! Total number of capacity reservations
        unsigned int shortTermStorages = 0;
        unsigned int shortTermStorageCumulativeConstraints = 0;
        unsigned int hydros = 0;
    } counts;

    //! Override enable/disable TS generation per cluster
    bool thermalTSRefresh = false;

    /*!
    ** \brief The number of simulation days per month
    */
    unsigned int simulationDaysPerMonth[12];

    /*!
    ** \brief The number of simulation days per week
    */
    unsigned int simulationDaysPerWeek[53];

    /*!
    ** \brief Flag to know if at least one error occured during the quadratic optimization
    **
    ** In this case, error on NaN should be disabled
    */
    bool quadraticOptimizationHasFailed;

    std::vector<std::vector<double>> transitMoyenInterconnexionsRecalculQuadratique;

private:
    void initializeRangeLimits(const Study& study, StudyRangeLimits& limits);
    void removeDisabledShortTermStorageClustersFromSolverComputations(Study& study);
    void removeAllRenewableClustersFromSolverComputations(Study& study);
    void disableAllFilters(Study& study);
    void checkThermalTSGeneration(Study& study);
}; // struct StudyRuntimeInfos

} // namespace Antares::Data

#endif // __ANTARES_LIBS_STUDY_RUNTIME_RUNTIME_INFOS_H__
