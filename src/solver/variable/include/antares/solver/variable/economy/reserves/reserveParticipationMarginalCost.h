// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "antares/solver/variable/variable.h"

#include "vCardReserveParticipationMarginalCost.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

/*!
** \brief Reserve participation marginal cost for an area
*/
class ReserveParticipationMarginalCost
    : public IVariable<ReserveParticipationMarginalCost, VCardReserveParticipationMarginalCost>
{
public:
    using VCardType = VCardReserveParticipationMarginalCost;
    using AncestorType = IVariable<ReserveParticipationMarginalCost, VCardType>;

    using ResultsType = typename VCardType::ResultsType;

    using VariableAccessorType = VariableAccessor<ResultsType, VCardType::columnCount>;

    static constexpr std::size_t count = 1;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count = ((VCardType::categoryDataLevel & CDataLevel
                                       && VCardType::categoryFileLevel & CFile)
                                        ? static_cast<int>(VCardType::columnCount)
                                            * static_cast<int>(ResultsType::count)
                                        : 0);
    };

    ReserveParticipationMarginalCost() = default;

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear.resize(pNbYearsParallel);

        pSize = study->parameters.include.reserves
                  ? area->allCapacityReservations.value().areaCapacityReservations.size()
                  : 0;
        if (pSize)
        {
            AncestorType::pResults.resize(pSize);

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
            {
                pValuesForTheCurrentYear[numSpace].resize(pSize);
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
    }

    [[nodiscard]] size_t getMaxNumberColumns() const
    {
        return pSize * ResultsType::count;
    }

    void yearBegin(unsigned int /*year*/, unsigned int numSpace)
    {
        for (unsigned int i = 0; i != pSize; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }
    }

    void yearEnd(unsigned int /*year*/, unsigned int numSpace)
    {
        for (unsigned int i = 0; i < pSize; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].computeStatisticsForTheCurrentYear();
        }
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        VariableAccessorType::ComputeSummary(pValuesForTheCurrentYear[numSpace],
                                             AncestorType::pResults,
                                             year);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        if (state.study.parameters.include.reserves)
        {
            auto& area = state.area;
            int column = 0;
            auto reserves = state.problemeHebdo->allReserves.value()[area->index];
            for (const auto& reserve: reserves.areaCapacityReservations)
            {
                pValuesForTheCurrentYear[numSpace][column++].hour[state.hourInTheYear]
                  += state.hourlyResults->Reserves.value()[state.hourInTheWeek]
                       .CoutsMarginauxHoraires[reserve.areaReserveIndex];
            }
        }
    }

    Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
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

        if (AncestorType::isPrinted[0] && results.data.area->allCapacityReservations)
        {
            assert(results.data.area != nullptr);
            results.variableUnit = VCardType::Unit();
            int column = 0;
            for (const auto& reserveID:
                 results.data.area->allCapacityReservations.value().areaCapacityReservations
                   | std::views::keys)
            {
                Yuni::String caption = results.data.study.runtime.reserveIDToName.value().at(
                  reserveID);
                caption << "_MRG.COST";
                results.variableCaption = caption;
                pValuesForTheCurrentYear[numSpace][column]
                  .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
                column++;
            }
        }
    }

private:
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    size_t pSize = 0;
    unsigned int pNbYearsParallel = 0;

}; // class ReserveParticipationMarginalCost

} // namespace Antares::Solver::Variable::Economy::Reserves
