/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <antares/utils/utils.h>
#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable::Economy
{
struct VCardRenewableGeneration
{
    //! Caption
    static std::string Caption()
    {
        return "Renewable Gen.";
    }

    //! Unit
    static std::string Unit()
    {
        return "MWh";
    }

    //! The short description of the variable
    static std::string Description()
    {
        return "Value of all the renewable generation throughout all MC years";
    }

    //! The expecte results
    typedef Results<R::AllYears::Average< // The average values throughout all years
      R::AllYears::StdDeviation<          // The standard deviation values throughout all years
        R::AllYears::Min<                 // The minimum values throughout all years
          R::AllYears::Max<               // The maximum values throughout all years
            >>>>>
      ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardRenewableGeneration VCardForSpatialAggregate;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    //! Precision (views)
    static constexpr uint8_t precision = Category::all;
    //! Indentation (GUI)
    static constexpr uint8_t nodeDepthForGUI = +0;
    //! Decimal precision
    static constexpr uint8_t decimal = 0;
    //! Nb of columns occupied by this variable in year-by-year results
    static constexpr int columnCount = Category::dynamicColumns;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues = 1;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    typedef IntermediateValues IntermediateValuesDeepType;
    typedef std::vector<IntermediateValues> IntermediateValuesBaseType;
    typedef std::vector<IntermediateValuesBaseType> IntermediateValuesType;

}; // class VCard

/*!
** \brief Marginal RenewableGeneration
*/
template<class NextT = Container::EndOfList>
class RenewableGeneration
    : public Variable::IVariable<RenewableGeneration<NextT>, NextT, VCardRenewableGeneration>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardRenewableGeneration VCardType;
    //! Ancestor
    typedef Variable::IVariable<RenewableGeneration<NextT>, NextT, VCardType> AncestorType;

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

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear.resize(pNbYearsParallel);

        // Building the vector of group names the clusters belong to.
        std::set<std::string> names;
        for (const auto& cluster: area->renewable.list.each_enabled())
        {
            names.insert(cluster->getGroup());
        }
        groupNames_ = {names.begin(), names.end()};
        groupToNumbers_ = Utils::giveNumbersToStrings(groupNames_);

        nbColumns_ = groupNames_.size();

        if (nbColumns_)
        {
            AncestorType::pResults.resize(nbColumns_);

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                pValuesForTheCurrentYear[numSpace].resize(nbColumns_);
            }

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                for (unsigned int i = 0; i != nbColumns_; ++i)
                {
                    pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(*study);
                }
            }

            for (unsigned int i = 0; i != nbColumns_; ++i)
            {
                AncestorType::pResults[i].initializeFromStudy(*study);
                AncestorType::pResults[i].reset();
            }
        }
        else
        {
            AncestorType::pResults.clear();
        }
        // Next
        NextType::initializeFromArea(study, area);
    }

    size_t getMaxNumberColumns() const
    {
        return nbColumns_ * ResultsType::count;
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        // Reset the values for the current year
        for (unsigned int i = 0; i != nbColumns_; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }
        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        for (unsigned int column = 0; column < nbColumns_; column++)
        {
            pValuesForTheCurrentYear[numSpace][column].computeStatisticsForTheCurrentYear();
        }

        // Next variable
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        // Merge all those values with the global results

        VariableAccessorType::ComputeSummary(pValuesForTheCurrentYear[numSpace],
                                             AncestorType::pResults,
                                             year);

        // Next variable
        NextType::computeSummary(year, numSpace);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        // Next variable
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        for (const auto& renewableCluster: state.area->renewable.list.each_enabled())
        {
            unsigned int groupNumber = groupToNumbers_[renewableCluster->getGroup()];

            double renewableClusterProduction = renewableCluster->valueAtTimeStep(
              state.year,
              state.hourInTheYear);

            pValuesForTheCurrentYear[numSpace][groupNumber][state.hourInTheYear]
              += renewableClusterProduction;
        }

        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    inline void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        // Ask to build the digest to the next variable
        NextType::buildDigest(results, digestLevel, dataLevel);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int column,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace][column].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int numSpace) const
    {
        // The current variable is actually a multiple-variable.
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (!AncestorType::isPrinted[0])
        {
            return;
        }

        for (unsigned int column = 0; column < nbColumns_; column++)
        {
            results.variableCaption = groupNames_[column];
            results.variableUnit = VCardType::Unit();
            pValuesForTheCurrentYear[numSpace][column]
              .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
        }
    }

    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const
    {
        // Building synthesis results
        // ------------------------------

        if (AncestorType::isPrinted[0])
        {
            // And only if we match the current data level _and_ precision level
            if ((dataLevel & VCardType::categoryDataLevel)
                && (fileLevel & VCardType::categoryFileLevel) && (precision & VCardType::precision))
            {
                results.isCurrentVarNA[0] = AncestorType::isNonApplicable[0];

                for (unsigned int column = 0; column < nbColumns_; column++)
                {
                    results.variableCaption = groupNames_[column];
                    results.variableUnit = VCardType::Unit();
                    AncestorType::pResults[column]
                      .template buildSurveyReport<ResultsType, VCardType>(
                        results,
                        AncestorType::pResults[column],
                        dataLevel,
                        fileLevel,
                        precision);
                }
            }
        }
        // Ask to the next item in the static list to export its results as well
        NextType::buildSurveyReport(results, dataLevel, fileLevel, precision);
    }

private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    std::vector<std::string> groupNames_; // Names of group containing the clusters of the area
    std::map<std::string, unsigned int> groupToNumbers_; // Gives to each group (of area) a number
    size_t nbColumns_ = 0;
    unsigned int pNbYearsParallel;

}; // class RenewableGeneration

} // namespace Antares::Solver::Variable::Economy
