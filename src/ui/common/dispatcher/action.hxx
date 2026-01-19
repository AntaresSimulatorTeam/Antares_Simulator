// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_COMMON_ACTION_ACTION_HXX__
#define __ANTARES_COMMON_ACTION_ACTION_HXX__

#include "action.h"

namespace Antares::Private::Dispatcher
{
class JobSimpleDispatcher final: public Yuni::Job::IJob
{
public:
    JobSimpleDispatcher()
    {
    }

    explicit JobSimpleDispatcher(const Yuni::Bind<void()>& bind):
        callback(bind)
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

} // namespace Antares::Private::Dispatcher

namespace Antares::Dispatcher
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

} // namespace Antares::Dispatcher

namespace Antares::Dispatcher::Internal
{
/*!
** \brief Execute the queue event
*/
void ExecuteQueueDispatcher();

} // namespace Antares::Dispatcher::Internal

#endif // __ANTARES_COMMON_ACTION_ACTION_HXX__
