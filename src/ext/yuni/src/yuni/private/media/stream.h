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
#ifndef __YUNI_PRIVATE_MEDIA_FILE_H__
# error "Do not include stream.h directly, please include file.h instead !"
#endif

#ifndef __YUNI_PRIVATE_MEDIA_STREAM_H__
# define __YUNI_PRIVATE_MEDIA_STREAM_H__

# include "../../yuni.h"
# include "../../core/smartptr.h"
# include <map>

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

# include "streamtype.h"
# include "frame.h"


namespace Yuni
{
namespace Private
{
namespace Media
{

	//! Forward declaration
	class File;


	/*!
	** \brief A media stream can be either audio or video data encoded with a given codec
	*/
	template<StreamType TypeT>
	class Stream final
	{
	public:
		enum // anonymous
		{
			IsVideo = TypeT == stVideo,
			IsAudio = TypeT == stAudio
		};


	public:
		//! Smart pointer
		typedef SmartPtr<Stream>  Ptr;

		//! Map
		typedef std::map<uint, Ptr>  Map;

		//! Packet queue
		typedef std::list<AVPacket*>  PacketQueue;

	public:
		//! Constructor
		Stream(File* parent, AVFormatContext* format, AVCodecContext* codec, uint index);
		//! Destructor
		~Stream();

		//! Stream index in file
		uint index() const;

		//! Parent file
		File* parent() { return pParent; }

		//! Image width (Video only !)
		uint width() const;
		//! Image height (Video only !)
		uint height() const;
		//! Color depth, in bits per pixel (Video only !)
		uint depth() const;
		//! Number of frames per second (Video only !)
		float fps() const;

		//! Sample rate (Audio only !)
		uint rate() const;
		//! Number of channels (Audio only !)
		uint channels() const;
		//! Bits per sample (Audio only !)
		uint bits() const;

		//! Stream duration in seconds
		uint duration() const;

		//! Get the stream type
		StreamType type() const;

		//! OpenAL audio format (Audio only !)
		uint alFormat() const { YUNI_STATIC_ASSERT(IsAudio, NotAccessibleInVideo); return pALFormat; }

		/*!
		** \brief Get the next frame
		*/
		Frame::Ptr nextFrame();

		//! Rewind the stream
		void rewind();

		/*!
		** \brief Is the stream ready for decoding ?
		**
		** \note The OpenAL format check is done only for audio
		*/
		bool valid() const { return nullptr != pCodec && IsAudio == (0 != pALFormat); }

	private:
		//! Read a frame from the stream
		uint readFrame();

		//! Get the next packet, either from queue, or from the stream if the queue is empty
		AVPacket* nextPacket();

	private:
		//! Codec information
		AVCodecContext* pCodec;

		//! Format information
		AVFormatContext* pFormat;

		//! Index in the media file
		uint pIndex;

		//! Stream format
		uint pALFormat;

		//! Stream data full size
		uint64 pSize;

		//! Current presentation time stamp
		double pCrtPts;

		//! Current frame index
		uint pCrtFrameIndex;

		//! Currently read frame
		AVFrame* pFrame;

		//! Queue for this stream's packets
		PacketQueue pPackets;

		//! Parent file
		File* pParent;

	private:
		//! Friend declaration
		friend class File;

	}; // class Stream



} // namespace Media
} // namespace Private
} // namespace Yuni

#endif // __YUNI_PRIVATE_MEDIA_STREAM_H__
