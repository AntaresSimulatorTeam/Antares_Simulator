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
#include "../null.h"
#include "../../event/event.h"


namespace Yuni
{
namespace Logs
{

	/*!
	** \brief Log Handler: The standard output (cout & cerr)
	*/
	template<class NextHandler = NullHandler>
	class YUNI_DECL Callback : public NextHandler
	{
	public:
		enum Settings
		{
			// Colors are not allowed in a file
			unixColorsAllowed = 0,
		};

	public:
		template<class LoggerT, class VerbosityType>
		void internalDecoratorWriteWL(LoggerT& logger, const AnyString& s) const
		{
			if ((uint) VerbosityType::level != (uint) Verbosity::Debug::level)
			{
				if (not callback.empty() and not s.empty())
				{
					// A mutex is already locked
					pDispatchedMessage = s;
					callback(VerbosityType::level, pDispatchedMessage);
				}
			}

			// Transmit the message to the next handler
			NextHandler::template internalDecoratorWriteWL<LoggerT, VerbosityType>(logger, s);
		}


	public:
		mutable Yuni::Event<void (int, const String&)> callback;
		mutable String pDispatchedMessage;

	}; // class Callback





} // namespace Logs
} // namespace Yuni

