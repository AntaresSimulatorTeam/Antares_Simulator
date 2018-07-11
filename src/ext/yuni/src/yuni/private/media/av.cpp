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
#include <cstdlib>
#include <memory.h>

#include "av.h"
# if (YUNI_OS_GCC_VERSION >= 40102)
#	pragma GCC diagnostic ignored "-Wconversion"
#	pragma GCC diagnostic ignored "-Wundef"
# endif

extern "C"
{
# include "libavutil/pixfmt.h"
# include "libavutil/pixdesc.h"
# include "libavcodec/avcodec.h"
# include "libavformat/avformat.h"
}



namespace Yuni
{
namespace Private
{
namespace Media
{


	bool AV::Init()
	{
		// Initialize libavformat and register all the muxers
		::av_register_all();

		# ifdef NDEBUG
		// Silence warning output from the lib
		::av_log_set_level(AV_LOG_ERROR);
		# else // NDEBUG
		// Only write output when encountering unrecoverable errors
		::av_log_set_level(AV_LOG_FATAL);
		# endif // NDEBUG

		return true;
	}






} // namespace Media
} // namespace Private
} // namespace Yuni
