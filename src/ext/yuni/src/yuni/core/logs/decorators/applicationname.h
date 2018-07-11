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
namespace Logs
{

	template<class LeftType = NullDecorator>
	class YUNI_DECL ApplicationName : public LeftType
	{
	public:
		ApplicationName() :
			pAppName("noname")
		{}


		//! \name Apllication Name
		//@{
		const String& applicationName() const {return pAppName;}

		/*!
		** \brief Set the Application name
		**
		** \warning This method is not thread-safe and should only be used after the creation
		** of the logger
		*/
		void applicationName(const AnyString& s) {pAppName = s;}
		//@}


		template<class Handler, class VerbosityType, class O>
		inline void internalDecoratorAddPrefix(O& out, const AnyString& s) const
		{
			// Write the verbosity to the output
			out.put('[');
			out.write(pAppName.c_str(), pAppName.size());
			out.put(']');
			// Transmit the message to the next handler
			LeftType::template internalDecoratorAddPrefix<Handler, VerbosityType,O>(out, s);
		}


	private:
		//! The Application name
		String pAppName;

	}; // class ApplicationName





} // namespace Logs
} // namespace Yuni

