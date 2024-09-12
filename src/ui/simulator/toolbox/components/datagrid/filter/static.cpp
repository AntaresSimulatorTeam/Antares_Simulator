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
    using Type = const wxChar*;
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
    using Type = AFilterBase*;
    static AFilterBase* Default()
    {
        return nullptr;
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
