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
#include "service.h"
#include "worker.inc.hpp"
#include "../thread/array.h"


typedef Yuni::Thread::Array<Yuni::Private::Messaging::Worker>  Workers;

typedef Yuni::Messaging::Transport::ITransport::Map  TransportList;



namespace Yuni
{
namespace Private
{
namespace Messaging
{

	class ServiceData final
	{
	public:
		//! All workers
		Workers::Ptr workers;
		//! All addresses to listen
		TransportList transports;
		//! Protocol
		Yuni::Messaging::Protocol::Ptr protocol;

	}; // class ServiceData




} // namespace Messaging
} // namespace Private
} // namespace Yuni




namespace Yuni
{
namespace Messaging
{

	//! ensuring that the internal data is properly initialized
	static inline void InitializeInternalData(Yuni::Private::Messaging::ServiceData*& data)
	{
		if (not data)
			data = new Yuni::Private::Messaging::ServiceData();
	}



	Service::Service() :
		pState(stStopped),
		pData(nullptr)
	{
		transports.pService = this;
		heavyTasks.pService = this;
	}


	Service::~Service()
	{
		// Stopping all workers
		stop();

		// for code robustness
		ThreadingPolicy::MutexLocker locker(*this);
		// release internal data
		delete pData;
	}


	Net::Error  Service::Transports::add(const AnyString& address, const Net::Port& port, Transport::ITransport::Ptr transport)
	{
		if (not port.valid())
			return Net::errInvalidPort;
		if (not transport or transport->mode != Transport::tmServer)
			return Net::errInvalidTransport;

		// The address
		if (not address)
			return Net::errInvalidHostAddress;

		Net::HostAddressPort addrport;
		addrport.address    = address;
		addrport.port       = port;
		transport->address  = address;
		transport->port     = port;
		transport->pService = pService;

		// Adding the new address
		{
			// The item to insert
			std::pair<Net::HostAddressPort, Messaging::Transport::ITransport::Ptr> item(addrport, transport);

			ThreadingPolicy::MutexLocker locker(*pService);
			InitializeInternalData(pService->pData);

			Protocol::Ptr& protocol = pService->pData->protocol;
			if (not protocol)
				pService->pData->protocol = new Protocol();
			transport->protocol(*protocol);

			if (not pService->pData->transports.insert(item).second)
				return Net::errDupplicatedAddress;
		}
		return Net::errNone;
	}


	void Service::Transports::clear()
	{
		ThreadingPolicy::MutexLocker locker(*pService);
		if (pService->pData)
			pService->pData->transports.clear();
	}


	Net::Error Service::start()
	{
		// Checking if the service is not already running
		{
			ThreadingPolicy::MutexLocker locker(*this);
			InitializeInternalData(pData);

			// checking the current state
			if (pState != stStopped)
				return (pState == stRunning) ? Net::errNone : Net::errUnknown;

			// ok, let's start
			pState = stStarting;
		}

		// note: from now on, we know that pData is initialized due to the previous check

		// no error by default
		Net::Error err = Net::errNone;

		// The internal mutex must be unlocked whenever an event is called to prevent
		// any deadlocks.
		events.starting(err);
		if (err != Net::errNone)
		{
			ThreadingPolicy::MutexLocker locker(*this);
			pState = stStopped;
			return err;
		}

		// pointer to all workers
		Workers::Ptr workers;

		// Trying to start all workers
		{
			ThreadingPolicy::MutexLocker locker(*this);

			// releasing memory held by the protocol if not already done
			if (!(!pData->protocol))
				pData->protocol->shrinkMemory();

			// start the task manager before any worker / transport
			reloadTaskQueue();
			// destroy the old instance if not already done
			pData->workers = nullptr;
			// recreate our workers
			workers = new Workers();
			// disable the autostart
			workers->autoStart(false);

			// Ok, attempt to start the server from the real implementation
			// recreating all workers
			TransportList& transports = pData->transports;

			if (not transports.empty())
			{
				TransportList::iterator end = transports.end();
				for (TransportList::iterator i = transports.begin(); i != end; ++i)
				{
					// start the transport
					if (Net::errNone != (err = (i->second)->start()))
						break;

					// creating a new worker
					(*workers) += new Yuni::Private::Messaging::Worker(*this, i->second);
				}

				// The new state
				if (err == Net::errNone)
				{
					pData->workers = workers;
					pState = stRunning;
				}
				else
					pState = stStopped;
			}
			else
			{
				err = Net::errNoTransport;
				pState = stStopped;
			}
		}

		if (err == Net::errNone)
		{
			// start all workers
			workers->start();

			// Great ! The server is working !
			events.started();
			return Net::errNone;
		}

		if (!(!workers))
		{
			workers->stop();
			workers = nullptr; // should be destroyed here
		}

		// An error has occured
		events.error(stStarting, err);
		return err;
	}


	void Service::wait()
	{
		Workers::Ptr workers;

		// retrieving a pointer to the workers
		{
			ThreadingPolicy::MutexLocker locker(*this);
			if (pData)
				workers = pData->workers;
			else
				return;
		}

		if (!(!workers))
			workers->wait();
	}


	Net::Error Service::stop()
	{
		// Checking if the service is not already running
		{
			ThreadingPolicy::MutexLocker locker(*this);
			if (pData == NULL or not pData->workers)
				return Net::errNone;
			if (pState != stRunning)
				return (pState == stStopped) ? Net::errNone : Net::errUnknown;
			pState = stStopping;
		}

		// The internal mutex must be unlocked whenever an event is called to prevent
		// any deadlocks.
		events.stopping();

		Net::Error err = Net::errNone;
		Workers::Ptr workers;

		// Trying to start all workers
		{
			ThreadingPolicy::MutexLocker locker(*this);

			workers = pData->workers;
			pData->workers = nullptr;
			// The current state
			pState = (err == Net::errNone) ? stStopped : stRunning;
		}

		if (err == Net::errNone)
		{
			// stopping then destroying all workers
			if (!(!workers))
			{
				workers->stop();
				workers = nullptr; // should be destroyed here
			}

			// waiting for heavy tasks, after all workers have stopped
			heavyTasks.queue.gracefulStop();
			heavyTasks.queue.wait(qseStop);

			// Great ! The server is working !
			events.stopped();
			return Net::errNone;
		}

		// An error has occured
		events.error(stStopping, err);
		return err;
	}


	namespace // anonymous
	{

		class WorkerStopper final
		{
		public:
			bool operator () (Yuni::Private::Messaging::Worker::Ptr& worker) const
			{
				worker->stopAsSoonAsPossible();
				return true;
			}
		};

	} // anonymous

	void Service::gracefulStop()
	{
		ThreadingPolicy::MutexLocker locker(*this);

		// checking if the service is still alive
		if ((pState == stStarting) or (pState == stRunning))
		{
			if (pData != NULL and !(!pData->workers))
			{
				// stop all transports
				WorkerStopper workerStopper;
				pData->workers->foreachThread(workerStopper);
				// tell to the thread to stop as well
				pData->workers->gracefulStop();
			}
		}
	}


	void Service::reloadTaskQueue()
	{
		heavyTasks.queue.stop();
		if (heavyTasks.enabled())
			heavyTasks.queue.start();
	}


	Net::Error Service::reload()
	{
		ThreadingPolicy::MutexLocker locker(*this);
		if (pState == stStarting or pState == stRunning)
			reloadTaskQueue();
		return Net::errNone;
	}


	bool Service::running() const
	{
		ThreadingPolicy::MutexLocker locker(*this);
		return (pState == stStarting) or (pState == stRunning);
	}


	void Service::protocol(Protocol* newapi)
	{
		// creating a new smart pointer for the new api
		Protocol::Ptr newproto = (not newapi) ? new Protocol() : newapi;

		// Locking the whole object
		ThreadingPolicy::MutexLocker locker(*this);

		// making sure that internal data are allocated
		InitializeInternalData(pData);
		assert(pData != NULL && "internal error");

		// Reference to the new protocol
		Protocol& protoref = *newproto;

		// Making all transports switch to the new protocol
		TransportList::iterator end = pData->transports.end();
		for (TransportList::iterator i = pData->transports.begin(); i != end; ++i)
			(i->second)->protocol(protoref);

		// relesing some memory
		protoref.shrinkMemory();

		// All transports have switched to the new protocol
		// we can release our own pointer. The old protocol should
		// be destroyed here, in the current thread, instead
		// of inside a worker of one of the transports
		pData->protocol = newproto;
	}





} // namespace Messaging
} // namespace Yuni

