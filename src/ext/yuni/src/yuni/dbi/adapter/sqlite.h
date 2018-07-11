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
#include "adapter.h"


namespace Yuni
{
namespace DBI
{
namespace Adapter
{

	/*!
	** \brief SQLite Adapter (sqlite v3)
	** \ingroup DBIAdapter
	*/
	class SQLite final : public IAdapter
	{
	public:
		SQLite() {}
		virtual ~SQLite() {}

		virtual void retrieveEntries(::yn_dbi_adapter& entries) override;

	}; // class SQLite





} // namespace Adapter
} // namespace DBI
} // namespace Yuni
