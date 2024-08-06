/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include <iomanip>
#include <sstream>
#include <string>

#include "antares/solver/constraints-builder/cbuilder.h"

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
                             * (*line)->ptr->parameters[columnLoopFlow][hour] * (int)includeLoopFlow
                           + (*line)->ptr->parameters[Data::fhlPShiftMinus][hour]
                               * includePhaseShift)
                          * currentCycle.sign[i];
                }
                else if (currentCycle.opType == Data::BindingConstraint::opBoth
                         && hour + 1 <= calendarEnd && hour + 1 >= calendarStart)
                {
                    ub += ((*line)->ptr->parameters[columnImpedance][hour]
                           * (*line)->ptr->parameters[columnLoopFlow][hour] * (int)includeLoopFlow)
                            * currentCycle.sign[i]
                          + std::min(((*line)->ptr->parameters[Data::fhlPShiftMinus][hour]
                                      * includePhaseShift)
                                       * currentCycle.sign[i],
                                     ((*line)->ptr->parameters[Data::fhlPShiftPlus][hour]
                                      * includePhaseShift)
                                       * currentCycle.sign[i]);

                    lb += ((*line)->ptr->parameters[columnImpedance][hour]
                           * (*line)->ptr->parameters[columnLoopFlow][hour] * (int)includeLoopFlow)
                            * currentCycle.sign[i]
                          + std::max(((*line)->ptr->parameters[Data::fhlPShiftMinus][hour]
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

                wm[(*line)] = (*line)->ptr->parameters[columnImpedance][hour]
                              * currentCycle.sign[i];
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
                auto constraint = addConstraint(
                  name1,
                  "both",
                  "hourly",
                  state->WeightMap,
                  0); // vocabulary is not so obvious here (less or greater)
                ret = constraint != nullptr;
                state->secondMember.resizeWithoutDataLost(constraint->RHSTimeSeries().width,
                                                          constraint->RHSTimeSeries().height,
                                                          0);
                constraint->RHSTimeSeries() = state->secondMember;

                // iterate the counter
                ++nSubCount;
            }
            else
            {
                auto constraint = addConstraint(
                  name1,
                  "equal",
                  "hourly",
                  state->WeightMap,
                  0); // vocabulary is not so obvious here (less or greater)
                ret = constraint != nullptr;
                state->secondMember.resizeWithoutDataLost(constraint->RHSTimeSeries().width,
                                                          constraint->RHSTimeSeries().height,
                                                          0);
                constraint->RHSTimeSeries() = state->secondMember;
            }
        }
        ++nCount;
    }

    return ret;
}

std::shared_ptr<Antares::Data::BindingConstraint> CBuilder::addConstraint(
  const Data::ConstraintName& name,
  const String& op,
  const String& type,
  const WeightMap& weights,
  const double& secondMember)
{
    // Create a new contraint
    auto constraint = pStudy.bindingConstraints.add(name);
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
        if (!Utils::isZero(j->second))
        {
            constraint->weight(j->first->ptr, j->second);
        }
    }

    // second members
    if (!Utils::isZero(secondMember))
    {
        constraint->RHSTimeSeries().fill(secondMember);
        constraint->RHSTimeSeries().markAsModified();
    }

    // mark all values as modified
    constraint->markAsModified();

    return constraint;
}

} // namespace Antares
