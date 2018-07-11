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
#include "view.h"
#include "shadermanager.h"
#include "texture.h"
#include "shaders.h"

namespace Yuni
{
namespace UI
{


	View::View(float x, float y, float w, float h, uint8 z, bool visible) :
		pID(UUID::fGenerate),
		pX(x),
		pY(y),
		pWidth(w),
		pHeight(Math::Max(h, 1.0f)), // Forbid null height to avoid divide by 0
		pZ(z),
		pVisible(visible),
		pTextShaders(nullptr),
		pPictureShaders(nullptr)
	{}


	View::~View()
	{}


	bool View::initShaders()
	{
		// Shaders are created here temporarily, because creation in the constructor requires
		// a GL context that the first created view does not have yet
		if (!pTextShaders and !pPictureShaders)
		{
			auto& shaderManager = Gfx3D::ShaderManager::Instance();
			pTextShaders = shaderManager.getFromMemory(Gfx3D::vsTexCoord, Gfx3D::fsText);
			assert(pTextShaders && "Failed to load necessary shaders for text overlay !");
			// Bind attributes before linking
			pTextShaders->bindAttribute("attrVertex", Gfx3D::Vertex<>::vaPosition);
			pTextShaders->bindAttribute("attrTexCoord", Gfx3D::Vertex<>::vaTextureCoord);
			if (!pTextShaders->load())
			{
				std::cerr << pTextShaders->errorMessage();
				return false;
			}
			pTextShaders->activate();
			pTextShaders->deactivate();

			pPictureShaders = shaderManager.getFromMemory(Gfx3D::vsImageRect, Gfx3D::fsImageRect);
			assert(pPictureShaders && "Failed to load necessary shaders for picture overlay !");
			// Bind attributes before linking
			pPictureShaders->bindAttribute("attrVertex", Gfx3D::Vertex<>::vaPosition);
			pPictureShaders->bindAttribute("attrTexCoord", Gfx3D::Vertex<>::vaTextureCoord);
			if (!pPictureShaders->load())
			{
				std::cerr << pPictureShaders->errorMessage();
				return false;
			}
			pPictureShaders->activate();
			pPictureShaders->bindUniform("Texture0", Gfx3D::Vertex<>::vaTexture0);
			pPictureShaders->deactivate();
		}
		return true;
	}


	void View::draw(uint /*msMultiplier*/) const
	{
		if (!pVisible)
			return;

		// Clear depth buffer
		::glClear(GL_DEPTH_BUFFER_BIT);

		::glPushAttrib(GL_VIEWPORT_BIT);

		// Reset The Current Viewport
		::glViewport((int)pX, (int)pY, (uint)pWidth, (uint)pHeight);

		draw2D();

		::glPopAttrib();
	}


	void View::draw2D() const
	{
		// Save current states
		::glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT);
		// Disable depth test to avoid having the overlay hidden
		::glDisable(GL_DEPTH_TEST);

		// Set view matrices for 2D overlay display
		::glMatrixMode(GL_PROJECTION);
		::glPushMatrix();
		::glLoadIdentity();
		// Window coordinates : [0,0] at top-left, [width,height] at bottom-right
		::gluOrtho2D(0.0f, pWidth, pHeight, 0.0f);

		::glMatrixMode(GL_MODELVIEW);
		::glPushMatrix();
		::glLoadIdentity();

		// Alpha blending
		::glEnable(GL_BLEND);
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Draw the overlays
		for (auto picOverlay : pPictures)
			drawOverlay(*picOverlay);

		for (auto textOverlay : pTexts)
			drawOverlay(*textOverlay);

		// Render the control tree
		if (!(!pControl) and pControl->visible())
		{
			if (!pUISurface)
			{
				// Avoid texture creation with a 0 dimension
				pUISurface = new DrawingSurface(Math::Max(pControl->width(), 1.0f),
					Math::Max(pControl->height(), 1.0f));
				pUISurface->begin();
				pControl->draw(pUISurface);
				pUISurface->commit();
			}
			else if (pControl->modified())
			{
				pUISurface->resize(Math::Max(pControl->width(), 1.0f),
					Math::Max(pControl->height(), 1.0f));
				pUISurface->begin();
				pControl->draw(pUISurface);
				pUISurface->commit();
			}

			if (pPictureShaders and pPictureShaders->valid())
			{
				drawPicture(pUISurface->texture(), pControl->x(), pControl->y(),
							pControl->width(), pControl->height(), true, true);
			}
		}

		// Reset matrices
		::glMatrixMode(GL_PROJECTION);
		::glPopMatrix();
		::glMatrixMode(GL_MODELVIEW);
		::glPopMatrix();

		// Restore enable settings
		::glPopAttrib();
	}


	void View::drawOverlay(TextOverlay& text) const
	{
		// Update the data if necessary
		text.update();

		// Draw the texture
		text.draw(pTextShaders);
	}


	void View::drawOverlay(const PictureOverlay& picture) const
	{
		picture.draw(pPictureShaders);
	}


	void View::drawPicture(const Gfx3D::Texture::Ptr& texture, float x, float y, float width,
		float height, bool flip, bool invert) const
	{
		if (!texture)
			return;

		pPictureShaders->activate();
		::glActiveTexture(GL_TEXTURE0);
		// Bind the texture
		::glBindTexture(GL_TEXTURE_2D, texture->id());
		pPictureShaders->bindUniform("Texture0", Yuni::Gfx3D::Vertex<>::vaTexture0);
		pPictureShaders->bindUniform("FillColor", Color::RGBA<float>(0.0f, 0.0f, 0.0f, 0.0f));
		pPictureShaders->bindUniform("Bounds", x, y, x + width, y + height);

		// Set texture coordinates
		::glEnableVertexAttribArray(Gfx3D::Vertex<>::vaTextureCoord);
		float texCoord[] =
			{
				0.0f, 1.0f,
				0.0f, 0.0f,
				1.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 0.0f,
				1.0f, 1.0f
			};
		if (flip)
		{
			for (uint i = 1; i < 12; i += 2)
				texCoord[i] = -1.0f * texCoord[i] + 1.0f; // Switch 1s and 0s
		}
		if (invert)
		{
			std::swap(texCoord[2], texCoord[4]);
			std::swap(texCoord[3], texCoord[5]);
			std::swap(texCoord[8], texCoord[10]);
			std::swap(texCoord[9], texCoord[11]);
		}
		::glVertexAttribPointer(Gfx3D::Vertex<>::vaTextureCoord, 2, GL_FLOAT, 0, 0, texCoord);

		// Set vertices
		::glEnableVertexAttribArray(Gfx3D::Vertex<>::vaPosition);
		float vertices[] =
			{
				x, y + height,
				x, y,
				x + width, y,
				x, y + height,
				x + width, y,
				x + width, y + height
			};
		if (invert)
		{
			std::swap(vertices[2], vertices[4]);
			std::swap(vertices[3], vertices[5]);
			std::swap(vertices[8], vertices[10]);
			std::swap(vertices[9], vertices[11]);
		}
		::glVertexAttribPointer(Gfx3D::Vertex<>::vaPosition, 2, GL_FLOAT, 0, 0, vertices);

		// Draw
		::glDrawArrays(GL_TRIANGLES, 0, 6);
		// Clean up
		::glDisableVertexAttribArray(Gfx3D::Vertex<>::vaPosition);
		::glDisableVertexAttribArray(Gfx3D::Vertex<>::vaTextureCoord);

		// Unbind the texture
		::glBindTexture(GL_TEXTURE_2D, 0);
		pPictureShaders->deactivate();
	}


	IControl* View::getControlAt(float x, float y)
	{
		if (!pControl)
			return nullptr;
		return pControl->getControlAt(x, y);
	}


	EventPropagation View::doMouseMove(int x, int y)
	{
		EventPropagation propagate = epContinue;
		// If there are UI controls in this view
		if (!(!pControl))
			// Dispatch the event
			propagate = pControl->doMouseMove((float)x, (float)y, pEnteredControls);
		return propagate;
	}


	EventPropagation View::doMouseDown(Input::IMouse::Button btn, int x, int y)
	{
		EventPropagation propagate = epContinue;
		// If there are UI controls in this view
		if (!(!pControl))
			// Dispatch the event
			propagate = pControl->doMouseDown(btn, (float)x, (float)y);
		return propagate;
	}


	EventPropagation View::doMouseUp(Input::IMouse::Button btn, int x, int y)
	{
		EventPropagation propagate = epContinue;
		// If there are UI controls in this view
		if (!(!pControl))
			// Dispatch the event
			propagate = pControl->doMouseUp(btn, (float)x, (float)y);
		return propagate;
	}


	EventPropagation View::doMouseDblClick(Input::IMouse::Button btn, int x, int y)
	{
		EventPropagation propagate = epContinue;
		// If there are UI controls in this view
		if (!(!pControl))
			// Dispatch the event
			propagate = pControl->doMouseDblClick(btn, (float)x, (float)y);
		return propagate;
	}


	EventPropagation View::doMouseScroll(float delta, int x, int y)
	{
		EventPropagation propagate = epContinue;
		// If there are UI controls in this view
		if (!(!pControl))
			// Dispatch the event
			propagate = pControl->doMouseScroll(delta, (float)x, (float)y);
		return propagate;
	}


	EventPropagation View::doMouseHover(int x, int y)
	{
		EventPropagation propagate = epContinue;
		// If there are UI controls in this view
		if (!(!pControl))
			// Dispatch the event
			propagate = pControl->doMouseHover((float)x, (float)y);
		return propagate;
	}


	EventPropagation View::doMouseLeave()
	{
		EventPropagation propagate = epContinue;
		// If there are UI controls in this view
		if (!(!pControl))
			// Dispatch the event
			propagate = pControl->doMouseLeave();
		return propagate;
	}


	EventPropagation View::doKeyDown(Input::Key key, int x, int y)
	{
		EventPropagation propagate = epContinue;
		// If there are UI controls in this view
		if (!(!pControl))
			// Dispatch the event
			propagate = pControl->doKeyDown(key, (float)x, (float)y);
		return propagate;
	}


	EventPropagation View::doKeyUp(Input::Key key, int x, int y)
	{
		EventPropagation propagate = epContinue;
		// If there are UI controls in this view
		if (!(!pControl))
			// Dispatch the event
			propagate = pControl->doKeyUp(key, (float)x, (float)y);
		return propagate;
	}


	EventPropagation View::doCharInput(const AnyString& str, int x, int y)
	{
		EventPropagation propagate = epContinue;
		// If there are UI controls in this view
		if (!(!pControl))
			// Dispatch the event
			propagate = pControl->doCharInput(str, (float)x, (float)y);
		return propagate;
	}




} // namespace UI
} // namespace Yuni
