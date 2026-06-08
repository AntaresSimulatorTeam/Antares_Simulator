// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_VARIABLE_HXX__
#define __SOLVER_VARIABLE_VARIABLE_HXX__

#include <type_traits>

#include <antares/study/variable-print-info.h>

namespace Antares::Solver::Variable
{

template<class ChildT, class VCardT>
inline IVariable<ChildT, VCardT>::IVariable()
{
    // Number of column, where dimension -1 (dynamic) is avoided
    pColumnCount = VCardType::columnCount > 1 ? VCardType::columnCount : 1;

    isNonApplicable = new bool[pColumnCount];
    isPrinted = new bool[pColumnCount];

    for (uint i = 0; i < pColumnCount; i++)
    {
        isPrinted[i] = true;
    }
}

template<class ChildT, class VCardT>
inline IVariable<ChildT, VCardT>::~IVariable()
{
    delete[] isNonApplicable;
    delete[] isPrinted;
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::initializeFromStudy(Data::Study&)
{
}

template<class ChildT, class VCardT>
template<class R>
inline void IVariable<ChildT, VCardT>::InitializeResultsFromStudy(R& results, Data::Study& study)
{
    VariableAccessorType::InitializeAndReset(results, study);
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::initializeFromArea(Data::Study*, Data::Area*)
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::initializeFromLink(Data::Study*, Data::AreaLink*)
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::broadcastNonApplicability(bool applyNonApplicable)
{
    const bool value = VCardType::isPossiblyNonApplicable != 0 && applyNonApplicable;
    for (uint i = 0; i != pColumnCount; ++i)
    {
        isNonApplicable[i] = value;
    }
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::simulationBegin()
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::simulationEnd()
{
}

template<class ChildT, class VCardT>
size_t IVariable<ChildT, VCardT>::getMaxNumberColumns() const
{
    return VCardT::ResultsType::count;
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::yearBegin(uint)
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::yearEnd(uint)
{
}

template<class ChildT, class VCardT>
template<class V>
inline void IVariable<ChildT, VCardT>::yearEndSpatialAggregates(V&, uint, unsigned int)
{
}

template<class ChildT, class VCardT>
template<class V, class SetT>
inline void IVariable<ChildT, VCardT>::yearEndSpatialAggregates(V&, uint, const SetT&)
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::yearEndBuildPrepareDataForEachThermalCluster(State&,
                                                                                    uint,
                                                                                    uint)
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::yearEndBuildForEachThermalCluster(State&,
                                                                         unsigned int,
                                                                         unsigned int)
{
}

template<class ChildT, class VCardT>
template<class V>
inline void IVariable<ChildT, VCardT>::simulationEndSpatialAggregates(V&)
{
}

template<class ChildT, class VCardT>
template<class V, class SetT>
inline void IVariable<ChildT, VCardT>::simulationEndSpatialAggregates(V&, const SetT&)
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::weekBegin(State&)
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::weekForEachArea(State&, unsigned int)
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::hourBegin(uint)
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::hourForEachArea(State&)
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::hourForEachArea(State&, unsigned int)
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::hourForEachLink(State&, unsigned int)
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::hourEnd(State&, uint)
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::weekEnd(State&)
{
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::buildSurveyReport(SurveyResults& results,
                                                         int dataLevel,
                                                         int fileLevel,
                                                         int precision) const
{
    // Generating value for the area
    // Only if there are some results to export...
    if (0 != ResultsType::count)
    {
        // And only if we match the current data level _and_ precision level
        if ((dataLevel & VCardType::categoryDataLevel) && (fileLevel & VCardType::categoryFileLevel)
            && (precision & VCardType::precision))
        {
            // Initializing pointer on variable non applicable and print stati arrays to beginning
            results.isPrinted = isPrinted;
            results.isCurrentVarNA = isNonApplicable;

            VariableAccessorType::template BuildSurveyReport<VCardType>(results,
                                                                        pResults,
                                                                        dataLevel,
                                                                        fileLevel,
                                                                        precision);
        }
    }
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::buildAnnualSurveyReport(SurveyResults& results,
                                                               int dataLevel,
                                                               int fileLevel,
                                                               int precision,
                                                               uint numSpace) const
{
    // Generating value for the area
    // Only if there are some results to export...
    if (0 != ResultsType::count)
    {
        // And only if we match the current data level _and_ precision level
        if ((dataLevel & VCardType::categoryDataLevel) && (fileLevel & VCardType::categoryFileLevel)
            && (precision & VCardType::precision))
        {
            // Getting its intermediate results
            static_cast<const ChildT*>(this)->localBuildAnnualSurveyReport(results,
                                                                           fileLevel,
                                                                           precision,
                                                                           numSpace);
        }
    }
}

template<class ChildT, class VCardT>
inline bool IVariable<ChildT, VCardT>::hasColumn() const
{
    // Leverage the fact that dynamicType has columnCount = -1
    return VCardType::columnCount > 0;
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::buildDigest(SurveyResults& results,
                                                   int digestLevel,
                                                   int dataLevel) const
{
    // Generate the Digest for the local results (areas part)
    if (hasColumn()
        && (VCardType::categoryDataLevel & Category::DataLevel::setOfAreas
            || VCardType::categoryDataLevel & Category::DataLevel::area
            || VCardType::categoryDataLevel & Category::DataLevel::link))
    {
        // Initializing pointer on variable non applicable and print stati arrays to beginning
        results.isPrinted = isPrinted;
        results.isCurrentVarNA = isNonApplicable;

        VariableAccessorType::template BuildDigest<VCardT>(results,
                                                           pResults,
                                                           digestLevel,
                                                           dataLevel);
    }
}

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::beforeYearByYearExport(uint, uint)
{
}

template<class ChildT, class VCardT>
template<class SearchVCardT, class O>
inline void IVariable<ChildT, VCardT>::computeSpatialAggregateWith(O& out, uint numSpace)
{
    // if this variable has the vcard we are looking for,
    // then we will add our results
    // In the most cases, the variable `out` is intermediate results.

    if constexpr (std::is_same_v<VCardT, SearchVCardT>)
    {
        SpatialAggregateOperation<std::is_same_v<VCardT, SearchVCardT>, // To avoid instanciation
                                  VCardT::spatialAggregate, // The spatial cluster operation to
                                                            // perform
                                  VCardType                 // The VCard
                                  >::Perform(out, *(static_cast<ChildT*>(this)), numSpace);
        return;
    }
}

template<class ChildT, class VCardT>
template<class SearchVCardT, class O>
inline void IVariable<ChildT, VCardT>::computeSpatialAggregateWith(O&, const Data::Area*)
{
}

template<class ChildT, class VCardT>
template<class VCardToFindT>
inline void IVariable<ChildT, VCardT>::retrieveResultsForArea(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::Area*)
{
    if constexpr (std::is_same_v<VCardT, VCardToFindT>)
    {
        *result = &pResults;
    }
    else
    {
        *result = nullptr;
    }
}

template<class ChildT, class VCardT>
template<class VCardToFindT>
inline void IVariable<ChildT, VCardT>::retrieveResultsForThermalCluster(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::ThermalCluster*)
{
    if constexpr (std::is_same_v<VCardT, VCardToFindT>)
    {
        *result = &pResults;
    }
    else
    {
        *result = nullptr;
    }
}

template<class ChildT, class VCardT>
template<class VCardToFindT>
inline void IVariable<ChildT, VCardT>::retrieveResultsForLink(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::AreaLink*)
{
    if constexpr (std::is_same_v<VCardT, VCardToFindT>)
    {
        *result = &pResults;
    }
    else
    {
        *result = nullptr;
    }
}

template<class ChildT, class VCardT>
inline Antares::Memory::Stored<double>::ConstReturnType
IVariable<ChildT, VCardT>::retrieveRawHourlyValuesForCurrentYear(uint column, uint) const
{
    if constexpr (VCardType::columnCount > 1)
    {
        return pResults[column].hourlyValuesForSpatialAggregate();
    }
    else if constexpr (VCardType::columnCount == 1)
    {
        return pResults.hourlyValuesForSpatialAggregate();
    }
    else
    {
        return Antares::Memory::Stored<double>::NullValue();
    }
}

template<class ChildT, class VCardT>
inline const typename Storage<VCardT>::ResultsType& IVariable<ChildT, VCardT>::results() const
{
    return pResults;
}

namespace
{
template<int ColumnT, class VCardT, class ChildT>
class RetrieveVariableListHelper
{
public:
    template<class PredicateT>
    static void Do(PredicateT& predicate)
    {
        for (int i = 0; i < VCardT::columnCount; ++i)
        {
            predicate.add(VCardT::Multiple::Caption(i),
                          VCardT::Multiple::Unit(i),
                          VCardT::Description());
        }
    }

    // Function used to build the collection of variables print info from the static variables list.
    // Multiple variable function version.
    static void Do(Data::variablePrintInfoCollector& printInfoCollector)
    {
        for (int i = 0; i < VCardT::columnCount; ++i)
        {
            printInfoCollector.add(VCardT::Multiple::Caption(i),
                                   VCardT::categoryDataLevel,
                                   VCardT::categoryFileLevel);
        }
    }
};

template<class VCardT, class ChildT>
class RetrieveVariableListHelper<Category::singleColumn, VCardT, ChildT>
{
public:
    template<class PredicateT>
    static void Do(PredicateT& predicate)
    {
        predicate.add(VCardT::Caption(), VCardT::Unit(), VCardT::Description());
    }

    // Function used to build the collection of variables print info from the static variables list.
    // Single variable function version.
    static void Do(Data::variablePrintInfoCollector& printInfoCollector)
    {
        printInfoCollector.add(VCardT::Caption(),
                               VCardT::categoryDataLevel,
                               VCardT::categoryFileLevel);
    }
};

template<class VCardT, class ChildT>
class RetrieveVariableListHelper<Category::dynamicColumns, VCardT, ChildT>
{
public:
    template<class PredicateT>
    static void Do(PredicateT&)
    {
    }

    // Function used to build the collection of variables print info from the static variables list.
    // Dynamic variable function version.
    static void Do(Data::variablePrintInfoCollector& printInfoCollector)
    {
        printInfoCollector.add(VCardT::Caption(),
                               VCardT::categoryDataLevel,
                               VCardT::categoryFileLevel);
    }
};

} // anonymous namespace

template<class ChildT, class VCardT>
template<class PredicateT>
void IVariable<ChildT, VCardT>::RetrieveVariableList(PredicateT& predicate)
{
    RetrieveVariableListHelper<VCardType::columnCount, VCardType, ChildT>::Do(predicate);
}

namespace
{
template<int ColumnT, class VCardT>
class GetPrintStatusHelper
{
public:
    static void Do(Data::Study& study, bool* isPrinted)
    {
        for (uint i = 0; i != VCardT::columnCount; ++i)
        {
            // Shifting inside the variables print info collection until reaching the print info
            // associated with the current name, and then getting its print status.
            isPrinted[i] = study.parameters.variablesPrintInfo.isPrinted(
              VCardT::Multiple::Caption(i));
        }
    }
};

template<class VCardT>
class GetPrintStatusHelper<Category::singleColumn, VCardT>
{
public:
    static void Do(Data::Study& study, bool* isPrinted)
    {
        // Shifting inside the variables print info collection until reaching the print info
        // associated with the current name, and then getting its print status.
        isPrinted[0] = study.parameters.variablesPrintInfo.isPrinted(VCardT::Caption());
    }
};

template<class VCardT>
class GetPrintStatusHelper<Category::dynamicColumns, VCardT>
{
public:
    static void Do(Data::Study& study, bool* isPrinted)
    {
        // Shifting inside the variables print info collection until reaching the print info
        // associated with the current name, and then getting its print status.
        isPrinted[0] = study.parameters.variablesPrintInfo.isPrinted(VCardT::Caption());
    }
};
} // namespace

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::getPrintStatusFromStudy(Data::Study& study)
{
    GetPrintStatusHelper<VCardType::columnCount, VCardType>::Do(study, isPrinted);
}

namespace
{
template<int ColumnT, class VCardT>
class SupplyMaxNbColumnsHelper
{
public:
    static void Do(Data::Study& study, uint maxNumberColumns)
    {
        for (uint i = 0; i != VCardT::columnCount; ++i)
        {
            study.parameters.variablesPrintInfo.setMaxColumns(VCardT::Multiple::Caption(i),
                                                              maxNumberColumns);
        }
    }
};

template<class VCardT>
class SupplyMaxNbColumnsHelper<Category::singleColumn, VCardT>
{
public:
    static void Do(Data::Study& study, uint maxNumberColumns)
    {
        study.parameters.variablesPrintInfo.setMaxColumns(VCardT::Caption(), maxNumberColumns);
    }
};

template<class VCardT>
class SupplyMaxNbColumnsHelper<Category::dynamicColumns, VCardT>
{
public:
    static void Do(Data::Study& study, uint maxNumberColumns)
    {
        study.parameters.variablesPrintInfo.setMaxColumns(VCardT::Caption(), maxNumberColumns);
    }
};
} // namespace

template<class ChildT, class VCardT>
inline void IVariable<ChildT, VCardT>::supplyMaxNumberOfColumns(Data::Study& study)
{
    auto maxColumns = static_cast<const ChildT*>(this)->getMaxNumberColumns();
    SupplyMaxNbColumnsHelper<VCardType::columnCount, VCardType>::Do(study,
                                                                    static_cast<uint>(maxColumns));
}

} // namespace Antares::Solver::Variable

#endif // __SOLVER_VARIABLE_VARIABLE_HXX__
