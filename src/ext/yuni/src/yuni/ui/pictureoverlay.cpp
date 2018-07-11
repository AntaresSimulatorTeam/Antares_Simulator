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
#include "pictureoverlay.h"

namespace Yuni
{
namespace UI
{


	void PictureOverlay::draw(const Gfx3D::ShaderProgram::Ptr& shader) const
	{
		if (!visible() || !shader || !shader->valid() || !pTexture)
			return;

		// Set shaders
		shader->activate();
		::glActiveTexture(GL_TEXTURE0);
		::glBindTexture(GL_TEXTURE_2D, pTexture->id());
		shader->bindUniform("Texture0", Yuni::Gfx3D::Vertex<>::vaTexture0);

		const float texWidth = (float)pTexture->width();
		const float texHeight = (float)pTexture->height();
		const float overlayWidth = (float)pWidth;
		const float overlayHeight = (float)pHeight;
		// pDisplay == dmNone
		float offsetX = 0.0f;
		float offsetY = 0.0f;
		if (pDisplay == dmOffset)
		{
			offsetX = (float)pOffsetX;
			offsetY = (float)pOffsetY;
		}
		else if (pDisplay == dmCenter)
		{
			offsetX = (texWidth - overlayWidth) / 2.0f;
			offsetY = (texHeight - overlayHeight) / 2.0f;
		}
		float minTexX = 0.0f;
		float maxTexX = 1.0f;
		float minTexY = 0.0f;
		float maxTexY = 1.0f;
		float xStart = (float)pX;
		float yStart = (float)pY;
		float xEnd = xStart + overlayWidth;
		float yEnd = yStart + overlayHeight;
		switch (pDisplay)
		{
			case dmStretch:
				// Nothing to do, coordinates are perfect
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

		shader->bindUniform("Bounds", xStart, yStart, texWidth + xStart, texHeight + yStart);
		shader->bindUniform("FillColor", pFillColor);

		// Set texture coordinates
		const float texCoord[] =
			{
				minTexX, minTexY,
				minTexX, maxTexY,
				maxTexX, minTexY,
				maxTexX, minTexY,
				minTexX, maxTexY,
				maxTexX, maxTexY
			};
		::glEnableVertexAttribArray(Gfx3D::Vertex<>::vaTextureCoord);
		::glVertexAttribPointer(Gfx3D::Vertex<>::vaTextureCoord, 2, GL_FLOAT, 0, 0, texCoord);
		// Set vertex positions
		const float vertices[] =
			{
				xStart, yStart,
				xStart, yEnd,
				xEnd, yStart,
				xEnd, yStart,
				xStart, yEnd,
				xEnd, yEnd
			};
		::glEnableVertexAttribArray(Gfx3D::Vertex<>::vaPosition);
		::glVertexAttribPointer(Gfx3D::Vertex<>::vaPosition, 2, GL_FLOAT, 0, 0, vertices);
		// Draw
		::glDrawArrays(GL_TRIANGLES, 0, 6);

		// Clean-up
		::glDisableVertexAttribArray(Gfx3D::Vertex<>::vaPosition);
		::glDisableVertexAttribArray(Gfx3D::Vertex<>::vaTextureCoord);
		::glBindTexture(GL_TEXTURE_2D, 0);
		shader->deactivate();
	}



} // namespace UI
} // namespace Yuni
