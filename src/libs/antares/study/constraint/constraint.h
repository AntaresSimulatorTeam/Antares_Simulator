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
#ifndef __ANTARES_LIBS_STUDY_CONSTRAINT_CONSTRAINT_H__
#define __ANTARES_LIBS_STUDY_CONSTRAINT_CONSTRAINT_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/noncopyable.h>
#include "../fwd.h"
#include "../../constants.h"
#include "../area/links.h"
#include "../parts/thermal/cluster.h"
#include "../../array/matrix.h"
#include "../../inifile/inifile.h"
#include "antares/study/filter.h"
#include <vector>
#include <set>

namespace Antares
{
namespace Data
{
// Forward declaration
struct CompareBindingConstraintName;

class BindingConstraint final : public Yuni::NonCopyable<BindingConstraint>
{
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
    using thermalOffsetIterator = clusterOffsetMap::iterator;
    //! Const iterator
    using thermalOffsetConst_iterator = clusterOffsetMap::const_iterator;

    //! Vector of binding constraints
    using Vector = std::vector<BindingConstraint*>;
    //! Ordered Set of binding constraints
    using Set = std::set<BindingConstraint*, CompareBindingConstraintName>;

    class EnvForLoading final
    {
    public:
        EnvForLoading(AreaList& l, uint v) : areaList(l), version(v)
        {
        }
        //! INI file
        Yuni::Clob iniFilename;
        //! Current section
        IniFile::Section* section;

        Yuni::Clob buffer;
        Matrix<>::BufferType matrixBuffer;
        Yuni::Clob folder;

        //! List of areas
        AreaList& areaList;
        //! Study version
        uint version;
    };

    class EnvForSaving final
    {
    public:
        EnvForSaving()
        {
        }

        //! Current section
        IniFile::Section* section;

        Yuni::Clob folder;
        Yuni::Clob matrixFilename;
        Yuni::CString<2 * (ant_k_area_name_max_length + 8), false> key;
    };

public:
    /*!
    ** \brief Convert a binding constraint type into a mere C-String
    */
    static const char* TypeToCString(const Type t);

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

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    BindingConstraint();
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

    //! \name Values
    //@{
    //! Values for inequalities (const)
    const Matrix<>& matrix() const;
    //! Values for inequalities
    Matrix<>& matrix();

    //@}

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
    void copyWeights(const Study& study, const BindingConstraint& rhs, bool emptyBefore = true);

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
    ** \brief Remove all offsets
    */
    void removeAllOffsets();

    /*!
    ** \brief Copy all offsets from another constraint
    */
    void copyOffsets(const Study& study, const BindingConstraint& rhs, bool emptyBefore = true);

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

    /*!
    ** \brief Remove an interconnection
    */
    bool removeLink(const AreaLink* lnk);
    //@}

    /*!
    ** \brief Remove a thermalcluster
    */
    bool removeCluster(const ThermalCluster* clstr);
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
    bool invalidate(bool reload = false) const;

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
    ** \brief Load the binding constraint from a folder and an INI file
    **
    ** \param env All information needed to perform the task
    ** \return True if the operation succeeded, false otherwise
    */
    bool loadFromEnv(EnvForLoading& env);

    /*!
    ** \brief Save the binding constraint into a folder and an INI file
    **
    ** \param env All information needed to perform the task
    ** \return True if the operation succeeded, false otherwise
    */
    bool saveToEnv(EnvForSaving& env);

    /*!
    ** \brief Reverse the sign of the weight for a given interconnection or thermal cluster
    **
    ** This method is used when reverting an interconnection or thermal cluster
    */
    void reverseWeightSign(const AreaLink* lnk);

    void reverseWeightSign(const ThermalCluster* clstr);

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
    void buildHTMLFormula(YString& s) const;

    void initLinkArrays(double* weigth,
                        double* cWeigth,
                        int* o,
                        int* cO,
                        long* linkIndex,
                        long* clusterIndex,
                        long* clustersAreaIndex) const;

    /*!
    ** \brief Fill the second member matrix with all member to the same value
    */
    void matrix(const double onevalue);

private:
    //! Raw name
    ConstraintName pName;
    //! Raw ID
    ConstraintName pID;
    //! Matrix<> where values for inequalities could be found
    Matrix<> pValues;
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
    uint pFilterYearByYear = filterAll;
    //! Print binding constraint's marginal prices synthesis for which time step granularity ?
    uint pFilterSynthesis = filterAll;
    //! Enabled / Disabled
    bool pEnabled;
    //! Comments
    YString pComments;

}; // class BindingConstraint

class BindConstList final : public Yuni::NonCopyable<BindConstList>
{
public:
    using iterator = BindingConstraint::Vector::iterator;
    using const_iterator = BindingConstraint::Vector::const_iterator;

public:
    //! \name Constructor && Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    BindConstList();
    /*!
    ** \brief Destructor
    */
    ~BindConstList();
    //@}

    /*!
    ** \brief Delete all constraints
    */
    void clear();

    //! \name Iterating through all constraints
    //@{
    /*!
    ** \brief Iterate through all constraints
    */
    template<class PredicateT>
    void each(const PredicateT& predicate);
    /*!
    ** \brief Iterate through all constraints (const)
    */
    template<class PredicateT>
    void each(const PredicateT& predicate) const;

    /*!
    ** \brief Iterate through all enabled constraints
    */
    template<class PredicateT>
    void eachEnabled(const PredicateT& predicate);
    /*!
    ** \brief Iterate through all enabled constraints (const)
    */
    template<class PredicateT>
    void eachEnabled(const PredicateT& predicate) const;

    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

    bool empty() const;
    //@}

    /*!
    ** \brief Add a new binding constraint
    */
    BindingConstraint* add(const AnyString& name);

    /*!
    ** Try to find a constraint from its id
    */
    BindingConstraint* find(const AnyString& id);

    /*!
    ** \brief Try to find a constraint from its id (const)
    */
    const BindingConstraint* find(const AnyString& id) const;

    /*!
    ** \brief Try to find a constraint from its name
    */
    BindingConstraint* findByName(const AnyString& name);

    /*!
    ** \brief Try to find a constraint from its name (const)
    */
    const BindingConstraint* findByName(const AnyString& name) const;

    /*!
    ** \brief Load all binding constraints from a folder
    */
    bool loadFromFolder(Study& s, const StudyLoadOptions& options, const AnyString& folder);

    /*!
    ** \brief Save all binding constraints into a folder
    */
    bool saveToFolder(const AnyString& folder) const;

    /*!
    ** \brief Reverse the sign of the weight for a given interconnection or thermal cluster
    **
    ** This method is used when reverting an interconnection or thermal cluster
    */
    void reverseWeightSign(const AreaLink* lnk);

    void reverseWeightSign(const ThermalCluster* clstr);

    //! Get the number of binding constraints
    uint size() const;

    /*!
    ** \brief Remove a binding constraint
    */
    void remove(const BindingConstraint* bc);
    /*!
    ** \brief Remove any binding constraint linked with a given area
    */
    void remove(const Area* area);
    /*!
    ** \brief Remove any binding constraint linked with a given interconnection
    */
    void remove(const AreaLink* area);

    /*!
    ** \brief Remove any binding constraint whose name contains the string in argument
    */
    void removeConstraintsWhoseNameConstains(const AnyString& filter);

    /*!
    ** \brief Rename a binding constraint
    */
    bool rename(BindingConstraint* bc, const AnyString& newname);

    /*!
    ** \brief Convert all weekly constraints into daily ones
    */
    void mutateWeeklyConstraintsIntoDailyOnes();

    /*!
    ** \brief Get the memory usage
    */
    yuint64 memoryUsage() const;

    /*!
    ** \brief Estimate
    */
    void estimateMemoryUsage(StudyMemoryUsage& u) const;

    /*!
    ** \brief Invalidate all matrices of all binding constraints
    */
    bool invalidate(bool reload = false) const;

    /*!
    ** \brief Mark the constraint as modified
    */
    void markAsModified() const;

private:
    bool internalSaveToFolder(BindingConstraint::EnvForSaving& env) const;

private:
    //! All constraints
    BindingConstraint::Vector pList;

}; // class BindConstList

struct CompareBindingConstraintName final
{
    bool operator()(const BindingConstraint* s1, const BindingConstraint* s2) const
    {
        return ((s1->name()) < (s2->name()));
    }
};

struct WhoseNameContains final
{
public:
    WhoseNameContains(const AnyString& filter) : pFilter(filter)
    {
    }
    bool operator()(const BindingConstraint* s) const
    {
        return (s->name()).contains(pFilter);
    }

private:
    AnyString pFilter;
};

} // namespace Data
} // namespace Antares

#include "constraint.hxx"

#endif // __ANTARES_LIBS_STUDY_CONSTRAINT_CONSTRAINT_H__
