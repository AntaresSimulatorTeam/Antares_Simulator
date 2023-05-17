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
#pragma once

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/noncopyable.h>
#include "../fwd.h"
#include "../../constants.h"
#include "../area/links.h"
#include "../parts/thermal/cluster.h"
#include "../../array/matrix.h"
#include "../../inifile/inifile.h"
#include "EnvForLoading.h"
#include "antares/study/filter.h"
#include "BindingConstraintTimeSeries.h"
#include "BindingConstraintTimeSeriesNumbers.h"
#include <memory>
#include <utility>
#include <vector>
#include <set>

namespace Antares::Data
{
// Forward declaration
struct CompareBindingConstraintName;

class BindingConstraint final : public Yuni::NonCopyable<BindingConstraint>
{
    friend class BindingConstraintLoader;
    friend class BindingConstraintSaver;
public:
    enum Type
    {
        //! Unknown status
        typeUnknown = 0,
        //! The binding constraint has hourly values
        typeHourly,
        //! The binding constraint has daily values
        typeDaily,
        //! The binding constraint has weekly values
        typeWeekly,
        //! The maximum number of types
        typeMax
    };
    enum Operator
    {
        opUnknown = 0,
        opEquality,
        opLess,
        opGreater,
        opBoth,
        opMax
    };
    enum Column
    {
        columnInferior = 0,
        columnSuperior,
        columnEquality,
        //! The maximum number of columns which can be found in the value matrix
        columnMax,
    };

    //! Map of weight (for links)
    using linkWeightMap = std::map<const AreaLink*, double, CompareLinkName>;
    //! Iterator
    using iterator = linkWeightMap::iterator;
    //! Const iterator
    using const_iterator = linkWeightMap::const_iterator;

    //! Map of weight (for thermal clusters)
    using clusterWeightMap = std::map<const ThermalCluster*, double, CompareClusterName>;
    //! Iterator
    using thermalIterator = clusterWeightMap::iterator;
    //! Const iterator
    using const_thermalIterator = clusterWeightMap::const_iterator;

    //! Map of offset (for links)
    using linkOffsetMap = std::map<const AreaLink*, int>;
    //! Iterator
    using OffsetIterator = linkOffsetMap::iterator;
    //! Const iterator
    using OffsetConst_iterator = linkOffsetMap::const_iterator;

    //! Map of offset (for links)
    using clusterOffsetMap = std::map<const ThermalCluster*, int>;
    //! Iterator
    //! Const iterator

    //! Vector of binding constraints
    using Vector = std::vector<std::shared_ptr<BindingConstraint>>;
    //! Ordered Set of binding constraints
    using Set = std::set<std::shared_ptr<BindingConstraint>, CompareBindingConstraintName>;

    /*!
    ** \brief Convert a binding constraint type into a mere C-String
    */
    static const char* TypeToCString(Type t);

    /*!
    ** \brief Convert a string into its corresponding type
    */
    static Type StringToType(const AnyString& text);

    /*!
    ** \brief Convert a binding constraint operator into a mere C-String
    */
    static const char* OperatorToCString(Operator o);
    /*!
    ** \brief Convert a binding constraint operator into a short C-String
    */
    static const char* OperatorToShortCString(Operator o);

    /*!
    ** \brief Convert a binding constraint operator into a short C-String (symbol)
    */
    static const char* MathOperatorToCString(Operator o);

    /*!
    ** \brief Converter a raw string into an operator
    */
    static Operator StringToOperator(const AnyString& text);

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Destructor
    */
    ~BindingConstraint();
    //@}

    //! \name / ID
    //@{
    /*!
    ** \brief Get the name of the binding constraint
    */
    const ConstraintName& name() const;
    /*!
    ** \brief Set the name of the binding constraint
    **
    ** The ID will be changed in the same time
    */
    void name(const AnyString& newname);

    /*!
    ** \brief Get the ID of the binding constraint
    */
    const ConstraintName& id() const;
    //@}

    //! \name Comments
    //@{
    /*!
    ** \brief Get the comments
    */
    const YString& comments() const;

    //! \name Group
    //@{
    /*!
    ** \brief Get the group
    */
    std::string group() const;
    void group(std::string group_name);

    /*!
    ** \brief Set the comments
    */
    void comments(const AnyString& newcomments);
    //@}

    void resetToDefaultValues();

    //! \name iterator
    //@{
    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;
    //@}

    bool skipped() const;

    //Ref to prevent copy. const ref to prevent modification.
    const Matrix<>& TimeSeries() const;
    Matrix<> &TimeSeries();


    bool hasAllWeightedLinksOnLayer(size_t layerID);

    bool hasAllWeightedClustersOnLayer(size_t layerID);

    //! \name Links
    //@{
    /*!
    ** \brief Get the weight of a given interconnection
    **
    ** \return The weight of the interconnection. 0. if not found
    */
    double weight(const AreaLink* lnk) const;

    /*!
    ** \brief Set the weight of an interconnection
    */
    void weight(const AreaLink* lnk, double w);

    //! \name Thermal clusters
    //@{
    /*!
    ** \brief Get the weight of a given thermal cluster
    **
    ** \return The weight of the thermal cluster. 0. if not found
    */
    double weight(const ThermalCluster* clstr) const;

    /*!
    ** \brief Set the weight of a thermal cluster
    */
    void weight(const ThermalCluster* clstr, double w);

    /*!
    ** \brief Remove all weights
    */
    void removeAllWeights();

    /*!
    ** \brief Copy all weights from another constraint
    */
    void copyWeights(const Study& study,
                     const BindingConstraint& rhs,
                     bool emptyBefore,
                     Yuni::Bind<void(AreaName&, const AreaName&)>& translate);

    /*!
    ** \brief Get the offset of a given interconnection
    **
    ** \return The offset of the interconnection. 0. if not found
    */
    int offset(const AreaLink* lnk) const;

    /*!
    ** \brief Get the offset of a given thermal cluster
    **
    ** \return The offset of the cluster. 0. if not found
    */
    int offset(const ThermalCluster* lnk) const;

    /*!
    ** \brief Set the weight of an interconnection
    */
    void offset(const AreaLink* lnk, int o);

    /*!
    ** \brief Set the weight of a thermal cluster
    */
    void offset(const ThermalCluster* clstr, int o);

    /*!
    ** \brief Copy all offsets from another constraint
    */
    void copyOffsets(const Study& study,
                     const BindingConstraint& rhs,
                     bool emptyBefore,
                     Yuni::Bind<void(AreaName&, const AreaName&)>& translate);

    /*!
    ** \brief Get how many links the binding constraint contains
    */
    uint linkCount() const;

    /*!
    ** \brief Get how many thermal clusters the binding constraint contains
    */
    uint clusterCount() const;

    /*!
    ** \brief Get how many thermal clusters the binding constraint contains
    */
    uint enabledClusterCount() const;

    //@}

    //@}

    //! \name Type of the binding constraint
    //@{
    /*!
    ** \brief Get the type of the binding constraint
    */
    Type type() const;

    /*!
    ** \brief Set the type of the binding constraint
    */
    void mutateTypeWithoutCheck(Type t);
    //@}

    //! \name Operator
    //@{
    Operator operatorType() const;
    void operatorType(Operator o);
    //@}

    uint yearByYearFilter() const;
    uint synthesisFilter() const;

    //! \name Enabled / Disabled
    //@{
    //! Get if the binding constraint is enabled
    bool enabled() const;
    //! Enabled / Disabled the binding constraint
    void enabled(bool v);
    //@}

    //! \name Reset
    //@{
    /*!
    ** \brief Clear all values and reset the binding constraint to its new type
    **
    ** \param name Name of the binding constraint
    ** \param newType Its new type (hourly, daily...)
    */
    void clearAndReset(const AnyString& name, Type newType, Operator op);
    //@}

    /*!
    ** \brief Invalidate all matrices
    */
    bool forceReload(bool reload = false) const;

    /*!
    ** \brief Mark the constraint as modified
    */
    void markAsModified() const;

    //! \name Memory Usage
    //@{
    /*!
    ** \brief Get the memory usage
    */
    Yuni::uint64 memoryUsage() const;
    //@}

    /*!
    ** \brief Reverse the sign of the weight for a given interconnection or thermal cluster
    **
    ** This method is used when reverting an interconnection or thermal cluster
    */
    void reverseWeightSign(const AreaLink* lnk);

    /*!
    ** \brief Get if the given binding constraint is identical
    */
    bool contains(const BindingConstraint* bc) const;
    /*!
    ** \brief Get if the binding constraint is linked with a given area
    */
    bool contains(const Area* area) const;

    /*!
    ** \brief Get if the binding constraint is linked with an interconnection or thermal cluster
    */
    bool contains(const AreaLink* lnk) const;

    bool contains(const ThermalCluster* clstr) const;

    /*!
    ** \brief Build a human readable formula for the binding constraint
    */
    void buildFormula(YString& s) const;

    void initLinkArrays(std::vector<double>& weigth,
                        std::vector<double>& cWeigth,
                        std::vector<int>& o,
                        std::vector<int>& cO,
                        std::vector<long>& linkIndex,
                        std::vector<long>& clusterIndex,
                        std::vector<long>& clustersAreaIndex) const;

    template<class Env>
    std::string timeSeriesFileName(const Env &env) const;


private:
    //! Raw name
    ConstraintName pName;
    //! Raw ID
    ConstraintName pID;
    //! Time series of the binding constraint. Width = number of series. Height = nbTimeSteps. Only store series for operatorType
    Matrix<> time_series;
    //! Weights for links
    linkWeightMap pLinkWeights;
    //! Weights for thermal clusters
    clusterWeightMap pClusterWeights;
    //! Link Offsets
    linkOffsetMap pLinkOffsets;
    //! Thermal Offsets
    clusterOffsetMap pClusterOffsets;
    //! Type of the binding constraint
    Type pType;
    //! Operator
    Operator pOperator;
    //! Print binding constraint's marginal prices of any year for which time step granularity ?
    // By default, print nothing
    uint pFilterYearByYear = filterNone;
    //! Print binding constraint's marginal prices synthesis for which time step granularity ?
    // By default, print nothing
    uint pFilterSynthesis = filterNone;
    //! Enabled / Disabled
    bool pEnabled = false;
    //! Comments
    YString pComments;
    //! Group
    std::string group_;

    void clear();

    void copyFrom(BindingConstraint const *original);
};;; // class BindingConstraint

// class BindConstList

struct CompareBindingConstraintName final
{
    bool operator()(const std::shared_ptr<BindingConstraint>& s1, const std::shared_ptr<BindingConstraint>& s2) const
    {
        return s1->name() < s2->name();
    }
};

} // namespace Antares::Data

#include "BindingConstraint.hxx"