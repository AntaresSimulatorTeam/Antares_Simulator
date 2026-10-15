// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>
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

inline const std::string& BindingConstraint::comments() const
{
    return pComments;
}

inline void BindingConstraint::comments(const std::string& newcomments)
{
    pComments = newcomments;
}

inline unsigned int BindingConstraint::linkCount() const
{
    return (unsigned int)pLinkWeights.size();
}

inline unsigned int BindingConstraint::clusterCount() const
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
    const auto idName = id();
    switch (operatorType())
    {
    case BindingConstraint::opLess:
        return (std::filesystem::path(env.folder) / (idName + "_lt.txt")).string();
    case BindingConstraint::opGreater:
        return (std::filesystem::path(env.folder) / (idName + "_gt.txt")).string();
    case BindingConstraint::opEquality:
        return (std::filesystem::path(env.folder) / (idName + "_eq.txt")).string();
    default:
        logs.error("Cannot load/save time series of type other that eq/gt/lt");
        return "";
    }
}

} // namespace Antares::Data
