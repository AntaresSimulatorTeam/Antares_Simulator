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



namespace Yuni
{

	inline uint Semaphore::maxReaders() const
	{
		return pMaxReaders;
	}


	inline Semaphore& Semaphore::operator -- ()
	{
		acquire();
		return *this;
	}


	inline Semaphore& Semaphore::operator ++ ()
	{
		release();
		return *this;
	}




} // namespace Yuni
