// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_TEXT_HXX__
#define __ANTARES_LIBS_STUDY_ACTION_TEXT_HXX__

namespace Antares::Action
{
template<bool AutoExpandT>
template<class StringT>
inline Text<AutoExpandT>::Text(const StringT& text)
{
    pInfos.caption = text;
}

template<bool AutoExpandT>
inline Text<AutoExpandT>::~Text()
{
}

template<bool AutoExpandT>
inline bool Text<AutoExpandT>::prepareWL(Context&)
{
    pInfos.state = stNothingToDo;
    return true;
}

template<bool AutoExpandT>
inline bool Text<AutoExpandT>::performWL(Context&)
{
    return true;
}

template<bool AutoExpandT>
inline bool Text<AutoExpandT>::bold() const
{
    return true;
}

template<bool AutoExpandT>
inline bool Text<AutoExpandT>::autoExpand() const
{
    return AutoExpandT;
}

template<bool AutoExpandT>
inline bool Text<AutoExpandT>::canDoSomething() const
{
    return false;
}

} // namespace Antares::Action

#endif // __ANTARES_LIBS_STUDY_ACTION_TEXT_HXX__
