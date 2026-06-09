// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_RUNTIME_RUNTIME_INFOS_H__
#define __ANTARES_LIBS_STUDY_RUNTIME_RUNTIME_INFOS_H__

#include <string>
#include <vector>

#include <boost/bimap.hpp>

#include <antares/mersenne-twister/mersenne-twister.h>
#include <antares/study/parameters.h>
#include "antares/study/area/ReserveOpt.h"
#include "antares/study/fwd.h"

struct PROBLEME_HEBDO;

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
    uint hour[rangeMax];
    //! Day
    uint day[rangeMax];
    //! Week
    uint week[rangeMax];
    //! Month
    uint month[rangeMax];
    //! Year
    uint year[rangeMax];

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
    void initializeReservesIndexMaps(const Study& study, const PROBLEME_HEBDO& problem);

public:
    //! The number of years to process
    uint nbYears;

    //! Range limits
    StudyRangeLimits rangeLimits;

    //! Mode of the simulation
    SimulationMode mode;

    //! The number of interconnections
    uint interconnectionsCount() const;
    //! All interconnections
    std::vector<AreaLink*> areaLink;

    //! Random numbers generators
    MersenneTwister random[seedMax];

    //! Total
    struct Counts
    {
        uint thermalPlants = 0;
        uint thermalPlantsMustRun = 0;
        uint reserveParticipations = 0; //! Total number of reserve participations
        uint capacityReservations = 0;  //! Total number of capacity reservations
        uint shortTermStorages = 0;
        uint shortTermStorageCumulativeConstraints = 0;
        uint hydros = 0;
    } counts;

    //! Name of the reserve
    using ReserveName = std::string;
    //! ID of a reserve, obtained by transforming its name
    using ReserveID = std::string;

    struct ReserveIndexMap
    {
        boost::bimap<std::pair<ReserveID, std::string>, int> thermalClusters;
        boost::bimap<std::pair<ReserveID, std::string>, int> STStorageClusters;
        boost::bimap<ReserveID, int> Hydro;
    };

    //! Map used to access reserves participation indices
    ReserveOpt<std::map<AreaName, ReserveIndexMap>> reserveParticipationIndexMaps;
    ReserveOpt<std::map<ReserveID, ReserveName>> reserveIDToName;

    //! Override enable/disable TS generation per cluster
    bool thermalTSRefresh = false;

    /*!
    ** \brief The number of simulation days per month
    */
    uint simulationDaysPerMonth[12];

    /*!
    ** \brief The number of simulation days per week
    */
    uint simulationDaysPerWeek[53];

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
