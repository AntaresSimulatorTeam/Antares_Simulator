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
#ifndef __ANTARES_COMMON_ACTION_ACTION_HXX__
#define __ANTARES_COMMON_ACTION_ACTION_HXX__

#include "action.h"

namespace Antares
{
namespace Private
{
namespace Dispatcher
{
class JobSimpleDispatcher final : public Yuni::Job::IJob
{
public:
    JobSimpleDispatcher()
    {
    }

    explicit JobSimpleDispatcher(const Yuni::Bind<void()>& bind) : callback(bind)
    {
    }

    template<class C>
    JobSimpleDispatcher(const C* object, void (C::*method)(void))
    {
        typedef void (C::*MemberType)();
        callback.bind(const_cast<C*>(object), reinterpret_cast<MemberType>(method));
    }

    virtual ~JobSimpleDispatcher()
    {
    }

protected:
    virtual void onExecute()
    {
        callback();
    }

public:
    //! Delayed callback
    Yuni::Bind<void()> callback;
};

} // namespace Dispatcher
} // namespace Private
} // namespace Antares

namespace Antares
{
namespace Dispatcher
{
namespace GUI
{
template<class C>
inline void Post(const C* object, void (C::*method)(void))
{
    ::Antares::Dispatcher::GUI::Post(
      (const Yuni::Job::IJob::Ptr&)new
      typename ::Antares::Private::Dispatcher::JobSimpleDispatcher(object, method));
}

template<class C>
inline void Post(const C* object, void (C::*method)(void), uint delay)
{
    auto j = new ::Antares::Private::Dispatcher::JobSimpleDispatcher(object, method);
    ::Antares::Dispatcher::GUI::Post((const Yuni::Job::IJob::Ptr&)j, delay);
}

inline void Post(const Yuni::Bind<void()>& job, uint delay)
{
    ::Antares::Dispatcher::GUI::Post(
      (const Yuni::Job::IJob::Ptr&)new ::Antares::Private::Dispatcher::JobSimpleDispatcher(job),
      delay);
}

} // namespace GUI

inline void Post(const Yuni::Bind<void()>& job)
{
    ::Antares::Dispatcher::Post(
      (const Yuni::Job::IJob::Ptr&)new ::Antares::Private::Dispatcher::JobSimpleDispatcher(job));
}

template<class C>
inline void Post(const C* object, void (C::*method)())
{
    ::Antares::Dispatcher::Post(
      (const Yuni::Job::IJob::Ptr&)new
      typename ::Antares::Private::Dispatcher::JobSimpleDispatcher(object, method));
}

template<class C, class UserDataT>
inline void Post(const C* object, void (C::*method)(), const UserDataT& userdata)
{
    ::Antares::Dispatcher::Post(
      (const Yuni::Job::IJob::Ptr&)new
      typename ::Antares::Private::Dispatcher::JobSimpleDispatcher(object, method, userdata));
}

} // namespace Dispatcher
} // namespace Antares

namespace Antares
{
namespace Dispatcher
{
namespace Internal
{
/*!
** \brief Execute the queue event
*/
void ExecuteQueueDispatcher();

} // namespace Internal
} // namespace Dispatcher
} // namespace Antares

#endif // __ANTARES_COMMON_ACTION_ACTION_HXX__
