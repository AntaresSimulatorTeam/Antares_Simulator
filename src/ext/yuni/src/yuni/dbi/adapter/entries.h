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
#include "../error.h"


/*!
** \brief Adapter entries table
**
** \note All routines must be reentrant
*/
struct yn_dbi_adapter
{
	//! Database handle, provided for convenient uses
	void* dbh;

	//! Start a new transaction
	yn_dbierr (*begin)(void* dbh);
	//! Commit the current transaction
	yn_dbierr (*commit)(void* dbh);
	//! Create a new savepoint in the current transaction
	yn_dbierr (*savepoint)(void* dbh, const char* name, uint length);
	//! Create a new savepoint in the current transaction
	yn_dbierr (*commit_savepoint)(void* dbh, const char* name, uint length);
	//! Rollback the current transaction
	yn_dbierr (*rollback)(void* dbh);
	//! Rollback a savepoint
	yn_dbierr (*rollback_savepoint)(void* dbh, const char* name, uint length);

	//! Execute a simple query
	yn_dbierr (*query_exec)(void* dbh, const char* stmt, uint length);
	//! Start a new query (and acquire it)
	yn_dbierr (*query_new)(void** qh, void* dbh, const char* stmt, uint length);
	//! Acquire a query pointer
	void (*query_ref_acquire)(void* qh);
	//! Release a query
	void (*query_ref_release)(void* qh);
	//! Bind a string
	yn_dbierr (*bind_str)(void* qh, uint index, const char* str, uint length);
	//! Bind a bool
	yn_dbierr (*bind_bool)(void* qh, uint index, int value);
	//! Bind a sint32
	yn_dbierr (*bind_int32)(void* qh, uint index, yint32 value);
	//! Bind a sint64
	yn_dbierr (*bind_int64)(void* qh, uint index, yint64 value);
	//! Bind a double
	yn_dbierr (*bind_double)(void* qh, uint index, double value);
	//! Bind a null value
	yn_dbierr (*bind_null)(void* qh, uint index);

	//! Execute a query
	yn_dbierr (*query_execute)(void* qh);
	//! Execute a query, and release the handler
	yn_dbierr (*query_perform_and_release)(void* qh);

	//! Go to the next row
	yn_dbierr (*cursor_go_to_next)(void* qh);
	//! Go to the previous row
	yn_dbierr (*cursor_go_to_previous)(void* qh);
	//! Go to a specific row
	yn_dbierr (*cursor_go_to)(void* qh, yuint64 rowindex);

	//! Get the value as an int32 of a specific column for the current row
	yint32 (*column_to_int32)(void* qh, uint colindex);
	//! Get the value as an int64 of a specific column for the current row
	yint64 (*column_to_int64)(void* qh, uint colindex);
	//! Get the value as a double of a specific column for the current row
	double (*column_to_double)(void* qh, uint colindex);
	//! Get the value as a string of a specific column for the current row
	const char* (*column_to_cstring)(void* qh, uint colindex, uint* length);
	//! Get whether a column is null or not
	int (*column_is_null)(void* qh, uint colindex);

	//! garbage-collect and optionally analyze a database
	yn_dbierr (*vacuum)(void* dbh);
	//! truncate a table
	yn_dbierr (*truncate)(void* dbh, const char* tablename, uint length);

	//! Open a connection to the remote database
	yn_dbierr (*open) (void** dbh, const char* host, uint port, const char* user, const char* pass, const char* dbname);
	//! Open a schema
	yn_dbierr (*open_schema) (void* dbh, const char* name, uint length);
	//! Close the connection
	void (*close) (void* dbh);

}; // class Adapter
