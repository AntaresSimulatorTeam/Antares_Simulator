/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDRO_CONTAINER_H__
#define __ANTARES_LIBS_STUDY_PARTS_HYDRO_CONTAINER_H__

#include "prepro.h"
#include "series.h"
#include "../../fwd.h"
#include "allocation.h"

#include <yuni/yuni.h>
#include "../../fwd.h"
#include "cluster.h"
#include "cluster_list.h"


namespace Antares
{
namespace Data
{
/*!
** \brief Hydro for a single area
*/
class PartHydro
{
public:
    enum
    {
        //! The minimum value
        minimum = 0,
        //! The average value
        average,
        //! The maximum value
        maximum,
    };

    enum powerDailyE
    {
        //! Generated max power
        genMaxP = 0,
        //! Generated max energy
        genMaxE,
        //! Pumping max Power
        pumpMaxP,
        // Pumping max Energy
        pumpMaxE,
    };

    enum weeklyHydroMod
    {
        //! Weekly generating modulation
        genMod = 0,
        //! Weekly pumping modulation
        pumpMod,
    };

public:
    /*!
    ** \brief Load data for hydro container from a folder
    **
    ** \param folder The targer folder
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    static bool LoadFromFolder(Study& study, const AnyString& folder);

    /*!
    ** \brief Save data from several containers to a folder (except data for the prepro and
    *time-series)
    **
    ** \param l List of areas
    ** \param folder The targer folder
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    static bool SaveToFolder(const AreaList& areas, const AnyString& folder);

public:
    /*!
    ** \brief Default Constructor
    */
    PartHydro();
    //! Destructor
    ~PartHydro();

    /*!
    ** \brief Reset internal data
    */
    void reset();

    void copyFrom(const PartHydro& rhs);

    bool invalidate(bool reload = false) const;

    /*!
    ** \brief Mark all data as modified
    */
    void markAsModified() const;

public:
    //! Inter-daily breakdown (previously called Smoothing Factor or alpha)
    double interDailyBreakdown;
    //! Intra-daily modulation
    double intraDailyModulation;

    //! Intermonthly breakdown
    double intermonthlyBreakdown;

    //! Enabled reservoir management
    bool reservoirManagement;

    //! Following load modulation
    bool followLoadModulations;
    //! Use water values
    bool useWaterValue;
    //! Hard bound on rule curves
    bool hardBoundsOnRuleCurves;
    //! Use heuristic target
    bool useHeuristicTarget;
    //! Reservoir capacity (MWh)
    double reservoirCapacity;
    // gp : pb - initializeReservoirLevelDate must be an enum from january (= 0) to december (= 11)
    //! Initialize reservoir level date (month)
    int initializeReservoirLevelDate;
    //! Use Leeway
    bool useLeeway;
    //! Power to level modulations
    bool powerToLevel;
    //! Leeway low bound
    double leewayLowerBound;
    //! Leeway upper bound
    double leewayUpperBound;
    //! Puming efficiency
    double pumpingEfficiency;
    //! Daily max power ({generating max Power, generating max energy, pumping max power, pumping
    //! max energy}x365)
    Matrix<double, double> maxPower;
    //! Credit Modulation (default 0, 101 * 2)
    Matrix<double, double> creditModulation;

    //! Daily Inflow Patern ([default 1, 0<x<dayspermonth]x365)
    Matrix<double> inflowPattern;

    // Useful for solver RAM estimation
    bool hydroModulable;

    //! Daily reservoir level ({min,avg,max}x365)
    Matrix<double> reservoirLevel;

    //! Daily water value ({0,1,2%...100%}x365)
    Matrix<double> waterValues;

    //! Hydro allocation, from other areas
    HydroAllocation allocation;

    //! Data for the pre-processor
    PreproHydro* prepro;
    //! Data for time-series
    DataSeriesHydro* series; //CR13 1124

}; // class PartHydro

// Type encapsulating working variables for next function :
// As the next function can be called a lot of times, passing an already created variable
// avoids the overhead of local variables creation
struct h2oValueWorkVarsType
{
    double levelUp;
    double levelDown;
};

// Interpolates a water value from a table according to a level and a day.
// As this function can be called a lot of times, we pass working variables and returned variables
// as arguments, so that we don't have to create them locally (as in a classical function) each
// time.
void getWaterValue(const double& level,
                   const Matrix<double>& waterValues,
                   const uint day,
                   h2oValueWorkVarsType& workVar,
                   double& waterValueToReturn);

// Interpolates a rate from the credit modulation table according to a level
double getWeeklyModulation(const double& level /* format : in % of reservoir capacity */,
                           Matrix<double, double>& creditMod,
                           int modType);



class PartHydrocluster
{
public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Default constructor
    */
    PartHydrocluster();
    //! Destructor
    ~PartHydrocluster();
    //@}

    /*!
    ** \brief Reset internal data
    */
    void reset();

    /*!
    ** \brief Resize all matrices dedicated to the sampled timeseries numbers
    **
    ** \param n A number of years
    */
    void resizeAllTimeseriesNumbers(uint n);

    void estimateMemoryUsage(StudyMemoryUsage&) const;

    /*!
    ** \brief Create and initialize the list of all clusters (with the mustrun flag or not)
    **
    ** This method must be called before prepareClustersInMustRunMode()
    ** to ensure the same order whatever the value of the 'mustrun' flag is.
    */
    void prepareAreaWideIndexes();

    /*!
    ** \brief Removes disabled hydrocluster clusters
    **
    ** All clusters with the flag 'enabled' turned to false will be removed from 'list'.
    ** As a consequence, they will no longer be seen as hydrocluster clusters
    ** from the solver's point of view.
    ** \warning This method should only be used from the solver
    **
    ** \return The number of disabled clusters found
    */
    uint removeDisabledClusters();

    /*!
    ** \brief Invalidate all JIT data
    */
    bool invalidate(bool reload) const;

    /*!
    ** \brief Mark the hydrocluster cluster as modified
    */
    void markAsModified() const;

public:
    //! List of all hydrocluster clusters (enabled and disabled)
    HydroclusterClusterList list;

    /*!
    ** \brief All clusters for the area
    **
    ** This variable is only available at runtime from the solver.
    ** It is initialized in the same time that the runtime data.
    **
    ** This list is mainly used to ensure the same order of the
    ** hydrocluster clusters in the outputs.
    */
    std::vector<HydroclusterCluster*> clusters;
    //! How many clusters have we got ?
    // Only available from the solver
    inline size_t clusterCount() const
    {
        return clusters.size();
    }
}; // class PartHydrocluster

} // namespace Data
} // namespace Antares

#include "container.hxx"

#include "../../area.h"

#endif /* __ANTARES_LIBS_STUDY_PARTS_HYDRO_CONTAINER_H__ */
