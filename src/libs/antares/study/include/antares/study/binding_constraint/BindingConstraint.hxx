// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <ranges>

namespace Antares::Data
{
inline const std::string& BindingConstraint::name() const
{
    return pName;
}

inline const std::string& BindingConstraint::id() const
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
    return std::ranges::count_if(pClusterWeights | std::views::keys,
                                 [](const Data::ThermalCluster* coeff)
                                 { return coeff->isActive(); });
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
