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
#pragma once

#include "state.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
struct VCardAllBindingConstraints
{
    //! Caption
    static const char* Caption()
    {
        return "Binding constraints";
    }
    //! Unit
    static const char* Unit()
    {
        return "";
    }

    //! The short description of the variable
    static const char* Description()
    {
        return "";
    }

    //! The expecte results
    typedef Results<> ResultsType;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::bindingConstraint,
        //! File level (provided by the type of the results)
        categoryFileLevel = ResultsType::categoryFile & Category::bc,
        //! Indentation (GUI)
        nodeDepthForGUI = +1,
        //! Number of columns used by the variable (One ResultsType per column)
        columnCount = 0,
        //! The Spatial aggregation
        spatialAggregate = Category::noSpatialAggregate,
        //! Intermediate values
        hasIntermediateValues = 0,
    };

}; // class VCardAllBindingConstraints

template<class NextT = Container::EndOfList>
class BindingConstraints
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardAllBindingConstraints VCardType;

    //! List of expected results
    typedef typename VCardType::ResultsType ResultsType;

    enum
    {
        //! How many items have we got
        count = NextT::count,
    };

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count = NextType::template Statistics<CDataLevel, CFile>::count
        };
    };

public:
    /*!
    ** \brief Try to estimate the memory footprint that the solver will require to make a simulation
    */
    static void EstimateMemoryUsage(Data::StudyMemoryUsage& u);

    /*!
    ** \brief Retrieve the list of all individual variables
    **
    ** The predicate must implement the method `add(name, unit, comment)`.
    */
    template<class PredicateT>
    static void RetrieveVariableList(PredicateT& predicate);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    BindingConstraints() = default;

    BindingConstraints(BindingConstraints& other) = delete;
    BindingConstraints(BindingConstraints&& other) = delete;
    BindingConstraints const& operator=(BindingConstraints&& other) = delete;

    //! Destructor
    ~BindingConstraints() = default;
    //@}

    void initializeFromStudy(Data::Study& study);

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary);

    void yearBegin(uint year, uint numSpace);
    void yearEnd(uint year, uint numSpace);

    void weekBegin(State& state);

    void hourBegin(uint hourInTheYear);
    void hourEnd(State& state, uint hourInTheYear);

    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const;

    void buildAnnualSurveyReport(SurveyResults& results,
                                 int dataLevel,
                                 int fileLevel,
                                 int precision,
                                 uint numSpace) const;

    Yuni::uint64 memoryUsage() const;

    template<class I>
    static void provideInformations(I& infos);

private:
    // For each binding constraint, output variable static list associated.
    std::vector<NextType> pBindConstraints;
    // The number of counted binding constraints
    uint pBCcount;

}; // class Areas

} // namespace Variable
} // namespace Solver
} // namespace Antares

#include "bindConstraints.hxx"

