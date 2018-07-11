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



namespace Yuni
{
namespace Private
{
namespace LogsDecorator
{

	// Forward declarations
	# if defined(YUNI_OS_MINGW)
	YUNI_DECL char* WriteCurrentTimestampToBufferMinGW(void);
	# else
	YUNI_DECL void WriteCurrentTimestampToBuffer(char buffer[32]);
	# endif

} // namespace LogsDecorator
} // namespace Private
} // namespace Yuni



namespace Yuni
{
namespace Logs
{

	template<class LeftType = NullDecorator>
	class YUNI_DECL Time : public LeftType
	{
	public:
		template<class Handler, class VerbosityType, class O>
		void internalDecoratorAddPrefix(O& out, const AnyString& s) const
		{
			out.put('[');

			# ifndef YUNI_OS_MINGW
			char asc[32]; // MSDN specifies that the buffer length value must be >= 26 for validity
			Private::LogsDecorator::WriteCurrentTimestampToBuffer(asc);
			out.write(asc, 24);
			# else
			out.write(Private::LogsDecorator::WriteCurrentTimestampToBufferMinGW(), 24);
			# endif

			out.put(']');

			// Transmit the message to the next decorator
			LeftType::template internalDecoratorAddPrefix<Handler, VerbosityType,O>(out, s);
		}

	}; // class Time




} // namespace Logs
} // namespace Yuni

