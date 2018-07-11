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
#ifndef __YUNI_GFX3D_FRAMEBUFFER_HXX__
# define __YUNI_GFX3D_FRAMEBUFFER_HXX__

# include <yuni/private/graphics/opengl/glew/glew.h>
# include <cassert>
# include "glerror.h"
# include "texture.h"

namespace Yuni
{
namespace Gfx3D
{



	inline FrameBuffer::FrameBuffer():
		pID(-1),
		pTexture(nullptr),
		pDepth(-1),
		pSize(1, 1),
		pUsage(fbDraw)
	{}


	inline FrameBuffer::FrameBuffer(uint width, uint height):
		pID(-1),
		pTexture(nullptr),
		pDepth(-1),
		pSize(width, height),
		pUsage(fbDraw)
	{}


	inline FrameBuffer::~FrameBuffer()
	{
		::glDeleteRenderbuffers(1, (uint*)&pDepth);
		::glDeleteFramebuffers(1, (uint*)&pID);
	}


	inline void FrameBuffer::swap()
	{
		assert(pUsage == fbPingPong && "FrameBuffer::swap() has no meaning when usage is not Ping-Pong !");

		// Ping-pong !
		std::swap(pTexture, pBackTexture);
		::glFramebufferTexture2D(pUsage, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pTexture->id(), 0);
	}


	inline uint FrameBuffer::width() const
	{
		return pSize.x;
	}


	inline uint FrameBuffer::height() const
	{
		return pSize.y;
	}


	inline const Texture::Ptr& FrameBuffer::texture() const
	{
		return pTexture;
	}


	inline bool FrameBuffer::valid() const
	{
		return pTexture >= 0 && pDepth >= 0 && pID >= 0;
	}



} // namespace Gfx3D
} // namespace Yuni

#endif // __YUNI_GFX3D_FRAMEBUFFER_HXX__
