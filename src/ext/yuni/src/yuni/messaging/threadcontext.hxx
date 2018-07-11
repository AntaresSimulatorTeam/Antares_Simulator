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
#ifndef __YUNI_MESSAGING_THREAD_CONTEXT_HXX__
# define __YUNI_MESSAGING_THREAD_CONTEXT_HXX__


namespace Yuni
{
namespace Messaging
{

	inline void Context::clear()
	{
		text.clear();
		clob.clear();
	}


	template<uint MaxSize, class StringT>
	inline void Context::AutoShrink(StringT& variable)
	{
		variable.clear();
		if (variable.capacity() > MaxSize)
			variable.shrink();
	}


	inline void Context::autoshrink()
	{
		AutoShrink<6 * 1024>(text);
		AutoShrink<5 * 1024 * 1024>(clob);
		AutoShrink<1 * 1024 * 1024>(buffer);
	}




} // namespace Messaging
} // namespace Yuni

#endif // __YUNI_MESSAGING_THREAD_CONTEXT_H__
