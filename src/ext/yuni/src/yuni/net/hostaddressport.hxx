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
#ifndef __YUNI_NET_HOST_ADDRESS_PORT_HXX__
# define __YUNI_NET_HOST_ADDRESS_PORT_HXX__


namespace Yuni
{
namespace Net
{


	inline void HostAddressPort::reset()
	{
		address.clear();
		port = nullptr;
	}


	inline bool HostAddressPort::operator < (const HostAddressPort& rhs) const
	{
		return (port < rhs.port) and (address < rhs.address);
	}




} // namespace Net
} // namespace Yuni

#endif // __YUNI_NET_HOST_ADDRESS_PORT_H__
