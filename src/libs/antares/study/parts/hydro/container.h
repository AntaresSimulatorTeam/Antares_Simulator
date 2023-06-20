/*
** Copyright 2007-2023 RTE
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

namespace Antares::Data
{
/*!
 ** \brief Final Reservoir Level data for a single area
 */
class FinalReservoirLevelRuntimeData
{
public:
    // FInal reservoir level runtime data
    std::vector<bool> includeFinalReservoirLevel;
    std::vector<double> endLevel;
    std::vector<double> deltaLevel;

    // Simulation Data
    uint simEndDay;

    // simulation year
    uint yearIndex;

    // data per area
    double initialReservoirLevel;
    double finalReservoirLevel;
    double deltaReservoirLevel;
    double reservoirCapacity;
    double lowLevelLastDay;
    double highLevelLastDay;
    int initReservoirLvlMonth;

    // methods:

    void fillEmpty()
    {
        includeFinalReservoirLevel.push_back(false);
        endLevel.push_back(0.);
        deltaLevel.push_back(0.);
    }

    void assignEndLevelAndDelta()
    {
        includeFinalReservoirLevel.at(yearIndex) = true;
        endLevel.at(yearIndex) = finalReservoirLevel;
        deltaLevel.at(yearIndex) = deltaReservoirLevel;
    }

    double calculateTotalInflows(Antares::Memory::Stored<double>::Type& srcinflows) const
    {
        double totalYearInflows = 0.0;
        for (uint day = 0; day < DAYS_PER_YEAR; ++day)
            totalYearInflows += srcinflows[day];
        return totalYearInflows;
    }

    bool preCheckStartAndEndSim(const AreaName& name) const
    {
        if (simEndDay == DAYS_PER_YEAR && initReservoirLvlMonth == 0)
            return true;
        else
        {
            logs.error() << "Year: " << yearIndex + 1 << ". Area: " << name
                         << ". Simulation must end on day 365 and reservoir level must be "
                            "initiated in January";
            return false;
        }
    }

    bool preCheckYearlyInflow(double totalYearInflows, const AreaName& name) const
    {
        if ((-deltaReservoirLevel) * reservoirCapacity
            > totalYearInflows) // ROR time-series in MW (power), SP time-series in MWh
                                // (energy)
        {
            logs.error() << "Year: " << yearIndex + 1 << ". Area: " << name
                         << ". Incompatible total inflows: " << totalYearInflows
                         << " with initial: " << initialReservoirLevel
                         << " and final: " << finalReservoirLevel << " reservoir levels.";
            return false;
        }
        return true;
    }

    bool preCheckRuleCurves(const AreaName& name) const
    {
        if (finalReservoirLevel < lowLevelLastDay || finalReservoirLevel > highLevelLastDay)
        {
            logs.error() << "Year: " << yearIndex + 1 << ". Area: " << name
                         << ". Specifed final reservoir level: " << finalReservoirLevel
                         << " is incompatible with reservoir level rule curve [" << lowLevelLastDay
                         << " , " << highLevelLastDay << "]";
            return false;
        }
        return true;
    }
};

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

    bool forceReload(bool reload = false) const;

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
    DataSeriesHydro* series;

    //! Data for final reservoir level
    FinalReservoirLevelRuntimeData finalReservoirLevelRuntimeData;

}; // class PartHydro


// Interpolates a water value from a table according to a level and a day.
// As this function can be called a lot of times, we pass working variables and returned variables
// as arguments, so that we don't have to create them locally (as in a classical function) each
// time.
void getWaterValue(const double& level,
                   const Matrix<double>& waterValues,
                   const uint day,
                   double& waterValueToReturn);

// Interpolates a rate from the credit modulation table according to a level
double getWeeklyModulation(const double& level /* format : in % of reservoir capacity */,
                           Matrix<double, double>& creditMod,
                           int modType);

} // namespace Antares::Data

#include "../../area.h"

#endif /* __ANTARES_LIBS_STUDY_PARTS_HYDRO_CONTAINER_H__ */
