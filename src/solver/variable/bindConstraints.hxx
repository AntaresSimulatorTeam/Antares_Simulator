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
#pragma once

#include <vector>
#include <utility>
#include "antares/study/binding_constraint/BindingConstraint.h"
#include "antares/study/fwd.h"
#include "bindConstraints.h"

namespace Antares::Solver::Variable
{
template<class NextT>
void BindingConstraints<NextT>::buildSurveyReport(SurveyResults& results,
                                                  int dataLevel,
                                                  int fileLevel,
                                                  int precision) const
{
    if (bool bcDataLevel = dataLevel & Category::bindingConstraint; !bcDataLevel)
        return;

    for (uint i = 0; i != pBCcount; ++i)
    {
        const NextType& bc = pBindConstraints[i];

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
    if (bool bcDataLevel = dataLevel & Category::bindingConstraint; !bcDataLevel)
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

template<class NextT>
void BindingConstraints<NextT>::initializeFromStudy(Data::Study& study)
{
    const std::vector<uint> InequalityBCindices
      = Data::BindingConstraintsRepository::getIndicesForInequalityBindingConstraints(study.bindingConstraints);

    // The total number of inequality binding constraints count
    // (we don't count BCs with equality sign)
    pBCcount = (uint)InequalityBCindices.size();

    // Reserving the memory
    if (pBCcount > 0)
    {
        pBindConstraints.resize(pBCcount);
    }

    for (uint i = 0; i != pBCcount; ++i)
    {
        NextType& bc = pBindConstraints[i];

        bc.setBindConstraintGlobalIndex(InequalityBCindices[i]);
        bc.initializeFromStudy(study);

        // Does user want to print output results related to the current binding constraint ?
        bc.getPrintStatusFromStudy(study);
    }

    // Here we supply the max number of columns to the variable print info collector 
    // This is a ugly hack (it's a work around).
    // We should have a simple call to :
    //      NextType::supplyMaxNumberOfColumns(study);
    // Instead, we have a few lines as a hack.
    // What we have to do is add to the print info collector a single VariablePrintInfo
    // that has a max columns size of : (nb of inequality BCs) x ResultsType::count
    // But note that for now, BC output variables are chained statically (one output variable per inequality BC).
    // The hack is to make the first BC output variable able to supply max columns size for all BC output variables
    // with its method getMaxNumberColumns().
    // A solution would be to make BC output variables (like BindingConstMarginCost) some DYNAMIC variables.
    if (pBCcount > 0)
    {
        NextType& bc = pBindConstraints[0];
        bc.setBindConstraintsCount(pBCcount);
        bc.supplyMaxNumberOfColumns(study);
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
} // namespace Antares::Solver::Variable
