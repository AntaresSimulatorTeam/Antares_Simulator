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
#include "../../yuni.h"
#include "../../core/dictionary.h"
#include "../../datetime/timestamp.h"
#include "../../core/noncopyable.h"
#include "../../dbi/settings.h"
#include "../../dbi/error.h"
#include "../../dbi/adapter/entries.h"

#ifdef YUNI_OS_32
#	define YUNI_PRIVATE_DBI_ATOMIC_INT  32
#else
#	define YUNI_PRIVATE_DBI_ATOMIC_INT  64
#endif



namespace Yuni
{
namespace Private
{
namespace DBI
{

	//! Error
	typedef Yuni::DBI::Error  Error;




	/*!
	** \brief Single connection to a remote database
	*/
	class Channel final : private Yuni::NonCopyable<Channel>
	{
	public:
		//! The most suitable smart pointer
		typedef Yuni::SmartPtr<Channel> Ptr;
		//! Channel table, per thread-pseudo id
		typedef Dictionary<uint64, Ptr>::Hash  Table;


	public:
		//! \name Constructor & Destructor
		//@{
		Channel(const Yuni::DBI::Settings& settings, const ::yn_dbi_adapter& adapter);
		//! Destructor
		~Channel();
		//@}


		//! \name Connection
		//@{
		/*!
		** \brief Open the connection
		** \see status
		*/
		void open();
		//@}


		//! \name Transactions
		//@{
		//! Start a new transaction
		Error begin(uint& handle);

		//! Try to commit the transaction
		Error commit(uint handle);

		//! Rollback a transaction
		Error rollback(uint handle);
		//@}


	public:
		//! Mutex, locked/released by transaction
		Yuni::Mutex mutex;
		//! Status of the connection
		Error status;
		//! Adapter
		::yn_dbi_adapter adapter;
		//! The total number of nested transactions
		uint nestedTransactionCount;
		//! Channel settings
		Yuni::DBI::Settings settings;

		//! Timestamp when the channel was last used
		Atomic::Int<YUNI_PRIVATE_DBI_ATOMIC_INT> lastUsed;

	}; // class Channel





} // namespace DBI
} // namespace Private
} // namespace Yuni

#include "channel.hxx"
