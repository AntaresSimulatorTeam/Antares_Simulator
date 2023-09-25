/*
** Copyright 2007-2023 RTE
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
#ifndef __SOLVER_VARIABLE_VARIABLE_HXX__
#define __SOLVER_VARIABLE_VARIABLE_HXX__

#include <yuni/core/static/types.h>
#include <antares/study/variable-print-info.h>

namespace Antares::Solver::Variable
{

template<class ChildT, class NextT, class VCardT>
inline IVariable<ChildT, NextT, VCardT>::IVariable()
{
    // Initialization
    // You should prefer the methods initializeFromStudy() or similiar
    // to initialize the internal variables

    // Number of column, where dimension -1 (dynamic) is avoided
    pColumnCount = VCardType::columnCount > 1 ? VCardType::columnCount : 1;

    // Allocation
    // Does current output variable appear non applicable in all output reports (of any kind :
    // area or district reports, annual or over all years reports, digest, ...) ?
    isNonApplicable = new bool[pColumnCount];
    // Does current output variable column(s) appear in all reports ?
    isPrinted = new bool[pColumnCount];

    // Initializing default print to true
    for (uint i = 0; i < pColumnCount; i++)
        isPrinted[i] = true;
}

template<class ChildT, class NextT, class VCardT>
inline IVariable<ChildT, NextT, VCardT>::~IVariable()
{
    delete[] isNonApplicable;
    delete[] isPrinted;
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::initializeFromStudy(Data::Study& study)
{
    // Next
    NextType::initializeFromStudy(study);
}

template<class ChildT, class NextT, class VCardT>
template<class R>
inline void IVariable<ChildT, NextT, VCardT>::InitializeResultsFromStudy(R& results,
                                                                         Data::Study& study)
{
    VariableAccessorType::InitializeAndReset(results, study);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::initializeFromArea(Data::Study* study,
                                                                 Data::Area* area)
{
    // Next
    NextType::initializeFromArea(study, area);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::initializeFromLink(Data::Study* study,
                                                                 Data::AreaLink* link)
{
    // Next
    NextType::initializeFromAreaLink(study, link);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::initializeFromThermalCluster(
  Data::Study* study,
  Data::Area* area,
  Data::ThermalCluster* cluster)
{
    // Next
    NextType::initializeFromThermalCluster(study, area, cluster);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::broadcastNonApplicability(bool applyNonApplicable)
{
    if (VCardType::isPossiblyNonApplicable != 0 && applyNonApplicable)
    {
        for (uint i = 0; i != pColumnCount; ++i)
            isNonApplicable[i] = true;
    }
    else
    {
        for (uint i = 0; i != pColumnCount; ++i)
            isNonApplicable[i] = false;
    }

    NextType::broadcastNonApplicability(applyNonApplicable);
}



template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::simulationBegin()
{
    // Next
    NextType::simulationBegin();
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::simulationEnd()
{
    NextType::simulationEnd();
}
template<class ChildT, class NextT, class VCardT>
size_t IVariable<ChildT, NextT, VCardT>::getMaxNumberColumns() const
{
    return VCardT::ResultsType::count;
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::yearBegin(uint year)
{
    // Next variable
    NextType::yearBegin(year);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::yearEnd(uint year)
{
    // Next variable
    NextType::yearEnd(year);
}

template<class ChildT, class NextT, class VCardT>
template<class V>
inline void IVariable<ChildT, NextT, VCardT>::yearEndSpatialAggregates(V& allVars, uint year)
{
    // Next variable
    NextType::template yearEndSpatialAggregates(allVars, year);
}

template<class ChildT, class NextT, class VCardT>
template<class V, class SetT>
inline void IVariable<ChildT, NextT, VCardT>::yearEndSpatialAggregates(V& allVars,
                                                                       uint year,
                                                                       const SetT& set)
{
    // Next variable
    NextType::template yearEndSpatialAggregates(allVars, year, set);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::yearEndBuildPrepareDataForEachThermalCluster(
  State& state,
  uint year,
  uint numSpace)
{
    // Next variable
    NextType::yearEndBuildPrepareDataForEachThermalCluster(state, year, numSpace);
}

template<class ChildT, class NextT, class VCardT>
template<class V>
inline void IVariable<ChildT, NextT, VCardT>::simulationEndSpatialAggregates(V& allVars)
{
    // Next variable
    NextType::template simulationEndSpatialAggregates(allVars);
}

template<class ChildT, class NextT, class VCardT>
template<class V, class SetT>
inline void IVariable<ChildT, NextT, VCardT>::simulationEndSpatialAggregates(V& allVars,
                                                                             const SetT& set)
{
    // Next variable
    NextType::template simulationEndSpatialAggregates(allVars, set);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::weekBegin(State& state)
{
    // Next variable
    NextType::weekBegin(state);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::weekForEachArea(State& state, unsigned int numSpace)
{
    // Next variable
    NextType::weekForEachArea(state, numSpace);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::hourBegin(uint hourInTheYear)
{
    // Next variable
    NextType::hourBegin(hourInTheYear);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::hourForEachArea(State& state)
{
    // Next variable
    NextType::hourForEachArea(state);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::hourForEachArea(State& state, unsigned int numSpace)
{
    // Next variable
    NextType::hourForEachArea(state, numSpace);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::yearEndBuildForEachThermalCluster(
  State& state,
  unsigned int year,
  unsigned int numSpace)
{
    // Next item in the list
    NextType::yearEndBuildForEachThermalCluster(state, year, numSpace);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::hourForEachLink(State& state, unsigned int numSpace)
{
    // Next item in the list
    NextType::hourForEachLink(state, numSpace);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::hourEnd(State& state, uint hourInTheYear)
{
    // Next
    NextType::hourEnd(state, hourInTheYear);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::weekEnd(State& state)
{
    // Next
    NextType::weekEnd(state);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::buildSurveyReport(SurveyResults& results,
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

            VariableAccessorType::template BuildSurveyReport<VCardType>(
              results, pResults, dataLevel, fileLevel, precision);
        }
    }

    // Ask to the next item in the static list to export
    // its results as well
    NextType::buildSurveyReport(results, dataLevel, fileLevel, precision);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::buildAnnualSurveyReport(SurveyResults& results,
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
            static_cast<const ChildT*>(this)->localBuildAnnualSurveyReport(
              results, fileLevel, precision, numSpace);
        }
    }

    // Ask to the next item in the static list to export
    // its results as well
    NextType::buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::buildDigest(SurveyResults& results,
                                                          int digestLevel,
                                                          int dataLevel) const
{
    // Generate the Digest for the local results (areas part)
    if (VCardType::columnCount != 0
        && (VCardType::categoryDataLevel & Category::setOfAreas
            || VCardType::categoryDataLevel & Category::area
            || VCardType::categoryDataLevel & Category::link))
    {
        // Initializing pointer on variable non applicable and print stati arrays to beginning
        results.isPrinted = isPrinted;
        results.isCurrentVarNA = isNonApplicable;

        VariableAccessorType::template BuildDigest<VCardT>(
          results, pResults, digestLevel, dataLevel);
    }
    // Ask to build the digest to the next variable
    NextType::buildDigest(results, digestLevel, dataLevel);
}

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::beforeYearByYearExport(uint year, uint numspace)
{
    NextType::beforeYearByYearExport(year, numspace);
}

template<class ChildT, class NextT, class VCardT>
inline uint64_t IVariable<ChildT, NextT, VCardT>::memoryUsage() const
{
    uint64_t r = VariableAccessorType::Value(pResults);
    if ((int)VCardT::columnCount != (int)Category::dynamicColumns)
    {
        // Intermediate values
        if (VCardT::hasIntermediateValues)
        {
            for (uint i = 0; i != (uint)VCardT::columnCount; ++i)
                r += IntermediateValues::MemoryUsage();
        }
    }
    r += NextType::memoryUsage();
    return r;
}

template<class ChildT, class NextT, class VCardT>
template<class I>
inline void IVariable<ChildT, NextT, VCardT>::provideInformations(I& infos)
{
    // Begining of the node
    if (VCardType::nodeDepthForGUI)
    {
        infos.template beginNode<VCardType>();
        // Next variable in the list
        NextType::template provideInformations<I>(infos);
        // End of the node
        infos.endNode();
    }
    else
    {
        // Giving our VCard
        infos.template addVCard<VCardType>();
        // Next variable in the list
        NextType::template provideInformations<I>(infos);
    }
}

template<class ChildT, class NextT, class VCardT>
template<class SearchVCardT, class O>
inline void IVariable<ChildT, NextT, VCardT>::computeSpatialAggregateWith(O& out, uint numSpace)
{
    // if this variable has the vcard we are looking for,
    // then we will add our results
    // In the most cases, the variable `out` is intermediate results.

    if (Yuni::Static::Type::StrictlyEqual<VCardT, SearchVCardT>::Yes)
    {
        SpatialAggregateOperation<
          Yuni::Static::Type::StrictlyEqual<VCardT, SearchVCardT>::Yes, // To avoid instanciation
          VCardT::spatialAggregate, // The spatial cluster operation to perform
          VCardType                 // The VCard
          >::Perform(out, *(static_cast<ChildT*>(this)), numSpace);
        return;
    }
    // Otherwise we keep looking
    NextType::template computeSpatialAggregateWith<SearchVCardT, O>(out, numSpace);
}

template<class ChildT, class NextT, class VCardT>
template<class SearchVCardT, class O>
inline void IVariable<ChildT, NextT, VCardT>::computeSpatialAggregateWith(O& out,
                                                                          const Data::Area* area)
{
    NextType::template computeSpatialAggregateWith<SearchVCardT, O>(out, area);
}

namespace // anonymous
{
template<int Match>
struct RetrieveResultsAssignment
{
    enum
    {
        Yes = 1
    };
    template<class ResultsT, class O>
    static void Do(ResultsT& varResults, O** result)
    {
        *result = &varResults;
    }
};

template<>
struct RetrieveResultsAssignment<0>
{
    enum
    {
        Yes = 0
    };
    template<class ResultsT, class O>
    static void Do(ResultsT&, O**)
    {
        // Do nothing
    }
};

} // namespace

template<class ChildT, class NextT, class VCardT>
template<class VCardToFindT>
inline const double* IVariable<ChildT, NextT, VCardT>::retrieveHourlyResultsForCurrentYear(
  uint numSpace) const
{
    using AssignT
      = RetrieveResultsAssignment<Yuni::Static::Type::StrictlyEqual<VCardT, VCardToFindT>::Yes>;
    return (AssignT::Yes)
             ? nullptr
             : NextType::template retrieveHourlyResultsForCurrentYear<VCardToFindT>(numSpace);
}

template<class ChildT, class NextT, class VCardT>
template<class VCardToFindT>
inline void IVariable<ChildT, NextT, VCardT>::retrieveResultsForArea(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::Area* area)
{
    using AssignT
      = RetrieveResultsAssignment<Yuni::Static::Type::StrictlyEqual<VCardT, VCardToFindT>::Yes>;
    AssignT::Do(pResults, result);
    if (!AssignT::Yes)
        NextType::template retrieveResultsForArea<VCardToFindT>(result, area);
}

template<class ChildT, class NextT, class VCardT>
template<class VCardToFindT>
inline void IVariable<ChildT, NextT, VCardT>::retrieveResultsForThermalCluster(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::ThermalCluster* cluster)
{
    using AssignT
      = RetrieveResultsAssignment<Yuni::Static::Type::StrictlyEqual<VCardT, VCardToFindT>::Yes>;
    AssignT::Do(pResults, result);
    if (!AssignT::Yes)
        NextType::template retrieveResultsForThermalCluster<VCardToFindT>(result, cluster);
}

template<class ChildT, class NextT, class VCardT>
template<class VCardToFindT>
inline void IVariable<ChildT, NextT, VCardT>::retrieveResultsForLink(
  typename Storage<VCardToFindT>::ResultsType** result,
  const Data::AreaLink* link)
{
    using AssignT
      = RetrieveResultsAssignment<Yuni::Static::Type::StrictlyEqual<VCardT, VCardToFindT>::Yes>;
    AssignT::Do(pResults, result);
    if (!AssignT::Yes)
        NextType::template retrieveResultsForLink<VCardToFindT>(result, link);
}

namespace // anonymous
{
template<int ColumnT>
struct HourlyResultsForCurrentYear
{
    template<class R>
    static Antares::Memory::Stored<double>::ConstReturnType Get(const R& results, uint column)
    {
        return results[column].hourlyValuesForSpatialAggregate();
    }
};

template<>
struct HourlyResultsForCurrentYear<Category::singleColumn>
{
    template<class R>
    static Antares::Memory::Stored<double>::ConstReturnType Get(const R& results, uint)
    {
        return results.hourlyValuesForSpatialAggregate();
    }
};

template<>
struct HourlyResultsForCurrentYear<Category::noColumn>
{
    template<class R>
    static Antares::Memory::Stored<double>::ConstReturnType Get(const R&, uint)
    {
        return Antares::Memory::Stored<double>::NullValue();
    }
};

} // anonymous namespace

template<class ChildT, class NextT, class VCardT>
inline Antares::Memory::Stored<double>::ConstReturnType
  IVariable<ChildT, NextT, VCardT>::retrieveRawHourlyValuesForCurrentYear(uint column,
                                                                          uint /* numSpace */) const
{
    return HourlyResultsForCurrentYear<VCardType::columnCount>::Get(pResults, column);
}

template<class ChildT, class NextT, class VCardT>
inline const typename Storage<VCardT>::ResultsType& IVariable<ChildT, NextT, VCardT>::results()
  const
{
    return pResults;
}


// ===================================================================
// Each output variable gets registered in the print info collector
// ===================================================================

// class RetrieveVariableListHelper goes with function RetrieveVariableList(...).
// This class is used to make a different Do(...) treatment depending on current
// VCardType::columnCount. Recall that a variable can be single, dynamic or multiple.
namespace // anonymous
{
template<int ColumnT, class VCardT, class ChildT>
class RetrieveVariableListHelper
{
public:
    template<class PredicateT>
    static void Do(PredicateT& predicate)
    {
        for (int i = 0; i < VCardT::columnCount; ++i)
            predicate.add(VCardT::Multiple::Caption(i), VCardT::Multiple::Unit(i), VCardT::Description());
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

template<class ChildT, class NextT, class VCardT>
template<class PredicateT>
void IVariable<ChildT, NextT, VCardT>::RetrieveVariableList(PredicateT& predicate)
{
    RetrieveVariableListHelper<VCardType::columnCount, VCardType, ChildT>::Do(predicate);
    // Go to the next variable
    NextType::RetrieveVariableList(predicate);
}


// ============================================================================= 
// Each output variable gets its print status from the study parameters
// =============================================================================

// The class GetPrintStatusHelper is used to make a different Do(...) treatment depending on current
// VCardType::columnCount. Recall that a variable can be single, dynamic or multiple.
namespace // anonymous
{
    // Case : the variable is multiple
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
                isPrinted[i] = study.parameters.variablesPrintInfo.isPrinted(VCardT::Multiple::Caption(i));
            }
        }
    };

    // Case : the variable is single
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

    // Case : the variable is dynamic
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

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::getPrintStatusFromStudy(Data::Study& study)
{
    GetPrintStatusHelper<VCardType::columnCount, VCardType>::Do(study, isPrinted);
    // Go to the next variable
    NextType::getPrintStatusFromStudy(study);
}



// =======================================================================
// Each output variable supplies the maximum number of columns it takes 
// in an ouptut report to the variable print info instance
// =======================================================================

// The class SupplyMaxNbColumnsHelper is used to make a different Do(...) treatment depending on current
// VCardType::columnCount : recall that a variable can be single, dynamic or multiple.
namespace // anonymous
{
    // Case : the variable is multiple
    template<int ColumnT, class VCardT>
    class SupplyMaxNbColumnsHelper
    {
    public:
        static void Do(Data::Study& study, uint maxNumberColumns)
        {
            for (uint i = 0; i != VCardT::columnCount; ++i)
            {
                study.parameters.variablesPrintInfo.setMaxColumns(VCardT::Multiple::Caption(i), maxNumberColumns);
            }
        }
    };

    // Case : the variable is single
    template<class VCardT>
    class SupplyMaxNbColumnsHelper<Category::singleColumn, VCardT>
    {
    public:
        static void Do(Data::Study& study, uint maxNumberColumns)
        {
            study.parameters.variablesPrintInfo.setMaxColumns(VCardT::Caption(), maxNumberColumns);
        }
    };

    // Case : the variable is dynamic
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

template<class ChildT, class NextT, class VCardT>
inline void IVariable<ChildT, NextT, VCardT>::supplyMaxNumberOfColumns(Data::Study& study)
{
    auto max_columns = static_cast<const ChildT*>(this)->getMaxNumberColumns();
    SupplyMaxNbColumnsHelper<VCardType::columnCount, VCardType>::Do(study, static_cast<uint>(max_columns));
    // Go to the next variable
    NextType::supplyMaxNumberOfColumns(study);
}

} // namespace Antares::Solver::Variable



#endif // __SOLVER_VARIABLE_VARIABLE_HXX__
