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
** SPDX-License-Identifier: MPL-2.0
*/
#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_COLUMN_INDEX_H__
#define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_COLUMN_INDEX_H__

#include "../filter.h"
#include <antares/date/date.h>

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
class ColumnIndex : public AFilterBase
{
public:
    static const wxChar* Name()
    {
        return wxT("columnindex");
    }
    static const wxChar* Caption()
    {
        return wxT("Column");
    }
    static Date::Precision Precision()
    {
        return Date::stepAny;
    }

public:
    ColumnIndex(Input* parent) : AFilterBase(parent)
    {
        operators.addStdArithmetic();
    }

    virtual ~ColumnIndex()
    {
    }

    virtual Date::Precision precision() const
    {
        return ColumnIndex::Precision();
    }

    virtual bool checkOnColsLabels() const
    {
        return true;
    }

    virtual const wxChar* name() const
    {
        return ColumnIndex::Name();
    }
    virtual const wxChar* caption() const
    {
        return ColumnIndex::Caption();
    }

    virtual bool colIsValid(int col) const
    {
        return currentOperator->compute(col + 1);
    }

}; // class ColumnIndex

} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_COLUMN_INDEX_H__
