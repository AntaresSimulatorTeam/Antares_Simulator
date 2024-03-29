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
        using MemberType = void (C::*)();
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
