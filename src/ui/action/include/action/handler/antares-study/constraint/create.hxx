// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_CREATE_HXX__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_CREATE_HXX__

namespace Antares::Action::AntaresStudy::Constraint
{
template<class StringT>
inline Create::Create(const StringT& areaname,
                      Data::BindingConstraint::Type t,
                      Data::BindingConstraint::Operator o):
    pOriginalConstraintName(areaname),
    pType(t),
    pOperator(o)
{
    pInfos.behavior = bhOverwrite;
}

template<class StringT1, class StringT2>
inline Create::Create(const StringT1& areaname,
                      const StringT2& targetname,
                      Data::BindingConstraint::Type t,
                      Data::BindingConstraint::Operator o):
    pOriginalConstraintName(areaname),
    pTargetConstraintName(targetname),
    pType(t),
    pOperator(o)
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

} // namespace Antares::Action::AntaresStudy::Constraint

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_CREATE_HXX__
