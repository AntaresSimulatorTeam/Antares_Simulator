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
#include "../../yuni.h"



namespace Yuni
{

	/*!
	** \brief Suspend thread execution for an interval measured in seconds
	** \ingroup Core
	**
	** \param seconds Number of seconds to wait
	**
	** \note You should consider that this method can not be interrupted.
	** You should not use this routine when using the thread facility provided
	** by the Yuni library and you should prefer the method `suspend()` instead.
	**
	** \see Yuni::Threads::AThread::suspend()
	*/
	YUNI_DECL void Suspend(uint seconds);


	/*!
	** \brief Suspend thread execution for an interval measured in milliseconds
	** \ingroup Core
	**
	** \param milliseconds NUmber of milliseconds to wait
	**
	** \note You should consider that this method can not be interrupted.
	** You should not use this routine when using the thread facility provided
	** by the Yuni library and you should prefer the method `suspend()` instead.
	*/
	YUNI_DECL void SuspendMilliSeconds(uint milliseconds);



} // namespace Yuni

