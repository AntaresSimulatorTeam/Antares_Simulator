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
#include "entries.h"


namespace Yuni
{
namespace DBI
{
namespace Adapter
{

	/*!
	** \brief Abstract adapter
	** \ingroup DBIAdapter
	*/
	class IAdapter
	{
	public:
		//! \name Default constructor
		//@[
		//! Default constructor
		IAdapter() {}
		//! Destructor
		virtual ~IAdapter() {}
		//@}

		//! \name Entries
		//@{
		//! Retrieve the entry table for the adapter
		virtual void retrieveEntries(::yn_dbi_adapter& entries) = 0;
		//@}

	}; // class IAdapter






} // namespace Adapter
} // namespace DBI
} // namespace Yuni
