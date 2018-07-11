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
# include "file.h"
# include <cassert>

namespace Yuni
{
namespace Private
{
namespace Media
{


	File::File(const AnyString& filePath):
		pFormat(nullptr)
	{
		if (filePath.empty())
			return;

		# if LIBAVFORMAT_VERSION_MAJOR < 53
		if (::av_open_input_file(&pFormat, filePath.c_str(), nullptr, 0, nullptr))
		# else
		if (::avformat_open_input(&pFormat, filePath.c_str(), nullptr, nullptr))
		# endif // LIBAVFORMAT_VERSION_MAJOR < 53
		{
			pFormat = nullptr;
			return;
		}

		// After opening, we must search for the stream information because not
		// all formats will have it in stream headers (eg. system MPEG streams)
		# if LIBAVFORMAT_VERSION_MAJOR < 53
		if (::av_find_stream_info(pFormat) < 0)
		# else
		if (::avformat_find_stream_info(pFormat, nullptr) < 0)
		# endif // LIBAVFORMAT_VERSION_MAJOR < 53
		{
			# if LIBAVFORMAT_VERSION_MAJOR < 53
			::av_close_input_file(pFormat);
			# else
			::avformat_close_input(&pFormat);
			# endif // LIBAVFORMAT_VERSION_MAJOR < 53
			pFormat = nullptr;
			return;
		}
	}


	File::~File()
	{
		pAStreams.clear();
		pVStreams.clear();

		if (pFormat)
		{
			# if LIBAVFORMAT_VERSION_MAJOR < 53
			::av_close_input_file(pFormat);
			# else
			::avformat_close_input(&pFormat);
			# endif // LIBAVFORMAT_VERSION_MAJOR < 53
		}
	}


	uint File::duration() const
	{
		assert(pFormat and "invalid format");
		return pFormat ? (uint)(pFormat->duration / AV_TIME_BASE) : 0;
	}


	void File::rewind()
	{
		assert(pFormat and "invalid format");

		for (auto stream : pVStreams)
			stream.second->rewind();
		for (auto stream : pAStreams)
			stream.second->rewind();
	}




} // namespace Media
} // namespace Private
} // namespace Yuni

