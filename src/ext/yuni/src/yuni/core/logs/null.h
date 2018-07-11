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
#include "../string.h"



namespace Yuni
{
namespace Logs
{

	/*!
	** \brief Log Handler: The Null device
	**
	** This handler does produce nothing and skip all messages
	*/
	class YUNI_DECL NullHandler
	{
	public:
		template<class LoggerT, class VerbosityType>
		static void internalDecoratorWriteWL(LoggerT&, const AnyString&)
		{
			// Do nothing
		}
	};


	/*!
	** \brief Log Handler: The Null device
	**
	** This handler does produce nothing and skip all messages
	*/
	class YUNI_DECL NullDecorator
	{
	public:
		template<class Handler, class VerbosityType, class O>
		static void internalDecoratorAddPrefix(O&, const AnyString&)
		{
			// Do nothing
		}

	};




} // namespace Logs
} // namespace Yuni

