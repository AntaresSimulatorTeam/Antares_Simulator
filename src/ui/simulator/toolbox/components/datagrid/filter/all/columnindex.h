// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_COLUMN_INDEX_H__
#define __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_COLUMN_INDEX_H__

#include "../filter.h"
#include <antares/date/date.h>

namespace Antares::Toolbox::Filter
{
class ColumnIndex: public AFilterBase
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
    ColumnIndex(Input* parent):
        AFilterBase(parent)
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

} // namespace Antares::Toolbox::Filter

#endif // __ANTARES_TOOLBOX_COMPONENTS_DATAGRID_FILTER_ALL_COLUMN_INDEX_H__
