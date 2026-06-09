// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <type_traits>

#include "antares/solver/variable/tuple_variable_list.h"
#include "antares/solver/variable/variable.h"

// #include <antares/logs/logs.h>	// In case it is needed

namespace Antares::Solver::Variable::Common
{
template<class VCardOriginT, class = void>
struct VCardForSpatialAggregateSelector
{
    using type = VCardOriginT;
};

template<class VCardOriginT>
struct VCardForSpatialAggregateSelector<
  VCardOriginT,
  std::void_t<typename VCardOriginT::VCardForSpatialAggregate>>
{
    using type = typename VCardOriginT::VCardForSpatialAggregate;
};

template<int ColumnCountT, class VCardT>
struct MultipleCaptionProxy
{
    static std::string Caption(const uint indx)
    {
        return VCardT::Multiple::Caption(indx);
    }

    static std::string Unit(const unsigned int indx)
    {
        return VCardT::Multiple::Unit(indx);
    }
};

template<class VCardT>
struct MultipleCaptionProxy<0, VCardT>
{
    static std::string Caption(const uint)
    {
        return "";
    }

    static std::string Unit(const uint)
    {
        return "";
    }
};

template<class VCardT>
struct MultipleCaptionProxy<1, VCardT>
{
    static std::string Caption(const uint)
    {
        return "";
    }

    static std::string Unit(const uint)
    {
        return "";
    }
};

template<class VCardT>
struct MultipleCaptionProxy<Category::dynamicColumns, VCardT>
{
    static std::string Caption(const uint)
    {
        return "";
    }

    static std::string Unit(const uint)
    {
        return "";
    }
};

template<class V>
struct VCardProxy
{
    //! The real VCard for the variable
    using VCardOrigin = typename V::VCardType;

    //! Caption
    static std::string Caption()
    {
        return VCardOrigin::Caption();
    }

    //! Unit
    static std::string Unit()
    {
        return VCardOrigin::Unit();
    }

    //! The short description of the variable
    static std::string Description()
    {
        return VCardOrigin::Description();
    }

    //! The expecte results
    using ResultsType = typename VCardOrigin::ResultsType;
    //! The VCard to look for for calculating spatial aggregates
    using VCardForSpatialAggregate = typename VCardForSpatialAggregateSelector<VCardOrigin>::type;

    using IntermediateValuesType = typename VCardOrigin::IntermediateValuesType;
    using IntermediateValuesBaseType = typename VCardOrigin::IntermediateValuesBaseType;
    using IntermediateValuesTypeForSpatialAg = typename VCardOrigin::
      IntermediateValuesTypeForSpatialAg;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::setOfAreas;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = VCardOrigin::categoryFileLevel;
    //! Precision (views)
    static constexpr uint8_t precision = VCardOrigin::precision;
    //! Decimal precision
    static constexpr uint8_t decimal = VCardOrigin::decimal;
    //! Number of columns used by the variable (One ResultsType per column)
    static constexpr int columnCount = VCardOrigin::columnCount;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::noSpatialAggregate;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;

    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = VCardOrigin::isPossiblyNonApplicable;

    struct Multiple
    {
        static std::string Caption(const uint indx)
        {
            return MultipleCaptionProxy<columnCount, VCardOrigin>::Caption(indx);
        }

        static std::string Unit(const unsigned int indx)
        {
            return MultipleCaptionProxy<columnCount, VCardOrigin>::Unit(indx);
        }
    };

}; // class VCard

template<class VarT>
class SpatialAggregate: public Variable::IVariable<SpatialAggregate<VarT>, VCardProxy<VarT>>
{
public:
    //! VCard
    typedef VCardProxy<VarT> VCardType;
    //! Ancestor
    typedef Variable::IVariable<SpatialAggregate<VarT>, VCardType> AncestorType;

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
    void initializeFromStudy(Data::Study& study)
    {
        typedef
          typename VCardType::VCardOrigin::IntermediateValuesBaseType IntermediateValuesBaseType;
        pNbYearsParallel = study.maxNbYearsInParallel;

        // Intermediate values
        VarT::InitializeResultsFromStudy(AncestorType::pResults, study);
        pValuesForTheCurrentYear = std::make_unique<IntermediateValuesBaseType[]>(pNbYearsParallel);
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            VariableAccessorType::InitializeAndReset(pValuesForTheCurrentYear[numSpace], study);
        }
    }

    template<class V, class SetT>
    void yearEndSpatialAggregates(V& allVars, uint year, const SetT& set, uint numSpace)
    {
        if (VCardType::VCardOrigin::spatialAggregateMode & Category::spatialAggregateEachYear)
        {
            internalSpatialAggregateForCurrentYear(allVars, set, numSpace);
        }
    }

    template<class V>
    void computeSpatialAggregatesSummary(V& allVars, unsigned int year, unsigned int numSpace)
    {
        if (VCardType::VCardOrigin::spatialAggregateMode & Category::spatialAggregateEachYear)
        {
            internalSpatialAggregateForParallelYears(year, numSpace);
        }
    }

    template<class V, class SetT>
    void simulationEndSpatialAggregates(V& allVars, const SetT& set)
    {
        if (VCardType::VCardOrigin::spatialAggregateMode & Category::spatialAggregateOnce)
        {
            internalSpatialAggregate(allVars, 0, set);
        }
    }

    inline void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        // Generate the Digest for the local results (districts part)
        if (VCardType::columnCount != 0
            && (VCardType::categoryDataLevel & Category::DataLevel::setOfAreas))
        {
            // Initializing pointer on variable non applicable and print stati arrays to beginning
            results.isPrinted = AncestorType::isPrinted;
            results.isCurrentVarNA = AncestorType::isNonApplicable;

            VariableAccessorType::template BuildDigest<typename VCardType::VCardOrigin>(
              results,
              AncestorType::pResults,
              digestLevel,
              dataLevel);
        }
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      uint numSpace) const
    {
        if (VCardType::columnCount != 0
            && (VCardType::categoryDataLevel & Category::DataLevel::setOfAreas))
        {
            // Initializing pointer on variable non applicable and print stati arrays to beginning
            results.isPrinted = AncestorType::isPrinted;
            results.isCurrentVarNA = AncestorType::isNonApplicable;

            typedef VariableAccessor<typename VCardType::IntermediateValuesBaseType,
                                     VCardType::columnCount>
              VAType;
            VAType::template BuildAnnualSurveyReport<typename VCardType::VCardOrigin>(
              results,
              pValuesForTheCurrentYear[numSpace],
              fileLevel,
              precision);
        }
    }

private:
    template<class V, class SetT>
    void internalSpatialAggregate(V& allVars, uint year, const SetT& set)
    {
        typedef typename VCardType::VCardOrigin VCardOrigin;
        // Reset the results
        VariableAccessorType::Reset(pValuesForTheCurrentYear[0]);

        // Make the spatial cluster
        if (!set.empty())
        {
            auto end = set.end();
            for (auto i = set.begin(); i != end; ++i)
            {
                allVars.template computeSpatialAggregateWith<
                  typename VCardType::VCardForSpatialAggregate> //<typename VCardType::VCardOrigin>
                  (pValuesForTheCurrentYear[0], *i /* the current area */, 0);
            }

            // The spatial cluster may be an average
            if (VCardType::VCardOrigin::spatialAggregate & Category::spatialAggregateAverage)
            {
                VariableAccessorType::MultiplyHourlyResultsBy(pValuesForTheCurrentYear[0],
                                                              1. / set.size());
            }
            // The spatial cluster may be an average
            if (VCardType::VCardOrigin::spatialAggregate
                & Category::spatialAggregateSumThen1IfPositive)
            {
                VariableAccessorType::SetTo1IfPositive(pValuesForTheCurrentYear[0]);
            }
            if (VCardType::VCardOrigin::spatialAggregate & Category::spatialAggregateOr)
            {
                VariableAccessorType::Or(pValuesForTheCurrentYear[0]);
            }
        }
        else
        {
            assert(!set.empty() && "The set should not be empty at this point");
        }

        // Compute all statistics for the current year (daily,weekly,monthly,...)
        VariableAccessorType::template ComputeStatistics<VCardOrigin>(pValuesForTheCurrentYear[0]);
        VariableAccessorType::ComputeSummary(pValuesForTheCurrentYear[0],
                                             AncestorType::pResults,
                                             year);
    }

    template<class V, class SetT>
    void internalSpatialAggregateForCurrentYear(V& allVars, const SetT& set, uint numSpace)
    {
        typedef typename VCardType::VCardOrigin VCardOrigin;
        // Reset the results
        VariableAccessorType::Reset(pValuesForTheCurrentYear[numSpace]);

        // Make the spatial cluster
        if (!set.empty())
        {
            // We compute the district's (or set) hourly values by looping over district's areas
            // and calling computeSpatialAggregateWith on each area.
            // The aggregated results is stored in pValuesForTheCurrentYear[numSpace].
            auto end = set.end();
            for (auto i = set.begin(); i != end; ++i)
            {
                // computeSpatialAggregateWith is called with VCardType::VCardForSpatialAggregate
                // as a template parameter.
                // Note that VCardType represents the vcard of the current spatial aggregate's
                // variable (template parameter VarT).
                // For most variables VCardType::VCardOrigin and
                // VCardType::VCardForSpatialAggregate are identical vcard types.
                // For example : for var OverallCost, VCardOverallCost::VCardForSpatialAggregate
                // is simply VCardOverallCost itself.
                // But var LOLD is different : VCardLOLD::VCardForSpatialAggregate is actually
                // VCardUnsupliedEnergy.
                // This means that, here :
                // - for OverallCost, we sum the hourly values of var OverallCost for each
                //   district's areas.
                allVars.template computeSpatialAggregateWith<
                  typename VCardType::VCardForSpatialAggregate>(pValuesForTheCurrentYear[numSpace],
                                                                *i /* the current area */,
                                                                numSpace);
            }

            // The spatial cluster may be an average
            if (VCardType::VCardOrigin::spatialAggregate & Category::spatialAggregateAverage)
            {
                VariableAccessorType::MultiplyHourlyResultsBy(pValuesForTheCurrentYear[numSpace],
                                                              1. / set.size());
            }
            // The spatial cluster may be an average
            if (VCardType::VCardOrigin::spatialAggregate
                & Category::spatialAggregateSumThen1IfPositive)
            {
                VariableAccessorType::SetTo1IfPositive(pValuesForTheCurrentYear[numSpace]);
            }
            if (VCardType::VCardOrigin::spatialAggregate & Category::spatialAggregateOr)
            {
                VariableAccessorType::Or(pValuesForTheCurrentYear[numSpace]);
            }
        }
        else
        {
            assert(!set.empty() && "The set should not be empty at this point");
        }

        // Compute all statistics for the current year (daily,weekly,monthly,...)
        VariableAccessorType::template ComputeStatistics<VCardOrigin>(
          pValuesForTheCurrentYear[numSpace]);
    }

    void internalSpatialAggregateForParallelYears(unsigned int year, unsigned int numSpace)
    {
        // Merge all those values with the global results
        VariableAccessorType::ComputeSummary(pValuesForTheCurrentYear[numSpace],
                                             AncestorType::pResults,
                                             year);
    }

private:
    //! Intermediate values for each year
    VCardType::IntermediateValuesTypeForSpatialAg pValuesForTheCurrentYear;

    unsigned int pNbYearsParallel = 0;

}; // class SpatialAggregate

// Variadic composition of spatial-aggregate variables. Produces a flat tuple
// dispatcher. Each Vs... is an already-instantiated variable type.
template<class... Vs>
struct SpatialAggregateAll
{
    using type = Container::TupleVariableList<SpatialAggregate<Vs>...>;
};

} // namespace Antares::Solver::Variable::Common
