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
#include <iostream>

#if defined(YUNI_OS_WINDOWS)
# define YUNI_LOGS_COLORS_ALLOWED  1
#else
# define YUNI_LOGS_COLORS_ALLOWED  1
#endif


namespace Yuni
{
namespace Logs
{

	/*!
	** \brief Log Handler: The standard output (cout & cerr)
	*/
	template<class NextHandler = NullHandler>
	class YUNI_DECL StdCout : public NextHandler
	{
	public:
		enum Settings
		{
			colorsAllowed = YUNI_LOGS_COLORS_ALLOWED,
		};

	public:
		template<class LoggerT, class VerbosityType>
		void internalDecoratorWriteWL(LoggerT& logger, const AnyString& s) const
		{
			typedef typename LoggerT::DecoratorsType DecoratorsType;

			// Write the message to the std::cout/cerr
			if (VerbosityType::shouldUsesStdCerr)
			{
				logger.DecoratorsType::template internalDecoratorAddPrefix<StdCout, VerbosityType>(std::cerr, s);
				// Flush
				std::cerr << std::endl;
			}
			else
			{
				logger.DecoratorsType::template internalDecoratorAddPrefix<StdCout, VerbosityType>(std::cout, s);
				// Flush
				std::cout << std::endl;
			}

			// Transmit the message to the next handler
			NextHandler::template internalDecoratorWriteWL<LoggerT, VerbosityType>(logger, s);
		}

	}; // class StdCout






} // namespace Logs
} // namespace Yuni

#undef YUNI_LOGS_UNIX_COLORS_ALLOWED

