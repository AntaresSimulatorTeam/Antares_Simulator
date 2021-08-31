/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_STUDY_CONSTRAINT_CONSTRAINT_HXX__
#define __ANTARES_LIBS_STUDY_CONSTRAINT_CONSTRAINT_HXX__

#include "../../utils.h"

namespace Antares
{
namespace Data
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

inline const Matrix<>& BindingConstraint::matrix() const
{
    return pValues;
}

inline Matrix<>& BindingConstraint::matrix()
{
    return pValues;
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

inline void BindingConstraint::mutateTypeWithoutCheck(Type t)
{
    if (t != typeUnknown and t != typeMax)
        pType = t;
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

inline bool BindingConstraint::skipped() const
{
    return !(linkCount() > 0 || enabledClusterCount() > 0);
}

inline uint BindConstList::size() const
{
    return (uint)pList.size();
}

inline bool BindConstList::empty() const
{
    return pList.empty();
}

template<class PredicateT>
inline void BindConstList::each(const PredicateT& predicate)
{
    uint count = (uint)pList.size();
    for (uint i = 0; i != count; ++i)
        predicate(*(pList[i]));
}

template<class PredicateT>
inline void BindConstList::each(const PredicateT& predicate) const
{
    uint count = (uint)pList.size();
    for (uint i = 0; i != count; ++i)
        predicate(*(pList[i]));
}

template<class PredicateT>
inline void BindConstList::eachEnabled(const PredicateT& predicate)
{
    uint count = (uint)pList.size();
    for (uint i = 0; i != count; ++i)
    {
        auto& constraint = *(pList[i]);
        if (constraint.enabled() && !constraint.skipped())
            predicate(constraint);
    }
}

template<class PredicateT>
inline void BindConstList::eachEnabled(const PredicateT& predicate) const
{
    uint count = (uint)pList.size();
    for (uint i = 0; i != count; ++i)
    {
        auto& constraint = *(pList[i]);
        if (constraint.enabled() && !constraint.skipped())
            predicate(constraint);
    }
}

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_CONSTRAINT_CONSTRAINT_HXX__
