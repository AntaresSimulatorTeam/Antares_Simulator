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
# define __YUNI_PRIVATE_MEDIA_FILE_H__

# include "../../yuni.h"
# include "../../core/smartptr.h"
# include "../../core/string.h"
# include "stream.h"
# include <cassert>


namespace Yuni
{
namespace Private
{
namespace Media
{

	/*!
	** \brief A media file contains one or several video and/or audio streams
	*/
	class File final
	{
	public:
		//! Smart pointer
		typedef SmartPtr<File>  Ptr;

		# if LIBAVFORMAT_VERSION_MAJOR < 53
		typedef enum CodecType  MediaType;
		# else
		typedef enum AVMediaType  MediaType;
		# endif // LIBAVFORMAT_VERSION_MAJOR < 53

	public:
		//! Constructor
		File(const AnyString& filePath);
		//! Destructor
		~File();

		//! Get a video or audio stream, add it if necessary
		template<StreamType TypeT>
		typename Stream<TypeT>::Ptr getStream(uint index = 0);

		//! Get next packet for a given stream
		template<StreamType TypeT>
		AVPacket* getNextPacket(Stream<TypeT>* stream);

		//! Get file duration
		uint duration() const;

		//! Is the file valid for reading ?
		bool valid() const;

		//! Rewind the file
		void rewind();


	private:
		//! Add a stream of the given media type at given index
		template<StreamType TypeT>
		typename Stream<TypeT>::Ptr addStream(uint index = 0);

		//! Get the proper cache depending on the media type (audio or video)
		template<StreamType TypeT>
		typename Stream<TypeT>::Map& getCache();


	private:
		//! File format and streams
		AVFormatContext* pFormat;

		//! Video stream cache
		mutable Stream<stVideo>::Map pVStreams;
		//! Audio stream cache
		mutable Stream<stAudio>::Map pAStreams;

		//! Friend declaration
		template<StreamType> friend class Stream;

	}; // class File





} // namespace Media
} // namespace Private
} // namespace Yuni

# include "stream.hxx"
# include "file.hxx"

#endif // __YUNI_PRIVATE_MEDIA_FILE_H__
