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
#include "dataholder.h"


namespace Yuni
{
namespace Private
{
namespace Variant
{

	/*!
	** \brief Specialisation for Bool to String.
	*/
	template<>
	struct Converter<bool, String> final
	{
		// Please use Yuni::String << bool
		static bool Value(const bool& from, String& to)
		{
			to = (from) ? "true" : "false";
			return true;
		}
	};


	template<class T> inline T IDataHolder::to() const
	{
		DataConverter<T> dc;
		convertUsing(dc);
		return dc.result;
	}


	template<class T> inline bool IDataHolder::to(T& out) const
	{
		DataConverter<T> dc;
		if (convertUsing(dc))
		{
			out = dc.result;
			return true;
		}
		out = T();
		return false;
	}






} // namespace Variant
} // namespace Private
} // namespace Yuni
