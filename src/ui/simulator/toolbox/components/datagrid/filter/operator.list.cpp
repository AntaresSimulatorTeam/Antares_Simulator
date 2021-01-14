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

#include "operator.list.h"
#include "operator/all.h"
#include "filter.h"
#include <antares/date.h>

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
namespace Operator
{
List::List(AFilterBase* parent) : pParentFilter(parent)
{
}

List::~List()
{
    this->clear();
}

void List::addStdArithmetic(bool withModulo)
{
    pItems.push_back(new Operator::EqualsTo(pParentFilter));
    pItems.push_back(new Operator::NotEqualsTo(pParentFilter));
    pItems.push_back(new Operator::LessThan(pParentFilter));
    pItems.push_back(new Operator::LessThanOrEqualsTo(pParentFilter));
    pItems.push_back(new Operator::GreaterThan(pParentFilter));
    pItems.push_back(new Operator::GreaterThanOrEqualsTo(pParentFilter));
    if (withModulo)
        pItems.push_back(new Operator::Modulo(pParentFilter));
}

template<class T>
static inline T* WeekDayOp(AFilterBase* parent)
{
    T* t = new T(parent);
    Parameter& p = t->parameters[0];
    p.dataType = DataType::dtList;
    for (int i = 0; i < 7; ++i)
        p.defaultValues.push_back(wxStringFromUTF8(Date::WeekdayToString(i)));
    return t;
}

void List::addStdWeekday(bool withModulo)
{
    pItems.push_back(WeekDayOp<Operator::EqualsTo>(pParentFilter));
    pItems.push_back(WeekDayOp<Operator::NotEqualsTo>(pParentFilter));
    pItems.push_back(WeekDayOp<Operator::LessThan>(pParentFilter));
    pItems.push_back(WeekDayOp<Operator::LessThanOrEqualsTo>(pParentFilter));
    pItems.push_back(WeekDayOp<Operator::GreaterThan>(pParentFilter));
    pItems.push_back(WeekDayOp<Operator::GreaterThanOrEqualsTo>(pParentFilter));
    if (withModulo)
        pItems.push_back(WeekDayOp<Operator::Modulo>(pParentFilter));
}

template<class T>
static inline T* MonthOp(AFilterBase* parent)
{
    auto* t = new T(parent);
    Parameter& p = t->parameters[0];
    p.dataType = DataType::dtList;
    for (int i = 0; i < 12; ++i)
        p.defaultValues.push_back(wxStringFromUTF8(Date::MonthToString(i)));
    return t;
}

void List::addStdMonth(bool withModulo)
{
    pItems.push_back(MonthOp<Operator::EqualsTo>(pParentFilter));
    pItems.push_back(MonthOp<Operator::NotEqualsTo>(pParentFilter));
    pItems.push_back(MonthOp<Operator::LessThan>(pParentFilter));
    pItems.push_back(MonthOp<Operator::LessThanOrEqualsTo>(pParentFilter));
    pItems.push_back(MonthOp<Operator::GreaterThan>(pParentFilter));
    pItems.push_back(MonthOp<Operator::GreaterThanOrEqualsTo>(pParentFilter));
    if (withModulo)
        pItems.push_back(MonthOp<Operator::Modulo>(pParentFilter));
}

bool List::add(const wxString& name)
{
    switch (name.size())
    {
    case 1:
    {
        if (wxT("<") == name)
        {
            pItems.push_back(new Operator::LessThan(pParentFilter));
            return true;
        }
        if (wxT(">") == name)
        {
            pItems.push_back(new Operator::GreaterThan(pParentFilter));
            return true;
        }
        if (wxT("=") == name)
        {
            pItems.push_back(new Operator::EqualsTo(pParentFilter));
            return true;
        }
        if (wxT("%") == name)
        {
            pItems.push_back(new Operator::Modulo(pParentFilter));
            return true;
        }
        break;
    }
    case 2:
    {
        if (wxT("<=") == name)
        {
            pItems.push_back(new Operator::LessThanOrEqualsTo(pParentFilter));
            return true;
        }
        if (wxT(">=") == name)
        {
            pItems.push_back(new Operator::GreaterThanOrEqualsTo(pParentFilter));
            return true;
        }
        if (wxT("!=") == name)
        {
            pItems.push_back(new Operator::NotEqualsTo(pParentFilter));
            return true;
        }
        break;
    }
    default:
        return false;
    }
    return false;
}

void List::clear()
{
    if (!pItems.empty())
    {
        // Delete all operators
        auto end = pItems.end();
        for (auto i = pItems.begin(); i != end; ++i)
            delete *i;
        // Clear the back-end container
        OperatorList empty; // swap idiom
        pItems.swap(empty);
    }
}

} // namespace Operator
} // namespace Filter
} // namespace Toolbox
} // namespace Antares
