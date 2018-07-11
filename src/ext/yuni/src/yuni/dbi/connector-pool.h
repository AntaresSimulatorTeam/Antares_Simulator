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
#ifndef __YUNI_DBI_CONNECTOR_H__
# define __YUNI_DBI_CONNECTOR_H__

# include "error.h"
# include "fwd.h"
# include "settings.h"
# include "adapter.h"
# include "../core/event/event.h"



namespace Yuni
{
/*!
** \brief Database Independent Abstraction Layer
** \ingroup DBI
*/
namespace DBI
{


	/*!
	** \brief Database interface
	** \ingroup DBI
	**
	** All methods of this class are thread-safe. This means that manipulating a
	** connector pool from several threads is safe (thus starting new transactions)
	** However, queries and resultset are NOT thread-safe.
	*/
	class ConnectorPool final
	{
	public:
		//! The most suitable smart pointer for the class
		typedef SmartPtr<ConnectorPool>  Ptr;

	public:
		//! \name Constructor & Destructor
		//@{
		//! Default constructor
		ConnectorPool();
		//! Copy constructor
		ConnectorPool(const ConnectorPool& rhs);
		/*!
		** \brief Destructor
		**
		** The destructor will try to close all connections
		** \see close()
		*/
		~ConnectorPool();
		//@}


		//! \name Channel
		//@{
		/*!
		** \brief Try to open a connection to a remote database
		**
		** A real connexion attempt to the database will be performed in order to validate
		** input parameters.
		*/
		Error open(Adapter::IAdapter* adapter, const Settings& settings);

		/*!
		** \brief Try to open a connection to a remote database with default settings
		**
		** A real connexion attempt to the database will be performed in order to validate
		** input parameters.
		**
		** \param adapter Adapter name (sqlite, psql...)
		** \param host    host name (or filename)
		*/
		Error open(Adapter::IAdapter* adapter, AnyString host);

		/*!
		** \brief Try to open a connection to a remote database with default settings
		**
		** A real connexion attempt to the database will be performed in order to validate
		** input parameters.
		**
		** \param adapter Adapter name (sqlite, psql...)
		** \param host    host name (or filename)
		*/
		Error open(Adapter::IAdapter* adapter, AnyString host, AnyString dbname, AnyString user, AnyString password);

		/*!
		** \brief Close all connections to the remote database
		**
		** It is safe to call this method several times. The pool will try to
		** close all connections (all which are not currently in use)
		*/
		void close();
		//@}


		//! \name Transactions
		//@{
		/*!
		** \brief Start a new transaction
		*/
		Transaction begin();

		/*!
		** \brief Start a new transaction which will be automatically commited
		*/
		AutoCommit autocommit();
		//@}


		//! \name Settings
		//@{
		//! Get a copy of the connector settings
		bool retrieveSettings(Settings& out) const;
		//@}


		//! \name Maintenance
		//@{
		/*!
		** \brief Close all connections which are idle for too long
		**
		** Calling this method should not be necessary since there is worker thread
		** dedicated to maintenance
		**
		** \param[out] remainingCount the number of connections which remain active
		** \param[out] closedCount The number of connection which have been closed
		*/
		void closeIdleConnections(uint* remainingCount = nullptr, uint* closedCount = nullptr);

		/*!
		** \brief Close all connections which are idle for too long
		**
		** Calling this method should not be necessary since there is worker thread
		** dedicated to maintenance
		**
		** \param idleTime The maximum idle time in seconds (override the value provided by the connection settings)
		** \param[out] remainingCount the number of connections which remain active
		** \param[out] closedCount The number of connection which have been closed
		*/
		void closeIdleConnections(uint idleTime, uint* remainingCount = nullptr, uint* closedCount = nullptr);
		//@}


		//! \name Operators
		//@{
		//! Operator =
		ConnectorPool& operator = (const ConnectorPool& rhs);
		//@}


	public:
		//! Event trigered when a SQL error occurs
		// \important NOT IMPLEMENTED YET !!!
		Event<void ()> onSQLError;


	private:
		//! Specific connector data
		Yuni::Private::DBI::ConnectorDataPtr pData;

		friend class Transaction;

	}; // class ConnectorPool





} // namespace DBI
} // namespace Yuni

# include "transaction.h"
# include "connector-pool.hxx"

#endif // __YUNI_DBI_CONNECTOR_H__
