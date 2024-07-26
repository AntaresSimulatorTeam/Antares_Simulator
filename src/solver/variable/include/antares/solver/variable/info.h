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
#ifndef __SOLVER_VARIABLE_INFO_H__
#define __SOLVER_VARIABLE_INFO_H__

#include <cmath>
#include "./economy/vCardReserveParticipationByDispatchablePlant.h"
#include "./economy/vCardReserveParticipationBySTStorage.h"
#include "./economy/vCardReserveParticipationBySTStorageGroup.h"
#include "./economy/vCardReserveParticipationByThermalGroup.h"
#include "./economy/vCardReserveParticipationUnsuppliedSpilled.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
template<class T>
struct SpecifierRemover
{
    typedef T Type;
};

template<class T, int N>
struct SpecifierRemover<T[N]>
{
    typedef T Type;
};

template<class T>
struct SpecifierRemover<T*>
{
    typedef T Type;
};

template<class T, int N>
struct SpecifierRemover<const T[N]>
{
    typedef T Type;
};

template<class T>
struct SpecifierRemover<const T*>
{
    typedef T Type;
};

template<class ResultsT, int ColumnCountT>
struct VariableAccessor
{
    typedef typename SpecifierRemover<ResultsT>::Type CleanType;
    typedef CleanType Type[ColumnCountT];

    template<class U>
    static void MultiplyHourlyResultsBy(U& intermediateValues, const double v)
    {
        assert(!std::isnan(v));
        for (uint i = 0; i != ColumnCountT; ++i)
        {
            Antares::Memory::Stored<double>::ReturnType array = intermediateValues[i].hour;
            for (uint y = 0; y != maxHoursInAYear; ++y)
                array[y] *= v;
        }
    }

    template<class U>
    static void SetTo1IfPositive(U& intermediateValues)
    {
        for (uint i = 0; i != ColumnCountT; ++i)
        {
            Antares::Memory::Stored<double>::ReturnType array = intermediateValues[i].hour;
            for (uint y = 0; y != maxHoursInAYear; ++y)
            {
                array[y] = std::abs(array[y]) > 0. ? 1. : 0.;
            }
        }
    }

    template<class U>
    static void Or(U& intermediateValues)
    {
        for (uint i = 0; i != ColumnCountT; ++i)
        {
            Antares::Memory::Stored<double>::ReturnType array = intermediateValues[i].hour;
            for (uint y = 0; y != maxHoursInAYear; ++y)
                array[y] = std::abs(array[y]) > 0. ? 100. : 0.;
        }
    }

    template<class U>
    static void InitializeAndReset(U& out, Data::Study& study)
    {
        for (uint i = 0; i != ColumnCountT; ++i)
        {
            out[i].initializeFromStudy(study);
            out[i].reset();
        }
    }

    template<class U>
    static void Reset(U& out)
    {
        for (uint i = 0; i != ColumnCountT; ++i)
            out[i].reset();
    }

    template<class VCardT, class U>
    static void ComputeStatistics(U& intermediateValues)
    {
        for (uint i = 0; i != ColumnCountT; ++i)
        {
            // Compute all statistics for the current year (daily,weekly,monthly)
            if (VCardT::spatialAggregate & Category::spatialAggregateOr)
            {
                intermediateValues[i].computeStatisticsOrForTheCurrentYear();
            }
            else
            {
                if (VCardT::spatialAggregatePostProcessing
                    == (int)Category::spatialAggregatePostProcessingPrice)
                    intermediateValues[i].computeAveragesForCurrentYearFromHourlyResults();
                else
                    intermediateValues[i].computeStatisticsForTheCurrentYear();
            }
        }
    }

    template<class U>
    static void ComputeSummary(U& intermediateValues, Type& container, uint year)
    {
        for (uint i = 0; i != ColumnCountT; ++i)
        {
            // Merge all those values with the global results
            container[i].merge(year, intermediateValues[i]);
        }
    }

    static uint64_t Value(const Type& container)
    {
        uint64_t result = 0;
        for (uint i = 0; i != ColumnCountT; ++i)
            result += container[i].memoryUsage();
        return result;
    }

    template<class VCardT>
    static void BuildDigest(SurveyResults& results,
                            const Type& container,
                            int digestLevel,
                            int dataLevel)
    {
        for (uint i = 0; i != ColumnCountT; ++i)
        {
            if (*results.isPrinted)
            {
                results.variableCaption = VCardT::Multiple::Caption(i);
                results.variableUnit = VCardT::Multiple::Unit(i);
                container[i].template buildDigest<VCardT>(results, digestLevel, dataLevel);
            }
            // Shift to the next internal variable's non applicable status and print status
            results.isCurrentVarNA++;
            results.isPrinted++;
        }
    }

    template<class VCardType>
    static void BuildSurveyReport(SurveyResults& results,
                                  const Type& container,
                                  int dataLevel,
                                  int fileLevel,
                                  int precision)
    {
        for (uint i = 0; i != ColumnCountT; ++i)
        {
            if (*results.isPrinted)
            {
                results.variableCaption = VCardType::Multiple::Caption(i);
                results.variableUnit = VCardType::Multiple::Unit(i);
                container[i].template buildSurveyReport<ResultsT, VCardType>(
                  results, container[i], dataLevel, fileLevel, precision);
            }
            // Shift to the next internal variable's non applicable status and print status
            results.isCurrentVarNA++;
            results.isPrinted++;
        }
    }

    template<class VCardType>
    static void BuildAnnualSurveyReport(SurveyResults& results,
                                        const Type& container,
                                        int fileLevel,
                                        int precision)
    {
        for (uint i = 0; i != ColumnCountT; ++i)
        {
            if (*results.isPrinted)
            {
                results.variableCaption = VCardType::Multiple::Caption(i);
                container[i].template buildAnnualSurveyReport<VCardType>(
                  results, fileLevel, precision);
            }
            // Shift to the next internal variable's non applicable status and print status
            results.isCurrentVarNA++;
            results.isPrinted++;
        }
    }

    template<class U, class VarT>
    static void ComputeSum(U& out, const VarT& var, uint numSpace)
    {
        for (uint i = 0; i != ColumnCountT; ++i)
        {
            Antares::Memory::Stored<double>::ConstReturnType src
              = var.retrieveRawHourlyValuesForCurrentYear(i, numSpace);

            assert(src != NULL);
            for (uint h = 0; h != maxHoursInAYear; ++h)
                out[i].hour[h] += src[h];
        }
    }

    template<class U, class VarT>
    static void ComputeMax(U& out, const VarT& var, uint numSpace)
    {
        for (uint i = 0; i != ColumnCountT; ++i)
        {
            Antares::Memory::Stored<double>::ConstReturnType src
              = var.retrieveRawHourlyValuesForCurrentYear(i, numSpace);

            assert(src != NULL);
            for (uint h = 0; h != maxHoursInAYear; ++h)
            {
                if (out[i].hour[h] < src[h])
                    out[i].hour[h] = src[h];
            }
        }
    }
};

template<class ResultsT>
struct VariableAccessor<ResultsT, Category::dynamicColumns>
{
    typedef typename SpecifierRemover<ResultsT>::Type CleanType;
    typedef std::vector<CleanType> Type;

    template<class U>
    static void MultiplyHourlyResultsBy(U& intermediateValues, const double v)
    {
        assert(!std::isnan(v));
        double* array;
        const typename Type::const_iterator end = intermediateValues.end();
        for (typename Type::const_iterator i = intermediateValues.begin(); i != end; ++i)
        {
            array = (*i).hour;
            for (uint y = 0; y != maxHoursInAYear; ++y)
                array[y] *= v;
        }
    }

    template<class U>
    static void SetTo1IfPositive(U& intermediateValues)
    {
        double* array;
        const typename Type::const_iterator end = intermediateValues.end();
        for (typename Type::const_iterator i = intermediateValues.begin(); i != end; ++i)
        {
            array = (*i).hour;
            for (uint y = 0; y != maxHoursInAYear; ++y)
                array[y] = std::abs(array[y]) > 0. ? 1. : 0.;
        }
    }

    template<class U>
    static void Or(U& intermediateValues)
    {
        double* array;
        const typename Type::const_iterator end = intermediateValues.end();
        for (typename Type::const_iterator i = intermediateValues.begin(); i != end; ++i)
        {
            array = (*i).hour;
            for (uint y = 0; y != maxHoursInAYear; ++y)
                array[y] = std::abs(array[y]) > 0. ? 100. : 0.;
        }
    }

    template<class U>
    static void InitializeAndReset(U& out, Data::Study& study)
    {
        const typename Type::const_iterator end = out.end();
        for (typename Type::const_iterator i = out.begin(); i != end; ++i)
        {
            (*i).initializeFromStudy(study);
            (*i).reset();
        }
    }

    template<class U>
    static void Reset(U& out)
    {
        const typename Type::const_iterator end = out.end();
        for (typename Type::const_iterator i = out.begin(); i != end; ++i)
            (*i).reset();
    }

    template<class VCardT, class U>
    static void ComputeStatistics(U& intermediateValues, Type& container, uint)
    {
        for (uint i = 0; i != container.size(); ++i)
        {
            if (VCardT::spatialAggregate & Category::spatialAggregateOr)
            {
                intermediateValues[i].computeStatisticsOrForTheCurrentYear();
            }
            else
            {
                // Compute all statistics for the current year (daily,weekly,monthly)
                if (VCardT::spatialAggregatePostProcessing
                    == (int)Category::spatialAggregatePostProcessingPrice)
                    // intermediateValues[i].adjustValuesWhenRelatedToAPrice();
                    intermediateValues[i].computeAveragesForCurrentYearFromHourlyResults();
                else
                    intermediateValues[i].computeStatisticsForTheCurrentYear();
            }
        }
    }

    template<class U>
    static void ComputeSummary(U& intermediateValues, Type& container, uint year)
    {
        for (uint i = 0; i != container.size(); ++i)
        {
            // Merge all those values with the global results
            container[i].merge(year, intermediateValues[i]);
        }
    }

    static uint64_t Value(const Type& container)
    {
        uint64_t result = 0;
        const typename Type::const_iterator end = container.end();
        for (typename Type::const_iterator i = container.begin(); i != end; ++i)
            result += sizeof(ResultsT) + (*i).memoryUsage();
        return result;
    }

    template<class VCardT>
    static void BuildDigest(SurveyResults& results,
                            const Type& container,
                            int digestLevel,
                            int dataLevel)
    {
        if (*results.isPrinted)
        {
            const Data::PartThermal& thermal = results.data.area->thermal;
            const auto& shortTermStorage = results.data.area->shortTermStorage;
            for (uint i = 0; i != container.size(); ++i)
            {
                if constexpr (std::is_same_v<VCardT,
                                             Economy::VCardReserveParticipationByDispatchablePlant>)
                {
                    auto [clusterName, reserveName]
                      = thermal.list.reserveParticipationClusterAt(results.data.area, i);
                    results.variableCaption = reserveName + "_" + clusterName;
                }
                else if constexpr (std::is_same_v<VCardT,
                                                  Economy::VCardReserveParticipationByThermalGroup>)
                {
                    auto [groupName, reserveName]
                      = thermal.list.reserveParticipationGroupAt(results.data.area, i);
                    results.variableCaption
                      = reserveName + "_" + Economy::thermalDispatchableGroupToString(groupName);
                }
                else if constexpr (std::is_same_v<VCardT,
                                                  Economy::VCardReserveParticipationBySTStorage>)
                {
                    auto [clusterName, reserveName]
                      = shortTermStorage.reserveParticipationClusterAt(results.data.area, i);
                    results.variableCaption = reserveName + "_" + clusterName;
                }
                else if constexpr (std::is_same_v<
                                     VCardT,
                                     Economy::VCardReserveParticipationBySTStorageGroup>)
                {
                    auto [groupName, reserveName]
                      = shortTermStorage.reserveParticipationGroupAt(results.data.area, i);
                    results.variableCaption
                      = reserveName + "_" + Economy::STStorageGroupToString(groupName);
                }
                else if constexpr (std::is_same_v<
                                     VCardT,
                                     Economy::VCardReserveParticipationUnsuppliedSpilled>)
                {
                    auto [unsuppliedOrSpilled, reserveName]
                      = thermal.list.reserveParticipationUnsuppliedSpilledAt(results.data.area, i);
                    results.variableCaption
                      = reserveName + "_" + Economy::unsuppliedSpilledToString(unsuppliedOrSpilled);
                }
                else
                    results.variableCaption = thermal.list.enabledClusterAt(i)->name();

                container[i].template buildDigest<VCardT>(results, digestLevel, dataLevel);
            }
        }
    }

    static bool setClusterReserveCaption(SurveyResults& results, uint reserveParticipationIdx)
    {
        assert(results.data.area && "Area is NULL");

        auto& thermal = results.data.area->thermal;
        auto [clusterName, reserveName]
          = thermal.list.reserveParticipationClusterAt(results.data.area, reserveParticipationIdx);
        results.variableCaption = clusterName + " - " + reserveName;
        return true;
    }

    static bool setClusterCaption(SurveyResults& results, int fileLevel, uint idx)
    {
        assert(results.data.area && "Area is NULL");
        const bool thermal_details = fileLevel & Category::de;
        const bool renewable_details = fileLevel & Category::de_res;
        const bool st_storage_details = fileLevel & Category::de_sts;

        std::array<bool, 3> kind_of_details = { thermal_details, renewable_details , st_storage_details };

        // The current result file must be a detail file and of one kind only.
        // So the vector above must contain one true. No less, no more.
        auto how_many_kinds_of_details = std::count(kind_of_details.begin(), kind_of_details.end(), true);

        if (how_many_kinds_of_details != 1)
        {
            logs.error() << "Inconsistent fileLevel detected";
            return false;
        }

        if (thermal_details)
        {
            auto& thermal = results.data.area->thermal;
            results.variableCaption = thermal.list.enabledClusterAt(idx)->name();
            return true;
        }
        if (renewable_details)
        {
            auto& renewable = results.data.area->renewable;
            results.variableCaption = renewable.list.enabledClusterAt(idx)->name();
            return true;
        }
        if (st_storage_details)
        {
            auto& st_storage_part = results.data.area->shortTermStorage;
            results.variableCaption = st_storage_part.storagesByIndex[idx].properties.name;
            return true;
        }
        return true;
    }

    template<class VCardType>
    static void BuildSurveyReport(SurveyResults& results,
                                  const Type& container,
                                  int dataLevel,
                                  int fileLevel,
                                  int precision)
    {
        bool res;
        if (*results.isPrinted)
        {
            const Data::PartThermal& thermal = results.data.area->thermal;
            const auto& shortTermStorage = results.data.area->shortTermStorage;
            for (uint i = 0; i != container.size(); ++i)
            {
                if constexpr (std::is_same_v<VCardType,
                                             Economy::VCardReserveParticipationByDispatchablePlant>)
                {
                    auto [clusterName, reserveName]
                      = thermal.list.reserveParticipationClusterAt(results.data.area, i);
                    results.variableCaption = reserveName + "_" + clusterName;
                    res = true;
                }
                else if constexpr (std::is_same_v<VCardType,
                                                  Economy::VCardReserveParticipationBySTStorage>)
                {
                    auto [clusterName, reserveName]
                      = shortTermStorage.reserveParticipationClusterAt(results.data.area, i);
                    results.variableCaption = reserveName + "_" + clusterName;
                    res = true;
                }
                else if constexpr (std::is_same_v<VCardType,
                                                  Economy::VCardReserveParticipationByThermalGroup>)
                {
                    auto [groupName, reserveName]
                      = thermal.list.reserveParticipationGroupAt(results.data.area, i);
                    results.variableCaption
                      = reserveName + "_" + Economy::thermalDispatchableGroupToString(groupName);
                    res = true;
                }
                else if constexpr (std::is_same_v<
                                     VCardType,
                                     Economy::VCardReserveParticipationBySTStorageGroup>)
                {
                    auto [groupName, reserveName]
                      = shortTermStorage.reserveParticipationGroupAt(results.data.area, i);
                    results.variableCaption
                      = reserveName + "_" + Economy::STStorageGroupToString(groupName);
                    res = true;
                }
                 else if constexpr (std::is_same_v<
                                      VCardType,
                                      Economy::VCardReserveParticipationUnsuppliedSpilled>)
                 {
                     auto [unsuppliedOrSpilled, reserveName]
                       = thermal.list.reserveParticipationUnsuppliedSpilledAt(results.data.area, i);
                     results.variableCaption
                       = reserveName + "_"
                         + Economy::unsuppliedSpilledToString(unsuppliedOrSpilled);
                     res = true;
                 }
                else
                    res = setClusterCaption(results, fileLevel, i);
                if (!res)
                    return;
                results.variableUnit = VCardType::Unit();

                container[i].template buildSurveyReport<ResultsT, VCardType>(
                  results, container[i], dataLevel, fileLevel, precision);
            }
        }
    }

    template<class VCardType>
    static void BuildAnnualSurveyReport(SurveyResults& results,
                                        const Type& container,
                                        int fileLevel,
                                        int precision)
    {
        bool res;
        if (*results.isPrinted)
        {
            for (uint i = 0; i != container.size(); ++i)
            {
                res = setClusterCaption(results, fileLevel, i);
                if (!res)
                    return;
                container[i].template buildAnnualSurveyReport<VCardType>(
                  results, fileLevel, precision);
            }
        }
    }

    template<class U, class VarT>
    static void ComputeSum(U& out, const VarT& var, uint numSpace)
    {
        for (uint i = 0; i != var.results().size(); ++i)
        {
            Antares::Memory::Stored<double>::ConstReturnType src
              = var.retrieveRawHourlyValuesForCurrentYear(i, numSpace);

            assert(src != NULL);
            for (uint h = 0; h != maxHoursInAYear; ++h)
                out[i].hour[h] += src[h];
        }
    }

    template<class U, class VarT>
    static void ComputeMax(U& out, const VarT& var, uint numSpace)
    {
        for (uint i = 0; i != var.results().size(); ++i)
        {
            Antares::Memory::Stored<double>::ConstReturnType src
              = var.retrieveRawHourlyValuesForCurrentYear(i, numSpace);

            assert(src != NULL);
            for (uint h = 0; h != maxHoursInAYear; ++h)
            {
                if (out[i].hour[h] < src[h])
                    out[i].hour[h] = src[h];
            }
        }
    }
};

template<class ResultsT>
struct VariableAccessor<ResultsT, Category::singleColumn /* The default */>
{
    typedef typename SpecifierRemover<ResultsT>::Type CleanType;
    typedef CleanType Type;

    template<class U>
    static void MultiplyHourlyResultsBy(U& intermediateValues, const double v)
    {
        assert(!std::isnan(v));
        for (uint y = 0; y != maxHoursInAYear; ++y)
            intermediateValues.hour[y] *= v;
    }

    template<class U>
    static void SetTo1IfPositive(U& intermediateValues)
    {
        for (uint y = 0; y != maxHoursInAYear; ++y)
            intermediateValues.hour[y] = std::abs(intermediateValues.hour[y]) > 0. ? 1. : 0.;
    }

    template<class U>
    static void Or(U& intermediateValues)
    {
        for (uint y = 0; y != maxHoursInAYear; ++y)
            intermediateValues.hour[y]
              = std::abs(intermediateValues.hour[y]) > 0. ? 100. : 0.;
    }

    template<class U>
    static void InitializeAndReset(U& out, Data::Study& study)
    {
        out.initializeFromStudy(study);
        out.reset();
    }

    template<class U>
    static void Reset(U& out)
    {
        out.reset();
    }

    template<class VCardT, class U>
    static void ComputeStatistics(U& intermediateValues)
    {
        if (VCardT::spatialAggregate & Category::spatialAggregateOr)
        {
            intermediateValues.computeStatisticsOrForTheCurrentYear();
        }
        else
        {
            // Compute all statistics for the current year (daily,weekly,monthly)
            if (VCardT::spatialAggregatePostProcessing
                == (int)Category::spatialAggregatePostProcessingPrice)
                // intermediateValues[i].adjustValuesWhenRelatedToAPrice();
                intermediateValues.computeAveragesForCurrentYearFromHourlyResults();
            else
                intermediateValues.computeStatisticsForTheCurrentYear();
        }
    }

    template<class U>
    static void ComputeSummary(U& intermediateValues, Type& container, uint year)
    {
        // Merge all those values with the global results
        container.merge(year, intermediateValues);
    }

    static uint64_t Value(const Type& container)
    {
        return container.memoryUsage();
    }

    template<class VCardT>
    static void BuildDigest(SurveyResults& results,
                            const Type& container,
                            int digestLevel,
                            int dataLevel)
    {
        if (*results.isPrinted)
        {
            results.variableCaption = VCardT::Caption();
            results.variableUnit = VCardT::Unit();
            container.template buildDigest<VCardT>(results, digestLevel, dataLevel);
        }
    }

    template<class VCardType>
    static void BuildSurveyReport(SurveyResults& results,
                                  const Type& container,
                                  int dataLevel,
                                  int fileLevel,
                                  int precision,
                                  bool updateCaption = true)
    {
        if (*results.isPrinted)
        {
            if (updateCaption)
            {
                results.variableCaption = VCardType::Caption();
                results.variableUnit = VCardType::Unit();
            }
            container.template buildSurveyReport<ResultsT, VCardType>(
              results, container, dataLevel, fileLevel, precision);
        }
    }

    template<class VCardType>
    static void BuildAnnualSurveyReport(SurveyResults& results,
                                        const Type& container,
                                        int fileLevel,
                                        int precision)
    {
        if (*results.isPrinted)
        {
            results.variableCaption = VCardType::Caption();
            results.variableUnit = VCardType::Unit();
            container.template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
        }
    }

    template<class U, class VarT>
    static void ComputeSum(U& out, const VarT& var, uint numSpace)
    {
        Antares::Memory::Stored<double>::ConstReturnType src
          = var.retrieveRawHourlyValuesForCurrentYear(-1, numSpace);

        assert(src != NULL);
        for (uint h = 0; h != maxHoursInAYear; ++h)
            out.hour[h] += src[h];
    }

    template<class U, class VarT>
    static void ComputeMax(U& out, const VarT& var, uint numSpace)
    {
        Antares::Memory::Stored<double>::ConstReturnType src
          = var.retrieveRawHourlyValuesForCurrentYear(-1, numSpace);

        assert(src != NULL);
        for (uint h = 0; h != maxHoursInAYear; ++h)
        {
            if (out.hour[h] < src[h])
                out.hour[h] = src[h];
        }
    }
};

template<class ResultsT>
struct VariableAccessor<ResultsT, Category::noColumn>
{
    typedef typename SpecifierRemover<ResultsT>::Type CleanType;
    typedef CleanType Type;

    template<class U>
    static void MultiplyHourlyResultsBy(U&, const double)
    {
        // Do nothing
    }

    template<class U>
    static void SetTo1IfPositive(U&)
    {
    }

    template<class U>
    static void Or(U&)
    {
    }

    template<class U>
    static void InitializeAndReset(U&, Data::Study&)
    {
        // Do nothing
    }

    template<class U>
    static void Reset(U&)
    {
        // Do nothing
    }

    template<class VCardT, class U>
    static void ComputeStatisticsAndMerge(U&, Type&, uint)
    {
        // Do nothing
    }

    static uint64_t Value(const Type&)
    {
        return 0;
    }

    template<class VCardType>
    static void BuildSurveyReport(SurveyResults&, const Type&, int, int, int)
    {
        // Do nothing
    }

    template<class VCardType>
    static void BuildAnnualSurveyReport(SurveyResults&, const Type&, int, int)
    {
        // Do nothing
    }

    template<class VCardT>
    static void BuildDigest(SurveyResults&, const Type&, int, int)
    {
        // Do nothing
    }

    template<class U, class VarT>
    static void ComputeSum(U&, const VarT&, uint)
    {
        // Do nothing
    }

    template<class U, class VarT>
    static void ComputeMax(U&, const VarT&, uint)
    {
        // Do nothing
    }
};

template<class VCardT>
struct Storage
{
    //! The true type used for the results
    typedef typename VariableAccessor<typename VCardT::ResultsType, VCardT::columnCount>::Type
      ResultsType;
};

template<bool Allowed, int OperationT, class VCardT>
struct SpatialAggregateOperation
{
    template<class U, class VarT>
    static void Perform(U&, const VarT&, uint)
    {
    }
};

// `+`
template<class VCardT>
struct SpatialAggregateOperation<true, Category::spatialAggregateSum, VCardT>
{
    template<class U, class VarT>
    static void Perform(U& intermediateResults, const VarT& var, uint numSpace)
    {
        typedef typename VCardT::ResultsType ResultsType;
        VariableAccessor<ResultsType, VCardT::columnCount>::ComputeSum(
          intermediateResults, var, numSpace);
    }
};

// `+`
template<class VCardT>
struct SpatialAggregateOperation<true, Category::spatialAggregateOr, VCardT>
{
    template<class U, class VarT>
    static void Perform(U& intermediateResults, const VarT& var, uint numSpace)
    {
        typedef typename VCardT::ResultsType ResultsType;
        VariableAccessor<ResultsType, VCardT::columnCount>::ComputeSum(
          intermediateResults, var, numSpace);
    }
};

// `+`
template<class VCardT>
struct SpatialAggregateOperation<true, Category::spatialAggregateSumThen1IfPositive, VCardT>
{
    template<class U, class VarT>
    static void Perform(U& intermediateResults, const VarT& var, uint numSpace)
    {
        typedef typename VCardT::ResultsType ResultsType;
        VariableAccessor<ResultsType, VCardT::columnCount>::ComputeSum(
          intermediateResults, var, numSpace);
    }
};

// `+`
template<class VCardT>
struct SpatialAggregateOperation<true, Category::spatialAggregateAverage, VCardT>
{
    template<class U, class VarT>
    static void Perform(U& intermediateResults, const VarT& var, uint numSpace)
    {
        typedef typename VCardT::ResultsType ResultsType;
        VariableAccessor<ResultsType, VCardT::columnCount>::ComputeSum(
          intermediateResults, var, numSpace);
    }
};

// `>`
template<class VCardT>
struct SpatialAggregateOperation<true, Category::spatialAggregateMax, VCardT>
{
    template<class U, class VarT>
    static void Perform(U& intermediateResults, const VarT& var, uint numSpace)
    {
        typedef typename VCardT::ResultsType ResultsType;
        VariableAccessor<ResultsType, VCardT::columnCount>::ComputeMax(
          intermediateResults, var, numSpace);
    }
};

} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_INFO_H__
