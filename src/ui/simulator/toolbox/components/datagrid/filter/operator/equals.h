// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_FILTER_OPERATOR_NOT_EQUALS_H__
#define __ANTARES_TOOLBOX_FILTER_OPERATOR_NOT_EQUALS_H__

#include "../operator.h"

namespace Antares::Toolbox::Filter::Operator
{
class EqualsTo: public AOperator
{
public:
    EqualsTo(AFilterBase* parent):
        AOperator(parent, wxT("="), wxT("="))
    {
    }

    virtual ~EqualsTo()
    {
    }

    virtual bool compute(const int a) const
    {
        return a == parameters[0].value.asInt;
    }

    virtual bool compute(const double a) const
    {
        return fabs(a - parameters[0].value.asDouble) < 0.00001;
    }

    virtual bool compute(const wxString& a) const
    {
        return a == parameters[0].value.asString;
    }

}; // class EqualsTo

} // namespace Antares::Toolbox::Filter::Operator

#endif // __ANTARES_TOOLBOX_FILTER_OPERATOR_NOT_EQUALS_H__
