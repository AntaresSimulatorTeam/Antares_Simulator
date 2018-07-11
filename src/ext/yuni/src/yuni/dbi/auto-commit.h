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


namespace Yuni
{
namespace DBI
{

	/*!
	** \brief Class helper for automatically commit a transaction at the end of the scope
	** \ingroup DBI
	*/
	class AutoCommit final
	{
	public:
		explicit AutoCommit(ConnectorPool& connectors) :
			tx(std::move(connectors.begin()))
		{}

		AutoCommit(AutoCommit&& rhs) :
			tx(std::move(rhs.tx))
		{}

		~AutoCommit()
		{
			tx.commit();
		}

	public:
		//! The transaction
		Transaction tx;

	}; // class AutoCommit





} // namespace DBI
} // namespace Yuni
