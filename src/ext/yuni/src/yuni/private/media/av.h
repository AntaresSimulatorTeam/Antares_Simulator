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
#ifndef __YUNI_PRIVATE_AUDIO_AV_H__
# define __YUNI_PRIVATE_AUDIO_AV_H__

# include "../../yuni.h"

namespace Yuni
{
namespace Private
{
namespace Media
{

	/*!
	** \brief This is a wrapper around the AV* libs from ffmpeg.
	*/
	class AV
	{
	public:
		//! Initialize ffmpeg
		static bool Init();

	}; // class AV




} // namespace Media
} // namespace Private
} // namespace Yuni

#endif // __YUNI_PRIVATE_AUDIO_AV_H__
