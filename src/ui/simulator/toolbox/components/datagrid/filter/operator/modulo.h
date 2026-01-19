// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_FILTER_OPERATOR_MODULO_H__
#define __ANTARES_TOOLBOX_FILTER_OPERATOR_MODULO_H__

#include "../operator.h"

namespace Antares::Toolbox::Filter::Operator
{
class Modulo: public AOperator
{
public:
    Modulo(AFilterBase* parent):
        AOperator(parent, wxT("%"), wxT("modulo"))
    {
        // When the operator is the modulo, we must have another
        // value
        parameters.push_back(Parameter(*this).presetModuloAddon());
    }

    virtual ~Modulo()
    {
    }

    virtual bool compute(const int a) const
    {
        return parameters[0].value.asInt
                 ? (a % parameters[0].value.asInt == parameters[1].value.asInt)
                 : false;
    }

    virtual bool compute(const double a) const
    {
        return (int(floor(a)) % parameters[0].value.asInt == parameters[1].value.asInt);
    }

    virtual bool compute(const wxString&) const
    {
        return false;
    }

}; // class Modulo

} // namespace Antares::Toolbox::Filter::Operator

#endif // __ANTARES_TOOLBOX_FILTER_OPERATOR_MODULO_H__
