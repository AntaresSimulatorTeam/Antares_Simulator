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
#ifndef __YUNI_NET_HOST_ADDRESS_PORT_H__
# define __YUNI_NET_HOST_ADDRESS_PORT_H__

# include "../yuni.h"
# include "port.h"
# include "../core/string.h"
# include <vector>


namespace Yuni
{
namespace Net
{

	//! Raw Address for a host (IP/DNS)
	typedef ShortString256  HostAddress;



	class HostAddressPort
	{
	public:
		//! Vector
		typedef std::vector<HostAddressPort>  Vector;


	public:
		//! Reset the host address and port
		void reset();

	public:
		//! Raw host address (IP/DNS)
		HostAddress  address;
		//! Socket port number
		Port port;

		//! operator <
		bool operator < (const HostAddressPort& rhs) const;

	}; // class HostAddressPort





} // namespace Net
} // namespace Yuni

# include "hostaddressport.hxx"

#endif // __YUNI_NET_HOST_ADDRESS_PORT_H__
