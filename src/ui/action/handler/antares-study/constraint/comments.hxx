// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_COMMENTS_HXX__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_COMMENTS_HXX__

namespace Antares::Action::AntaresStudy::Constraint
{
template<class StringT>
inline Comments::Comments(const StringT& name):
    pOriginalConstraintName(name)
{
    pInfos.caption << "Comments";
}

inline Comments::~Comments()
{
}

} // namespace Antares::Action::AntaresStudy::Constraint

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_COMMENTS_HXX__
