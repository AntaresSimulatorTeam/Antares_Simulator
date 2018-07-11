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
#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>
#include "fwd.h"
#include "cursor.h"
#include "error.h"



namespace Yuni
{
namespace DBI
{

	/*!
	** \brief Transaction
	**
	** \code
	**	auto tx = dbconnexion.begin();
	**	auto query = tx.query("SELECT * FROM data WHERE year >= ? AND year <= ?");
	**	query.map(2010, 2042);
	**	if (DBI::errNone != query.each([] (const DBI::Row& row)
	**	{
	**		return true;
	**	})
	**	{
	**
	**	}
	** \endcode
	*/
	class Transaction final : private NonCopyable<Transaction>
	{
	public:
		//! \name Constructors & Destructor
		//@{
		/*!
		** \brief Move constructor
		*/
		Transaction(Transaction&& other);
		//! Destructor
		~Transaction();
		//@}


		//! \name Actions
		//@{
		/*!
		** \brief Try to commit the current transaction
		*/
		DBI::Error commit();

		/*!
		** \brief Rollback the transaction
		*/
		DBI::Error rollback();

		/*!
		** \brief re-start with a new transaction
		**
		** If a transaction was already taking place, it will be rolled back
		*/
		DBI::Error rebegin();
		//@}


		//! \name SQL queries
		//@{
		/*!
		** \brief Create a new query
		*/
		Cursor prepare(const AnyString& stmt);

		/*!
		** \brief Perform a query and discard the resultset
		*/
		DBI::Error perform(const AnyString& script);

		//! Perform a query and discard the resultset with one bound parameter
		template<class A1>
		DBI::Error perform(const AnyString& script, const A1& a1);

		//! Perform a query and discard the resultset with 2 parameters
		template<class A1, class A2>
		DBI::Error perform(const AnyString& script, const A1& a1, const A2& a2);

		//! Perform a query and discard the resultset with 3 parameters
		template<class A1, class A2, class A3>
		DBI::Error perform(const AnyString& script, const A1& a1, const A2& a2, const A3& a3);

		//! Perform a query and discard the resultset with 4 parameters
		template<class A1, class A2, class A3, class A4>
		DBI::Error perform(const AnyString& script, const A1& a1, const A2& a2, const A3& a3, const A4& a4);

		//! Iterate over all rows of the resultset of a query (without any parameter)
		template<class CallbackT>
		DBI::Error each(const AnyString& query, const CallbackT& callback);

		//! Iterate over all rows of the resultset of a query with 1 parameter
		template<class CallbackT, class A1>
		DBI::Error each(const AnyString& query, const A1& a1, const CallbackT& callback);

		//! Iterate over all rows of the resultset of a query with 2 parameters
		template<class CallbackT, class A1, class A2>
		DBI::Error each(const AnyString& query, const A1& a1, const A2& a2, const CallbackT& callback);

		//! Iterate over all rows of the resultset of a query with 3 parameters
		template<class CallbackT, class A1, class A2, class A3>
		DBI::Error each(const AnyString& query, const A1& a1, const A2& a2, const A3& a3, const CallbackT& callback);

		//! Iterate over all rows of the resultset of a query with 4 parameters
		template<class CallbackT, class A1, class A2, class A3, class A4>
		DBI::Error each(const AnyString& query, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const CallbackT& callback);
		//@}


		//! \name Convenient routines
		//@{
		/*!
		** \brief Truncate a table
		**
		** The real SQL query is adapter-dependent.
		** This method should be prefered instead of using the
		** SQL query `truncate <tablename>`, which may not exist.
		*/
		DBI::Error truncate(const AnyString& tablename);

		/*!
		** \brief Garbage-collect and optionally analyze a database
		**
		** The real SQL query is adapter-dependent.
		*/
		DBI::Error vacuum();
		//@}


		//! \name Connector
		//@{
		//! Retrieve a connector from the current transaction
		//Connector connector() const;
		//@}


		//! \name Operators
		//@{
		//! operator (), equivalent to query()
		Cursor operator () (const AnyString& stmt);
		//@}


	protected:
		//! Constructor from a connector
		explicit Transaction(Yuni::Private::DBI::ConnectorDataPtr& data);
		//! Constructor from a channel
		explicit Transaction(Yuni::Private::DBI::ChannelPtr& data);

	private:
		//! Communication channel with the remote database
		Yuni::Private::DBI::ChannelPtr pChannel;
		//! Transaction handle
		uint pTxHandle;
		// friends
		friend class ConnectorPool;

	}; // class Transaction





} // namespace DBI
} // namespace Yuni

# include "auto-commit.h"
# include "transaction.hxx"

