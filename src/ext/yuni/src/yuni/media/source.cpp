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
# include "../private/media/openal.h"
# include "source.h"
# include "../private/media/frame.h"

namespace Yuni
{
namespace Media
{

	uint Source::fillBuffer()
	{
		pData.clear();
		uint size = 0;
		while (size < minBufferSize)
		{
			// Make sure we get some data to give to the buffer
			Private::Media::Frame::Ptr frame = pAStream->nextFrame();
			if (!frame)
				return size;
			uint count = frame->audioSize();
			if (!count)
				return size;
			count = Math::Min(count, (uint)maxBufferSize - size);
			pData.append((const char*)frame->audioData(), count);
			size += count;
		}
		return size;
	}


	void Source::fillQueue()
	{
		for (uint i = pFrames.size(); i < bestQueueSize; )
		{
			Private::Media::Frame::Ptr frame = pVStream->nextFrame();
			if (!frame)
				return;

			if (frame->valid())
			{
				pFrames.push_back(frame);
				++i;
			}
		}
	}


	bool Source::prepareDispatched(uint source)
	{
		if (!pAStream and !pVStream)
			return false;

		// Audio
		if (nullptr != pAStream)
		{
			pBufferCount = 0;
			for (uint i = 0; i < maxBufferCount; ++i)
			{
				uint size = fillBuffer();
				if (!size)
					return false;

				if (!Private::Media::OpenAL::CreateBuffer(pIDs + i))
					return false;

				++pBufferCount;

				// Buffer the data with OpenAL
				if (!Private::Media::OpenAL::SetBufferData(pIDs[i], pAStream->alFormat(),
					pData.data(), size, pAStream->rate() / pAStream->channels()))
					return false;
				// Queue the buffers onto the source
				if (!Private::Media::OpenAL::QueueBufferToSource(pIDs[i], source))
					return false;
				// No more data to read
				if (size < minBufferSize)
					break;
			}
		}

		// Video
		if (nullptr != pVStream)
		{
			// fillQueue();
			// if (not pFrames.size())
			// 	return false;
		}

		return true;
	}


	bool Source::updateDispatched(uint source)
	{
		if (!valid())
			return false;

		// Audio
		if (hasAudio())
		{
			// Update time progression in the current buffer
			::alGetSourcef(source, AL_SEC_OFFSET, &pSecondsCurrent);
			// Check if a buffer has finished playing
			ALint processed = 0;
			::alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
			if (!processed)
				return true;

			// A buffer has finished playing, unqueue it
			ALuint buffer = Private::Media::OpenAL::UnqueueBufferFromSource(source);
			// Reset current buffer time
			pSecondsCurrent = 0.0f;
			uint bits = pAStream->bits();
			uint channels = pAStream->channels();
			uint frequency = pAStream->rate();
			int bufferSize;
			::alGetBufferi(buffer, AL_SIZE, &bufferSize);
			pSecondsElapsed += bufferSize * 8.0f / bits / frequency;
			// Get the next data to feed the buffer
			uint size = fillBuffer();
			if (!size)
				return false;

			// Buffer the data with OpenAL and queue the buffer onto the source
			if (!Private::Media::OpenAL::SetBufferData(buffer, pAStream->alFormat(), pData.data(),
				size, frequency / channels))
				return false;
			if (!Private::Media::OpenAL::QueueBufferToSource(buffer, source))
				return false;
		}

		// Video
		if (hasVideo())
		{
			/*
			if (hasAudio() and Private::Media::OpenAL::IsSourcePlaying(source))
			{
				std::cout << "Video and audio sync !" << std::endl;
				// Try to sync with audio
				ALfloat elapsed;
				::alGetSourcef(source, AL_SEC_OFFSET, &elapsed);
				while (!pFrames.empty() and elapsed > pFrames.front()->timestamp())
				{
					pFrames.pop_front();
					if (pFrames.empty())
						fillQueue();
				}
			}
			*/

			// TEMPORARY
			// The sync code is not working yet, just get some frames when we need them for now
			// if (pFrames.empty())
			// 	fillQueue();

			if (pFrames.empty())
				// Failed to load anymore
				return false;
		}

		return true;
	}


	bool Source::rewindDispatched(uint source)
	{
		for (uint i = 0; i < pBufferCount; ++i)
			Private::Media::OpenAL::UnqueueBufferFromSource(source);
		Private::Media::OpenAL::UnbindBufferFromSource(source);
		Private::Media::OpenAL::DestroyBuffers(pBufferCount, pIDs);
		if (pAStream)
			pAStream->rewind();
		if (pVStream)
			pVStream->rewind();
		return prepareDispatched(source);
	}


	bool Source::destroyDispatched(Thread::Signal* signal)
	{
		if (!pAStream and !pVStream)
			return false;

		pFrames.clear();
		if (pAStream)
		{
			delete pAStream->parent();
			Private::Media::OpenAL::DestroyBuffers(pBufferCount, pIDs);
			pBufferCount = 0;
		}
		else
			delete pVStream->parent();
		pAStream = nullptr;
		pVStream = nullptr;
		if (signal)
			signal->notify();
		return true;
	}


	Private::Media::Frame::Ptr Source::nextFrame()
	{
		assert(hasVideo() && "Source::nextFrame : Cannot get a video frame from an audio-only source !");
		// FIXME : There is probably a race condition here !
		// We need a mutex lock
		if (pFrames.empty())
		{
			return pVStream->nextFrame();
		}
		auto frame = pFrames.front();
		pFrames.pop_front();
		return frame;
	}



} // namespace Media
} // namespace Yuni
