// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_ACTION_HXX__
#define __ANTARES_LIBS_STUDY_ACTION_ACTION_HXX__

namespace Antares::Action
{
inline bool IAction::bold() const
{
    return false;
}

inline bool IAction::autoExpand() const
{
    return false;
}

inline Behavior IAction::behavior() const
{
    ThreadingPolicy::MutexLocker locker(*this);
    return pInfos.behavior;
}

inline State IAction::state() const
{
    ThreadingPolicy::MutexLocker locker(*this);
    return pInfos.state;
}

inline bool IAction::canDoSomething() const
{
    return true;
}

inline bool IAction::allowUpdate() const
{
    return false;
}

inline bool IAction::allowSkip() const
{
    return true;
}

inline bool IAction::allowOverwrite() const
{
    return true;
}

inline bool IAction::shouldPrepareRootNode() const
{
    return false;
}

inline void IAction::datagridCaption(Yuni::String&)
{
}

inline bool IAction::visible() const
{
    return true;
}

inline void IAction::createPostActionsWL(const IAction::Ptr&)
{
    // do nothing
}

} // namespace Antares::Action

#endif // __ANTARES_LIBS_STUDY_ACTION_ACTION_HXX__
