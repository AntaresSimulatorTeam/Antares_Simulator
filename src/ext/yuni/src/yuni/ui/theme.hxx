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
#ifndef __YUNI_UI_THEME_HXX__
# define __YUNI_UI_THEME_HXX__

namespace Yuni
{
namespace UI
{

	inline Theme::Ptr Theme::Current()
	{
		if (!sCurrent)
			sCurrent = sDefault;
		return sCurrent;
	}


	inline Theme::Ptr Theme::Default()
	{
		return sDefault;
	}



} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_THEME_HXX__
