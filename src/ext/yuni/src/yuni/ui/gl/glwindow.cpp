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
#include "../../core/logs.h"
#include "../../private/graphics/opengl/glew/glew.h"
#include <iostream>
#include "glwindow.h"
#include "texture.h"

namespace Yuni
{
namespace UI
{

	bool GLWindow::initialize()
	{
		// Black background
		::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// Depth Buffer setup
		::glClearDepth(1.0f);
		// Enables Depth Testing
		::glEnable(GL_DEPTH_TEST);
		// The type of Depth Testing to do
		::glDepthFunc(GL_LEQUAL);
		// Enable back-face culling
		//::glEnable(GL_CULL_FACE);

		// Display OpenGL version
#ifndef NDEBUG
		Yuni::Logs::Logger<> logs;
		const uint8* version = ::glGetString(GL_VERSION);
		logs.notice() << "OpenGL " << (const char*)version;
#endif

		// Init GLEW
		GLenum error = ::glewInit();
		if (GLEW_OK != error)
		{
#ifndef NDEBUG
			logs.error() << ::glewGetErrorString(error);
#endif
			return false;
		}

		// Wait for the context to be ready before initializing some GL objects in the main window
		RenderWindow::initialize();
		activeView()->initShaders();

		return true;
	}

	void GLWindow::resize(uint width, uint height)
	{
		// Prevent divide by zero, or texture resize to 0
		if (0 == width)
			width = 1;
		if (0 == height)
			height = 1;

		// Reset The Current Viewport
		::glViewport(0, 0, width, height);

		// Select the Projection Matrix
		::glMatrixMode(GL_PROJECTION);
		// Reset the Projection Matrix
		::glLoadIdentity();

		// Calculate the Aspect Ratio of the window
		::gluPerspective(60.0f, (GLfloat)width / (GLfloat)height, 0.01f, 1000.0f);

		// Select the Modelview Matrix
		::glMatrixMode(GL_MODELVIEW);
		// Reset the Modelview Matrix
		::glLoadIdentity();

		RenderWindow::resize(width, height);
	}


	void GLWindow::drawFullWindowQuad(const Gfx3D::Texture::Ptr& texture) const
	{
		auto textureType = multiSampling() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		::glBindTexture(textureType, texture->id());

		::glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT);
		::glDisable(GL_DEPTH_TEST);
		//::glCullFace(GL_FRONT);

		::glMatrixMode(GL_PROJECTION);
		::glPushMatrix();
		::glLoadIdentity();
		::gluOrtho2D(-1.0f, 1.0f, 1.0f, -1.0f);

		::glMatrixMode(GL_MODELVIEW);
		::glPushMatrix();
		::glLoadIdentity();

		::glBegin(GL_QUADS);
		::glTexCoord2f(0.0f, 1.0f);
		::glVertex2f(-1.0f, -1.0f);
		::glTexCoord2f(0.0f, 0.0f);
		::glVertex2f(-1.0f, 1.0f);
		::glTexCoord2f(1.0f, 0.0f);
		::glVertex2f(1.0f, 1.0f);
		::glTexCoord2f(1.0f, 1.0f);
		::glVertex2f(1.0f, -1.0f);
		::glEnd();


		// Texture coordinates are useless when 2D shaders are activated
		// but they are used when no post-processing effect is present
		// const float texCoord[] =
		// 	{
		// 		0.0f, 1.0f,
		// 		0.0f, 0.0f,
		// 		1.0f, 0.0f,
		// 		0.0f, 1.0f,
		// 		1.0f, 0.0f,
		// 		1.0f, 1.0f
		// 	};
		// ::glEnableVertexAttribArray(Gfx3D::Vertex<>::vaTextureCoord);
		// ::glVertexAttribPointer(Gfx3D::Vertex<>::vaTextureCoord, 2, GL_FLOAT, 0, 0, texCoord);
		// Set vertex positions
		/*const float vertices[] =
			{
				-1.0f, 1.0f,
				-1.0f, -1.0f,
				1.0f, -1.0f,
				-1.0f, 1.0f,
				1.0f, -1.0f,
				1.0f, 1.0f
			};
		::glEnableVertexAttribArray(Gfx3D::Vertex<>::vaPosition);
		::glVertexAttribPointer(Gfx3D::Vertex<>::vaPosition, 2, GL_FLOAT, 0, 0, vertices);
		// Draw
		::glDrawArrays(GL_TRIANGLES, 0, 6);

		::glDisableVertexAttribArray(Gfx3D::Vertex<>::vaPosition);
		::glDisableVertexAttribArray(Gfx3D::Vertex<>::vaTextureCoord);
		*/
		::glMatrixMode(GL_PROJECTION);
		::glPopMatrix();
		::glMatrixMode(GL_MODELVIEW);
		::glPopMatrix();

		::glPopAttrib();
		//::glCullFace(GL_BACK);

		::glBindTexture(textureType, 0);
	}


	void GLWindow::clear() const
	{
		pFB.activate();
		::glClear(GL_COLOR_BUFFER_BIT);
		pFB.deactivate();
	}

	void GLWindow::clear(const Color::RGB<>& color) const
	{
		pFB.activate();
		::glClearColor(color.red, color.green, color.blue, 1.0f);
		::glClear(GL_COLOR_BUFFER_BIT);
		pFB.deactivate();
	}


	void GLWindow::clear(float red, float green, float blue) const
	{
		pFB.activate();
		::glClearColor(red, green, blue, 1.0f);
		::glClear(GL_COLOR_BUFFER_BIT);
		pFB.deactivate();
	}

	void GLWindow::clear(uint red, uint green, uint blue) const
	{
		pFB.activate();
		::glClearColor((float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f, 1.0f);
		::glClear(GL_COLOR_BUFFER_BIT);
		pFB.deactivate();
	}


	void GLWindow::clearRect(int x, int y, uint width, uint height) const
	{
		pFB.activate();
		// We offer coordinates top-left to bottom-right
		// but GL coordinates are bottom-left to top-right, so convert Y
		::glEnable(GL_SCISSOR_TEST);
		::glScissor(x, pHeight - height - y, width, height);
		::glClear(GL_COLOR_BUFFER_BIT);
		::glDisable(GL_SCISSOR_TEST);
		pFB.deactivate();
	}



} // namespace UI
} // namespace Yuni
