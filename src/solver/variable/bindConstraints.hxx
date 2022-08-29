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
#pragma once

#include <vector>
#include <utility>
#include "antares/study/constraint/constraint.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
template<class NextT>
void BindingConstraints<NextT>::buildSurveyReport(SurveyResults& results,
                                                  int dataLevel,
                                                  int fileLevel,
                                                  int precision) const
{
    bool bcDataLevel = dataLevel & Category::bindingConstraint;
    if (not bcDataLevel)
        return;

    for (uint i = 0; i != pBCcount; ++i)
    {
        NextType& bc = pBindConstraints[i];

        bc.buildSurveyReport(results, dataLevel, fileLevel, precision);
    }
}

template<class NextT>
void BindingConstraints<NextT>::buildAnnualSurveyReport(SurveyResults& results,
                                                        int dataLevel,
                                                        int fileLevel,
                                                        int precision,
                                                        uint numSpace) const
{
    bool bcDataLevel = dataLevel & Category::bindingConstraint;
    if (not bcDataLevel)
        return;

    for (uint i = 0; i != pBCcount; ++i)
    {
        const NextType& bc = pBindConstraints[i];

        bc.buildAnnualSurveyReport(results, dataLevel, fileLevel, precision, numSpace);
    }
}

template<class NextT>
template<class PredicateT>
inline void BindingConstraints<NextT>::RetrieveVariableList(PredicateT& predicate)
{
    NextType::RetrieveVariableList(predicate);
}

template<class NextT>
template<class I>
inline void BindingConstraints<NextT>::provideInformations(I& infos)
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

inline std::vector<uint> getInequalityBindConstraintGlobalNumbers(const Data::Study& study)
{
    std::vector<uint> bindConstGlobalNumbers;
    Data::BindingConstraintRTI* allBindConst = study.runtime->bindingConstraint;
    for (uint k = 0; k < study.runtime->bindingConstraintCount; k++)
    {
        // We pick only inequality binding constraints.
        if (allBindConst[k].operatorType == '<' || allBindConst[k].operatorType == '>')
        {
            bindConstGlobalNumbers.push_back(k);
        }
    }
    return bindConstGlobalNumbers;
}

template<class NextT>
void BindingConstraints<NextT>::initializeFromStudy(Data::Study& study)
{
    std::vector<uint> InequalityBCnumbers = getInequalityBindConstraintGlobalNumbers(study);

    // The total number of inequality binding constraints count
    // (we don't count BCs with equality sign)
    pBCcount = (uint)InequalityBCnumbers.size();

    // Reserving the memory
    if (pBCcount > 0)
    {
        pBindConstraints.resize(pBCcount);
    }

    for (uint i = 0; i != pBCcount; ++i)
    {
        NextType& bc = pBindConstraints[i];

        bc.setBindConstraintGlobalNumber(InequalityBCnumbers[i]);
        bc.initializeFromStudy(study);

        // Does user want to print output results related to the current binding constraint ?
        bc.getPrintStatusFromStudy(study);
    }
}

template<class NextT>
void BindingConstraints<NextT>::computeSummary(std::map<unsigned int, unsigned int>& numSpaceToYear,
                                               unsigned int nbYearsForCurrentSummary)
{
    for (uint i = 0; i != pBCcount; ++i)
    {
        // Broadcast to all constraints
        pBindConstraints[i].computeSummary(numSpaceToYear, nbYearsForCurrentSummary);
    }
}

template<class NextT>
void BindingConstraints<NextT>::yearBegin(uint year, uint numSpace)
{
    // Broadcast to all binding constraints
    for (uint i = 0; i != pBCcount; ++i)
    {
        pBindConstraints[i].yearBegin(year, numSpace);
    }
}

template<class NextT>
void BindingConstraints<NextT>::yearEnd(uint year, uint numSpace)
{
    // Broadcast to all binding constraints
    for (uint i = 0; i != pBCcount; ++i)
    {
        pBindConstraints[i].yearEnd(year, numSpace);
    }
}

template<class NextT>
void BindingConstraints<NextT>::weekBegin(State& state)
{
    for (uint i = 0; i != pBCcount; ++i)
        pBindConstraints[i].weekBegin(state);
}

template<class NextT>
void BindingConstraints<NextT>::hourBegin(uint hourInTheYear)
{
    for (uint i = 0; i != pBCcount; ++i)
        pBindConstraints[i].hourBegin(hourInTheYear);
}

template<class NextT>
void BindingConstraints<NextT>::hourEnd(State& state, uint hourInTheYear)
{
    for (uint i = 0; i != pBCcount; ++i)
        pBindConstraints[i].hourEnd(state, hourInTheYear);
}

} // namespace Variable
} // namespace Solver
} // namespace Antares
