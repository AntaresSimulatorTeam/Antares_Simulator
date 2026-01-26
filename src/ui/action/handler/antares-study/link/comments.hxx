// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_LINK_COMMENTS_HXX__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_LINK_COMMENTS_HXX__

namespace Antares::Action::AntaresStudy::Link
{
template<class StringT1, class StringT2>
inline Comments::Comments(const StringT1& fromarea, const StringT2& toarea):
    pOriginalFromAreaName(fromarea),
    pOriginalToAreaName(toarea)
{
    pInfos.caption << "Comments";
}

inline Comments::~Comments()
{
}

} // namespace Antares::Action::AntaresStudy::Link

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_LINK_COMMENTS_HXX__
