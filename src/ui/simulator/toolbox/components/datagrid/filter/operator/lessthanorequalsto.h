// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_FILTER_OPERATOR_LESS_THAN_OR_EQUALS_TO_H__
#define __ANTARES_TOOLBOX_FILTER_OPERATOR_LESS_THAN_OR_EQUALS_TO_H__

#include "../operator.h"

namespace Antares::Toolbox::Filter::Operator
{
class LessThanOrEqualsTo: public AOperator
{
public:
    LessThanOrEqualsTo(AFilterBase* parent):
        AOperator(parent, wxT("<="), wxT("<="))
    {
    }

    virtual ~LessThanOrEqualsTo()
    {
    }

    virtual bool compute(const int a) const
    {
        return a <= parameters[0].value.asInt;
    }

    virtual bool compute(const double a) const
    {
        return a <= parameters[0].value.asDouble;
    }

    virtual bool compute(const wxString& a) const
    {
        return a <= parameters[0].value.asString;
    }

}; // class LessThanOrEqualsTo

} // namespace Antares::Toolbox::Filter::Operator

#endif // __ANTARES_TOOLBOX_FILTER_OPERATOR_LESS_THAN_OR_EQUALS_TO_H__
