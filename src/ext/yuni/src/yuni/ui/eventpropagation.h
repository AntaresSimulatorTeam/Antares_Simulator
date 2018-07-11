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
#ifndef __YUNI_UI_EVENTPROPAGATION_H__
# define __YUNI_UI_EVENTPROPAGATION_H__

namespace Yuni
{
namespace UI
{

	//! Propagation types for keyboard events
	enum EventPropagation
	{
		//! Continue propagation
		epContinue = 0,

		//! Continue propagation until you have notified all controls in the view
		epFinishView = 1,

		//! Stop propagation
		epStop = 2

	}; // enum EventPropagation


} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_EVENTPROPAGATION_H__
