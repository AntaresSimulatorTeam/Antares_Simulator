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
#pragma once

namespace Antares::Data
{
inline const ConstraintName& BindingConstraint::name() const
{
    return pName;
}

inline const ConstraintName& BindingConstraint::id() const
{
    return pID;
}

inline const YString& BindingConstraint::comments() const
{
    return pComments;
}

inline void BindingConstraint::comments(const AnyString& newcomments)
{
    pComments = newcomments;
}

inline uint BindingConstraint::linkCount() const
{
    return (uint)pLinkWeights.size();
}

inline uint BindingConstraint::clusterCount() const
{
    return (uint)pClusterWeights.size();
}

inline bool BindingConstraint::enabled() const
{
    return pEnabled;
}

inline BindingConstraint::Operator BindingConstraint::operatorType() const
{
    return pOperator;
}

inline BindingConstraint::Type BindingConstraint::type() const
{
    return pType;
}

inline void BindingConstraint::setTimeGranularity(Type t)
{
    if (t != typeUnknown and t != typeMax)
    {
        pType = t;
    }
}

inline bool BindingConstraint::skipped() const
{
    return linkCount() == 0 && clusterCount() == 0;
}

inline bool BindingConstraint::isActive() const
{
    return enabled() && !skipped();
}

inline BindingConstraint::iterator BindingConstraint::begin()
{
    return pLinkWeights.begin();
}

inline BindingConstraint::iterator BindingConstraint::end()
{
    return pLinkWeights.end();
}

inline BindingConstraint::const_iterator BindingConstraint::begin() const
{
    return pLinkWeights.begin();
}

inline BindingConstraint::const_iterator BindingConstraint::end() const
{
    return pLinkWeights.end();
}

template<class Env>
inline std::string BindingConstraint::timeSeriesFileName(const Env& env) const
{
    switch (operatorType())
    {
    case BindingConstraint::opLess:
        return std::string() + env.folder.c_str() + Yuni::IO::Separator + id().c_str() + "_lt"
               + ".txt";
    case BindingConstraint::opGreater:
        return std::string() + env.folder.c_str() + Yuni::IO::Separator + id().c_str() + "_gt"
               + ".txt";
    case BindingConstraint::opEquality:
        return std::string() + env.folder.c_str() + Yuni::IO::Separator + id().c_str() + "_eq"
               + ".txt";
    default:
        logs.error("Cannot load/save time series of type other that eq/gt/lt");
        return "";
    }
}

} // namespace Antares::Data
