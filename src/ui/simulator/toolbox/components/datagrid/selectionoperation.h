/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_SELECTION_OPERATION_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_SELECTION_OPERATION_H__

#include "wx-wrapper.h"
#include <math.h>
#include <limits>

namespace Antares::Component::Datagrid::Selection
{
class IOperator
{
public:
    IOperator()
    {
    }

    virtual ~IOperator()
    {
    }

    /*!
    ** \brief Caption of the operator
    */
    virtual const wxChar* caption() const = 0;

    /*!
    ** \brief Reset all internal values
    */
    virtual void reset() = 0;

    /*!
    ** \brief Manage a new value
    */
    virtual void appendValue(const double v) = 0;

    /*!
    ** \brief Get the result
    */
    virtual double result() const = 0;

}; // class IOperator

class Average final: public IOperator
{
public:
    Average():
        pValue(0.),
        pCount(0)
    {
    }

    virtual ~Average()
    {
    }

    virtual const wxChar* caption() const
    {
        return wxT("Average");
    }

    virtual void reset()
    {
        pValue = 0.;
        pCount = 0;
    }

    virtual void appendValue(const double v)
    {
        pValue += v;
        ++pCount;
    }

    virtual double result() const
    {
        return pValue / (double)pCount;
    }

private:
    double pValue;
    uint pCount;

}; // class Average

class Sum final: public IOperator
{
public:
    Sum():
        pValue(0.)
    {
    }

    virtual const wxChar* caption() const
    {
        return wxT("Sum");
    }

    virtual void reset()
    {
        pValue = 0.;
    }

    virtual void appendValue(const double v)
    {
        pValue += v;
    }

    virtual double result() const
    {
        return pValue;
    }

private:
    double pValue;

}; // class Sum

class CellCount final: public IOperator
{
public:
    CellCount():
        pCount(0)
    {
    }

    virtual const wxChar* caption() const
    {
        return wxT("Cell Count");
    }

    virtual void reset()
    {
        pCount = 0;
    }

    virtual void appendValue(const double)
    {
        ++pCount;
    }

    virtual double result() const
    {
        return (double)pCount;
    }

private:
    uint pCount;

}; // class Average

class Minimum final: public IOperator
{
public:
    Minimum():
        pValue(std::numeric_limits<double>::infinity())
    {
    }

    virtual const wxChar* caption() const
    {
        return wxT("Minimum");
    }

    virtual void reset()
    {
        pValue = std::numeric_limits<double>::infinity();
    }

    virtual void appendValue(const double v)
    {
        if (v < pValue)
        {
            pValue = v;
        }
    }

    virtual double result() const
    {
        return pValue;
    }

private:
    double pValue;

}; // class Sum

class Maximum final: public IOperator
{
public:
    Maximum():
        pValue(-std::numeric_limits<double>::infinity())
    {
    }

    virtual const wxChar* caption() const
    {
        return wxT("Maximum");
    }

    virtual void reset()
    {
        pValue = -std::numeric_limits<double>::infinity();
    }

    virtual void appendValue(const double v)
    {
        if (v > pValue)
        {
            pValue = v;
        }
    }

    virtual double result() const
    {
        return pValue;
    }

private:
    double pValue;

}; // class Sum

} // namespace Antares::Component::Datagrid::Selection

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_SELECTION_OPERATION_H__
