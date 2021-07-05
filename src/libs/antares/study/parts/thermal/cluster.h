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
#include "series.h"
#include "../../fwd.h"
#include <set>
#include <map>
#include <vector>

namespace Antares
{
namespace Data
{
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
    thermalDispatchGrpMax
};

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

struct CompareThermalClusterName;

/*!
** \brief A single thermal cluster
*/
class ThermalCluster final : public Yuni::NonCopyable<ThermalCluster>
{
public:
    //! Set of thermal clusters
    typedef std::set<ThermalCluster*, CompareThermalClusterName> Set;
    //! Set of thermal clusters (pointer)
    typedef std::set<ThermalCluster*> SetPointer;
    //! Map of thermal clusters
    typedef std::map<ThermalClusterName, ThermalCluster*> Map;
    //! Vector of thermal clusters
    typedef std::vector<Data::ThermalCluster*> Vector;

public:
    /*!
    ** \brief Get the group name string
    ** \return A valid CString
    */
    static const char* GroupName(enum ThermalDispatchableGroup grp);

    /*!
    ** \brief Get if a value us valid for a flexibility
    */
    static bool FlexibilityIsValid(uint f);

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
    bool invalidate(bool reload) const;

    /*!
    ** \brief Mark the thermal cluster as modified
    */
    void markAsModified() const;

    /*!
    ** \brief Invalidate the whole attached area
    */
    void invalidateArea();

    /*!
    ** \brief Reset to default values
    **
    ** This method should only be called from the GUI
    */
    void reset();

    //! \name Name and ID
    //@{
    //! Get the thermal cluster ID
    const Data::ThermalClusterName& id() const;

    //! Get the thermal cluster name
    const Data::ThermalClusterName& name() const;

    //! Get the full thermal cluster name
    Yuni::String getFullName() const;

    //! Set the name/ID
    void name(const AnyString& newname);
    //@}

    //! \name Group
    //@{
    //! Get the group of the cluster
    const Data::ThermalClusterName& group() const;
    //! Set the group
    void group(Data::ThermalClusterName newgrp);
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
    bool integrityCheck();

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
    void flush();

    /*!
    ** \brief Get the memory consummed by the thermal cluster (in bytes)
    */
    Yuni::uint64 memoryUsage() const;
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

    /*!
    ** \brief Check wether the cluster is visible in a layer (it's parent area is visible in the
    *layer)
    */
    bool isVisibleOnLayer(const size_t& layerID) const;

public:
    /*!
    ** \brief The group ID
    **
    ** This value is computed from the field 'group' in 'group()
    ** \see group()
    */
    enum ThermalDispatchableGroup groupID;

    //! The index of the thermal cluster (within a list)
    uint index;
    //! The index of the thermal cluster from the area's point of view
    //! \warning this variable is only valid when used by the solver
    // (initialized in the same time that the runtime data)
    uint areaWideIndex;

    //! The associate area (alias)
    Area* parentArea;

    //! Enabled
    bool enabled;
    //! Mustrun
    bool mustrun;
    //! Mustrun (as it were at the loading of the data)
    //
    // This value might differ from mustrun, because `mustrun` might be
    // modified for different reasons.
    // Only used by the solver in adequacy mode
    bool mustrunOrigin;

    //! Count of unit
    uint unitCount;

    //! Capacity of reference per unit (MW) (pMax)
    double nominalCapacity;
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
    ** \brief Modulation matrix
    **
    ** It is merely a 3x8760 matrix
    ** [modulation cost, modulation capacity, market bid modulation] per hour
    */
    Matrix<> modulation;

    /*!
    ** \brief thermalMinGenModulation vector used in solver only to store the year values
    ** 8760 vector
    ** PthetaInf[hour]
    */
    std::vector<double> PthetaInf;

    //! Data for the preprocessor
    PreproThermal* prepro;
    //! Series
    DataSeriesThermal* series;

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

private:
    //! Name
    Data::ThermalClusterName pName;
    //! ID
    Data::ThermalClusterName pID;
    //! Group
    Data::ThermalClusterName pGroup;

    friend class ThermalClusterList;

}; // class ThermalCluster

/*!
** \brief List of clusters
** \ingroup thermalclusters
*/
class ThermalClusterList
{
public:
    //! iterator
    typedef ThermalCluster::Map::iterator iterator;
    //! const iterator
    typedef ThermalCluster::Map::const_iterator const_iterator;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    ThermalClusterList();
    /*!
    ** \brief Destructor
    */
    ~ThermalClusterList();
    //@}

    //! \name Iterating
    //@{
    /*!
    ** \brief Iterate through all thermal clusters
    */
    template<class PredicateT>
    void each(const PredicateT& predicate);
    /*!
    ** \brief Iterate through all thermal clusters (const)
    */
    template<class PredicateT>
    void each(const PredicateT& predicate) const;

    //! iterator to the begining of the list
    iterator begin();
    //! iterator to the begining of the list
    const_iterator begin() const;

    //! iterator to the end of the list
    iterator end();
    //! iterator to the end of the list
    const_iterator end() const;

    //@}

    //! \name Cluster management
    //@{
    /*!
    ** \brief Destroy all thermal clusters
    */
    void clear();

    /*!
    ** \brief Add a thermal cluster in the list
    **
    ** \param t The thermal cluster to add
    ** \return True if the thermal cluster has been added, false otherwise
    */
    bool add(ThermalCluster* t);

    /*!
    ** \brief Detach a cluster represented by an iterator
    **
    ** The thermal cluster will be removed from the list but _not_
    ** destroyed.
    ** The iterator should considered as invalid after using this method.
    ** \return A pointer to the thermal cluster, NULL if an error has occured
    */
    ThermalCluster* detach(iterator i);

    /*!
    ** \brief Remove a cluster represented by an iterator
    **
    ** The thermal cluster will be removed from the list but _not_
    ** destroyed.
    ** The iterator should considered as invalid after using this method.
    ** \return void
    */
    void remove(iterator i);

    /*!
    ** \brief Load a list of thermal cluster from a folder
    **
    ** \param folder The folder where the data are located (ex: `input/thermal/clusters/[area]/`)
    ** \param area The associate area
    ** \return True if the operation succeeded, false otherwise
    */
    bool loadFromFolder(Study& s, const AnyString& folder, Area* area);

    /*!
    ** \brief Try to find a thermal cluster from its id
    **
    ** \param id ID of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    ThermalCluster* find(const Data::ThermalClusterName& id);
    /*!
    ** \brief Try to find a thermal cluster from its id (const)
    **
    ** \param id ID of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    const ThermalCluster* find(const Data::ThermalClusterName& id) const;

    /*!
    ** \brief Try to find a thermal cluster from its pointer
    **
    ** \param  p Pointer of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    ThermalCluster* find(const ThermalCluster* p);
    /*!
    ** \brief Try to find a thermal cluster from its pointer (const)
    **
    ** \param  p Pointer of the cluster to find
    ** \return A pointer to a cluster. nullptr if not found
    */
    const ThermalCluster* find(const ThermalCluster* p) const;

    /*!
    ** \brief Get if a thermal cluster exists
    **
    ** \param id ID of the cluster to find
    ** \return True if the thermal cluster exists
    */
    bool exists(const Data::ThermalClusterName& id) const;

    /*!
    ** \brief Rename a thermal cluster
    **
    ** \param idToFind ID of the cluster to rename
    ** \param newName The new name for the cluster
    ** \return True if the operation succeeded (the cluster has been renammed)
    **   false otherwise (not found or if another thermal cluster has the same name)
    **
    ** The indexes for thermal clusters will be rebuilt.
    */
    bool rename(Data::ThermalClusterName idToFind, Data::ThermalClusterName newName);

    /*!
    ** \brief Remove properly a cluster
    */
    bool remove(const Data::ThermalClusterName& id);

    //! Get the number of items in the list
    uint size() const;

    //! Get if the list is empty
    bool empty() const;
    //@}

    //! \name Spinning
    //@{
    /*!
    ** \brief Calculation of Spinning for all thermal clusters
    */
    void calculationOfSpinning();

    /*!
    ** \brief Calculation of Spinning for all thermal clusters (reverse)
    */
    void reverseCalculationOfSpinning();
    //@}

    /*!
    ** \brief Resize all matrices dedicated to the sampled timeseries numbers
    **
    ** \param n A number of years
    */
    void resizeAllTimeseriesNumbers(uint n);

    bool storeTimeseriesNumbers(Study& study);

    //! \name Mustrun
    //@{
    /*!
    ** \brief Enable the 'mustrun' mode for every cluster
    **
    ** This method is especially useful for the adequacy mode, where all clusters
    ** must be in mustrun mode
    */
    void enableMustrunForEveryone();
    //@}

    //! \name Informations
    //@{
    /*!
    ** \brief Retrieve the total capacity and the total unit count
    **
    ** Pseudo code:
    ** \code
    ** each thermal cluster do
    ** 	total += cluster{unit count} * cluster{nominal capacity}
    **	unit  += cluster{unit count}
    ** \endcode
    */
    void retrieveTotalCapacityAndUnitCount(double& total, uint& unitCount) const;
    //@}

    //! \name Memory management
    //@{
    /*!
    ** \brief Invalidate all thermal clusters
    */
    bool invalidate(bool reload = false) const;

    /*!
    ** \brief Mark the thermal cluster as modified
    */
    void markAsModified() const;

    /*!
    ** \brief Flush memory to swap files
    */
    void flush();

    /*!
    ** \brief Rebuild the index of thermal clusters
    **
    ** As a list of thermal clusters is a hash table, it is not
    ** possible to directly accees to a thermal cluster from its index.
    ** However an index can be built but it must be re-built when
    ** the hash table is modified.
    */
    void rebuildIndex();

    /*!
    ** \brief Get the size (bytes) occupied in memory by a `ThermalClusterList` structure
    ** \return A size (in bytes)
    */
    Yuni::uint64 memoryUsage() const;

    /*!
    ** \brief Try to estimate the amount of memory which will be used by the solver
    */
    void estimateMemoryUsage(StudyMemoryUsage&) const;
    //@}

public:
    //! All thermal clusters by their index
    ThermalCluster** byIndex;
    //! All thermal clusters
    ThermalCluster::Map cluster;

    /*!
    ** \brief Number of thermal dispatchable cluster per group
    **
    ** You should rely on these values only after the loading of the study
    ** and until the study is not modified.
    ** These values are modified by 'ThermalClusterListAdd()'
    */
    uint groupCount[thermalDispatchGrpMax];

    /*!
    ** \brief Original mapping from the input
    **
    ** Without taking into consideration the enabled or mustrun flags
    */
    ThermalCluster::Map mapping;

}; // class ThermalClusterList

/*!
** \brief Ensure data for the prepro are initialized
** \ingroup thermalclusters
**
** \todo Remaining of old C-library. this routine should be moved into the appropriate class
** \param l The list of thermal clusters
*/
void ThermalClusterListEnsureDataPrepro(ThermalClusterList* l);

/*!
** \brief Ensure data for the time-series are initialized
** \ingroup thermalclusters
**
** \todo Remaining of old C-library. this routine should be moved into the appropriate class
** \param l The list of thermal clusters
*/
void ThermalClusterListEnsureDataTimeSeries(ThermalClusterList* l);

/*!
** \brief Save a list of thermal clusters to a folder
** \ingroup thermalclusters
**
** \todo Remaining of old C-library. this routine should be moved into the appropriate class
** \param l The list of thermal cluster
** \param folder The folder where the data will be written
** \return A non-zero value if the operation succeeded, 0 otherwise
*/
bool ThermalClusterListSaveToFolder(const ThermalClusterList* l, const AnyString& folder);

/*!
** \brief Load data related to the preprocessor from a list of thermal clusters from a folder
** \ingroup thermalclusters
**
** \todo Remaining of old C-library. this routine should be moved into the appropriate class
** \param l A list of thermal clusters
** \param folder The target folder
** \return A non-zero value if the operation succeeded, 0 otherwise
*/
bool ThermalClusterListLoadPreproFromFolder(Study& s,
                                            const StudyLoadOptions& options,
                                            ThermalClusterList* l,
                                            const AnyString& folder);

/*!
** \brief Save data related to the preprocessor from a list of thermal clusters to a folder
** \ingroup thermalclusters
**
** \todo Remaining of old C-library. this routine should be moved into the appropriate class
** \param l A list of thermal clusters
** \param folder The target folder
** \return A non-zero value if the operation succeeded, 0 otherwise
*/
bool ThermalClusterListSavePreproToFolder(const ThermalClusterList* l, const AnyString& folder);

/*!
** \brief Load data related to time-series from a list of thermal clusters from a folder
** \ingroup thermalclusters
**
** \todo Remaining of old C-library. this routine should be moved into the appropriate class
** \param l A list of thermal clusters
** \param folder The target folder
** \return A non-zero value if the operation succeeded, 0 otherwise
*/
int ThermalClusterListLoadDataSeriesFromFolder(Study& study,
                                               const StudyLoadOptions& options,
                                               ThermalClusterList* l,
                                               const AnyString& folder,
                                               int fast);

/*!
** \brief Save data related to time-series from a list of thermal clusters to a folder
** \ingroup thermalclusters
**
** \todo Remaining of old C-library. this routine should be moved into the appropriate class
** \param l A list of thermal clusters
** \param folder The target folder
** \param msg Message to display
** \return A non-zero value if the operation succeeded, 0 otherwise
*/
int ThermalClusterListSaveDataSeriesToFolder(const ThermalClusterList* l, const AnyString& folder);
int ThermalClusterListSaveDataSeriesToFolder(const ThermalClusterList* l,
                                             const AnyString& folder,
                                             const YString& msg);

struct CompareThermalClusterName final
{
    inline bool operator()(const ThermalCluster* s1, const ThermalCluster* s2) const
    {
        return (s1->getFullName() < s2->getFullName());
    }
};

} // namespace Data
} // namespace Antares

#include "cluster.hxx"
//# include "../../area.h"

#endif /* __ANTARES_LIBS_STUDY_PARTS_THERMAL_CLUSTER_H__ */
