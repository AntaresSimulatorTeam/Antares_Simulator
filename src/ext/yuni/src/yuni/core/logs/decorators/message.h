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
#include "../../system/console.h"



namespace Yuni
{
namespace Logs
{

	template<class LeftType = NullDecorator>
	class YUNI_DECL Message : public LeftType
	{
	public:
		template<class Handler, class VerbosityType, class O>
		void internalDecoratorAddPrefix(O& out, const AnyString& s) const
		{
			// Write the message itself
			out.put(' ');

			// Color
			if (VerbosityType::messageColor != System::Console::none && Handler::colorsAllowed)
				System::Console::TextColor<VerbosityType::messageColor>::Set(out);

			// The message
			out.write(s.c_str(), (uint) s.sizeInBytes());

			// Resetting the color
			if (VerbosityType::messageColor != System::Console::none && Handler::colorsAllowed)
				System::Console::ResetTextColor(out);

			// Transmit the message to the next handler
			LeftType::template internalDecoratorAddPrefix<Handler, VerbosityType,O>(out, s);
		}

	}; // class VerbosityLevel





} // namespace Logs
} // namespace Yuni

