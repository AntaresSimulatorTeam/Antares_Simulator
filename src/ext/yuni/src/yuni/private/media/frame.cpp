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
#include "frame.h"

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


	// Hidden implementation for PImpl idiom
	class FrameImpl final
	{
	public:
		FrameImpl():
			frame(nullptr)
		{}
		~FrameImpl()
		{
			::av_free(frame);
		}

		// The AV Frame
		::AVFrame* frame;

	}; // class FrameImpl



	Frame::Frame(uint index, double pts):
		pIndex(index),
		pTimestamp(pts),
		pImpl(new FrameImpl())
	{}


	Frame::~Frame()
	{
		delete pImpl;
	}


	bool Frame::valid() const
	{
		assert(pImpl && pImpl->frame);
		return pImpl->frame and pImpl->frame->linesize[0] > 0
			and pImpl->frame->linesize[1] > 0 and pImpl->frame->linesize[2] > 0;
	}

	bool Frame::isVideo() const
	{
		assert(pImpl && pImpl->frame);
		return pImpl->frame->width > 0;
	}

	bool Frame::isAudio() const
	{
		assert(pImpl && pImpl->frame);
		return pImpl->frame->nb_samples > 0;
	}


	uint Frame::width() const
	{
		assert(pImpl->frame);
		assert(isVideo());
		return pImpl->frame->width;
	}

	uint Frame::height() const
	{
		assert(pImpl->frame);
		assert(isVideo());
		return pImpl->frame->height;
	}


	uint8* Frame::audioData()
	{
		assert(pImpl->frame);
		assert(isAudio());
		return pImpl->frame->extended_data[0];
	}

	uint Frame::audioSize() const
	{
		assert(pImpl->frame);
		assert(isAudio());
		return pImpl->frame->linesize[0];
	}


	uint8* Frame::dataY() const
	{
		assert(pImpl->frame);
		assert(isVideo());
		return pImpl->frame->data[0];
	}

	uint8* Frame::dataCb() const
	{
		assert(pImpl->frame);
		assert(isVideo());
		return pImpl->frame->data[1];
	}

	uint8* Frame::dataCr() const
	{
		assert(pImpl->frame);
		assert(isVideo());
		return pImpl->frame->data[2];
	}


	uint Frame::lineSizeY() const
	{
		assert(pImpl->frame);
		assert(isVideo());
		return pImpl->frame->linesize[0];
	}

	uint Frame::lineSizeCb() const
	{
		assert(pImpl->frame);
		assert(isVideo());
		return pImpl->frame->linesize[1];
	}

	uint Frame::lineSizeCr() const
	{
		assert(pImpl->frame);
		assert(isVideo());
		return pImpl->frame->linesize[0];
	}


	double Frame::timestamp() const
	{
		return pTimestamp;
	}

	uint Frame::frameNumber() const
	{
		assert(isVideo());
		return ::av_frame_get_best_effort_timestamp(pImpl->frame);
	}


	void Frame::setData(void* data)
	{
		// This is not the common usage, but if we end up replacing the data
		// do not forget to release the previous
		if (pImpl->frame)
			::av_free(pImpl->frame);
		pImpl->frame = reinterpret_cast< ::AVFrame*>(data);
	}





} // namespace Media
} // namespace Private
} // namespace Yuni
