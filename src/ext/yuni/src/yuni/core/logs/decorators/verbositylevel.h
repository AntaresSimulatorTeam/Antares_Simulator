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
	class YUNI_DECL VerbosityLevel : public LeftType
	{
	public:
		template<class Handler, class VerbosityType, class O>
		void internalDecoratorAddPrefix(O& out, const AnyString& s) const
		{
			// Write the verbosity to the output
			if (VerbosityType::hasName)
			{
				// Set Color
				if (Handler::colorsAllowed && VerbosityType::color != System::Console::none)
					System::Console::TextColor<VerbosityType::color>::Set(out);
				// The verbosity
				VerbosityType::AppendName(out);
				// Reset Color
				if (Handler::colorsAllowed && VerbosityType::color != System::Console::none)
					System::Console::ResetTextColor(out);
			}
			// Transmit the message to the next decorator
			LeftType::template internalDecoratorAddPrefix<Handler, VerbosityType,O>(out, s);
		}

	}; // class VerbosityLevel





} // namespace Logs
} // namespace Yuni

