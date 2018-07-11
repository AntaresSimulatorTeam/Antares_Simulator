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
#ifndef __YUNI_MESSAGING_TRANSPORT_TRANSPORT_H__
# define __YUNI_MESSAGING_TRANSPORT_TRANSPORT_H__

# include "../../yuni.h"
# include "../../core/string.h"
# include "../../core/noncopyable.h"
# include "../../thread/thread.h"
# include "../fwd.h"
# include "../../net/errors.h"
# include "../../net/hostaddressport.h"
# include "../../net/port.h"
# include "../../core/dictionary.h"
# include "../protocol.h"


namespace Yuni
{
namespace Messaging
{
namespace Transport
{

	enum Mode
	{
		tmNone = 0,
		//! Server mode
		tmServer,
		//! Client mode
		tmClient
	};


	/*!
	** \brief Transport layer for messages (abstract)
	**
	** A transport layer is not thread-safe.
	*/
	class ITransport : private NonCopyable<ITransport>
	{
	public:
		//! The most suitable smart pointer for the class
		typedef SmartPtr<ITransport>  Ptr;

		//! Hash
		typedef Dictionary<Net::HostAddressPort, Ptr>::Hash Hash;
		//! Map
		typedef Dictionary<Net::HostAddressPort, Ptr>::Ordered Map; // TODO: remove this typdef

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		explicit ITransport(Mode m);
		//! Destructor
		virtual ~ITransport();
		//@}

		//! \name Attached thread
		//@{
		//! Get the attached thread
		Thread::IThread*  attachedThread();
		//! Get the attached thread (const)
		const Thread::IThread*  attachedThread() const;
		//! Set the attached thread
		void attachedThread(Thread::IThread* thread);
		//@}


		//! Service
		//@{
		//! Start the service
		virtual Yuni::Net::Error  start() = 0;
		//! Run the transport layer (must block until finished)
		virtual Yuni::Net::Error  run() = 0;
		//! Ask to stop the transport layer (if not already done)
		virtual void stop() = 0;
		//@}

		//! \name Protocol
		//@{
		/*!
		** \brief Install a new protocol
		*/
		virtual void protocol(const Protocol& protocol) = 0;
		//@}


	public:
		//! Address to listen
		Net::HostAddress  address;
		//! Port
		Net::Port port;
		//! Mode (server/client)
		const Mode mode;

	protected:
		//! The attached thread, if any
		Thread::IThread* pAttachedThread;
		//! Service (initialized by the service owner)
		Yuni::Messaging::Service* pService;
		// Our friends
		friend class Yuni::Messaging::Service;

	}; // class Transport






} // namespace Transport
} // namespace Server
} // namespace Yuni

# include "transport.hxx"

#endif // __YUNI_MESSAGING_TRANSPORT_TRANSPORT_H__
