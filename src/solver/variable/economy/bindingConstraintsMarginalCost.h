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

#include "../variable.h"
#include "antares/study/constraint/constraint.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{
struct VCardBindingConstMarginCost
{
    //! Caption
    static const char* Caption()
    {
        return "MARG. COST by BC";
    }
    //! Unit
    static const char* Unit()
    {
        return "€/MW";
    }

    //! The short description of the variable
    static const char* Description()
    {
        return "Marginal cost for binding constraints";
    }

    //! The expecte results
    typedef Results<R::AllYears::Average< // The average values thoughout all years
      R::AllYears::StdDeviation<          // The standard deviation values throughout all years
        R::AllYears::Min<                 // The minimum values thoughout all years
          R::AllYears::Max<               // The maximum values thoughout all years
            >>>>>
      ResultsType;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::bindingConstraint,
        //! File level (provided by the type of the results)
        categoryFileLevel = ResultsType::categoryFile & (Category::bc),
        //! Precision (views)
        precision = Category::all,
        //! Indentation (GUI)
        nodeDepthForGUI = +0,
        //! Decimal precision
        decimal = 0,
        //! Number of columns used by the variable
        columnCount = 1,
        //! Intermediate values
        hasIntermediateValues = 1,
        //! Can this variable be non applicable (0 : no, 1 : yes)
        isPossiblyNonApplicable = 0,
    };

    typedef IntermediateValues IntermediateValuesBaseType;
    typedef IntermediateValues* IntermediateValuesType;

}; // class VCard

/*
    Marginal cost associated to binding constraints :
    Suppose that the BC is hourly,
    - if binding constraint is not saturated (rhs is not reached) for a given hour, the value is 0;
    - if binding constraint is saturated (rhs is reached), the value is the total benefit (€/MW) for
   the system that would result in increasing the BC's rhs of 1 MW.
*/
template<class NextT = Container::EndOfList>
class BindingConstMarginCost
 : public Variable::IVariable<BindingConstMarginCost<NextT>, NextT, VCardBindingConstMarginCost>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardBindingConstMarginCost VCardType;
    //! Ancestor
    typedef Variable::IVariable<BindingConstMarginCost<NextT>, NextT, VCardType> AncestorType;

    //! List of expected results
    typedef typename VCardType::ResultsType ResultsType;

    typedef VariableAccessor<ResultsType, VCardType::columnCount> VariableAccessorType;

    enum
    {
        //! How many items have we got
        count = 1 + NextT::count,
    };

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        // gp : correct count if needed
        enum
        {
            count
            = ((VCardType::categoryDataLevel & CDataLevel && VCardType::categoryFileLevel & CFile)
                 ? (NextType::template Statistics<CDataLevel, CFile>::count
                    + VCardType::columnCount * ResultsType::count)
                 : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };

    static void EstimateMemoryUsage(Data::StudyMemoryUsage& u)
    {
        /*
            gp : estimate memory
        */
        NextType::EstimateMemoryUsage(u);
    }

public:
    BindingConstMarginCost() : pValuesForTheCurrentYear(nullptr)
    {
    }

    ~BindingConstMarginCost()
    {
        delete[] pValuesForTheCurrentYear;
    }

    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        // Statistics thoughout all years
        InitializeResultsFromStudy(AncestorType::pResults, study);

        // Intermediate values
        pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            pValuesForTheCurrentYear[numSpace].initializeFromStudy(study);

        NextType::initializeFromStudy(study);
    }

    void setBindConstraint(Data::BindingConstraintRTI* bc)
    {
        associatedBC_ = bc;

        // In case the current class is followed by another class related to a binding
        // constraint in the higher level static list, we'll have to add :
        //      NextType::setBindConstraint(bc);
    }

    void setBindConstraintGlobalNumber(uint bcNumber)
    {
        bindConstraintGlobalNumber_ = bcNumber;
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        // Reset the values for the current year
        pValuesForTheCurrentYear[numSpace].reset();

        // Store the year memory space for further use
        yearMemorySpace_ = numSpace;

        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        // Compute all statistics for the current year (daily,weekly,monthly)
        if (associatedBC_->type == Data::BindingConstraint::typeHourly)
            pValuesForTheCurrentYear[numSpace].computeAVGstatisticsForCurrentYear();

        if (associatedBC_->type == Data::BindingConstraint::typeDaily)
            pValuesForTheCurrentYear[numSpace].computeAnnualAveragesFromDailyValues();

        if (associatedBC_->type == Data::BindingConstraint::typeWeekly)
            pValuesForTheCurrentYear[numSpace].computeAnnualAveragesFromWeeklyValues();

        // Next variable
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
        for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
        {
            // Merge all those values with the global results
            AncestorType::pResults.merge(numSpaceToYear[numSpace] /*year*/,
                                         pValuesForTheCurrentYear[numSpace]);
        }

        // Next variable
        NextType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }

    void weekBegin(State& state)
    {
        // Storing a reference to the state, useful in method called later.
        state_ = &state;

        // For daily and weekly binding constraints, getting marginal price
        if (associatedBC_->type == Data::BindingConstraint::typeDaily)
        {
            int dayInTheYear = state_->weekInTheYear * 7;
            for (int dayInTheWeek = 0; dayInTheWeek < 7; dayInTheWeek++)
            {
                pValuesForTheCurrentYear[yearMemorySpace_].day[dayInTheYear] 
                    -= state_->problemeHebdo
                       ->ResultatsContraintesCouplantes[bindConstraintGlobalNumber_]
                       .variablesDuales[dayInTheWeek];

                dayInTheYear++;
            }
        }

        if (associatedBC_->type == Data::BindingConstraint::typeWeekly)
        {
            uint weekInTheYear = state_->weekInTheYear;
            pValuesForTheCurrentYear[yearMemorySpace_].week[weekInTheYear]
              -= state_->problemeHebdo->ResultatsContraintesCouplantes[bindConstraintGlobalNumber_]
                  .variablesDuales[0];
        }
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        if (associatedBC_->type == Data::BindingConstraint::typeHourly)
        {
            pValuesForTheCurrentYear[yearMemorySpace_][hourInTheYear]
              -= state_->problemeHebdo->ResultatsContraintesCouplantes[bindConstraintGlobalNumber_]
                   .variablesDuales[state_->hourInTheWeek];
        }

        // Next variable
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    void hourEnd(State& state, unsigned int hourInTheYear)
    {
        NextType::hourEnd(state, hourInTheYear);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int column,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int numSpace) const
    {
        // Initializing external pointer on current variable non applicable status
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (AncestorType::isPrinted[0])
        {
            // Write the data for the current year
            results.variableCaption = VCardType::Caption();
            pValuesForTheCurrentYear[numSpace].template buildAnnualSurveyReport<VCardType>(
              results, fileLevel, precision);
        }
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    unsigned int pNbYearsParallel;
    Data::BindingConstraintRTI* associatedBC_ = nullptr;
    State* state_ = nullptr;
    uint yearMemorySpace_ = 0;
    uint bindConstraintGlobalNumber_ = 0;

}; // class BindingConstMarginCost

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares
