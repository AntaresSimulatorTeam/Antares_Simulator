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
# include "textoverlay.h"


namespace Yuni
{
namespace UI
{


	void TextOverlay::update()
	{
		if (pModified || pPrevText != pText)
		{
			if (pModified)
				pFont->reset();
			pModified = false;
			pPrevText = pText;
			if (!pTexture)
				pTexture = Gfx3D::Texture::New(1, 1, 1, Gfx3D::Texture::UInt8, nullptr, false);
			if (pText.empty())
				pTexture->clear();
			else
			{
				uint width = 0u;
				uint height = 0u;
				int descent = 0;
				// We are only interested in the ascent here
				pFont->measure(pText, width, height, pAscent, descent, true, pTabWidth);
				// Draw
				pFont->draw(pText, pTexture, pAntiAliased, true, pTabWidth);
			}
		}
	}


	void TextOverlay::draw(const Gfx3D::ShaderProgram::Ptr& shader) const
	{
		if (!shader || !shader->valid() || !pTexture)
			return;
		assert(pMaterial && "Uninitialized material !");

		// Set shaders
		shader->activate();
		// Put the alpha (the actual text) on texture 0
		::glActiveTexture(GL_TEXTURE0);
		::glBindTexture(GL_TEXTURE_2D, pTexture->id());
		shader->bindUniform("Texture0", Yuni::Gfx3D::Vertex<>::vaTexture0);
		// Put the optional material surface on texture 1
		pMaterial->activate(shader, 1);
		//::glActiveTexture(GL_TEXTURE1);
		//::glBindTexture(GL_TEXTURE_2D, 8);
		// Temporary !!!
		shader->bindUniform("BackColor", Color::RGBA<float>(0.2863f, 0.5451f, 0.651f));
		shader->bindUniform("HasBGColor", 0.0f);

		// Set texture coordinates
		::glEnableVertexAttribArray(Gfx3D::Vertex<>::vaTextureCoord);
		const float texCoord[] =
			{
				0.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 0.0f,
				1.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 1.0f
			};
		::glVertexAttribPointer(Gfx3D::Vertex<>::vaTextureCoord, 2, GL_FLOAT, 0, 0, texCoord);

		float yPos = pY;
		// If necessary, offset by the Ascent to reach the text baseline
		if (pDrawOnBaseline)
		{
			yPos -= (float)pAscent;
		}
		// Ensure dimensions are pixel-aligned
		float xPos = (float)(int)pX;
		yPos = (float)(int)yPos;
		// Set vertex positions
		::glEnableVertexAttribArray(Gfx3D::Vertex<>::vaPosition);
		const float vertices[] =
			{
				xPos, yPos,
				xPos, yPos + height(),
				xPos + width(), yPos,
				xPos + width(), yPos,
				xPos, yPos + height(),
				xPos + width(), yPos + height()
			};
		::glVertexAttribPointer(Gfx3D::Vertex<>::vaPosition, 2, GL_FLOAT, 0, 0, vertices);

		// Draw
		::glDrawArrays(GL_TRIANGLES, 0, 6);

		// Clean-up
		::glDisableVertexAttribArray(Gfx3D::Vertex<>::vaPosition);
		::glDisableVertexAttribArray(Gfx3D::Vertex<>::vaTextureCoord);
		::glActiveTexture(GL_TEXTURE1);
		::glBindTexture(GL_TEXTURE_2D, 0);
		::glActiveTexture(GL_TEXTURE0);
		::glBindTexture(GL_TEXTURE_2D, 0);
		shader->deactivate();
	}


	void TextOverlay::draw(const Gfx3D::ShaderProgram::Ptr& shader, const Color::RGB<float>& backColor) const
	{
		draw(shader, Color::RGBA<float>(backColor.red, backColor.green, backColor.red, 1.0f));
	}


	void TextOverlay::draw(const Gfx3D::ShaderProgram::Ptr& shader, const Color::RGBA<float>& backColor) const
	{
		if (!shader || !shader->valid() || !pTexture)
			return;
		assert(pMaterial && "Uninitialized material !");

		// Set shaders
		shader->activate();
		// Put the alpha (the actual text) on texture 0
		::glActiveTexture(GL_TEXTURE0);
		::glBindTexture(GL_TEXTURE_2D, pTexture->id());
		shader->bindUniform("Texture0", Yuni::Gfx3D::Vertex<>::vaTexture0);
		// Put the optional material surface on texture 1
		pMaterial->activate(shader, 1);
		shader->bindUniform("BackColor", backColor);
		shader->bindUniform("HasBGColor", 1.0f);

		// Set texture coordinates
		::glEnableVertexAttribArray(Gfx3D::Vertex<>::vaTextureCoord);
		const float texCoord[] =
			{
				0.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 0.0f,
				1.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 1.0f
			};
		::glVertexAttribPointer(Gfx3D::Vertex<>::vaTextureCoord, 2, GL_FLOAT, 0, 0, texCoord);

		// Ensure dimensions are pixel-aligned
		float xPos = (float)(int)pX;
		float yPos = (float)(int)pY;
		// Set vertex positions
		::glEnableVertexAttribArray(Gfx3D::Vertex<>::vaPosition);
		const float vertices[] =
			{
				xPos, yPos,
				xPos, yPos + height(),
				xPos + width(), yPos,
				xPos + width(), yPos,
				xPos, yPos + height(),
				xPos + width(), yPos + height()
			};
		::glVertexAttribPointer(Gfx3D::Vertex<>::vaPosition, 2, GL_FLOAT, 0, 0, vertices);

		// Draw
		::glDrawArrays(GL_TRIANGLES, 0, 6);

		// Clean-up
		::glDisableVertexAttribArray(Gfx3D::Vertex<>::vaPosition);
		::glDisableVertexAttribArray(Gfx3D::Vertex<>::vaTextureCoord);
		::glActiveTexture(GL_TEXTURE1);
		::glBindTexture(GL_TEXTURE_2D, 0);
		::glActiveTexture(GL_TEXTURE0);
		::glBindTexture(GL_TEXTURE_2D, 0);
		shader->deactivate();
	}


	PictureOverlay::Ptr TextOverlay::toPicture() const
	{
		return new PictureOverlay(Gfx3D::Texture::Copy(pTexture), (int)pX, (int)pY);
	}



} // namespace UI
} // namespace Yuni
