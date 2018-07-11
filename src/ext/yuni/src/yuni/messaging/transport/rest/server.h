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
#ifndef __YUNI_MESSAGING_TRANSPORT_REST_SERVER_H__
# define __YUNI_MESSAGING_TRANSPORT_REST_SERVER_H__

# include "../transport.h"


namespace Yuni
{
namespace Messaging
{
namespace Transport
{
namespace REST
{

	class Server final : public ITransport
	{
	public:
		//! Opaque structure for Internal data related to the rest server
		class ServerData;

	public:
		//! Default constructor
		Server();
		//! Destructor
		virtual ~Server();

		virtual Net::Error start() override;

		virtual Net::Error run() override;

		virtual void stop() override;

		virtual void protocol(const Protocol& protocol) override;

	private:
		void wait();

	private:
		//! Internal data
		ServerData* pData;

	}; // class Server





} // namespace REST
} // namespace Transport
} // namespace Messaging
} // namespace Yuni

#endif // __YUNI_MESSAGING_TRANSPORT_REST_SERVER_H__
