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

#include "filter.h"
#include "all/any.h"
#include "all/houryear.h"
#include "all/weekday.h"
#include "all/month.h"
#include "all/day.h"
#include "all/dayyear.h"
#include "all/week.h"
#include "all/hour.h"
#include "all/columnindex.h"

using namespace Yuni;

#define TEST_CLASS(X)                                                    \
    if ((uint)precision <= (uint)X ::Precision() and X ::Name() == name) \
        return Result<X>::Value(i);

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
template<class T>
struct ResultCaption
{
    typedef const wxChar* Type;
    static const wxChar* Default()
    {
        return wxT("(null)");
    }
    template<typename U>
    static const wxChar* Value(U)
    {
        return T::Caption();
    }
};

template<class T>
struct ResultNewInstance
{
    typedef AFilterBase* Type;
    static AFilterBase* Default()
    {
        return NULL;
    }
    static T* Value(Input* parent)
    {
        return new T(parent);
    }
};

template<template<class> class Result, typename I>
static typename Result<void>::Type FindFilterFromName(const wxString& name,
                                                      I i,
                                                      Date::Precision precision = Date::stepNone)
{
    if (not name.IsEmpty())
    {
        switch (name[0].GetValue())
        {
        case wxT('a'):
        {
            TEST_CLASS(Any);
            break;
        }
        case wxT('c'):
        {
            TEST_CLASS(ColumnIndex);
            break;
        }
        case wxT('d'):
        {
            TEST_CLASS(Day);
            TEST_CLASS(DayYear);
            break;
        }
        case wxT('h'):
        {
            TEST_CLASS(HourYear);
            TEST_CLASS(Hour);
            break;
        }
        case wxT('m'):
        {
            TEST_CLASS(Month);
            break;
        }
        case wxT('w'):
        {
            TEST_CLASS(Weekday);
            TEST_CLASS(Week);
            break;
        }
        }
    }
    return nullptr;
    // return Result<void>::Default();
}

const wxChar* AFilterBase::CaptionFromName(const wxString& name, Date::Precision precision)
{
    return FindFilterFromName<ResultCaption, int>(name, 0, precision);
}

AFilterBase* AFilterBase::FactoryCreate(Input* parent, const wxString& name)
{
    auto* filter = FindFilterFromName<ResultNewInstance, Input*>(name, parent);
#ifndef NDEBUG
    if (!filter)
    {
        String textdbg;
        wxStringToString(name, textdbg);
        logs.error() << "failed to create filter from name '" << textdbg << "'";
    }
#endif
    return filter;
}

} // namespace Filter
} // namespace Toolbox
} // namespace Antares
