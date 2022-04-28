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
#ifndef __ANTARES_LIBS_STUDY_PARTS_THERMAL_CLUSTER_H__
#define __ANTARES_LIBS_STUDY_PARTS_THERMAL_CLUSTER_H__

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>
#include "../../../array/matrix.h"
#include "defines.h"
#include "prepro.h"
#include "../common/cluster.h"
#include "../../fwd.h"
#include <set>
#include <map>
#include <vector>

namespace Antares
{
namespace Data
{
enum ThermalLaw
{
    thermalLawUniform,
    thermalLawGeometric
};

enum ThermalModulation
{
    thermalModulationCost = 0,
    thermalModulationMarketBid,
    thermalModulationCapacity,
    thermalMinGenModulation,
    thermalModulationMax
};

enum class LocalTSGenerationBehavior
{
    useGlobalParameter = 0,
    forceGen,
    forceNoGen
};

/*!
** \brief A single thermal cluster
*/
class ThermalCluster final : public Cluster
{
public:
    enum ThermalDispatchableGroup
    {
        //! Nuclear
        thermalDispatchGrpNuclear = 0,
        //! Lignite
        thermalDispatchGrpLignite,
        //! Hard Coal
        thermalDispatchGrpHardCoal,
        //! Gas
        thermalDispatchGrpGas,
        //! Oil
        thermalDispatchGrpOil,
        //! Mixed fuel
        thermalDispatchGrpMixedFuel,
        //! Other 1
        thermalDispatchGrpOther1,
        //! Other 2
        thermalDispatchGrpOther2,
        //! Other 3
        thermalDispatchGrpOther3,
        //! Other 4
        thermalDispatchGrpOther4,

        //! The highest value
        groupMax
    };

    //! Set of thermal clusters
    using Set = std::set<ThermalCluster*, CompareClusterName>;
    //! Set of thermal clusters (pointer)
    using SetPointer = std::set<ThermalCluster*>;
    //! Vector of thermal clusters
    using Vector = std::vector<Data::ThermalCluster*>;

public:
    /*!
    ** \brief Get the group name string
    ** \return A valid CString
    */
    static const char* GroupName(enum ThermalDispatchableGroup grp);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor, with a parent area
    */
    explicit ThermalCluster(Data::Area* parent);
    explicit ThermalCluster(Data::Area* parent, uint nbParallelYears);
    //! Destructor
    ~ThermalCluster();
    //@}

    /*!
    ** \brief Invalidate all data associated to the thermal cluster
    */
    bool invalidate(bool reload) const override;

    /*!
    ** \brief Mark the thermal cluster as modified
    */
    void markAsModified() const override;

    /*!
    ** \brief Reset to default values
    **
    ** This method should only be called from the GUI
    */
    void reset() override;

    //! Set the group
    void setGroup(Data::ClusterName newgrp) override;
    //@}

    //! \name Spinning
    //@{
    /*!
    ** \brief Calculation of spinning
    **
    ** The formula is : TS[i,j] = TS[i,j] * (1 - Spinning / 100)
    */
    void calculationOfSpinning();

    /*!
    ** \brief Calculation of spinning (reverse)
    **
    ** The original formula for the calculation of the spinning is :
    ** TS[i,j] = TS[i,j] * (1 - Spinning / 100)
    **
    ** This method is used to removed the spinning, before exporting the TS matrices
    ** into the input.
    */
    void reverseCalculationOfSpinning();
    //@}

    /*!
    ** \brief Check and fix all values of a thermal cluster
    **
    ** \return False if an error has been detected and fixed with a default value
    */
    bool integrityCheck() override;

    /*!
    ** \brief Copy data from another cluster
    **
    ** The name and the id will remain untouched.
    */
    void copyFrom(const ThermalCluster& cluster);

    //! \name Memory management
    //@{
    /*!
    ** \brief Flush the memory to swap files (if swap support enabled)
    */
    void flush() override;

    /*!
    ** \brief Group ID as an uint
    */
    uint groupId() const override;

    /*!
    ** \brief Get the memory consummed by the thermal cluster (in bytes)
    */
    Yuni::uint64 memoryUsage() const override;
    //@}

    //! \name validity of Min Stable Power
    //@{
    // bool minStablePowerValidity() const;

    /*!
    ** \brief Calculte the minimum modulation/ceil(modulation) from 8760 capacity modulation
    */
    void calculatMinDivModulation();

    /*!
    ** \brief Check the validity of Min Stable Power
    */
    bool checkMinStablePower();

    /*!
    ** \brief Check the validity of Min Stable Power with a new modulation value
    */
    bool checkMinStablePowerWithNewModulation(uint index, double value);
    //@}

    bool doWeGenerateTS(bool globalThermalTSgeneration) const;

public:
    /*!
    ** \brief The group ID
    **
    ** This value is computed from the field 'group' in 'group()
    ** \see group()
    */
    enum ThermalDispatchableGroup groupID;

    //! Mustrun
    bool mustrun;
    //! Mustrun (as it were at the loading of the data)
    //
    // This value might differ from mustrun, because `mustrun` might be
    // modified for different reasons.
    // Only used by the solver in adequacy mode
    bool mustrunOrigin;

    //! Nominal capacity - spinning (solver only)
    double nominalCapacityWithSpinning;

    //! \name PMin
    //@{
    //! Min. Stable Power (MW)
    double minStablePower;

    struct DivModulation
    {
        DivModulation() : value(0.0), isCalculated(false), isValidated(false)
        {
        }

        double value;
        double border;
        uint index;
        bool isCalculated;
        bool isValidated;
    } minDivModulation;

    //! Min. Up time (1..168)
    uint minUpTime;
    //! Min. Down time (1..168)
    uint minDownTime;
    //! Max entre . minUp/minDown time (1..168)
    uint minUpDownTime;
    //@}

    //! Spinning (%)
    double spinning;

    //! CO2  / MWh
    double co2;

    //! Forced Volatility
    double forcedVolatility;
    //! Planned volatility
    double plannedVolatility;

    //! Law (ts-generator)
    ThermalLaw forcedLaw;
    //! Law (ts-generator)
    ThermalLaw plannedLaw;

    //! \name Costs
    //  Marginal (€/MWh)     MA
    //  Spread (€/MWh)       SP
    //  Fixed (€ / hour)     FI
    //  Start-up (€/start)   SU
    //  Market bid (€/ MWh)  MB
    //
    //  v3.4:
    //  O(h) = MB * P(h)
    //
    //  v3.5:
    //  solver input : MB
    //  output :
    //  O(h) = MA * P(h)
    //  if (P(h) > 0)
    //      O(h) += FI
    //  if (N(h) > N(h-1))
    //      O(h) += SU* (N(h)-N(h-1))
    //
    // \see 101206-antares-couts.doc
    //
    //  v5.0:
    // Abs( SU ) = 0 or in [0.005;5000000]
    //  v4.5:
    // Abs( MA ) = 0 or in [0.005;50000]
    // Abs( FI ) = 0 or in [0.005;50000]
    // Abs( SU ) = 0 or in [0.005;50000]
    // Abs( MB ) = 0 or in [0.005;50000]
    // SP >=0 or in [0.005;50000]
    //
    //@{
    //! Marginal cost (euros/MWh)
    double marginalCost;
    //! Spread (euros/MWh)
    double spreadCost;
    //! Fixed cost (euros/hour)
    double fixedCost;
    //! Startup cost (euros/startup)
    double startupCost;
    //! Market bid cost (euros/MWh)
    double marketBidCost;
    //@}

    //! Minimum number of group
    uint groupMinCount;
    //! Maximum number of group
    uint groupMaxCount;

    //! Annuity investment (kEuros/MW)
    uint annuityInvestment;

    /*!
    ** \brief thermalMinGenModulation vector used in solver only to store the year values
    ** 8760 vector
    ** PthetaInf[hour]
    */
    std::vector<double> PthetaInf;

    //! Data for the preprocessor
    PreproThermal* prepro;

    //! List of all other clusters linked with the current one
    SetPointer coupling;

    //! \name Temporary data for simulation
    //@{
    /*!
    ** \brief Production cost for the thermal cluster
    **
    ** This value is computed from `modulation` and the reference annual cost of
    ** the thermal cluster. The formula is :
    ** \code
    ** each hour (h) in the year do
    **     productionCost[h] = marginalCost * modulation[0][h]
    ** \endcode
    **
    ** This value is only set when loaded from a folder
    ** 8760 (HOURS_PER_YEAR) array
    */
    double* productionCost;

    /*!
    ** \brief The number of units used the last hour in the simulation
    **
    ** \warning This variable is only valid when used from the solver
    */
    uint* unitCountLastHour;

    /*!
    ** \brief The production of the last hour in the simulation
    **
    ** \warning This variable is only valid when used from the solver
    */
    double* productionLastHour;
    /*!
    ** \brief The minimum power of a group of the cluster
    **
    ** \warning This variable is only valid when used from the solver
    ** \Field PminDUnGroupeDuPalierThermique of the PALIERS_THERMIQUES structure
    */
    double* pminOfAGroup;

    LocalTSGenerationBehavior tsGenBehavior = LocalTSGenerationBehavior::useGlobalParameter;

    friend class ThermalClusterList;

private:
    unsigned int precision() const override;
}; // class ThermalCluster
} // namespace Data
} // namespace Antares

#include "cluster.hxx"

#endif /* __ANTARES_LIBS_STUDY_PARTS_THERMAL_CLUSTER_H__ */
