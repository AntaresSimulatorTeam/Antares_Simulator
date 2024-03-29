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
#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_CREATE_HXX__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_CREATE_HXX__

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Constraint
{
template<class StringT>
inline Create::Create(const StringT& areaname,
                      Data::BindingConstraint::Type t,
                      Data::BindingConstraint::Operator o) :
 pOriginalConstraintName(areaname), pType(t), pOperator(o)
{
    pInfos.behavior = bhOverwrite;
}

template<class StringT1, class StringT2>
inline Create::Create(const StringT1& areaname,
                      const StringT2& targetname,
                      Data::BindingConstraint::Type t,
                      Data::BindingConstraint::Operator o) :
 pOriginalConstraintName(areaname), pTargetConstraintName(targetname), pType(t), pOperator(o)
{
    pInfos.behavior = bhOverwrite;
}

inline Create::~Create()
{
}

inline bool Create::allowUpdate() const
{
    return true;
}

} // namespace Constraint
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_CREATE_HXX__
