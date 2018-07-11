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
#ifndef __YUNI_UI_GL_SHADERS_H__
# define __YUNI_UI_GL_SHADERS_H__

// This file contains necessary GLSL shaders for internal use

namespace Yuni
{
namespace Gfx3D
{



	/////////////// VERTEX SHADERS


	// Minimal vertex shader : only transform the vertex coordinates
	extern const char* const vsTransform;

	// Very simple vertex shader : transform coordinates and propagate texture coordinates
	extern const char* const vsTexCoord;

	// For 2D post shaders, texture coordinates are calculated by transforming vertex position
	// from [-1,1] to [0,1]
	extern const char* const vs2D;

	// Pass the color as attribute
	extern const char* const vsColorAttr;

	// Sample a texture using a rectangle, do not resize the image, fill empty parts with a color
	extern const char* const vsImageRect;

	// Phong shading
	extern const char* const vsPhong;

	extern const char* const vsCubeMap;



	/////////////// FRAGMENT SHADERS


	// Use a single color given as uniform
	extern const char* const fsColorUniform;

	// Use a single color given as attribute
	extern const char* const fsColorAttr;

	// Use directly the texture value, no lighting
	extern const char* const fsSimpleTexture;

	// Sample a texture using a rectangle, do not resize the image, fill empty parts with a color
	extern const char* const fsImageRect;

	// Freetype with normal render mode generates alpha-only bitmaps, stored as GL_R textures
	// This shader displays them with the proper color.
	extern const char* const fsText;

	// Color picking
	extern const char* const fsPicking;

	// Skybox : cube map sampling
	extern const char* const fsSkybox;

	// Phong shading
	extern const char* const fsPhong;


	//// POST FRAGMENT SHADERS

	extern const char* const fsYuv2Rgb;



	/////////////// GEOMETRY SHADERS


	// Generate empty borders for image rectangles
	extern const char* const gsImageRect;



} // namespace Gfx3D
} // namespace Yuni

#endif // __YUNI_UI_GL_SHADERS_H__
