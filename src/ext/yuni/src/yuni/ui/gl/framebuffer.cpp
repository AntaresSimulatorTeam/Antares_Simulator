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
# include "framebuffer.h"

namespace Yuni
{
namespace Gfx3D
{

	static GLenum UsageToGLUsage(FrameBuffer::Usage usage)
	{
		switch (usage)
		{
			case FrameBuffer::fbDraw:
			case FrameBuffer::fbPingPong:
				return GL_DRAW_FRAMEBUFFER;
				break;
			case FrameBuffer::fbRead:
				return GL_READ_FRAMEBUFFER;
			case FrameBuffer::fbReadDraw:
				return GL_FRAMEBUFFER;
				break;
		}
		return GL_FRAMEBUFFER;
	}


	bool FrameBuffer::initialize(Usage usage, UI::MultiSampling::Type msType, Texture::DataType type)
	{
		if (not pSize.x || not pSize.y)
			return false;

		pUsage = usage;
		if (UI::MultiSampling::msNone == msType)
		{
			// Screen texture 1
			pTexture = Texture::New(pSize.x, pSize.y, 4 /* RGBA */, type, nullptr, false);
			if (fbPingPong == pUsage)
				// Screen texture 2
				pBackTexture = Texture::New(pSize.x, pSize.y, 4 /* RGBA */, type, nullptr, false);
		}
		else
		{
			uint samples = UI::MultiSampling::Multiplier(msType);
			// Screen texture 1
			pTexture = Texture::NewMS(pSize.x, pSize.y, 4 /* RGBA */, type, samples, nullptr);
			if (fbPingPong == pUsage)
				// Screen texture 2
				pBackTexture = Texture::NewMS(pSize.x, pSize.y, 4 /* RGBA */, type, samples, nullptr);
		}

		auto textureType = UI::MultiSampling::msNone == msType ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
		// Unbind
		::glBindTexture(textureType, 0);

		// Depth buffer
		uint id;
		::glGenRenderbuffers(1, &id);
		::glBindRenderbuffer(GL_RENDERBUFFER, id);
		::glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, pSize.x, pSize.y);
		::glBindRenderbuffer(GL_RENDERBUFFER, 0);
		pDepth = id;

		GLenum frameBufferUsage = UsageToGLUsage(pUsage);

		// Framebuffer to link everything together
		::glGenFramebuffers(1, &id);
		GLTestError("glGenFramebuffers frame buffer creation");
		::glBindFramebuffer(frameBufferUsage, id);
		GLTestError("glBindFramebuffers frame buffer binding");
		::glFramebufferTexture2D(frameBufferUsage, GL_COLOR_ATTACHMENT0, textureType, pTexture->id(), 0);
		GLTestError("glFramebufferTexture2D frame buffer color attachment");
		::glFramebufferRenderbuffer(frameBufferUsage, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pDepth);
		GLTestError("glFramebufferRenderbuffer depth buffer attachment");
		::glBindFramebuffer(frameBufferUsage, 0);
		pID = id;

		GLenum status;
		if ((status = ::glCheckFramebufferStatus(frameBufferUsage)) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Framebuffer failed to load: error " << status << std::endl;
			pTexture = nullptr;
			::glDeleteRenderbuffers(1, (uint*)&pDepth);
			pDepth = -1;
			::glDeleteFramebuffers(1, (uint*)&pID);
			pID = -1;
			return false;
		}

		return true;
	}


	void FrameBuffer::activate() const
	{
		if (not valid())
			return;
		::glBindFramebuffer(UsageToGLUsage(pUsage), pID);
		GLTestError("glBindFrameBuffer() binding");
	}


	void FrameBuffer::deactivate() const
	{
		// Unbind
		::glBindFramebuffer(UsageToGLUsage(pUsage), 0);
		GLTestError("glBindFrameBuffer() unbinding");
	}



	void FrameBuffer::resize(uint width, uint height)
	{
		if (not pSize.x || not pSize.y)
			return;

		// Update the size internally
		pSize(width, height);

		// If the FB is not properly initialized, just update the size internally
		// for future use by initialize(), but do nothing else
		if (not valid())
			return;

		// Resize the texture
		pTexture->resize(pSize.x, pSize.y);
		GLTestError("glTexSubImage2D() texture resizing");
		::glBindRenderbuffer(GL_RENDERBUFFER, pDepth);
		::glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, pSize.x, pSize.y);
		::glBindRenderbuffer(GL_RENDERBUFFER, 0);
		GLTestError("glRenderBufferStorage() resizing");
	}



} // namespace Gfx3D
} // namespace Yuni
