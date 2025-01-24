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

#include "antares/study/binding_constraint/BindingConstraint.h"

#include "../../variable.h"

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
    static std::string Caption()
    {
        return "BC. MARG. COST";
    }

    //! Unit
    static std::string Unit()
    {
        return "Euro";
    }

    //! The short description of the variable
    static std::string Description()
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

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::bindingConstraint;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::bc);
    //! Precision (views)
    static constexpr uint8_t precision = Category::all;
    //! Indentation (GUI)
    static constexpr uint8_t nodeDepthForGUI = +0;
    //! Decimal precision
    static constexpr uint8_t decimal = 2;
    //! Number of columns used by the variable
    static constexpr int columnCount = 1;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues = 1;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 1;

    typedef IntermediateValues IntermediateValuesBaseType;
    typedef std::vector<IntermediateValues> IntermediateValuesType;

}; // class VCard

/*
    Marginal cost associated to binding constraints :
    Suppose that the BC is hourly,
    - if binding constraint is not saturated (rhs is not reached) for a given hour, the value is 0;
    - if binding constraint is saturated (rhs is reached), the value is the total benefit (�/MW) for
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
        enum
        {
            count = ((VCardType::categoryDataLevel & CDataLevel
                      && VCardType::categoryFileLevel & CFile)
                       ? (NextType::template Statistics<CDataLevel, CFile>::count
                          + VCardType::columnCount * ResultsType::count)
                       : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };

public:
    BindingConstMarginCost() = default;

    void simulationBegin()
    {
        NextType::simulationBegin();
    }

    void simulationEnd()
    {
        NextType::simulationEnd();
    }

    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        // Statistics thoughout all years
        InitializeResultsFromStudy(AncestorType::pResults, study);

        // Intermediate values
        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].initializeFromStudy(study);
        }

        NextType::initializeFromStudy(study);
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        VariableAccessorType::InitializeAndReset(results, study);
    }

    void setAssociatedBindConstraint(std::shared_ptr<Data::BindingConstraint> bc_ptr)
    {
        associatedBC_ = bc_ptr;
    }

    void setBindConstraintsCount(uint bcCount)
    {
        nbCount_ = bcCount;
    }

    size_t getMaxNumberColumns() const
    {
        return nbCount_ * ResultsType::count;
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        // Reset the values for the current year
        pValuesForTheCurrentYear[numSpace].reset();

        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        if (isInitialized())
        {
            // Compute statistics for the current year depending on
            // the BC type (hourly, daily, weekly)
            using namespace Data;
            switch (associatedBC_->type())
            {
            case BindingConstraint::typeHourly:
                pValuesForTheCurrentYear[numSpace].computeAveragesForCurrentYearFromHourlyResults();
                break;
            case BindingConstraint::typeDaily:
                pValuesForTheCurrentYear[numSpace].computeAveragesForCurrentYearFromDailyResults();
                break;
            case BindingConstraint::typeWeekly:
                pValuesForTheCurrentYear[numSpace].computeAveragesForCurrentYearFromWeeklyResults();
                break;
            case BindingConstraint::typeUnknown:
            case BindingConstraint::typeMax:
                break;
            }
        }

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
        if (isInitialized())
        {
            auto numSpace = state.numSpace;
            // For daily binding constraints, getting daily marginal price
            using namespace Data;
            switch (associatedBC_->type())
            {
            case BindingConstraint::typeHourly:
            case BindingConstraint::typeUnknown:
            case BindingConstraint::typeMax:
                break;

            case BindingConstraint::typeDaily:
            {
                int dayInTheYear = state.weekInTheYear * 7;
                for (int dayInTheWeek = 0; dayInTheWeek < 7; dayInTheWeek++)
                {
                    pValuesForTheCurrentYear[numSpace].day[dayInTheYear]
                      -= state.problemeHebdo
                           ->ResultatsContraintesCouplantes[associatedBC_][dayInTheWeek];

                    dayInTheYear++;
                }
                break;
            }

            // For weekly binding constraints, getting weekly marginal price
            case BindingConstraint::typeWeekly:
            {
                uint weekInTheYear = state.weekInTheYear;
                double weeklyValue = -state.problemeHebdo
                                        ->ResultatsContraintesCouplantes[associatedBC_][0];

                pValuesForTheCurrentYear[numSpace].week[weekInTheYear] = weeklyValue;

                int dayInTheYear = state.weekInTheYear * 7;
                for (int dayInTheWeek = 0; dayInTheWeek < 7; dayInTheWeek++)
                {
                    pValuesForTheCurrentYear[numSpace].day[dayInTheYear] = weeklyValue;
                    dayInTheYear++;
                }
                break;
            }
            }
        }
        NextType::weekBegin(state);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        // Next variable
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        NextType::hourForEachArea(state, numSpace);
    }

    void weekForEachArea(State& state, unsigned int numSpace)
    {
        NextType::weekForEachArea(state, numSpace);
    }

    template<class VCardToFindT>
    static void retrieveResultsForArea(typename Storage<VCardToFindT>::ResultsType** result,
                                       const Data::Area* area)
    {
        // Next variable
        NextType::template retrieveResultsForArea<VCardToFindT>(result, area);
    }

    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        NextType::buildDigest(results, digestLevel, dataLevel);
    }

    template<class V>
    static void simulationEndSpatialAggregates(V& allVars)
    {
        NextType::template simulationEndSpatialAggregates<V>(allVars);
    }

    template<class V>
    static void computeSpatialAggregatesSummary(
      V& allVars,
      std::map<unsigned int, unsigned int>& numSpaceToYear,
      unsigned int nbYearsForCurrentSummary)
    {
        NextType::template computeSpatialAggregatesSummary<V>(allVars,
                                                              numSpaceToYear,
                                                              nbYearsForCurrentSummary);
    }

    void beforeYearByYearExport(uint year, uint numSpace)
    {
        NextType::beforeYearByYearExport(year, numSpace);
    }

    template<class SearchVCardT, class O>
    static void computeSpatialAggregateWith(O& out, const Data::Area* area, uint numSpace)
    {
        NextType::template computeSpatialAggregateWith<SearchVCardT, O>(out, area, numSpace);
    }

    template<class VCardToFindT>
    static void retrieveResultsForLink(typename Storage<VCardToFindT>::ResultsType** result,
                                       const Data::AreaLink* link)
    {
        NextType::template retrieveResultsForLink<VCardToFindT>(result, link);
    }

    template<class VCardToFindT>
    static void retrieveResultsForThermalCluster(
      typename Storage<VCardToFindT>::ResultsType** result,
      const Data::ThermalCluster* cluster)
    {
        NextType::template retrieveResultsForThermalCluster<VCardToFindT>(result, cluster);
    }

    void hourEnd(State& state, unsigned int hourInTheYear)
    {
        if (isInitialized())
        {
            auto numSpace = state.numSpace;
            if (associatedBC_->type() == Data::BindingConstraint::typeHourly)
            {
                pValuesForTheCurrentYear[numSpace][hourInTheYear]
                  -= state.problemeHebdo
                       ->ResultatsContraintesCouplantes[associatedBC_][state.hourInTheWeek];
            }
        }

        NextType::hourEnd(state, hourInTheYear);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace].hour;
    }

    void localBuildAnnualSurveyReport(
      SurveyResults& results,
      int fileLevel,
      int precision /* printed results : hourly, daily, weekly, ...*/,
      unsigned int numSpace) const
    {
        if (!(precision & associatedBC_->yearByYearFilter()))
        {
            return;
        }

        // Initializing external pointer on current variable non applicable status
        results.isCurrentVarNA[0] = isCurrentOutputNonApplicable(precision);

        if (AncestorType::isPrinted[0])
        {
            // Write the data for the current year
            results.variableCaption = getBindConstraintCaption();
            results.variableUnit = VCardType::Unit();
            pValuesForTheCurrentYear[numSpace]
              .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
        }
    }

    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const
    {
        // Building syntheses results
        // ------------------------------
        if (precision & associatedBC_->yearByYearFilter())
        {
            // And only if we match the current data level _and_ precision level
            if ((dataLevel & VCardType::categoryDataLevel)
                && (fileLevel & VCardType::categoryFileLevel) && (precision & VCardType::precision))
            {
                results.isPrinted = AncestorType::isPrinted;
                results.isCurrentVarNA[0] = isCurrentOutputNonApplicable(precision);
                results.variableCaption = getBindConstraintCaption();

                VariableAccessorType::template BuildSurveyReport<VCardType>(results,
                                                                            AncestorType::pResults,
                                                                            dataLevel,
                                                                            fileLevel,
                                                                            precision,
                                                                            false);
            }
        }
        NextType::buildSurveyReport(results, dataLevel, fileLevel, precision);
    }

private:
    // Private methods
    // ---------------
    std::string getBindConstraintCaption() const
    {
        std::string mathOperator(
          Antares::Data::BindingConstraint::MathOperatorToCString(associatedBC_->operatorType()));
        return std::string() + associatedBC_->name().c_str() + " (" + mathOperator + ")";
    }

    bool isInitialized()
    {
        return associatedBC_ != nullptr;
    }

    bool isCurrentOutputNonApplicable(int precision) const
    {
        using namespace Antares::Data;
        // The current marginal prices to print becomes non applicable if they have a precision
        // (hour, day, week, ...) smaller than the associated binding constraint granularity.
        // Ex : if the BC is daily and we try to print hourly associated marginal prices,
        //      then these prices are set to N/A
        switch (associatedBC_->type())
        {
        case BindingConstraint::typeUnknown:
        case BindingConstraint::typeMax:
            return true;
        default:
            const auto precision_bc = 1 << (associatedBC_->type() - 1);
            return precision < precision_bc;
        }
    }

    // Private data mambers
    // ----------------------
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    unsigned int pNbYearsParallel = 0;
    std::shared_ptr<Data::BindingConstraint> associatedBC_ = nullptr;
    uint nbCount_ = 0; // Number of inequality BCs

}; // class BindingConstMarginCost

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares
