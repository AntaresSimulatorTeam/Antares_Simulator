// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/study/binding_constraint/BindingConstraint.h"

#include "../../variable.h"

namespace Antares::Solver::Variable::Economy
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
    using ResultsType = StandardResults<>;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::bindingConstraint;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::bc);
    //! Precision (views)
    static constexpr uint8_t precision = Category::all;
    //! Decimal precision
    static constexpr uint8_t decimal = 2;
    //! Number of columns used by the variable
    static constexpr int columnCount = 1;
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
class BindingConstMarginCost
    : public Variable::IVariable<BindingConstMarginCost, VCardBindingConstMarginCost>
{
public:
    //! VCard
    typedef VCardBindingConstMarginCost VCardType;
    //! Ancestor
    typedef Variable::IVariable<BindingConstMarginCost, VCardType> AncestorType;

    //! List of expected results
    typedef typename VCardType::ResultsType ResultsType;

    typedef VariableAccessor<ResultsType, VCardType::columnCount> VariableAccessorType;

    enum
    {
        count = 1,
    };

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count = ((VCardType::categoryDataLevel & CDataLevel
                      && VCardType::categoryFileLevel & CFile)
                       ? VCardType::columnCount * ResultsType::count
                       : 0),
        };
    };

public:
    BindingConstMarginCost() = default;
    BindingConstMarginCost(const BindingConstMarginCost&) = delete;
    BindingConstMarginCost(BindingConstMarginCost&&) noexcept = default;
    BindingConstMarginCost& operator=(const BindingConstMarginCost&) = delete;
    BindingConstMarginCost& operator=(BindingConstMarginCost&&) = delete;

    void simulationBegin()
    {
    }

    void simulationEnd()
    {
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
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        // Merge all those values with the global results
        AncestorType::pResults.merge(year, pValuesForTheCurrentYear[numSpace]);
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
    }

    void hourBegin(unsigned int hourInTheYear)
    {
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
    }

    void weekForEachArea(State& state, unsigned int numSpace)
    {
    }

    template<class VCardToFindT>
    static void retrieveResultsForArea(typename Storage<VCardToFindT>::ResultsType** result,
                                       const Data::Area* area)
    {
    }

    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
    }

    template<class V>
    static void simulationEndSpatialAggregates(V& allVars)
    {
    }

    template<class V>
    static void computeSpatialAggregatesSummary(V& allVars,
                                                unsigned int year,
                                                unsigned int numSpace)
    {
    }

    void beforeYearByYearExport(uint year, uint numSpace)
    {
    }

    template<class SearchVCardT, class O>
    static void computeSpatialAggregateWith(O& out, const Data::Area* area, uint numSpace)
    {
    }

    template<class VCardToFindT>
    static void retrieveResultsForLink(typename Storage<VCardToFindT>::ResultsType** result,
                                       const Data::AreaLink* link)
    {
    }

    template<class VCardToFindT>
    static void retrieveResultsForThermalCluster(
      typename Storage<VCardToFindT>::ResultsType** result,
      const Data::ThermalCluster* cluster)
    {
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

        results.isCurrentVarNA = AncestorType::isNonApplicable;
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
                results.isCurrentVarNA = AncestorType::isNonApplicable;
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

} // namespace Antares::Solver::Variable::Economy
