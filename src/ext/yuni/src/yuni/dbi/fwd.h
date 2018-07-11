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
#ifndef __YUNI_DBI_FWD_H__
# define __YUNI_DBI_FWD_H__

# include "../core/smartptr/smartptr.h"


namespace Yuni
{
namespace DBI
{

	enum
	{
		//! Invalid transaction handle
		nullHandle = 0
	};



	// Forward declarations
	class ConnectorPool;
	class Transaction;
	class Query;
	class AutoCommit;
	class Row;
	class Column;


} // namespace DBI
} // namespace Yuni



namespace Yuni
{
namespace Private
{
namespace DBI
{

	// Forward declarations
	class Channel;
	class ConnectorData;

	//! Connector data ptr
	typedef Yuni::SmartPtr<ConnectorData>  ConnectorDataPtr;
	typedef Yuni::SmartPtr<Channel>  ChannelPtr;


} // namespace DBI
} // namespace Private
} // namespace Yuni


#endif // __YUNI_DBI_FWD_H__
