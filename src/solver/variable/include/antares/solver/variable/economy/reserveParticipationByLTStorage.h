#ifndef __SOLVER_VARIABLE_ECONOMY_ReserveParticipationByLTStorage_H__
#define __SOLVER_VARIABLE_ECONOMY_ReserveParticipationByLTStorage_H__

#include "../variable.h"
#include "./vCardReserveParticipationByLTStorage.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{
template<class NextT = Container::EndOfList>
class ReserveParticipationByLTStorage
 : public Variable::
     IVariable<ReserveParticipationByLTStorage<NextT>, NextT, VCardReserveParticipationByLTStorage>
{
public:
    typedef NextT NextType;
    typedef VCardReserveParticipationByLTStorage VCardType;
    typedef Variable::IVariable<ReserveParticipationByLTStorage<NextT>, NextT, VCardType>
      AncestorType;

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
            count
            = ((VCardType::categoryDataLevel & CDataLevel && VCardType::categoryFileLevel & CFile)
                 ? (NextType::template Statistics<CDataLevel, CFile>::count
                    + VCardType::columnCount * ResultsType::count)
                 : NextType::template Statistics<CDataLevel, CFile>::count),
        };
    };

public:
    ReserveParticipationByLTStorage() : pValuesForTheCurrentYear(NULL), pSize(0)
    {
    }

    ~ReserveParticipationByLTStorage()
    {
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            delete[] pValuesForTheCurrentYear[numSpace];
        delete[] pValuesForTheCurrentYear;
    }

    void initializeFromStudy(Data::Study& study)
    {
        NextType::initializeFromStudy(study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];

        pSize = 0;
        for (int areaIndex = 0; areaIndex < study->areas.size(); areaIndex++)
        {
                pSize += study->areas[areaIndex]->allCapacityReservations.size();
        }
        if (pSize)
        {
            AncestorType::pResults.resize(pSize);
            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
                pValuesForTheCurrentYear[numSpace]
                  = new VCardType::IntermediateValuesDeepType[pSize];

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
                for (unsigned int i = 0; i != pSize; ++i)
                    pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(*study);

            for (unsigned int i = 0; i != pSize; ++i)
            {
                AncestorType::pResults[i].initializeFromStudy(*study);
                AncestorType::pResults[i].reset();
            }
        }
        else
        {
            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
                pValuesForTheCurrentYear[numSpace] = nullptr;

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
            pValuesForTheCurrentYear[numSpace][i].reset();

        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuildForEachLongTermStorage(State& state, uint year, unsigned int numSpace)
    {
        if (pSize)
        {
            for (unsigned int i = state.study.runtime->rangeLimits.hour[Data::rangeBegin];
                 i <= state.study.runtime->rangeLimits.hour[Data::rangeEnd];
                 ++i)
            {
                if (state.reserveParticipationPerLTStorageForYear[i].size())
                {
                    for (auto const& [reserveName, reserveParticipation] :
                         state.reserveParticipationPerLTStorageForYear[i]["LongTermStorage"])
                    {
                        pValuesForTheCurrentYear[numSpace]
                                                [state.area->reserveParticipationLTStorageIndexMap
                                                   .get(reserveName)]
                            .hour[i]
                          = reserveParticipation;
                    }
                }
            }
        }

        NextType::yearEndBuildForEachLongTermStorage(state, year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year)
    {
        NextType::yearEndBuild(state, year);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        for (unsigned int i = 0; i < pSize; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].computeStatisticsForTheCurrentYear();
        }
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                        unsigned int nbYearsForCurrentSummary)
    {
        for (unsigned int numSpace = 0; numSpace < nbYearsForCurrentSummary; ++numSpace)
        {
            for (unsigned int i = 0; i < pSize; ++i)
            {
                AncestorType::pResults[i].merge(numSpaceToYear[numSpace],
                                                pValuesForTheCurrentYear[numSpace][i]);
            }
        }

        NextType::computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        NextType::hourForEachArea(state, numSpace);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace]->hour;
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
                auto reserveName = results.data.area->reserveParticipationLTStorageIndexMap.get(i);
                results.variableCaption = "LongTermStorage_" + reserveName; // VCardType::Caption();
                results.variableUnit = VCardType::Unit();
                pValuesForTheCurrentYear[numSpace][0].template buildAnnualSurveyReport<VCardType>(
                  results, fileLevel, precision);
            }
        }
    }


private:
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    size_t pSize;
    unsigned int pNbYearsParallel;

}; // class ReserveParticipationByLTStorage

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_ECONOMY_ReserveParticipationByLTStorage_H__