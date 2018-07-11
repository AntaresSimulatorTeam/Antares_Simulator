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
# include <cstdlib>
# include <iostream>
# include "openal.h"

namespace Yuni
{
namespace Private
{
namespace Media
{

	bool OpenAL::Init()
	{
		ALCdevice* device = ::alcOpenDevice(nullptr);
		if (!device)
			return false;

		ALCcontext* context = ::alcCreateContext(device, nullptr);
		if (!context)
			return false;

		::alcMakeContextCurrent(context);

		// Set the listener at (0,0,0)
		ALfloat listenerPos[3] = {0.0,0.0,0.0};
		// Set the listener's velocity to 0
		ALfloat listenerVel[3] = {0.0,0.0,0.0};
		// Listener is oriented towards the screen (-Z), with the Up along Y
		ALfloat listenerOri[6] = {0.0,0.0,-1.0, 0.0,1.0,0.0};
		SetListener(listenerPos, listenerVel, listenerOri);

		// Clear errors
		::alGetError();
		return true;
	}


	bool OpenAL::Close()
	{
		ALCcontext* context = ::alcGetCurrentContext();
		ALCdevice* device   = ::alcGetContextsDevice(context);

		::alcMakeContextCurrent(nullptr);
		::alcDestroyContext(context);
		::alcCloseDevice(device);
		#ifndef NDEBUG
		std::cout << "Finished closing OpenAL." << std::endl;
		#endif
		return true;
	}


	ALenum OpenAL::GetFormat(uint bits, uint channels)
	{
		switch (bits)
		{
			case 16:
			{
				if (channels == 1)
					return AL_FORMAT_MONO16;
				if (channels == 2)
					return AL_FORMAT_STEREO16;

				if (::alIsExtensionPresent("AL_EXT_MCFORMATS"))
				{
					if (channels == 4)
						return alGetEnumValue("AL_FORMAT_QUAD16");
					if (channels == 6)
						return alGetEnumValue("AL_FORMAT_51CHN16");
				}
				break;
			}
			case 8:
			{
				if (channels == 1)
					return AL_FORMAT_MONO8;
				if (channels == 2)
					return AL_FORMAT_STEREO8;

				if (::alIsExtensionPresent("AL_EXT_MCFORMATS"))
				{
					if (channels == 4)
						return ::alGetEnumValue("AL_FORMAT_QUAD8");
					if (channels == 6)
						return ::alGetEnumValue("AL_FORMAT_51CHN8");
				}
				break;
			}
		}
		return 0;
	}


	void OpenAL::SetDistanceModel(DistanceModel model)
	{
		ALenum modelName;
		switch (model)
		{
			case None:
				modelName = AL_NONE;
				break;
			case InverseDistance:
				modelName = AL_INVERSE_DISTANCE;
				break;
			case InverseDistanceClamped:
				modelName = AL_INVERSE_DISTANCE_CLAMPED;
				break;
			case LinearDistance:
				modelName = AL_LINEAR_DISTANCE;
				break;
			case LinearDistanceClamped:
				modelName = AL_LINEAR_DISTANCE_CLAMPED;
				break;
			case ExponentDistance:
				modelName = AL_EXPONENT_DISTANCE;
				break;
			case ExponentDistanceClamped:
				modelName = AL_EXPONENT_DISTANCE_CLAMPED;
				break;
			default:
				modelName = AL_INVERSE_DISTANCE_CLAMPED;
		}

		::alDistanceModel(modelName);
	}


	bool OpenAL::CreateBuffer(uint* buffer)
	{
		::alGetError();
		::alGenBuffers(1, buffer);
		return alGetError() == AL_NO_ERROR;
	}


	bool OpenAL::CreateBuffers(int nbBuffers, uint* buffers)
	{
		::alGetError();
		::alGenBuffers(nbBuffers, buffers);
		return alGetError() == AL_NO_ERROR;
	}


	void OpenAL::DestroyBuffers(int nbBuffers, uint* buffers)
	{
		::alDeleteBuffers(nbBuffers, buffers);
	}


	void OpenAL::SetListener(float position[3], float velocity[3], float orientation[6])
	{
		::alListenerfv(AL_POSITION, position);
		::alListenerfv(AL_VELOCITY, velocity);
		::alListenerfv(AL_ORIENTATION, orientation);
	}


	uint OpenAL::CreateSource(Point3D<> position, Vector3D<> velocity,
		Vector3D<> direction, float pitch, float gain, bool attenuate, bool loop)
	{
		::alGetError();
		uint source;
		::alGenSources(1, &source);
		if (::alGetError() != AL_NO_ERROR)
			return 0;

		UnbindBufferFromSource(source);

		::alSourcef(source, AL_MIN_GAIN, 0.0f); // Allow the sound to fade to nothing
		::alSourcef(source, AL_MAX_GAIN, 1.5f); // Max amplification
		::alSourcef(source, AL_MAX_DISTANCE, 10000.0f);

		if (not MoveSource(source, position, velocity, direction)
			and not ModifySource(source, pitch, gain, attenuate, loop))
		{
			DestroySource(source);
			return 0;
		}

		return source;
	}


	void OpenAL::DestroySource(uint source)
	{
		::alDeleteSources(1, &source);
	}


	bool OpenAL::PlaySource(ALuint source)
	{
		::alGetError();
		::alSourcePlay(source);
		return AL_NO_ERROR == alGetError();
	}


	bool OpenAL::PauseSource(ALuint source)
	{
		::alGetError();
		::alSourcePause(source);
		return AL_NO_ERROR == ::alGetError();
	}


	bool OpenAL::StopSource(uint source)
	{
		::alGetError();
		::alSourceStop(source);
		return AL_NO_ERROR == ::alGetError();
	}


	bool OpenAL::IsSourcePlaying(uint source)
	{
		::alGetError();
		ALint state;
		::alGetSourcei(source, AL_SOURCE_STATE, &state);
		return AL_NO_ERROR == alGetError() and AL_PLAYING == state;
	}


	bool OpenAL::IsSourcePaused(uint source)
	{
		ALint state;
		::alGetSourcei(source, AL_SOURCE_STATE, &state);
		return AL_NO_ERROR == ::alGetError() and AL_PAUSED == state;
	}


	bool OpenAL::ModifySource(uint source, float pitch, float gain,
		bool attenuate, bool loop)
	{
		::alGetError();
		::alSourcef(source, AL_PITCH, pitch);
		::alSourcef(source, AL_GAIN, gain);
		::alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
		::alSourcef(source, AL_ROLLOFF_FACTOR, attenuate ? 1.0f : 0.0f);
		return ::alGetError() == AL_NO_ERROR;
	}


	bool OpenAL::MoveSource(uint source, const Point3D<>& position,
		const Vector3D<>& velocity, const Vector3D<>& direction)
	{
		// Uncomment this if you want the position / velocity / cone /
		// direction properties to be relative to listener position:
		//alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);

		float pos[3] = { position.x, position.y, position.z};
		float vel[3] = { velocity.x, velocity.y, velocity.z};
		float dir[3] = { direction.x, direction.y, direction.z};

		::alGetError();
		::alSourcefv(source, AL_POSITION, pos);
		::alSourcefv(source, AL_VELOCITY, vel);
		::alSourcefv(source, AL_DIRECTION, dir);
		return ::alGetError() == AL_NO_ERROR;
	}


	bool OpenAL::BindBufferToSource(uint buffer, uint source)
	{
		::alGetError();
		::alSourcei(source, AL_BUFFER, (int)buffer);
		return ::alGetError() == AL_NO_ERROR;
	}


	void OpenAL::UnbindBufferFromSource(uint source)
	{
		::alSourcei(source, AL_BUFFER, 0);
	}


	bool OpenAL::QueueBufferToSource(uint buffer, uint source)
	{
		::alGetError();
		::alSourceQueueBuffers(source, 1, &buffer);
		return ::alGetError() == AL_NO_ERROR;
	}


	uint OpenAL::UnqueueBufferFromSource(uint source)
	{
		uint buf;
		::alSourceUnqueueBuffers(source, 1, &buf);
		return buf;
	}


	float OpenAL::SourcePlaybackPosition(uint source)
	{
		float pos = 0;
		::alGetSourcef(source, AL_SEC_OFFSET, &pos);
		return pos / 60.0f; // Normalize the time
	}


	void OpenAL::SetSourcePlaybackPosition(uint source, float position)
	{
		::alSourcef(source, AL_SEC_OFFSET, position * 60.0f);
	}


	bool OpenAL::SetBufferData(uint buffer, int format, void* data, size_t count, int rate)
	{
		::alGetError();
		::alBufferData(buffer, format, data, static_cast<ALsizei>(count), rate);
		return ::alGetError() == AL_NO_ERROR;
	}





} // namespace Media
} // namespace Private
} // namespace Yuni
