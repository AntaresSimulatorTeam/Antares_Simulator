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
#ifndef __YUNI_AUDIO_PLAYLIST_H__
# define __YUNI_AUDIO_PLAYLIST_H__

# include <vector>
# include "sound.h"

namespace Yuni
{
namespace Audio
{



	/*!
	** \brief A play list contains an ordered list of sounds to be played
	*/
	class PlayList: public Policy::ObjectLevelLockable<PlayList>
	{
	public:
		//! \name Typedefs
		//@{

		//! The most suitable smart pointer for the class
		typedef SmartPtr<PlayList> Ptr;
		//! Size type
		typedef uint Size;
		//! The Threading Policy
		typedef Policy::ObjectLevelLockable<ISound> ThreadingPolicy;

		//@}

	public:
		//! Empty constructor
		PlayList()
			: pCurrentIndex(0), pLoop(false)
		{}

	public:
		Size size() const { return pSoundList.size(); }

		/*!
		** \brief Add a sound at the end of the playlist
		*/
		void append(SmartPtr<ISound>& sound);

		template<typename StringT>
		void remove(const StringT& soundID);

		void play() const;
		void pause(bool paused);

		void previous();
		void next();
		void shuffle();

	private:
		//! All the sounds currently in the play list (ordered)
		ISound::Vector pSoundList;
		//! Index of the current song being played / next to play
		uint pCurrentIndex;
		//! Should we loop around the selection?
		bool pLoop;

	}; // class PlayList



} // namespace Audio
} // namespace Yuni

#endif  // __YUNI_AUDIO_PLAYLIST_H__
