/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDRO_CONTAINER_H__
#define __ANTARES_LIBS_STUDY_PARTS_HYDRO_CONTAINER_H__

#include <optional>

#include "../../fwd.h"
#include "allocation.h"
#include "prepro.h"
#include "series.h"

namespace Antares::Data
{

//! The maximum number of days in a year
constexpr size_t dayYearCount = 366;

struct DailyDemand
{
    //! Net demand, for each day of the year, for each area
    double DLN = 0.;
    //! Daily local effective load
    double DLE = 0.;
};

struct MonthlyGenerationTargetData
{
    //! Monthly local effective demand
    double MLE = 0.;
    //! Monthly optimal generation
    double MOG = 0.;
    //! Monthly optimal level
    double MOL = 0.;
    //! Monthly target generations
    double MTG = 0.;
};

//!  Hydro Management Data for a given area
struct TimeDependantHydroManagementData
{
    std::array<DailyDemand, dayYearCount> daily{0};
    std::array<MonthlyGenerationTargetData, 12> monthly{0};
};

//! Area Hydro Management Data for a given year
struct AreaDependantHydroManagementData
{
    //! inflows
    std::array<double, 12> inflows{};
    //! monthly minimal generation
    std::array<double, 12> mingens{};

    //! daily minimal generation
    std::array<double, dayYearCount> dailyMinGen{};

    // Data for minGen<->inflows preChecks
    //! monthly total mingen
    std::array<double, 12> totalMonthMingen{};
    //! monthly total inflows
    std::array<double, 12> totalMonthInflows{};
    //! yearly total mingen
    double totalYearMingen = 0;
    //! yearly total inflows
    double totalYearInflows = 0;

}; // struct AreaDependantHydroManagementData

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

    enum weeklyHydroMod
    {
        //! Weekly generating modulation
        genMod = 0,
        //! Weekly pumping modulation
        pumpMod,
    };

    /*!
    ** \brief Load data for hydro container from a folder
    **
    ** \param folder The targer folder
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    static bool LoadFromFolder(Study& study, const std::filesystem::path& folder);

    /*!
    ** \brief Check and validate the loaded datas
    **
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    static bool validate(Study& study);

    /*!
    ** \brief Save data from several containers to a folder (except data for the prepro and
    *time-series)
    **
    ** \param l List of areas
    ** \param folder The targer folder
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    static bool SaveToFolder(const AreaList& areas,
                             const AnyString& folder,
                             const Parameters::Compatibility::HydroPmax hydroPmax);

    /*!
    ** \brief Default Constructor
    */
    PartHydro();
    //! Destructor
    ~PartHydro() = default;

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

    /*!
    ** \brief Load daily max energy
    */
    bool LoadDailyMaxEnergy(const std::filesystem::path& folder, const std::string& areaid);

    bool CheckDailyMaxEnergy(const AnyString& areaName);

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
    Matrix<double, double> dailyMaxPumpAndGen;
    //! Credit Modulation (default 0, 101 * 2)
    Matrix<double, double> creditModulation;

    //! Daily Inflow Patern ([default 1, 0<x<dayspermonth]x365)
    Matrix<double> inflowPattern;

    //! Daily reservoir level ({min,avg,max}x365)
    Matrix<double> reservoirLevel;

    //! Daily water value ({0,1,2%...100%}x365)
    Matrix<double> waterValues;

    //! Hydro allocation, from other areas
    HydroAllocation allocation;

    //! Data for the pre-processor
    std::unique_ptr<PreproHydro> prepro;

    //! Data for time-series
    std::unique_ptr<DataSeriesHydro> series;
    // TODO : following time series could be hosted by the series data member above (of type
    // DataSeriesHydro),
    //        which contains other time.
    Matrix<double, double> dailyNbHoursAtGenPmax;
    Matrix<double, double> dailyNbHoursAtPumpPmax;
    std::unordered_map<uint, AreaDependantHydroManagementData> managementData;

    std::vector<std::optional<double>> deltaBetweenFinalAndInitialLevels;

private:
    static bool checkReservoirLevels(const Study& study);
    static bool checkProperties(Study& study);

}; // class PartHydro

// Interpolates a water value from a table according to a level and a day.
// As this function can be called a lot of times, we pass working variables and returned variables
// as arguments, so that we don't have to create them locally (as in a classical function) each
// time.
double getWaterValue(const double& level, const Matrix<double>& waterValues, const uint day);

// Interpolates a rate from the credit modulation table according to a level
double getWeeklyModulation(const double& level /* format : in % of reservoir capacity */,
                           Matrix<double, double>& creditMod,
                           int modType);

} // namespace Antares::Data

#endif /* __ANTARES_LIBS_STUDY_PARTS_HYDRO_CONTAINER_H__ */
