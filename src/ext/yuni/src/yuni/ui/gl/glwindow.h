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
#ifndef __YUNI_UI_GLWINDOW_H__
# define __YUNI_UI_GLWINDOW_H__

# include "../../yuni.h"
# include "../renderwindow.h"


namespace Yuni
{
namespace UI
{

	/*!
	** \brief Window for OpenGL rendering
	*/
	class GLWindow: public RenderWindow
	{
	public:
		//! Constructor
		GLWindow(const AnyString& title, unsigned int width, unsigned int height, unsigned int bitDepth, bool fullScreen):
			RenderWindow(title, width, height, bitDepth, fullScreen)
		{}
		//! Virtual destructor
		virtual ~GLWindow() {}

	protected:
		/*!
		** \brief Initalize OpenGL
		**
		** This ought to be called from overriden implementations
		** (i.e. GLWindow::initialize(); )
		*/
		virtual bool initialize() override;

		/*!
		** \brief For GL, implementation does nothing
		**
		** This ought to be called from overriden implementations
		** (i.e. GLWindow::kill(); )
		*/
		virtual void kill() override
		{
			::glFlush();
			RenderWindow::kill();
		}

		//! Resize the GL window
		virtual void resize(unsigned int width, unsigned int height) override;

		//! Clear the rendering area to black
		virtual void clear() const override;

		//! Clear the rendering area to black
		virtual void clear(const Color::RGB<>& color) const override;

		//! Clear the rendering area to a given color using floats (0.0f -> 1.0f)
		virtual void clear(float red, float green, float blue) const override;

		//! Clear the rendering area to a given color using integers (0 -> 255)
		virtual void clear(uint red, uint green, uint blue) const override;

		//! Clear a rectangle in the rendering area to black
		virtual void clearRect(int x, int y, uint width, uint height) const override;

		//! Draw a textured quad to the whole window
		virtual void drawFullWindowQuad(const Gfx3D::Texture::Ptr& texture) const override;
	};





} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_GLWINDOW_H__
