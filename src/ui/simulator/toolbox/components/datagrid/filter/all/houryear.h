/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __ANTARES_TOOLBOX_FILTER_ALL_HOUR_YEAR_H__
#define __ANTARES_TOOLBOX_FILTER_ALL_HOUR_YEAR_H__

#include "../filter.h"

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
class HourYear : public AFilterBase
{
public:
    static const wxChar* Name()
    {
        return wxT("houryear");
    }
    static const wxChar* Caption()
    {
        return wxT("Hour Year");
    }
    static Date::Precision Precision()
    {
        return Date::hourly;
    }

public:
    HourYear(Input* parent) : AFilterBase(parent)
    {
        operators.addStdArithmetic();
    }

    virtual ~HourYear()
    {
    }

    virtual Date::Precision precision() const
    {
        return HourYear::Precision();
    }

    virtual bool checkOnRowsLabels() const
    {
        return true;
    }

    virtual const wxChar* name() const
    {
        return HourYear::Name();
    }
    virtual const wxChar* caption() const
    {
        return HourYear::Caption();
    }

    virtual bool rowIsValid(int row) const
    {
        return currentOperator->compute(row + 1);
    }

}; // class HourYear

} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_FILTER_FILTER_H__
