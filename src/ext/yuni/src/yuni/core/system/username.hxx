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
#include <stdlib.h>
#include "../traits/cstring.h"
#ifndef YUNI_OS_WINDOWS
# include "environment.h"
#endif



namespace Yuni
{
namespace Private
{
namespace System
{

	# ifdef YUNI_OS_WINDOWS
	/*!
	** \brief Retrieves the calling user's name into a mere C-String buffer (Windows only)
	*/
	YUNI_DECL uint WindowsUsername(char* cstring, uint size);
	# endif



} // namespace System
} // namespace Private
} // namespace Yuni



namespace Yuni
{
namespace System
{


	template<class StringT>
	bool Username(StringT& out, bool emptyBefore)
	{
		// Assert, if a C* container can not be found at compile time
		YUNI_STATIC_ASSERT(Traits::CString<StringT>::valid, SystemUsername_InvalidTypeForString);

		# ifdef YUNI_OS_WINDOWS
		{
			if (emptyBefore)
				out.clear();

			// The maximum size for a username is 256 on any platform
			// We will reserve enough space for that size
			out.reserve(out.size() + 256 + 1 /* zero-terminated */);

			// The target buffer
			char* target = const_cast<char*>(out.c_str()) + out.size();
			// Since it may be any string (like a static one), we may have less than 256 chars
			uint size = out.capacity() - out.size();
			if (!size)
				return false; // not enough rooms

			// Appending the username to our buffer and retrieving the size of
			// the username
			uint written = Yuni::Private::System::WindowsUsername(target, size);
			if (written)
			{
				// The username has been written, we have to properly resize the string
				// (absolutely required for zero-terminated strings)
				out.resize(out.size() + written);
				return true;
			}
			return false;
		}
		# else
		{
			return System::Environment::Read("LOGNAME", out, emptyBefore);
		}
		# endif
	}





} // namespace System
} // namespace Yuni

