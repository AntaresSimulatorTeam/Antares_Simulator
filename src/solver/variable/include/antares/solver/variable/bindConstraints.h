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
    static std::string Caption()
    {
        return "Binding constraints";
    }
    //! Unit
    static std::string Unit()
    {
        return "";
    }

    //! The short description of the variable
    static std::string Description()
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

    uint64_t memoryUsage() const;

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
