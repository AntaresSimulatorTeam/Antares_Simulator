#ifndef __YUNI_MESSAGING_WORKER_INC_HPP__
# define __YUNI_MESSAGING_WORKER_INC_HPP__

# include "../yuni.h"
# include "../net/net.h"
# include "fwd.h"
# include "transport.h"
# include "../thread/thread.h"
# include "../thread/array.h"


namespace Yuni
{
namespace Private
{
namespace Messaging
{

	/*!
	** \brief Worker for Net queue service
	*/
	class Worker final : public Yuni::Thread::IThread
	{
	public:
		//! The most suitable smart pointer
		typedef Yuni::Thread::IThread::Ptr::Promote<Worker>::Ptr  Ptr;
		//! Alias to the queue service
		typedef Yuni::Messaging::Service Service;
		//! Transport layer (abstract)
		typedef Yuni::Messaging::Transport::ITransport ITransport;

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		explicit Worker(Service& service, ITransport::Ptr transport);
		//! Destructor
		virtual ~Worker();
		//@}

		//! Tell to the transport to stop as soon as possible
		void stopAsSoonAsPossible();


	protected:
		//! Thread execution
		virtual bool onExecute() override;
		//! Thread terminate
		virtual void onStop() override;
		//! Thread terminate
		virtual void onKill() override;

	protected:
		//! The transport layer
		ITransport::Ptr pTransport;
		//! Pointer to the queue service
		Service& pService;

	}; // class Worker






	Worker::Worker(Service& service, ITransport::Ptr transport) :
		pTransport(transport),
		pService(service)
	{
	}


	Worker::~Worker()
	{
		// for code robustness
		stop();
	}


	bool Worker::onExecute()
	{
		// note : this method should not keep a lock on the smart pointer.
		// A memory leak would happen if the thread is killed in action.
		if (!(!pTransport)) // valid pointer to transport
		{
			// pointer aliasing
			ITransport& transport = *pTransport;

			// Attach the current thread to the transport layer
			transport.attachedThread(this);

			// run the transport layer, and wait for it
			Yuni::Net::Error error = transport.run();

			if (error != Yuni::Net::errNone)
				pService.events.error(Service::stRunning, error);
		}
		return false;
	}


	void Worker::stopAsSoonAsPossible()
	{
		// getting apointer to the transport
		ITransport::Ptr transport = pTransport;
		if (!(!transport))
			transport->stop();
	}


	void Worker::onStop()
	{
		// getting apointer to the transport
		ITransport::Ptr transport = pTransport;
		if (!(!transport))
		{
			// Detach the thread, just in case
			transport->attachedThread(nullptr);
			// directly unassign / destroy the transport here (from the thread
			// to avoid long unexplained delay from the destructor
			pTransport = nullptr;
		}
	}


	void Worker::onKill()
	{
		onStop();
	}




} // namespace Messaging
} // namespace Private
} // namespace Yuni

#endif // __YUNI_MESSAGING_WORKER_INC_HPP__
