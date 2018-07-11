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
#include "transaction.h"



namespace Yuni
{
namespace DBI
{


	inline Cursor Transaction::operator () (const AnyString& stmt)
	{
		return prepare(stmt);
	}


	template<class A1>
	inline DBI::Error Transaction::perform(const AnyString& script, const A1& a1)
	{
		Cursor stmt = prepare(script);
		stmt.map(a1);
		return stmt.perform();
	}

	template<class A1, class A2>
	inline DBI::Error Transaction::perform(const AnyString& script, const A1& a1, const A2& a2)
	{
		Cursor stmt = prepare(script);
		stmt.map(a1, a2);
		return stmt.perform();
	}


	template<class A1, class A2, class A3>
	inline DBI::Error Transaction::perform(const AnyString& script, const A1& a1, const A2& a2, const A3& a3)
	{
		Cursor stmt = prepare(script);
		stmt.map(a1, a2, a3);
		return stmt.perform();
	}


	template<class A1, class A2, class A3, class A4>
	inline DBI::Error Transaction::perform(const AnyString& script, const A1& a1, const A2& a2, const A3& a3, const A4& a4)
	{
		Cursor stmt = prepare(script);
		stmt.map(a1, a2, a3, a4);
		return stmt.perform();
	}


	template<class CallbackT>
	inline DBI::Error Transaction::each(const AnyString& query, const CallbackT& callback)
	{
		Cursor stmt = prepare(query);
		stmt.execute();
		return stmt.each(callback);
	}


	template<class CallbackT, class A1>
	inline
	DBI::Error Transaction::each(const AnyString& query, const A1& a1, const CallbackT& callback)
	{
		Cursor stmt = prepare(query);
		stmt.map(a1);
		stmt.execute();
		return stmt.each(callback);
	}


	template<class CallbackT, class A1, class A2>
	inline
	DBI::Error Transaction::each(const AnyString& query, const A1& a1, const A2& a2, const CallbackT& callback)
	{
		Cursor stmt = prepare(query);
		stmt.map(a1, a2);
		stmt.execute();
		return stmt.each(callback);
	}


	template<class CallbackT, class A1, class A2, class A3>
	inline
	DBI::Error Transaction::each(const AnyString& query, const A1& a1, const A2& a2, const A3& a3, const CallbackT& callback)
	{
		Cursor stmt = prepare(query);
		stmt.map(a1, a2, a3);
		stmt.execute();
		return stmt.each(callback);
	}


	template<class CallbackT, class A1, class A2, class A3, class A4>
	inline
	DBI::Error Transaction::each(const AnyString& query, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const CallbackT& callback)
	{
		Cursor stmt = prepare(query);
		stmt.map(a1, a2, a3, a4);
		stmt.execute();
		return stmt.each(callback);
	}





} // namespace DBI
} // namespace Yuni

