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
#include "texture.h"
#include "../../core/string.h"
#include "../../core/system/windows.hdr.h"
#include "../../private/graphics/opengl/glew/glew.h"
#include <fstream>
#include <cassert>
#include <climits>

// Lib for image loading
#include "../stb_image.h"

#include "glerror.h"


namespace Yuni
{
namespace Gfx3D
{

	uint Texture::PixelStoreAlignment = 4;


	namespace // anonymous
	{

		static GLenum DepthToGLEnum(uint depth)
		{
			switch (depth)
			{
				case 3:
				case 6:
					return GL_RGB;
				case 4:
				case 8:
					return GL_RGBA;
				case 1:
					return GL_RED;
				case 2:
					return GL_RG;
			}
			return GL_RGBA;
		}


		static GLint DepthToGLEnumInternal(uint depth)
		{
			switch (depth)
			{
				case 3:
				case 6:
					return GL_RGB8;
				case 4:
				case 8:
					return GL_RGBA8;
				case 1:
					return GL_R8;
				case 2:
					return GL_RG8;
			}
			return GL_RGBA8;
		}


		static GLenum DataTypeToGLEnum(Texture::DataType type)
		{
			switch (type)
			{
				case Texture::UInt8:
					return GL_UNSIGNED_BYTE;
				case Texture::Int8:
					return GL_BYTE;
				case Texture::UInt16:
					return GL_UNSIGNED_SHORT;
				case Texture::Int16:
					return GL_SHORT;
				case Texture::UInt32:
					return GL_UNSIGNED_INT;
				case Texture::Int32:
					return GL_INT;
				case Texture::Float:
					return GL_FLOAT;
			}
			return GL_UNSIGNED_BYTE;
		}


		static void SetPixelStore(uint depth)
		{
			switch (depth)
			{
				case 4:
				case 8:
					if (4 != Texture::PixelStoreAlignment)
						::glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
					break;
				case 2:
					if (2 != Texture::PixelStoreAlignment)
						::glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
					break;
				default:
					if (1 != Texture::PixelStoreAlignment)
						::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					break;
			}
		}


	} // namespace anonymous



	Texture::Ptr Texture::LoadFromFile(const AnyString& filePath, bool mipmaps)
	{
		int width;
		int height;
		int depth;

		uint8* data = ::stbi_load(filePath.c_str(), &width, &height, &depth, 0); // 0 means auto-detect
		if (!data)
			return nullptr;
		Texture::Ptr texture = Texture::New((uint)width, (uint)height, (uint)depth, UInt8, data, mipmaps);

		// free buffer
		::stbi_image_free(data);

		return texture;
	}


	Texture::Ptr Texture::LoadFromMemory(uint length, const uint8* data)
	{
		int width;
		int height;
		int depth;

		if (!data)
			return nullptr;
		uint8* texData = ::stbi_load_from_memory(data, length, &width, &height, &depth, 0); // 0 means auto-detect
		if (!texData)
			return nullptr;

		Texture::Ptr texture = Texture::New((uint)width, (uint)height, (uint)depth, UInt8, texData, true);

		// free buffer
		::stbi_image_free(texData);

		return texture;
	}


	Texture::Ptr Texture::LoadFromBoxFile(const AnyString& filePath)
	{
		std::ifstream fd(filePath.c_str());

		if (!fd)
			return nullptr;

		uint texture;
		// Allocate a texture name
		::glGenTextures(1, &texture);
		GLTestError("Texture::LoadFromBoxFile");

		// Select our current texture
		::glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

		// when texture area is small, bilinear filter the first mipmap
		::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// when texture area is large, bilinear filter the first mipmap
		::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// the texture ends at the edges (clamp)
		::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Read the textures one by one
		const GLenum CubeMapEnums[] =
		{
			// Top
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			// Bottom
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			// Left
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			// Right
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			// Back
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			// Front
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		};

		int width;
		int height;
		int colorDepth;

		#ifdef YUNI_OS_WINDOWS
		char textureFile[MAX_PATH + 1];
		#else
		char textureFile[PATH_MAX + 1];
		#endif
		for (uint i = 0; i < 6; ++i)
		{
			fd >> textureFile;
			uint8* data = ::stbi_load(textureFile, &width, &height, &colorDepth, 0);
			if (!data)
			{
				::glDeleteTextures(1, &texture);
				return nullptr;
			}
			GLenum format = DepthToGLEnum(colorDepth);
			GLint formatInt = DepthToGLEnumInternal(colorDepth);
			GLenum type = DataTypeToGLEnum(UInt8);
			SetPixelStore(colorDepth);
			::glTexImage2D(CubeMapEnums[i], 0, formatInt, width, height, 0, format, type, data);
			::stbi_image_free(data);
		}
		::glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		return new Texture(texture, (uint)width, (uint)height, (uint)colorDepth, UInt8);
	}


	void Texture::ReleaseGLTextures(uint nbTextures, uint* textures)
	{
		::glDeleteTextures(nbTextures, textures);
	}




	Texture::Ptr Texture::New(uint width, uint height, uint colorDepth, DataType type,
		const uint8* data, bool mipmaps)
	{
		assert(width > 0 && "Creating texture with width = 0 !");
		assert(height > 0 && "Creating texture with height = 0 !");
		ID id;

		// Allocate a texture name
		::glGenTextures(1, &id);

		// Select our current texture
		::glBindTexture(GL_TEXTURE_2D, id);

		// When texture area is small, bilinear filter and average with the 2 closest mipmaps
		::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmaps ?
			GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		// When texture area is large, bilinear filter the first mipmap
		::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// The texture ends at the edges (clamp)
		::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// This Enable right here is necessary in cases where no shader is activated
		// !!! However, it is freakishly buggy ! If you do it on every texture load,
		// !!! it will break the video player (it displays levels of red...)
		// So yeah, only do it once.
		static bool first = true;
		if (first)
		{
			::glEnable(GL_TEXTURE_2D);
			first = false;
		}

		// Set the texture in OpenGL
		GLenum format = DepthToGLEnum(colorDepth);
		GLenum formatInt = DepthToGLEnumInternal(colorDepth);
		GLenum dataType = DataTypeToGLEnum(type);
		SetPixelStore(colorDepth);
		::glTexImage2D(GL_TEXTURE_2D, 0, formatInt, width, height, 0, format, dataType, data);

		// Build our texture mipmaps
		if (mipmaps)
			::glGenerateMipmap(GL_TEXTURE_2D);

		return new Texture(id, width, height, colorDepth, type);
	}


	Texture::Ptr Texture::New3D(uint width, uint height, uint depth, uint colorDepth, DataType type,
		const uint8* data, bool mipmaps)
	{
		assert(width > 0 && "Creating texture with width=0 !");
		assert(height > 0 && "Creating texture with height=0 !");
		assert(depth > 0 && "Creating texture with depth=0 !");
		ID id;

		// Allocate a texture name
		::glGenTextures(1, &id);

		// Select our current texture
		::glBindTexture(GL_TEXTURE_2D, id);

		// When texture area is small, bilinear filter and average with the 2 closest mipmaps
		::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmaps ?
			GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		// When texture area is large, bilinear filter the first mipmap
		::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// The texture ends at the edges (clamp)
		::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// Color fusion mode : only used when no shader is activated
		::glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// This Enable right here is necessary in cases where no shader is activated
		// !!! However, it is freakishly buggy ! If you do it on every texture load,
		// !!! it will break the video player (it displays levels of red...)
		// So yeah, only do it once.
		static bool first = true;
		if (first)
		{
			::glEnable(GL_TEXTURE_2D);
			first = false;
		}

		// Set the texture in OpenGL
		GLenum format = DepthToGLEnum(colorDepth);
		GLenum formatInt = DepthToGLEnumInternal(colorDepth);
		GLenum dataType = DataTypeToGLEnum(type);
		SetPixelStore(colorDepth);
		::glTexImage2D(GL_TEXTURE_2D, 0, formatInt, width, height, 0, format, dataType, data);

		// Build our texture mipmaps
		if (mipmaps)
			::glGenerateMipmap(GL_TEXTURE_2D);

		return new Texture(id, width, height, depth, colorDepth, type);
	}


	Texture::Ptr Texture::Copy(const Texture::Ptr& other, bool mipmaps)
	{
		Texture::Ptr newTex = New3D(other->width(), other->height(), other->depth(),
			other->colorDepth(), other->type(), nullptr, mipmaps);
		::glCopyTexImage2D(GL_TEXTURE_2D, 0, DepthToGLEnumInternal(other->colorDepth()), 0, 0,
			other->width(), other->height(), 0);
		return newTex;
	}


	Texture::Ptr Texture::NewMS(uint width, uint height, uint colorDepth,
		DataType type, uint samples, const uint8* /*data*/)
	{
		assert(width > 0 && "Creating texture with width=0 !");
		assert(height > 0 && "Creating texture with height=0 !");
		ID id;

		// Allocate a texture name
		::glGenTextures(1, &id);

		// Select our current texture
		::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);

		// GLenum format = DepthToGLEnum(colorDepth);
		GLenum formatInt = DepthToGLEnumInternal(colorDepth);
		// GLenum dataType = DataTypeToGLEnum(type);
		SetPixelStore(colorDepth);
		::glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, formatInt, width, height, false);

		return new Texture(id, width, height, colorDepth, type);
	}




	Texture::Texture(ID id, uint width, uint height, uint colorDepth, DataType type):
		pID(id),
		pWidth(width),
		pHeight(height),
		// No depth on 2D textures
		pDepth(0u),
		pColorDepth(colorDepth),
		pType(type)
	{
	}


	Texture::Texture(ID id, uint width, uint height, uint depth, uint colorDepth, DataType type):
		pID(id),
		pWidth(width),
		pHeight(height),
		pDepth(depth),
		pColorDepth(colorDepth),
		pType(type)
	{
	}


	Texture::~Texture()
	{
		::glDeleteTextures(1, &pID);
	}


	void Texture::resize(uint width, uint height)
	{
		assert(width > 0 && "Texture resize : width is null !");
		assert(height > 0 && "Texture resize : height is null !");
		assert(width <= GL_MAX_TEXTURE_SIZE && "Texture resize : width is too high !");
		assert(height <= GL_MAX_TEXTURE_SIZE && "Texture resize : height is too high !");

		// Update sizes
		pWidth = width;
		pHeight = height;
		// Bind texture
		::glBindTexture(GL_TEXTURE_2D, pID);
		GLenum format = DepthToGLEnum(pColorDepth);
		GLenum formatInt = DepthToGLEnumInternal(pColorDepth);
		GLenum type = DataTypeToGLEnum(pType);
		SetPixelStore(pColorDepth);
		// Sadly, glTexSubImage2D does not do the trick, we need glTexImage2D
		::glTexImage2D(GL_TEXTURE_2D, 0, formatInt, width, height, 0, format, type, nullptr);
		if (!GLTestError("glTexImage2D Texture resize"))
			std::cerr << "On texture "<< pID << std::endl;
	}


	void Texture::update(const unsigned char* data)
	{
		::glBindTexture(GL_TEXTURE_2D, pID);
		GLenum format = DepthToGLEnum(pColorDepth);
		GLenum type = DataTypeToGLEnum(pType);
		::glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pWidth, pHeight, format, type, data);
		if (!GLTestError("glTexSubImage2D Texture update"))
			std::cerr << "On texture "<< pID << std::endl;
	}


	void Texture::update(uint offsetX, uint offsetY, uint width, uint height, uint colorDepth,
		const unsigned char* data)
	{
		if (width <= 0 || height <= 0)
			// No update to do
			return;
		assert(offsetX + width <= pWidth && "Texture update : X + width is out of bounds !");
		assert(offsetY + height <= pHeight && "Texture update : Y + height is out of bounds !");
		::glBindTexture(GL_TEXTURE_2D, pID);
		int format = DepthToGLEnum(colorDepth);
		pColorDepth = colorDepth;
		::glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, width, height, format, GL_UNSIGNED_BYTE, data);
		if (!GLTestError("glTexSubImage2D Texture update"))
			std::cerr << "On texture "<< pID << std::endl;
	}


	void Texture::clear()
	{
		std::vector<uint8> data(pWidth * pHeight * 4, 0);
		update(0, 0, pWidth, pHeight, pColorDepth, &data[0]);
	}


	void Texture::clear(uint offsetX, uint offsetY, uint width, uint height)
	{
		std::vector<uint8> data(width * height * 4, 0);
		update(offsetX, offsetY, width, height, pColorDepth, &data[0]);
	}

	void Texture::clearToWhite()
	{
		std::vector<uint8> data(pWidth * pHeight * 4, 255);
		update(0, 0, pWidth, pHeight, pColorDepth, &data[0]);
	}


	Texture::ID Texture::id() const
	{
		return pID;
	}


	uint Texture::width() const
	{
		return pWidth;
	}


	uint Texture::height() const
	{
		return pHeight;
	}


	uint Texture::depth() const
	{
		assert(pDepth > 0 && "Null depth for texture ! Make sure this is a 3D texture.");
		return pDepth;
	}


	uint Texture::colorDepth() const
	{
		return pColorDepth;
	}


	Texture::DataType Texture::type() const
	{
		return pType;
	}


} // namespace Gfx3D
} // namespace Yuni
