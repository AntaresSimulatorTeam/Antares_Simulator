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
#ifndef __YUNI_MEDIA_SOURCE_H__
# define __YUNI_MEDIA_SOURCE_H__

# include <map>
# include "../yuni.h"
# include "../core/string.h"
# include "../core/smartptr.h"
# include "../core/string.h"
# include "../thread/signal.h"
# include "../private/media/file.h"

namespace Yuni
{
namespace Media
{


	/*!
	** \brief A media source loaded from a file or network stream
	*/
	class Source final: public Policy::ObjectLevelLockable<Source>
	{
	public:
		//! Pointer type
		typedef SmartPtr<Source> Ptr;
		//! Map from a string to a Source::Ptr
		typedef std::map<String, Ptr> Map;
		//! Threading Policy
		typedef Policy::ObjectLevelLockable<Source>  ThreadingPolicy;
		//! Audio stream pointer type
		typedef Private::Media::Stream<Private::Media::stAudio>::Ptr  AStreamPtr;
		//! Video stream pointer type
		typedef Private::Media::Stream<Private::Media::stVideo>::Ptr  VStreamPtr;

	public:
		enum // anonymous
		{
			maxBufferCount = 4,
			minBufferSize = 61000,
			maxBufferSize = 65536,

			bestQueueSize = 150

		}; // enum anonymous

	public:
		//! Constructor
		Source();
		~Source();

		bool prepareDispatched(uint source);

		bool destroyDispatched(Thread::Signal* signal);

		bool updateDispatched(uint source);

		bool rewindDispatched(uint source);

		void stream(const AStreamPtr& audioStream) { pAStream = audioStream; }

		void stream(const VStreamPtr& videoStream) { pVStream = videoStream; }

		void stream(const VStreamPtr& videoStream, const AStreamPtr& audioStream)
		{
			pAStream = audioStream;
			pVStream = videoStream;
		}

		//! Has either valid audio, valid video, or both.
		bool valid() const { return hasAudio() or hasVideo(); }

		//! Has a valid audio stream
		bool hasAudio() const { return nullptr != pAStream and pAStream->valid(); }

		//! Has a valid video stream
		bool hasVideo() const { return nullptr != pVStream and pVStream->valid(); }

		//! Get the duration of the stream, 0 if not set
		uint duration() const;

		//! Get the current elapsed time of the source
		float elapsedTime() const
		{
			if (!pAStream)
				return 0.0f;
			return pSecondsElapsed + pSecondsCurrent;
		}

		//! Width of the video (in pixels). Only valid if there is video !
		uint width() const { assert(hasVideo()); return pVStream->width(); }

		//! Height of the video (in pixels). Only valid if there is video !
		uint height() const { assert(hasVideo()); return pVStream->height(); }

		//! Depth of the video (in bits / pixel). Only valid if there is video !
		uint depth() const { assert(hasVideo()); return pVStream->depth(); }

		//! Frames per second of the video. Only valid if there is video !
		float fps() const { assert(hasVideo()); return pVStream->fps(); }

		//! Audio sampling rate. Only valid if there is audio !
		uint samplingRate() const { assert(hasAudio()); return pAStream->rate(); }

		//! Number of audio channels. Only valid if there is audio !
		uint channels() const { assert(hasAudio()); return pAStream->channels(); }

		//! Bits per sample for audio. Only valid if there is audio !
		uint bitsPerSample() const { assert(hasAudio()); return pAStream->bits(); }

		//! Get the next video frame. Only valid if there is video !
		Private::Media::Frame::Ptr nextFrame();

	private:
		//! Refill audio buffer
		uint fillBuffer();

		//! Refill the video frame queue
		void fillQueue();

	private:
		//! The audio stream
		AStreamPtr pAStream;

		//! The video stream
		VStreamPtr pVStream;

		//! Actual number of buffers
		uint pBufferCount;

		//! Seconds already played from previous buffers
		float pSecondsElapsed;

		//! Seconds already played from current buffer
		float pSecondsCurrent;

		//! Identifiers of the OpenAL buffers used
		uint pIDs[maxBufferCount];

		//! Buffer for storing decoded audio data
		CString<maxBufferSize, false> pData;

		//! Currently loaded video frames
		Private::Media::Frame::Queue pFrames;

	}; // class Source






} // namespace Media
} // namespace Yuni

# include "source.hxx"

#endif // __YUNI_MEDIA_SOURCE_H__
