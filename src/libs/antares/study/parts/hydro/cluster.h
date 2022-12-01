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
#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDROCLUSTER_CLUSTER_H__
#define __ANTARES_LIBS_STUDY_PARTS_HYDROCLUSTER_CLUSTER_H__

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>
#include "../../../array/matrix.h"
#include "../common/cluster.h"
#include "../../fwd.h"
#include "series.h"

#include "prepro.h"
#include "allocation.h"

namespace Antares
{
namespace Data
{
/* gp : useless in v8.1, could be useful in a further version
enum HydroclusterModulation
{
    renewableModulationCost = 0,
    renewableModulationCapacity,
    renewableMinGenModulation,
    renewableModulationMax
};
*/

/*
** \brief A single Hydrocluster  //### Todo HydroclusterCluster is filled in with PartHydro
** //CR13 todo note there is a list of Hydrocluster inside PartHydrocluster in area,
** //CR13 : one HydroclusterCluster contains one DataSeriesHydrocluster (ror, storage, mingen)
*/
class HydroclusterCluster final : public Cluster
{

public:
    enum HydroclusterGroup
    {
        //! Wind offshore
        windOffShore = 0,
        //! Wind onshore
        windOnShore,
        //! Concentration solar
        thermalSolar,
        //! PV solar
        PVSolar,
        //! Rooftop solar
        rooftopSolar,
        //! Other 1
        renewableOther1,
        //! Other 2
        renewableOther2,
        //! Other 3
        renewableOther3,
        //! Other 4
        renewableOther4,

        //! The highest value
        groupMax
    };

       enum TimeSeriesMode
    {
        //! TS contain power generation for each unit
        //! Nominal capacity is *ignored*
        powerGeneration = 0,
        //! TS contain production factor for each unit
        //! Nominal capacity is used as a multiplicative factor
        productionFactor
    }; 
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
    /*!
    ** \brief Get the group name string
    ** \return A valid CString
    */
    static const char* GroupName(enum HydroclusterGroup grp);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor, with a parent area
    */
    explicit HydroclusterCluster(Data::Area* parent);
    //! Destructor
    ~HydroclusterCluster();
    //@}

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
    ** \brief Reset to default values
    **
    ** This method should only be called from the GUI
    */
    void reset() override;
    void setGroup(Data::ClusterName newgrp) override;

    void copyFrom(const HydroclusterCluster& cluster);

    /*!
    ** \brief Invalidate all data associated to the hydrocluster cluster
    */
    bool invalidate(bool reload) const override;

    /*!
    ** \brief Mark the hydrocluster cluster as modified
    */
    void markAsModified() const override;

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
    DataSeriesHydrocluster* series;

    // DataSeriesHydro* series; //CR13 1124


public:

    //! Set of hydrocluster clusters
    using Set = std::set<HydroclusterCluster*, CompareClusterName>;
    //! Series

    friend class HydroclusterClusterList;

public:
    /*!
    ** \brief Check and fix all values of a hydrocluster cluster
    **
    ** \return False if an error has been detected and fixed with a default value
    */
    bool integrityCheck() override;

    //! \name Memory management
    //@{
    /*!
    ** \brief Flush the memory to swap files (if swap support enabled)
    */
    void flush() override;

    uint groupId() const override;
    /*!
    ** \brief Get the memory consummed by the hydrocluster cluster (in bytes)
    */
    Yuni::uint64 memoryUsage() const override;
    //@}

    bool setTimeSeriesModeFromString(const YString& value);

    YString getTimeSeriesModeAsString() const;

    /* !
    ** Get production value at time-step ts
    */
    double valueAtTimeStep(uint timeSeriesIndex, uint timeStepIndex) const;

public:    
    enum HydroclusterGroup groupID;

    enum TimeSeriesMode tsMode;
private:
    unsigned int precision() const override;
}; // class HydroclusterCluster

} // namespace Data
} // namespace Antares

#endif //__ANTARES_LIBS_STUDY_PARTS_HYDROCLUSTER_CLUSTER_H__
