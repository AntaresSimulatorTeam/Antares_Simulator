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
#ifndef __YUNI_PRIVATE_MEDIA_OPENAL_H__
# define __YUNI_PRIVATE_MEDIA_OPENAL_H__

# include "../../yuni.h"
# include <list>
# include "../../core/vector3D.h"
# include "../../core/point3D.h"
# ifdef YUNI_OS_MACOS
#	include <OpenAL/al.h>
#	include <OpenAL/alc.h>
# else
#	include <al.h>
#	include <alc.h>
# endif

namespace Yuni
{
namespace Private
{
namespace Media
{

	/*!
	** \brief OpenAL wrapper
	*/
	class OpenAL final
	{
	public:
		//! \name Enums
		//@{
		enum DistanceModel
		{
			None,
			InverseDistance,
			InverseDistanceClamped,
			LinearDistance,
			LinearDistanceClamped,
			ExponentDistance,
			ExponentDistanceClamped
		};
		//@}


	public:
		/*!
		** \brief Initialize OpenAL device and context
		*/
		static bool Init();

		/*!
		** \brief Close OpenAL context and device
		*/
		static bool Close();

		/*!
		** \brief Convert to an OpenAL format
		** \param bits Number of bits per sample
		** \param channels Number of channels
		** \returns An ALenum containing the format, 0 if none found
		*/
		static ALenum GetFormat(uint bits, uint channels);

		static void SetDistanceModel(DistanceModel model);


		/*!
		** \brief Create an OpenAL buffer
		** \param[out] A pointer to the ID of the created buffer
		** \returns false on error, true otherwise
		*/
		static bool CreateBuffer(uint* buffer);
		/*!
		** \brief Create OpenAL buffers
		** \param[in] nbBuffers Number of buffers to create
		** \param[out] An array of IDs of the created buffers
		** \returns false on error, true otherwise
		*/
		static bool CreateBuffers(int nbBuffers, uint* buffers);

		static void DestroyBuffers(int nbBuffers, uint* buffers);

		static void SetListener(float position[3], float velocity[3], float orientation[6]);

		/*!
		** \brief Create an OpenAL source
		** \returns The source's ID, 0 if an error is encountered.
		*/
		static uint CreateSource(Point3D<> position, Vector3D<> velocity,
			Vector3D<> direction, float pitch, float gain, bool attenuate, bool loop);

		//! Destroy an OpenAL source
		static void DestroySource(uint source);

		//! Play an OpenAL source
		static bool PlaySource(uint source);

		//! Stop an OpenAL source
		static bool StopSource(uint source);

		//! Pause an OpenAL source
		static bool PauseSource(uint source);

		//! Is the source currently playing ?
		static bool IsSourcePlaying(uint source);

		//! Is the source currently playing ?
		static bool IsSourcePaused(uint source);

		//! Modify characteristics of an existing source
		static bool ModifySource(uint source, float pitch, float gain,
			bool attenuate, bool loop);
		//! Move an existing source
		static bool MoveSource(uint source, const Point3D<>& position,
			const Vector3D<>& velocity, const Vector3D<>& direction);

		//! Bind a sound buffer to an OpenAL source
		static bool BindBufferToSource(uint buffer, uint source);

		//! Unbind an OpenAL source from any buffer
		static void UnbindBufferFromSource(uint source);

		//! Queue a sound buffer for playing on a source
		static bool QueueBufferToSource(uint buffer, uint source);

		//! Unqueue a sound buffer from a source
		static uint UnqueueBufferFromSource(uint source);

		//! Get current playback position on a source
		static float SourcePlaybackPosition(uint source);

		//! Set current playback position on a source
		static void SetSourcePlaybackPosition(uint source, float position);

		//! Set data on a sound buffer
		static bool SetBufferData(uint buffer, int format, void* data, size_t count, int rate);

	}; // class OpenAL





} // namespace Media
} // namespace Private
} // namespace Yuni

#endif // __YUNI_PRIVATE_MEDIA_OPENAL_H__
