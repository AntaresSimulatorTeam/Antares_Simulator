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
#include "cbuilder.h"

#include <string>
#include <sstream>
#include <iomanip>

using namespace Yuni;

namespace Antares
{
bool CBuilder::createConstraints(const std::vector<Vector>& mesh)
{
    uint nCount = alreadyExistingNetworkConstraints(CB_PREFIX) + 1;
    uint nSubCount = 1;
    bool ret = false;

    uint columnImpedance = (uint)Antares::Data::fhlImpedances;
    uint columnLoopFlow = (uint)Antares::Data::fhlLoopFlow;

    std::vector<Cycle> cycleBase;
    int count = 1;
    auto i = mesh.begin();
    logs.debug() << "calendarstart: " << calendarStart;
    logs.debug() << "calendarEnd: " << calendarEnd;
    logs.debug() << "infinite_value: " << infiniteSecondMember;
    logs.debug() << "nodal_loopflow_check: " << checkNodalLoopFlow;
    logs.debug() << "delete: " << pDelete;

    for (; i != mesh.end(); i++, count++)
    {
        logs.info() << "Writing constraints (" << count << "/" << mesh.size() << ")";
        const Vector& loop = (*i);

        Cycle currentCycle(loop, infiniteSecondMember);
        if (calendarEnd != 8760 || calendarStart != 1)
        {
            currentCycle.opType = Data::BindingConstraint::opBoth;

            logs.error() << "Calendar start and end needs to be default values: 1 and 8760";
            logs.error() << "Actual values: start " << calendarStart << " end " << calendarEnd;
            return false;
        }

        for (uint hour = 0; hour < currentCycle.time; ++hour)
        {
            std::vector<double> impedanceVector;
            WeightMap wm;
            // initiate second members
            double lb(0), ub(0);
            int i = 0;
            for (auto line = loop.begin(); line != loop.end(); line++, i++)
            {
                impedanceVector.push_back((*line)->ptr->parameters[columnImpedance][hour]);
                /*PN-TODO: Check the formula (page 3)*/
                if (currentCycle.opType == Data::BindingConstraint::opEquality)
                {
                    ub += ((*line)->ptr->parameters[columnImpedance][hour]
                                 * (*line)->ptr->parameters[columnLoopFlow][hour]
                                 * (int)includeLoopFlow
                             + (*line)->ptr->parameters[Data::fhlPShiftMinus][hour]
                                 * includePhaseShift)
                         * currentCycle.sign[i];
                }
                else if (currentCycle.opType == Data::BindingConstraint::opBoth
                         && hour + 1 <= calendarEnd && hour + 1 >= calendarStart)
                {
                    ub += ((*line)->ptr->parameters[columnImpedance][hour]
                               * (*line)->ptr->parameters[columnLoopFlow][hour]
                               * (int)includeLoopFlow)
                               * currentCycle.sign[i]
                            + std::min(
                                 ((*line)->ptr->parameters[Data::fhlPShiftMinus][hour]
                                    * includePhaseShift)
                                    * currentCycle.sign[i],
                                 ((*line)->ptr->parameters[Data::fhlPShiftPlus][hour]
                                    * includePhaseShift)
                                    * currentCycle.sign[i]);

                    lb += ((*line)->ptr->parameters[columnImpedance][hour]
                               * (*line)->ptr->parameters[columnLoopFlow][hour]
                               * (int)includeLoopFlow)
                               * currentCycle.sign[i]
                            + std::max(
                                 ((*line)->ptr->parameters[Data::fhlPShiftMinus][hour]
                                    * includePhaseShift)
                                    * currentCycle.sign[i],
                                 ((*line)->ptr->parameters[Data::fhlPShiftPlus][hour]
                                    * includePhaseShift)
                                    * currentCycle.sign[i]);
                }
                else
                {
                    lb = infiniteSecondMember;
                    ub = -1 * infiniteSecondMember;
                }

                wm[(*line)]
                  = (*line)->ptr->parameters[columnImpedance][hour] * currentCycle.sign[i];
            }

            State& st = currentCycle.getState(impedanceVector);

            if (currentCycle.opType == Data::BindingConstraint::opBoth)
            {
                st.secondMember.entry[0][hour] = std::max(lb, ub);
                st.secondMember.entry[1][hour] = std::min(ub, lb);
            }
            else
            {
                st.secondMember.entry[2][hour] = ub;
            }
            st.WeightMap = wm;
        }

        cycleBase.push_back(currentCycle);
    }

    for (auto cycle = cycleBase.begin(); cycle != cycleBase.end(); cycle++)
    {
        nSubCount = 1;
        for (auto state = cycle->states.begin(); state != cycle->states.end(); state++)
        {
            Data::ConstraintName name1;
            std::stringstream ss;
            ss << std::setw(4) << std::setfill('0') << nCount;
            name1 << pPrefix << ss.str();
            if (cycle->opType == Data::BindingConstraint::opBoth)
            {
                name1 << "." << nSubCount;
                auto constraint
                  = addConstraint(name1,
                                  "both",
                                  "hourly",
                                  state->WeightMap,
                                  0); // vocabulary is not so obvious here (less or greater)
                ret = constraint != nullptr;
                state->secondMember.resizeWithoutDataLost(
                        constraint->RHSTimeSeries().width, constraint->RHSTimeSeries().height, 0);
                constraint->RHSTimeSeries() = state->secondMember;

                // iterate the counter
                ++nSubCount;
            }
            else
            {
                auto constraint
                  = addConstraint(name1,
                                  "equal",
                                  "hourly",
                                  state->WeightMap,
                                  0); // vocabulary is not so obvious here (less or greater)
                ret = constraint != nullptr;
                state->secondMember.resizeWithoutDataLost(
                        constraint->RHSTimeSeries().width, constraint->RHSTimeSeries().height, 0);
                constraint->RHSTimeSeries() = state->secondMember;
            }
        }
        ++nCount;
    }

    return ret;
}

std::shared_ptr<Antares::Data::BindingConstraint> CBuilder::addConstraint(const Data::ConstraintName& name,
                                                          const String& op,
                                                          const String& type,
                                                          const WeightMap& weights,
                                                          const double& secondMember)
{
    // Create a new contraint
    auto constraint = pStudy->bindingConstraints.add(name);
    const Data::BindingConstraint::Operator o = Data::BindingConstraint::StringToOperator(op);
    assert(o != Data::BindingConstraint::opUnknown);
    const Data::BindingConstraint::Type t = Data::BindingConstraint::StringToType(type);
    assert(t != Data::BindingConstraint::typeUnknown);

    // Reseting
    constraint->clearAndReset(name, t, o);
    constraint->removeAllWeights();
    constraint->enabled(true);

    // weights
    for (auto j = weights.begin(); j != weights.end(); j++)
    {
        if (!Math::Zero(j->second))
            constraint->weight(j->first->ptr, j->second);
    }

    // second members
    if (!Math::Zero(secondMember))
    {
        constraint->RHSTimeSeries().fill(secondMember);
        constraint->RHSTimeSeries().markAsModified();
    }

    // mark all values as modified
    constraint->markAsModified();

    return constraint;
}

} // namespace Antares
