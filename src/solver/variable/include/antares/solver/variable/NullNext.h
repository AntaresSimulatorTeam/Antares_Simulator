
/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once
#include "info.h"
#include "state.h"
#include "storage/intermediate.h"
#include "storage/results.h"

namespace Antares::Solver::Variable
{
// Type sentinelle interne remplaçant Yuni::Default
struct NullNext
{
    static constexpr int count = 0;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count = 0
        };
    };

    template<class PredicateT>
    static void RetrieveVariableList(PredicateT&)
    {
    }

    static void initializeFromStudy(Data::Study&)
    {
    }

    static void initializeFromArea(Data::Study*, Data::Area*)
    {
    }

    static void initializeFromLink(Data::Study*, Data::AreaLink*)
    {
    }

    static void initializeFromThermalCluster(Data::Study*, Data::Area*, Data::ThermalCluster*)
    {
    }

    static void broadcastNonApplicability(bool)
    {
    }

    static void getPrintStatusFromStudy(Data::Study&)
    {
    }

    static void supplyMaxNumberOfColumns(Data::Study&)
    {
    }

    static void simulationBegin()
    {
    }

    static void simulationEnd()
    {
    }

    static void yearBegin(uint)
    {
    }

    static void yearEnd(uint)
    {
    }

    template<class V>
    static void yearEndSpatialAggregates(V&, uint, unsigned int)
    {
    }

    template<class V, class SetT>
    static void yearEndSpatialAggregates(V&, uint, const SetT&)
    {
    }

    template<class V>
    static void simulationEndSpatialAggregates(V&)
    {
    }

    template<class V, class SetT>
    static void simulationEndSpatialAggregates(V&, const SetT&)
    {
    }

    static void hourBegin(uint)
    {
    }

    static void hourEnd(State&, uint)
    {
    }

    static void hourForEachArea(State&)
    {
    }

    static void hourForEachArea(State&, unsigned int)
    {
    }

    static void hourForEachLink(State&, unsigned int)
    {
    }

    static void weekBegin(State&)
    {
    }

    static void weekForEachArea(State&, uint)
    {
    }

    static void weekEnd(State&)
    {
    }

    static void buildSurveyReport(SurveyResults&, int, int, int)
    {
    }

    static void buildAnnualSurveyReport(SurveyResults&, int, int, int, uint)
    {
    }

    static void buildDigest(SurveyResults&, int, int)
    {
    }

    static void beforeYearByYearExport(uint, uint)
    {
    }

    template<class I>
    static void provideInformations(I&)
    {
    }

    template<class VCardSearchT, class O>
    static void computeSpatialAggregateWith(O&, uint)
    {
    }

    template<class VCardSearchT, class O>
    static void computeSpatialAggregateWith(O&, const Data::Area*)
    {
    }

    template<class VCardToFindT>
    static const double* retrieveHourlyResultsForCurrentYear(uint)
    {
        return nullptr;
    }

    template<class VCardToFindT>
    static void retrieveResultsForArea(typename Storage<VCardToFindT>::ResultsType**,
                                       const Data::Area*)
    {
    }

    template<class VCardToFindT>
    static void retrieveResultsForThermalCluster(typename Storage<VCardToFindT>::ResultsType**,
                                                 const Data::ThermalCluster*)
    {
    }

    template<class VCardToFindT>
    static void retrieveResultsForLink(typename Storage<VCardToFindT>::ResultsType**,
                                       const Data::AreaLink*)
    {
    }

    static Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      uint,
      uint)
    {
        return Antares::Memory::Stored<double>::NullValue();
    }
};
} // namespace Antares::Solver::Variable
