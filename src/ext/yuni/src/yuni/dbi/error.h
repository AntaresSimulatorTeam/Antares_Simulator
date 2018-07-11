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
#ifndef __YUNI_DBI_ERROR_H__
# define __YUNI_DBI_ERROR_H__


// yn_dbi_error and Yuni::DBI::Error must match

/*!
** \brief C error for DBI operations
** \ingroup DBI
*/
extern "C" enum yn_dbierr
{
	//! No error
	yerr_dbi_none,
	//! Generic error
	yerr_dbi_failed,
	//! Invalid adapter
	yerr_dbi_invalid_adapter,
	//! The database connection was lost, even after automatic reconnection
	yerr_dbi_connection_failed,
	//! No transaction
	yerr_dbi_no_transaction,
	//! No query
	yerr_dbi_no_query,
	//! Invalid nested transaction,
	yerr_dbi_invalid_nested_transaction,
	//! Invalid identifier
	yerr_dbi_invalid_identifier,
	//! The database is corrupted
	yerr_dbi_corrupt,
	//! No row
	yerr_dbi_no_row,
};


# ifdef __cplusplus

namespace Yuni
{
namespace DBI
{

	/*!
	** \brief C++ error for DBI operations
	** \ingroup DBI
	*/
	enum Error
	{
		//! No error
		errNone,
		//! Generic error
		errFailed,
		//! Invalid adapter
		errInvalidAdapter,
		//! The database connection was lost, even after automatic reconnection
		errConnectionFailed,
		//! No transaction
		errNoTransaction,
		//! No query
		errNoQuery,
		//! Invalid nested transaction,
		errInvalidNestedTransaction,
		//! Invalid identifier
		errInvalidIdentifier,
		//! The database is corrupted
		errCorrupt,
		//! No row
		errNoRow,
	};



} // namespace DBI
} // namespace Yuni

# endif // __cplusplus

#endif /* __YUNI_DBI_ERROR_H__ */
