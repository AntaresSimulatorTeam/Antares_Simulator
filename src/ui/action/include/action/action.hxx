/*
** Copyright 2007-2023 RTE
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
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __ANTARES_LIBS_STUDY_ACTION_ACTION_HXX__
#define __ANTARES_LIBS_STUDY_ACTION_ACTION_HXX__

namespace Antares
{
namespace Action
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

} // namespace Action
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_ACTION_ACTION_HXX__
