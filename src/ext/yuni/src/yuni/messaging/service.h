/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#pragma once
#include "../yuni.h"
#include "../net/net.h"
#include "../thread/policy.h"
#include "../core/event.h"
#include "fwd.h"
#include "transport.h"
#include "../core/noncopyable.h"
#include "protocol.h"
#include "../job/queue/service.h"
#include "../core/atomic/bool.h"



namespace Yuni
{
namespace Messaging
{

	class YUNI_DECL Service final
		: private Yuni::NonCopyable<Service>, public Policy::ObjectLevelLockable<Service>
	{
	public:
		//! The threading policy
		typedef Policy::ObjectLevelLockable<Service>  ThreadingPolicy;

		/*!
		** \brief Different states of a queue service
		*/
		enum State
		{
			//! The queue service is currently stopped
			stStopped,
			//! The queue service is currently starting
			stStarting,
			//! The queue service is working
			stRunning,
			//! The queue service is current stopping its work
			stStopping
		};


	public:
		//! \name Constructor & Destructor
		//@{
		//! Default constructor
		Service();
		//! Destructor
		~Service();
		//@}


		//! \name Service management
		//@{
		/*!
		** \brief Try to start the queue service
		*/
		Net::Error start();

		/*!
		** \brief Wait for the service to stop
		*/
		void wait();

		/*!
		** \brief Stop the server
		*/
		Net::Error stop();

		//! Reload internal settings
		Net::Error reload();

		/*!
		** \brief Stop the service gracefully (as soon as possible)
		*/
		void gracefulStop();

		//! Get if the queueservice is running (state running / starting)
		bool running() const;
		//@}


		//! \name Protocol
		//@{
		/*!
		** \brief Install a new protocol
		**
		** \note This method will take ownership of the pointer, thus this pointer
		**   should not be used any longer
		**
		** \param newapi The new protocol (can be null)
		*/
		void protocol(Protocol* newapi);
		//@}


	public:
		//! Listeners management
		class Transports final : private NonCopyable<Transports>
		{
		public:
			//! Default constructor
			Transports();

			/*!
			** \brief Add a new transport for a list of addresses and a specific port
			**
			** This action will be effective the next time the server starts
			** \code
			** Messaging::Service  server;
			** server.addListener("82.125.10.31", 4242, new Transport::REST());
			** server.start();
			** server.wait();
			** server.stop();
			** \endcode
			*/
			Net::Error add(const AnyString& address, const Net::Port& port, Transport::ITransport::Ptr transport);

			/*!
			** \brief Add a new transport for *:port
			*/
			Net::Error add(const Net::Port& port, Transport::ITransport::Ptr transport);

			/*!
			** \brief Clear all addresses where the server should listen for incoming connections
			**
			** This action will be effective the next time the server starts
			*/
			void clear();

		private:
			Service* pService;
			friend class Service;
		}
		transports;


		//! Heavy task manager (disabled by default)
		class HeavyTasks final
		{
		public:
			//! Default constructor
			HeavyTasks();

			//! Get if the task manager is enabled
			bool enabled() const;
			//! Enable / disable the task manager (valid for the next start / reload)
			void enabled(bool on);

			HeavyTasks& operator += (Job::IJob* job);
			HeavyTasks& operator += (Job::IJob::Ptr& job);

		public:
			//! Queue service for heavy tasks
			Job::QueueService queue;

		private:
			Atomic::Bool pEnabled; // 0 default
			Service* pService;
			friend class Service;
		}
		heavyTasks;


		//! Events
		class Events final : private NonCopyable<Events>
		{
		public:
			//! Prototype event: The queue service is starting
			typedef Event<void (Net::Error&)> Starting;
			//! Prototype event: The queue service has started
			typedef Event<void ()> Started;
			//! Prototype event: The queue service is stopping
			typedef Event<void ()> Stopping;
			//! Prototype event: The queue service has been stopped
			typedef Event<void ()> Stopped;
			//! Prototype event: An error has occured
			typedef Event<void (State, Net::Error)> ErrorTriggered;
			//! Prototype event: accepting a client
			typedef Event<void (bool&, const String&, Net::Port, const String&, Net::Port)> ClientAccept;

		public:
			//! Event: The queue service is starting
			Starting starting;
			//! Event: The queue service has started and is ready for incoming connections
			Started  started;
			//! Event: The queue service is shutting down
			Stopping stopping;
			//! Event: The queue service is stopped
			Stopped  stopped;
			//! Event: The queue service has encountered an error
			ErrorTriggered  error;
			//! Event: A client try to connect to the server
			ClientAccept   accept;
			//! Event: A client has been successfully connected to the server
		}
		events;


	protected:
		void reloadTaskQueue();

	protected:
		//! Flag to know the state of the server
		State pState;
		//! Internal data
		Yuni::Private::Messaging::ServiceData* pData;

		// Friends
		friend class Yuni::Private::Messaging::Worker;
		friend class Listeners;

	}; // class Service






} // namespace Messaging
} // namespace Yuni

#include "service.hxx"
