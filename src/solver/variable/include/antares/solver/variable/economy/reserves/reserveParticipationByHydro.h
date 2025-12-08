#pragma once

#include "../../variable.h"
#include "./vCardReserveParticipationByHydro.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{
template<class NextT = Container::EndOfList>
class ReserveParticipationByHydro: public Variable::IVariable<ReserveParticipationByHydro<NextT>,
                                                              NextT,
                                                              VCardReserveParticipationByHydro>
{
public:
    typedef NextT NextType;
    typedef VCardReserveParticipationByHydro VCardType;
    typedef Variable::IVariable<ReserveParticipationByHydro<NextT>, NextT, VCardType> AncestorType;

    typedef typename VCardType::ResultsType ResultsType;

    typedef VariableAccessor<ResultsType, VCardType::columnCount> VariableAccessorType;

    enum
    {
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
    ReserveParticipationByHydro() = default;

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear.resize(pNbYearsParallel);

        // Get the number of Hydro reserveParticipations
        pSize = study->parameters.reservesEnabled ? area->hydro.reserveParticipationsCount() : 0;
        if (pSize)
        {
            AncestorType::pResults.resize(pSize);
            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                pValuesForTheCurrentYear[numSpace].resize(pSize);
            }

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                for (unsigned int i = 0; i != pSize; ++i)
                {
                    pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(*study);
                }
            }

            for (unsigned int i = 0; i != pSize; ++i)
            {
                AncestorType::pResults[i].initializeFromStudy(*study);
                AncestorType::pResults[i].reset();
            }
        }
        else
        {
            AncestorType::pResults.clear();
        }

        NextType::initializeFromArea(study, area);
    }

    size_t getMaxNumberColumns() const
    {
        return pSize * ResultsType::count;
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
        NextType::initializeFromAreaLink(study, link);
    }

    void simulationBegin()
    {
        NextType::simulationBegin();
    }

    void simulationEnd()
    {
        NextType::simulationEnd();
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        for (unsigned int i = 0; i != pSize; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }

        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuildForEachThermalCluster(State& state, uint year, unsigned int numSpace)
    {
        NextType::yearEndBuildForEachThermalCluster(state, year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
    {
        NextType::yearEndBuild(state, year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        for (unsigned int i = 0; i < pSize; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].computeStatisticsForTheCurrentYear();
        }
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
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        if (state.study.parameters.reservesEnabled
            && state.study.runtime.reserveParticipationIndexMaps.value()
                 .at(state.area->id)
                 .Hydro.size())
        {
            for (const auto& [reserveName, reserveParticipation]:
                 state.reserveParticipationPerHydroForYear[state.hourInTheYear]["Hydro"])
            {
                pValuesForTheCurrentYear[numSpace]
                                        [state.study.runtime.reserveParticipationIndexMaps.value()
                                           .at(state.area->id)
                                           .Hydro.left.at(reserveName)]
                                          .hour[state.hourInTheYear]
                  = reserveParticipation;
            }
        }

        NextType::hourForEachArea(state, numSpace);
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
        results.isCurrentVarNA = AncestorType::isNonApplicable;
        if (AncestorType::isPrinted[0])
        {
            assert(NULL != results.data.area);
            for (uint i = 0; i < pSize; ++i)
            {
                if (results.data.study.parameters.reservesEnabled
                    && results.data.study.runtime.reserveParticipationIndexMaps.value()
                         .at(results.data.area->id)
                         .Hydro.size()) // Bimap is not empty
                {
                    auto reserveName = results.data.study.runtime.reserveParticipationIndexMaps
                                         .value()
                                         .at(results.data.area->id)
                                         .Hydro.right.at(i);
                    results.variableCaption = reserveName + "_Hydro"; // VCardType::Caption();
                    results.variableUnit = VCardType::Unit();
                    pValuesForTheCurrentYear[numSpace][i]
                      .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
                }
            }
        }
    }

private:
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    size_t pSize = 0;
    unsigned int pNbYearsParallel = 0;

}; // class ReserveParticipationByHydro

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares
