// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

/*!
** \file dynamic_multi_column_base.h
**
** \brief Base class for dynamic multi-column variables (runtime column count)
**
** ## Traits Contract
**
** A valid DynamicMultiColumn Traits must provide:
** - Required static methods:
**   - \c Caption() -> std::string
**   - \c Unit() -> std::string
**   - \c Description() -> std::string
**   - \c ResultsProfile : type alias for results template
**   - \c decimal : uint8_t
**   - \c buildColumnDescriptors(Data::Area*) -> std::vector<ColumnDescriptor>
**
** - Optional hooks:
**   - \c perColumnComputeStats(IntermediateValues&, size_t columnIndex) -> void
**   - \c setHourlyValue(IntermediateValuesBaseType&, State&, uint, const
*std::vector<ColumnDescriptor>&) -> void
**
** Note: The traits should rebuild groupToNumbers map internally from descriptors if needed.
*/

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct ColumnDescriptor
{
    std::string caption;
    std::string unit;
};

template<class Traits>
struct VCardDynamicMultiColumn
{
    static std::string Caption()
    {
        return Traits::Caption();
    }

    static std::string Unit()
    {
        return Traits::Unit();
    }

    static std::string Description()
    {
        return Traits::Description();
    }

    using ResultsType = typename Traits::ResultsProfile;
    using VCardForSpatialAggregate = VCardDynamicMultiColumn;

    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    static constexpr uint8_t precision = Category::all;
    static constexpr uint8_t decimal = Traits::decimal;
    static constexpr int columnCount = Category::dynamicColumns;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    using IntermediateValuesDeepType = IntermediateValues;
    using IntermediateValuesBaseType = std::vector<IntermediateValues>;
    using IntermediateValuesType = std::vector<IntermediateValuesBaseType>;

    using IntermediateValuesTypeForSpatialAg = std::unique_ptr<IntermediateValuesDeepType[]>;
};

//! Resolves per-column caption and unit from the runtime descriptor list.
//!
//! Kept out of VCardDynamicMultiColumn: it is used only internally by
//! DynamicMultiColumnBase and is not part of the generic multi-column VCard
//! interface (its signatures take the runtime descriptors, unlike the
//! single-index VCard::Multiple convention used elsewhere).
template<class Traits>
struct DynamicMultiColumnCaption
{
    static std::string Caption(uint indx, const std::vector<ColumnDescriptor>& descriptors)
    {
        return indx < descriptors.size() ? descriptors[indx].caption : "<unknown>";
    }

    static std::string Unit(uint indx, const std::vector<ColumnDescriptor>& descriptors)
    {
        return indx < descriptors.size() ? descriptors[indx].unit : Traits::Unit();
    }
};

template<class Traits>
class DynamicMultiColumnBase
    : public Variable::IVariable<DynamicMultiColumnBase<Traits>, VCardDynamicMultiColumn<Traits>>
{
public:
    using VCardType = VCardDynamicMultiColumn<Traits>;
    using AncestorType = Variable::IVariable<DynamicMultiColumnBase<Traits>, VCardType>;
    using ResultsType = typename VCardType::ResultsType;
    using VariableAccessorType = VariableAccessor<ResultsType, VCardType::columnCount>;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count = detail::
          statisticsCount<VCardType, ResultsType, CDataLevel, CFile>;
    };

public:
    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;
        pValuesForTheCurrentYear.resize(pNbYearsParallel);

        AncestorType::pResults.clear();
        AncestorType::pResults.resize(nbColumns_);
        for (uint i = 0; i < nbColumns_; ++i)
        {
            AncestorType::pResults[i].initializeFromStudy(study);
            AncestorType::pResults[i].reset();
        }
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        auto descriptors = Traits::buildColumnDescriptors(area);
        nbColumns_ = descriptors.size();
        descriptors_ = std::move(descriptors);

        groupToNumbers_.clear();
        for (size_t i = 0; i < nbColumns_; ++i)
        {
            groupToNumbers_[descriptors_[i].caption] = i;
        }

        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        for (auto& vec: pValuesForTheCurrentYear)
        {
            vec.resize(nbColumns_);
        }

        for (uint numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
        {
            for (size_t col = 0; col < nbColumns_; ++col)
            {
                pValuesForTheCurrentYear[numSpace][col].initializeFromStudy(*study);
            }
        }

        AncestorType::pResults.resize(nbColumns_);
        for (uint i = 0; i < nbColumns_; ++i)
        {
            AncestorType::pResults[i].initializeFromStudy(*study);
            AncestorType::pResults[i].reset();
        }
    }

    size_t getMaxNumberColumns() const
    {
        return nbColumns_ * ResultsType::count;
    }

    void yearBegin(uint year, uint numSpace)
    {
        for (size_t i = 0; i < nbColumns_; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }
    }

    void yearEnd(uint year, uint numSpace)
    {
        for (size_t column = 0; column < nbColumns_; ++column)
        {
            if constexpr (requires {
                              Traits::perColumnComputeStats(
                                pValuesForTheCurrentYear[numSpace][column],
                                column);
                          })
            {
                Traits::perColumnComputeStats(pValuesForTheCurrentYear[numSpace][column], column);
            }
            else
            {
                pValuesForTheCurrentYear[numSpace][column].computeStatisticsForTheCurrentYear();
            }
        }
    }

    void computeSummary(uint year, uint numSpace)
    {
        VariableAccessorType::ComputeSummary(pValuesForTheCurrentYear[numSpace],
                                             AncestorType::pResults,
                                             year);
    }

    void hourForEachArea(State& state, uint numSpace)
    {
        Traits::setHourlyValue(pValuesForTheCurrentYear[numSpace], state, numSpace, descriptors_);
    }

    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      uint column,
      uint numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace][column].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      uint numSpace) const
    {
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (!AncestorType::isPrinted[0])
        {
            return;
        }

        for (size_t column = 0; column < nbColumns_; ++column)
        {
            results.variableCaption = DynamicMultiColumnCaption<Traits>::Caption(column,
                                                                                 descriptors_);
            results.variableUnit = DynamicMultiColumnCaption<Traits>::Unit(column, descriptors_);
            pValuesForTheCurrentYear[numSpace][column]
              .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
        }
    }

    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const
    {
        if (!AncestorType::isPrinted[0])
        {
            return;
        }

        if ((dataLevel & VCardType::categoryDataLevel) && (fileLevel & VCardType::categoryFileLevel)
            && (precision & VCardType::precision))
        {
            results.isPrinted = AncestorType::isPrinted;
            results.isCurrentVarNA = AncestorType::isNonApplicable;

            for (size_t column = 0; column < nbColumns_; ++column)
            {
                results.variableCaption = descriptors_[column].caption;
                results.variableUnit = descriptors_[column].unit;
                AncestorType::pResults[column].template buildSurveyReport<ResultsType, VCardType>(
                  results,
                  AncestorType::pResults[column],
                  dataLevel,
                  fileLevel,
                  precision);
            }
        }
    }

private:
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    std::vector<ColumnDescriptor> descriptors_;
    std::map<std::string, size_t> groupToNumbers_;
    size_t nbColumns_ = 0;
    uint pNbYearsParallel;
};

} // namespace Antares::Solver::Variable::Economy
