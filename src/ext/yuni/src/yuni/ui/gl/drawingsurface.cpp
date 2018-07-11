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
# include "drawingsurface.h"
# include "../../core/math.h"
# include "../../io/file.h"
# include "../../private/graphics/opengl/glew/glew.h"
# include "framebuffer.h"
# include "shadermanager.h"
# include "shaders.h"
# include "linerenderer.h"
# include "../textoverlay.h"
# include <vector>

namespace Yuni
{
namespace UI
{


	/*!
	** \brief Implementation details for a DrawingSurface
	*/
	class DrawingSurfaceImpl
	{
	public:
		//! Clipping coordinates
		typedef std::pair<Point2D<float>, Point2D<float> > ClipCoord;

		//! Stack type to store clipping coordinates
		typedef std::vector<ClipCoord>  ClipStack;

	public:
		DrawingSurfaceImpl(float width, float height);
		~DrawingSurfaceImpl();

	public:
		//! Surface size
		Point2D<float> size;

		//! Is the surface locked for drawing ?
		bool locked;

		//! Stack containing the current clipping regions
		ClipStack clippings;

		//! Frame buffer to render to
		Gfx3D::FrameBuffer fb;

		//! Store previous frame-buffer value to restore it after drawing
		GLint previousFB;

		//! Shader program used to draw lines in the drawing surface
		Gfx3D::ShaderProgram::Ptr lineShader;

		//! Shader program used to draw everything else in the drawing surface
		Gfx3D::ShaderProgram::Ptr baseShader;

		//! Shader program used to draw text
		Gfx3D::ShaderProgram::Ptr textShader;

		//! Shader program used to draw images
		Gfx3D::ShaderProgram::Ptr pictureShader;

		//! A texture overlay used for text rendering, reused for all kinds of text
		TextOverlay text;

	}; // class DrawingSurfaceImpl



	DrawingSurfaceImpl::DrawingSurfaceImpl(float width, float height):
		size(width, height),
		locked(false),
		fb(Math::Max((uint)Math::Ceil(width), 0u), Math::Max((uint)Math::Ceil(height), 0u))
	{
		fb.initialize(Gfx3D::FrameBuffer::fbDraw);
		auto& shaderManager = Gfx3D::ShaderManager::Instance();

		baseShader = shaderManager.getFromMemory(Gfx3D::vsTransform, Gfx3D::fsColorUniform);
		assert(baseShader && "Shader loading or compilation for UI drawing failed ! ");
		baseShader->bindAttribute("attrVertex", Yuni::Gfx3D::Vertex<>::vaPosition);
		if (!baseShader->load())
		{
			std::cerr << "Shader program link for UI drawing failed !" << std::endl
					  << baseShader->errorMessage() << std::endl;
			baseShader = nullptr;
		}

		lineShader = shaderManager.getFromMemory(Gfx3D::vsColorAttr, Gfx3D::fsColorAttr);
		assert(lineShader && "Shader loading or compilation for line drawing failed !");
		lineShader->bindAttribute("attrVertex", Gfx3D::Vertex<>::vaPosition);
		lineShader->bindAttribute("attrColor", Gfx3D::Vertex<>::vaColor);
		if (!lineShader->load())
		{
			std::cerr << "Shader program link for line drawing failed !" << std::endl
					  << lineShader->errorMessage() << std::endl;
			lineShader = nullptr;
		}

		textShader = shaderManager.getFromMemory(Gfx3D::vsTexCoord, Gfx3D::fsText);
		assert(textShader && "Shader loading or compilation for text drawing failed !");
		textShader->bindAttribute("attrVertex", Gfx3D::Vertex<>::vaPosition);
		textShader->bindAttribute("attrColor", Gfx3D::Vertex<>::vaColor);
		if (!textShader->load())
		{
			std::cerr << "Shader program link for text drawing failed !" << std::endl
					  << textShader->errorMessage() << std::endl;
			textShader = nullptr;
		}
		textShader->activate();
		textShader->bindUniform("Texture0", Yuni::Gfx3D::Vertex<>::vaTexture0);
		textShader->deactivate();

		pictureShader = shaderManager.getFromMemory(Gfx3D::vsImageRect, Gfx3D::fsImageRect);
		assert(pictureShader && "Failed to load necessary shaders for picture overlay !");
		pictureShader->bindAttribute("attrVertex", Gfx3D::Vertex<>::vaPosition);
		pictureShader->bindAttribute("attrTexCoord", Gfx3D::Vertex<>::vaTextureCoord);
		if (!pictureShader->load())
		{
			std::cerr << "Shader program link for picture drawing failed !" << std::endl
					  << pictureShader->errorMessage() << std::endl;
			pictureShader = nullptr;
		}
		pictureShader->activate();
		pictureShader->bindUniform("Texture0", Yuni::Gfx3D::Vertex<>::vaTexture0);
		pictureShader->deactivate();

	}


	DrawingSurfaceImpl::~DrawingSurfaceImpl()
	{
	}



	///////////////////// DrawingSurface


	DrawingSurface::DrawingSurface(float width, float height):
		pImpl(new DrawingSurfaceImpl(width, height))
	{
	}


	DrawingSurface::~DrawingSurface()
	{
		delete pImpl;
	}


	void DrawingSurface::begin()
	{
		assert(!pImpl->locked && "DrawingSurface error : Cannot begin drawing on a locked surface !");
		assert(pImpl->baseShader->valid() && "Shaders were not properly loaded, cannot continue !");
		assert(pImpl->lineShader->valid() && "Shaders were not properly loaded, cannot continue !");

		pImpl->locked = true;

		// Set view matrices for 2D overlay display
		::glMatrixMode(GL_PROJECTION);
		::glPushMatrix();
		::glLoadIdentity();
		// Window coordinates : [0,0] at top-left, [width,height] at bottom-right
		::gluOrtho2D(0.0f, pImpl->size.x, pImpl->size.y, 0.0f);

		::glMatrixMode(GL_MODELVIEW);
		::glPushMatrix();
		::glLoadIdentity();

		// Store the previous framebuffer value
		::glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &pImpl->previousFB);
		// Bind framebuffer
		pImpl->fb.activate();
		// Bind shaders
		pImpl->baseShader->activate();

		// Clip on drawing surface
		::glPushAttrib(GL_ENABLE_BIT | GL_VIEWPORT_BIT);
		::glEnable(GL_SCISSOR_TEST);
		::glEnable(GL_BLEND);

		::glViewport(0, 0, (uint)Math::Ceil(pImpl->size.x), (uint)Math::Ceil(pImpl->size.y));

		clear();
	}


	void DrawingSurface::commit()
	{
		assert(pImpl->locked && "DrawingSurface error : Cannot commit on an unlocked surface ! (try calling the begin() function)");
		assert(pImpl->clippings.empty() && "DrawingSurface commit : Too few endClipping() calls, stack is not empty !");
		pImpl->locked = false;

		::glMatrixMode(GL_PROJECTION);
		::glPopMatrix();
		::glMatrixMode(GL_MODELVIEW);
		::glPopMatrix();
		::glPopAttrib();
		pImpl->baseShader->deactivate();
		pImpl->fb.deactivate();
		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pImpl->previousFB);
	}


	void DrawingSurface::rollback()
	{
		assert(pImpl->locked && "DrawingSurface error : Cannot rollback on an unlocked surface ! (try calling the begin() function)");
		pImpl->clippings.clear();
		pImpl->locked = false;

		::glMatrixMode(GL_PROJECTION);
		::glPopMatrix();
		::glMatrixMode(GL_MODELVIEW);
		::glPopMatrix();
		::glPopAttrib();
		pImpl->fb.deactivate();
	}


	void DrawingSurface::clear()
	{
		assert(pImpl->locked && "DrawingSurface error : Cannot clear an unlocked surface !");
		// Clear to full transparent
		::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	void DrawingSurface::resize(float width, float height)
	{
		auto& size = pImpl->size;
		if (width <= 0 || height <= 0 ||
			(Math::Equals(width, size.x) && Math::Equals(height, size.y)))
			return;
		size(width, height);
		pImpl->fb.resize((uint)Math::Ceil(width), (uint)Math::Ceil(height));
	}


	void DrawingSurface::drawText(const String& text, const FTFont::Ptr& font,
		const Color::RGBA<float>& color, float x, float y, uint tabWidth, bool drawOnBaseline)
	{
		assert(pImpl->locked && "DrawingSurface error : Cannot draw to an unlocked surface !");

		if (text.empty())
			return;

		auto& overlay = pImpl->text;
		overlay.clear() << text;
		overlay.font(font);
		overlay.color(color);
		overlay.move(x, y);
		overlay.tabWidth(tabWidth);
		overlay.drawOnBaseline(drawOnBaseline);

		// Update
		overlay.update();
		if (Math::Zero(overlay.width()) || Math::Zero(overlay.height()))
			return;
		// Draw
		overlay.draw(pImpl->textShader);
		// Restore base shader
		pImpl->baseShader->activate();
	}

	void DrawingSurface::drawTextOnColor(const String& text, const FTFont::Ptr& font,
		const Color::RGBA<float>& color, const Color::RGBA<float>& backColor,
		float x, float y, uint tabWidth)
	{
		assert(pImpl->locked && "DrawingSurface error : Cannot draw to an unlocked surface !");

		if (text.empty())
			return;

		auto& overlay = pImpl->text;
		overlay.clear() << text;
		overlay.font(font);
		overlay.color(color);
		overlay.move(x, y);
		overlay.tabWidth(tabWidth);

		// Update
		overlay.update();
		if (Math::Zero(overlay.width()) || Math::Zero(overlay.height()))
			return;
		// Draw
		overlay.draw(pImpl->textShader, backColor);
		// Restore base shader
		pImpl->baseShader->activate();
	}


	void DrawingSurface::drawTextInRect(const String& text, const FTFont::Ptr& font,
		const Color::RGBA<float>& color, float x, float y, float width, float height,
		uint tabWidth)
	{
		assert(pImpl->locked && "DrawingSurface error : Cannot draw to an unlocked surface !");

		if (text.empty() || Math::Zero(width) || Math::Zero(height))
			return;

		auto& overlay = pImpl->text;
		overlay.clear() << text;
		overlay.font(font);
		overlay.color(color);
		overlay.tabWidth(tabWidth);
		// Update to get the correct necessary size
		overlay.update();

		if (Math::Zero(overlay.width()) || Math::Zero(overlay.height()))
			return;
		// Moving does not require to call update() again
		overlay.move(x + (width - overlay.width()) / 2.0f, y + (height - overlay.height()) / 2.0f);

		// Draw
		overlay.draw(pImpl->textShader);

		// Restore base shader
		pImpl->baseShader->activate();
	}


	void DrawingSurface::drawTextOnColorInRect(const String& text, const FTFont::Ptr& font,
		const Color::RGBA<float>& color, const Color::RGBA<float>& backColor,
		float x, float y, float width, float height, uint tabWidth)
	{
		assert(pImpl->locked && "DrawingSurface error : Cannot draw to an unlocked surface !");

		if (text.empty() || Math::Zero(width) || Math::Zero(height))
			return;

		auto& overlay = pImpl->text;
		overlay.clear() << text;
		overlay.font(font);
		overlay.color(color);
		overlay.tabWidth(tabWidth);
		// Update to get the correct necessary size
		overlay.update();

		if (Math::Zero(overlay.width()) || Math::Zero(overlay.height()))
			return;
		// Moving does not require to call update() again
		overlay.move(x + (width - overlay.width()) / 2.0f, y + (height - overlay.height()) / 2.0f);

		// Draw
		overlay.draw(pImpl->textShader, backColor);

		// Restore base shader
		pImpl->baseShader->activate();
	}


	void DrawingSurface::drawLine(const Color::RGBA<float>& color, float startX, float startY,
		float endX, float endY, float lineWidth)
	{
		assert(pImpl->locked && "DrawingSurface error : Cannot draw to an unlocked surface !");

		line(startX, startY, endX, endY, lineWidth,
			 color.red, color.green, color.blue, color.alpha,
			 color.red, color.green, color.blue, color.alpha, true);
	}

	void DrawingSurface::drawLine(const Color::RGBA<float>& color, const Color::RGBA<float>& bgColor,
		float startX, float startY, float endX, float endY, float lineWidth)
	{
		assert(pImpl->locked && "DrawingSurface error : Cannot draw to an unlocked surface !");

		line(startX, startY, endX, endY, lineWidth,
			 color.red, color.green, color.blue, color.alpha,
			 bgColor.red, bgColor.green, bgColor.blue, bgColor.alpha, true);
	}


	void DrawingSurface::drawRectangle(const Color::RGBA<float>& frontColor,
		const Color::RGBA<float>& backColor, float x, float y, float width, float height,
		float lineWidth)
	{
		assert(pImpl->locked && "DrawingSurface error : Cannot draw to an unlocked surface !");

		if (Math::Zero(lineWidth))
			return;

		pImpl->lineShader->activate();

		// Top line
		line(x, y, x + width, y, lineWidth,
			frontColor.red, frontColor.green, frontColor.blue, frontColor.alpha,
			backColor.red, backColor.green, backColor.blue, backColor.alpha, true);
		// Bottom line
		line(x, y + height, x + width, y + height, lineWidth,
			frontColor.red, frontColor.green, frontColor.blue, frontColor.alpha,
			backColor.red, backColor.green, backColor.blue, backColor.alpha, true);
		// Left line
		line(x, y + lineWidth / 2, x, y + height - lineWidth, lineWidth,
			frontColor.red, frontColor.green, frontColor.blue, frontColor.alpha,
			backColor.red, backColor.green, backColor.blue, backColor.alpha, true);
		// Right line
		line(x + width, y + lineWidth / 2,
			x + width, y + height - lineWidth, lineWidth,
			frontColor.red, frontColor.green, frontColor.blue, frontColor.alpha,
			backColor.red, backColor.green, backColor.blue, backColor.alpha, true);

		pImpl->baseShader->activate();
	}


	void DrawingSurface::drawFilledRectangle(const Color::RGBA<float>& frontColor,
		const Color::RGBA<float>& backColor, float x, float y, float width, float height, float lineWidth)
	{
		if (backColor.alpha > 0)
		{
			// Draw the back as a quad with the proper color
			pImpl->baseShader->bindUniform("Color", backColor);
			const float vertices[] =
				{
					x, y + height,
					x, y,
					x + width, y,
					x, y + height,
					x + width, y,
					x + width, y + height
				};
			::glEnableVertexAttribArray(Gfx3D::Vertex<>::vaPosition);
			::glVertexAttribPointer(Gfx3D::Vertex<>::vaPosition, 2, GL_FLOAT, false, 0, vertices);
			// Draw
			::glDrawArrays(GL_TRIANGLES, 0, 6);
			::glDisableVertexAttribArray(Gfx3D::Vertex<>::vaPosition);
		}

		if (frontColor != backColor && lineWidth > 0)
			drawRectangle(frontColor, backColor, x, y, width, height, lineWidth);
	}


	void DrawingSurface::fill(const Color::RGBA<float>& color)
	{
		::glClearColor(color.red, color.green, color.blue, color.alpha);
		::glClear(GL_COLOR_BUFFER_BIT);
	}


	void DrawingSurface::drawImage(const Gfx3D::Texture::Ptr& texture, float x, float y,
		float width, float height, const Color::RGBA<float>& fillColor, DisplayMode dispMode,
		float reqOffsetX, float reqOffsetY, float imageOpacity)
	{
		pImpl->pictureShader->activate();

		const float texWidth = (float)texture->width();
		const float texHeight = (float)texture->height();
		const float overlayWidth = width;
		const float overlayHeight = height;
		// dispMode == dmNone
		float offsetX = 0.0f;
		float offsetY = 0.0f;
		if (dispMode == dmOffset)
		{
			offsetX = reqOffsetX;
			offsetY = reqOffsetY;
		}
		else if (dispMode == dmCenter)
		{
			offsetX = (texWidth - overlayWidth) / 2.0f;
			offsetY = (texHeight - overlayHeight) / 2.0f;
		}
		float minTexX = 0.0f;
		float maxTexX = 1.0f;
		float minTexY = 0.0f;
		float maxTexY = 1.0f;
		float xStart = x;
		float yStart = y;
		float xEnd = xStart + overlayWidth;
		float yEnd = yStart + overlayHeight;
		float maxXBound = xStart + texWidth;
		float maxYBound = yStart + texHeight;
		switch (dispMode)
		{
			case dmStretch:
				// We do not want Bounds to interfere with display, so increase them properly
				maxXBound = xEnd;
				maxYBound = yEnd;
				// Nothing to do on coordinates are perfect
				break;
			case dmNone:
			case dmOffset:
			case dmCenter:
				if (offsetX > 0.0f)
				{
					// Fix texture coordinates
					minTexX = offsetX / texWidth;
					if (minTexX < 0.0f)
						minTexX = 0.0f;
				}
				if (texWidth > overlayWidth + offsetX)
				{
					// Fix texture coordinates
					maxTexX = (overlayWidth + offsetX) / texWidth;
					if (maxTexX > 1.0f)
						maxTexX = 1.0f;
				}
				if (offsetY > 0.0f)
				{
					minTexY = offsetY / texHeight;
					if (minTexY < 0.0f)
						minTexY = 0.0f;
				}
				if (texHeight > overlayHeight + offsetY)
				{
					maxTexY = (overlayHeight + offsetY) / texHeight;
					if (maxTexY > 1.0f)
						maxTexY = 1.0f;
				}
				break;
			case dmFit:
				// TODO
				break;
			case dmFill:
				// TODO
				break;
			default:
				assert(false && "Invalid enum value for PictureOverlay::Display !");
				break;
		}

		pImpl->pictureShader->bindUniform("FillColor", fillColor);
		pImpl->pictureShader->bindUniform("Opacity", imageOpacity);
		pImpl->pictureShader->bindUniform("Bounds", xStart, yStart, maxXBound, maxYBound);
		::glBindTexture(GL_TEXTURE_2D, texture->id());
		// Tex coords
		::glEnableVertexAttribArray(Gfx3D::Vertex<>::vaTextureCoord);
		const float texCoord[] =
			{
				minTexX, maxTexY,
				maxTexX, minTexY,
				minTexX, minTexY,
				minTexX, maxTexY,
				maxTexX, maxTexY,
				maxTexX, minTexY
			};
		::glVertexAttribPointer(Gfx3D::Vertex<>::vaTextureCoord, 2, GL_FLOAT, 0, 0, texCoord);
		// Vertices
		::glEnableVertexAttribArray(Gfx3D::Vertex<>::vaPosition);
		const float vertices[] =
			{
				xStart, yEnd,
				xEnd, yStart,
				xStart, yStart,
				xStart, yEnd,
				xEnd, yEnd,
				xEnd, yStart
			};
		::glVertexAttribPointer(Gfx3D::Vertex<>::vaPosition, 2, GL_FLOAT, 0, 0, vertices);
		// Draw
		::glDrawArrays(GL_TRIANGLES, 0, 6);
		// Clean up
		::glDisableVertexAttribArray(Gfx3D::Vertex<>::vaPosition);
		::glDisableVertexAttribArray(Gfx3D::Vertex<>::vaTextureCoord);

		pImpl->baseShader->activate();
	}


	void DrawingSurface::beginRectangleClipping(float x, float y, float width, float height)
	{
		assert(pImpl->locked && "DrawingSurface error : Cannot manage clipping on an unlocked surface !");
		::glScissor((int)x, (int)(pImpl->size.y - height - y), (uint)width, (uint)height);
		pImpl->clippings.push_back(DrawingSurfaceImpl::ClipCoord(
			Point2D<float>(x, pImpl->size.y - height - y), Point2D<float>(width, height)));
	}


	void DrawingSurface::endClipping()
	{
		assert(pImpl->locked && "DrawingSurface error : Cannot manage clipping on an unlocked surface !");
		pImpl->clippings.pop_back();
		if (!pImpl->clippings.empty())
		{
			// Reapply previous clipping
			const auto& coord = pImpl->clippings.back();
			::glScissor((int)coord.first.x, (int)coord.first.y,
				(uint)coord.second.x, (uint)coord.second.y);
		}
	}


	const Gfx3D::Texture::Ptr& DrawingSurface::texture() const
	{
		return pImpl->fb.texture();
	}


	float DrawingSurface::width() const
	{
		return pImpl->size.x;
	}


	float DrawingSurface::height() const
	{
		return pImpl->size.y;
	}



} // namespace Cairo
} // namespace Yuni
