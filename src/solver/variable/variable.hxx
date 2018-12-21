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
#ifndef __SOLVER_VARIABLE_VARIABLE_HXX__
# define __SOLVER_VARIABLE_VARIABLE_HXX__

# include <yuni/core/static/types.h>



namespace Antares
{
namespace Solver
{
namespace Variable
{


	template<class ChildT, class NextT, class VCardT>
	inline void IVariable<ChildT,NextT,VCardT>::EstimateMemoryUsage(Data::StudyMemoryUsage& u)
	{
		if ((int)VCardT::columnCount != (int) Category::dynamicColumns)
		{
			// Results
			for (uint i = 0; i != VCardT::columnCount; ++i)
				ResultsType::EstimateMemoryUsage(u);

			// Intermediate values
			if (VCardT::hasIntermediateValues)
			{
				for (uint i = 0; i != VCardT::columnCount; ++i)
					IntermediateValues::EstimateMemoryUsage(u);
			}

			// Year-by-year
			if (!u.gatheringInformationsForInput)
			{
				if (u.study.parameters.yearByYear && u.mode != Data::stdmAdequacyDraft)
				{
					for (uint i = 0; i != u.years; ++i)
						u.takeIntoConsiderationANewTimeserieForDiskOutput(false);
				}
			}
		}
		NextType::EstimateMemoryUsage(u);
	}


	template<class ChildT, class NextT, class VCardT>
	inline IVariable<ChildT,NextT,VCardT>::IVariable()
	{
		// Initialization
		// You should prefer the methods initializeFromStudy() or similiar
		// to initialize the internal variables
	}



	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::initializeFromStudy(Data::Study& study)
	{
		// Next
		NextType::initializeFromStudy(study);
	}


	template<class ChildT, class NextT, class VCardT>
	template<class R>
	inline void
	IVariable<ChildT,NextT,VCardT>::InitializeResultsFromStudy(R& results, Data::Study& study)
	{
		VariableAccessorType::InitializeAndReset(results, study);
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::initializeFromArea(Data::Study* study, Data::Area* area)
	{
		// Next
		NextType::initializeFromArea(study, area);
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::initializeFromLink(Data::Study* study, Data::AreaLink* link)
	{
		// Next
		NextType::initializeFromAreaLink(study, link);
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::initializeFromThermalCluster(Data::Study* study, Data::Area* area, Data::ThermalCluster* cluster)
	{
		// Next
		NextType::initializeFromThermalCluster(study, area, cluster);
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::simulationBegin()
	{
		// Next
		NextType::simulationBegin();
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::simulationEnd()
	{
		NextType::simulationEnd();
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::yearBegin(uint year)
	{
		// Next variable
		NextType::yearBegin(year);
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::yearEnd(uint year)
	{
		// Next variable
		NextType::yearEnd(year);
	}


	template<class ChildT, class NextT, class VCardT>
	template<class V>
	inline void
	IVariable<ChildT,NextT,VCardT>::yearEndSpatialAggregates(V& allVars, uint year)
	{
		// Next variable
		NextType::template yearEndSpatialAggregates(allVars, year);
	}


	template<class ChildT, class NextT, class VCardT>
	template<class V, class SetT>
	inline void
	IVariable<ChildT,NextT,VCardT>::yearEndSpatialAggregates(V& allVars, uint year, const SetT& set)
	{
		// Next variable
		NextType::template yearEndSpatialAggregates(allVars, year, set);
	}


	template<class ChildT, class NextT, class VCardT>
	template<class V>
	inline void
	IVariable<ChildT,NextT,VCardT>::simulationEndSpatialAggregates(V& allVars)
	{
		// Next variable
		NextType::template simulationEndSpatialAggregates(allVars);
	}


	template<class ChildT, class NextT, class VCardT>
	template<class V, class SetT>
	inline void
	IVariable<ChildT,NextT,VCardT>::simulationEndSpatialAggregates(V& allVars, const SetT& set)
	{
		// Next variable
		NextType::template simulationEndSpatialAggregates(allVars, set);
	}

	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::weekBegin(State& state)
	{
		// Next variable
		NextType::weekBegin(state);
	}

	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::weekForEachArea(State& state, unsigned int numSpace)
	{
		// Next variable
		NextType::weekForEachArea(state, numSpace);
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::hourBegin(uint hourInTheYear)
	{
		// Next variable
		NextType::hourBegin(hourInTheYear);
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::hourForEachArea(State& state)
	{
		// Next variable
		NextType::hourForEachArea(state);
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::hourForEachThermalCluster(State& state)
	{
		// Next item in the list
		NextType::hourForEachThermalCluster(state);
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::hourForEachLink(State& state, unsigned int numSpace)
	{
		// Next item in the list
		NextType::hourForEachLink(state, numSpace);
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::hourEnd(State& state, uint hourInTheYear)
	{
		// Next
		NextType::hourEnd(state, hourInTheYear);
	}

	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::weekEnd(State& state)
	{
		// Next
		NextType::weekEnd(state);
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::buildSurveyReport(SurveyResults& results, int dataLevel, int fileLevel, int precision) const
	{
		// Generating value for the area
		// Only if there are some results to export...
		if (0 != ResultsType::count)
		{
			// And only if we match the current data level _and_ precision level
			if ((dataLevel & VCardType::categoryDataLevel) && (fileLevel & VCardType::categoryFileLevel)
				// Trying to use "VCardReservoirLevel::spatialAggregate" does not work here, another way has to be found,
				// avoiding to use "VCardReservoirLevel::Caption()", which is not generic enough.
				&& (precision & VCardType::precision) /*&& (ChildT::VCardType::spatialAggregate != Category::noSpatialAggregate)*/)
			{
				// Districts output & reservoir levels variable ("mc-all/areas/<district>") : prints "N/A" on reservoir levels columns
				// See what "ChildT::VCardType::spatialAggregate" is, and try not to select reservoir levels using its "Caption()".
				// if (VCardType::VCardOrigin::spatialAggregate != Category::noSpatialAggregate)
				if (std::strcmp(ChildT::VCardType::Caption(), "H. LEV") == 0)
				{
					if (!results.data.resLvlColRetrieved)
					{
						// Only min and max have to be printed in when dealing with "id-{precision}.txt"
						if (fileLevel & Category::id)
							for (int i = 0; i < 2; i++)
								results.data.ReservoirLvlColIdx.push_back(results.data.columnIndex + i);
						// All variable results are printed when dealing with "values-{precision}.txt"
						else
							for (int i = 0; i < ResultsType::count; i++)
								results.data.ReservoirLvlColIdx.push_back(results.data.columnIndex + i);

						results.data.resLvlColRetrieved = true;
					}
				}

				// Districts output & water value variable ("mc-all/areas/<district>") : prints "N/A" on water value columns
				if (std::strcmp(ChildT::VCardType::Caption(), "H. VAL") == 0)
				{
					if (!results.data.waterValColRetrieved)
					{
						// Only min and max have to be printed in when dealing with "id-{precision}.txt"
						if (fileLevel & Category::id)
							for (int i = 0; i < 2; i++)
								results.data.waterValuesColIdx.push_back(results.data.columnIndex + i);
						// All variable results are printed when dealing with "values-{precision}.txt"
						else
							for (int i = 0; i < ResultsType::count; i++)
								results.data.waterValuesColIdx.push_back(results.data.columnIndex + i);

						results.data.waterValColRetrieved = true;
					}
				}

				// Districts output & reservoir levels variable ("mc-all/areas/<district>") : prints "N/A" on overflows columns
				if (std::strcmp(ChildT::VCardType::Caption(), "H. OVFL") == 0)
				{
					if (!results.data.ovfColRetrieved)
					{
						// Only min and max have to be printed in when dealing with "id-{precision}.txt"
						if (fileLevel & Category::id)
							for (int i = 0; i < 2; i++)
								results.data.OverflowsColIdx.push_back(results.data.columnIndex + i);
						// All variable results are printed when dealing with "values-{precision}.txt"
						else
							for (int i = 0; i < ResultsType::count; i++)
								results.data.OverflowsColIdx.push_back(results.data.columnIndex + i);

						results.data.ovfColRetrieved = true;
					}
				}

				VariableAccessorType::template
					BuildSurveyReport<VCardType>(results, pResults, dataLevel, fileLevel, precision);
			}
		}
		// Ask to the next item in the static list to export
		// its results as well
		NextType:: buildSurveyReport(results, dataLevel, fileLevel, precision);
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::buildAnnualSurveyReport(SurveyResults& results, int dataLevel, int fileLevel, int precision, uint numSpace) const
	{
		// Generating value for the area
		// Only if there are some results to export...
		if (0 != ResultsType::count)
		{
			// And only if we match the current data level _and_ precision level
			if ((dataLevel & VCardType::categoryDataLevel) && (fileLevel & VCardType::categoryFileLevel)
				&& (precision & VCardType::precision))
			{
				// getting its imtermediate results
				static_cast<const ChildT*>(this)->localBuildAnnualSurveyReport(results, fileLevel, precision, numSpace);
			}
		}
		// Ask to the next item in the static list to export
		// its results as well
		NextType::buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
	{
		// Generate the Digest for the local results (areas part)
		if (VCardType::columnCount != 0
			&& (VCardType::categoryDataLevel & Category::setOfAreas
				|| VCardType::categoryDataLevel & Category::area
				|| VCardType::categoryDataLevel & Category::link))
		{
			// We don't print usual results in the digest file for reservoir levels (see above for improvement)
			if (std::strcmp(ChildT::VCardType::Caption(), "H. LEV") == 0)
			{
				if (!results.data.resLvlColRetrieved)
				{
					results.data.ReservoirLvlColIdx.push_back(results.data.columnIndex);
					results.data.resLvlColRetrieved = true;
				}
			}

			// We don't print usual results in the digest file for water values
			if (std::strcmp(ChildT::VCardType::Caption(), "H. VAL") == 0)
			{
				if (!results.data.waterValColRetrieved)
				{
					results.data.waterValuesColIdx.push_back(results.data.columnIndex);
					results.data.waterValColRetrieved = true;
				}
			}

			// We don't print usual results in the digest file for overflows (see above for improvement)
			if (std::strcmp(ChildT::VCardType::Caption(), "H. OVFL") == 0)
			{
				if (!results.data.ovfColRetrieved)
				{
					results.data.OverflowsColIdx.push_back(results.data.columnIndex);
					results.data.ovfColRetrieved = true;
				}
			}

			VariableAccessorType::template BuildDigest<VCardT>(results, pResults, digestLevel, dataLevel);
		}
		// Ask to build the digest to the next variable
		NextType::buildDigest(results, digestLevel, dataLevel);
	}


	template<class ChildT, class NextT, class VCardT>
	inline void
	IVariable<ChildT,NextT,VCardT>::beforeYearByYearExport(uint year, uint numspace)
	{
		NextType:: beforeYearByYearExport(year, numspace);
	}


	template<class ChildT, class NextT, class VCardT>
	inline Yuni::uint64
	IVariable<ChildT,NextT,VCardT>::memoryUsage() const
	{
		Yuni::uint64 r = VariableAccessorType::Value(pResults);
		if ((int)VCardT::columnCount != (int) Category::dynamicColumns)
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
	inline void
	IVariable<ChildT,NextT,VCardT>::provideInformations(I& infos)
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
	inline void
	IVariable<ChildT,NextT,VCardT>::computeSpatialAggregateWith(O& out, uint numSpace)
	{
		// if this variable has the vcard we are looking for,
		// then we will add our results
		// In the most cases, the variable `out` is intermediate results.

		if (Yuni::Static::Type::StrictlyEqual<VCardT,SearchVCardT>::Yes)
		{
			SpatialAggregateOperation<
				Yuni::Static::Type::StrictlyEqual<VCardT,SearchVCardT>::Yes, // To avoid instanciation
				VCardT::spatialAggregate, // The spatial cluster operation to perform
				VCardType // The VCard
				>::Perform(out, *(static_cast<ChildT*>(this)), numSpace);
			return;
		}
		// Otherwise we keep looking
		NextType::template computeSpatialAggregateWith<SearchVCardT,O>(out, numSpace);
	}


	template<class ChildT, class NextT, class VCardT>
	template<class SearchVCardT, class O>
	inline void
	IVariable<ChildT,NextT,VCardT>::computeSpatialAggregateWith(O& out, const Data::Area* area)
	{
		NextType::template computeSpatialAggregateWith<SearchVCardT,O>(out, area);
	}



	namespace // anonymous
	{

		template<int Match>
		struct RetrieveResultsAssignment
		{
			enum {Yes = 1};
			template<class ResultsT, class O> static void Do(ResultsT& varResults, O** result)
			{
				*result = &varResults;
			}
		};

		template<>
		struct RetrieveResultsAssignment<0>
		{
			enum {Yes = 0};
			template<class ResultsT, class O> static void Do(ResultsT&, O**)
			{
				// Do nothing
			}
		};

	} // anoymous namespace


	template<class ChildT, class NextT, class VCardT>
	template<class VCardToFindT>
	inline const double*
	IVariable<ChildT,NextT,VCardT>::retrieveHourlyResultsForCurrentYear(uint numSpace) const
	{
		typedef RetrieveResultsAssignment<
			Yuni::Static::Type::StrictlyEqual<VCardT,VCardToFindT>::Yes> AssignT;
		return (AssignT::Yes)
			? nullptr
			: NextType::template retrieveHourlyResultsForCurrentYear <VCardToFindT>(numSpace);
	}


	template<class ChildT, class NextT, class VCardT>
	template<class VCardToFindT>
	inline void
	IVariable<ChildT,NextT,VCardT>::retrieveResultsForArea(typename Storage<VCardToFindT>::ResultsType** result, const Data::Area* area)
	{
		typedef RetrieveResultsAssignment<
			Yuni::Static::Type::StrictlyEqual<VCardT,VCardToFindT>::Yes> AssignT;
		AssignT::Do(pResults, result);
		if (!AssignT::Yes)
			NextType::template retrieveResultsForArea<VCardToFindT>(result, area);
	}


	template<class ChildT, class NextT, class VCardT>
	template<class VCardToFindT>
	inline void
	IVariable<ChildT,NextT,VCardT>::retrieveResultsForThermalCluster(typename Storage<VCardToFindT>::ResultsType** result, const Data::ThermalCluster* cluster)
	{
		typedef RetrieveResultsAssignment<
			Yuni::Static::Type::StrictlyEqual<VCardT,VCardToFindT>::Yes> AssignT;
		AssignT::Do(pResults, result);
		if (!AssignT::Yes)
			NextType::template retrieveResultsForThermalCluster<VCardToFindT>(result, cluster);
	}


	template<class ChildT, class NextT, class VCardT>
	template<class VCardToFindT>
	inline void
	IVariable<ChildT,NextT,VCardT>::retrieveResultsForLink(typename Storage<VCardToFindT>::ResultsType** result, const Data::AreaLink* link)
	{
		typedef RetrieveResultsAssignment<
			Yuni::Static::Type::StrictlyEqual<VCardT,VCardToFindT>::Yes> AssignT;
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
		struct HourlyResultsForCurrentYear<1>
		{
			template<class R>
			static Antares::Memory::Stored<double>::ConstReturnType Get(const R& results, uint)
			{
				return results.hourlyValuesForSpatialAggregate();
			}
		};

		template<>
		struct HourlyResultsForCurrentYear<0>
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
	IVariable<ChildT,NextT,VCardT>::retrieveRawHourlyValuesForCurrentYear(uint column, uint /* numSpace */) const
	{
		return HourlyResultsForCurrentYear<VCardType::columnCount>::Get(pResults, column);
	}


	template<class ChildT, class NextT, class VCardT>
	inline const typename Storage<VCardT>::ResultsType&
	IVariable<ChildT,NextT,VCardT>::results() const
	{
		return pResults;
	}


	namespace // anonymous
	{
		template<int ColumnT, class VCardT>
		class RetrieveVariableListHelper
		{
		public:
			template<class PredicateT> static void Do(PredicateT& predicate)
			{
				for (int i = 0; i < VCardT::columnCount; ++i)
					predicate.add(VCardT::Multiple::Caption(i), VCardT::Unit(), VCardT::Description());
			}
		};

		template<class VCardT>
		class RetrieveVariableListHelper<1, VCardT>
		{
		public:
			template<class PredicateT> static void Do(PredicateT& predicate)
			{
				predicate.add(VCardT::Caption(), VCardT::Unit(), VCardT::Description());
			}
		};

		template<class VCardT>
		class RetrieveVariableListHelper<-1, VCardT>
		{
		public:
			template<class PredicateT> static void Do(PredicateT&)
			{}
		};

	} // anonymous namespace


	template<class ChildT, class NextT, class VCardT>
	template<class PredicateT>
	void IVariable<ChildT,NextT,VCardT>::RetrieveVariableList(PredicateT& predicate)
	{
		RetrieveVariableListHelper<VCardType::columnCount, VCardType>::Do(predicate);
		// Go to the next variable
		NextType::RetrieveVariableList(predicate);
	}






} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_VARIABLE_HXX__
