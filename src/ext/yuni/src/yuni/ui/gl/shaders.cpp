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
#include "shaders.h"


namespace Yuni
{
namespace Gfx3D
{



	/////////////// VERTEX SHADERS


	// Minimal vertex shader : only transform the vertex coordinates
	const char* const vsTransform =
		R"(
#version 130

in vec3 attrVertex;

// Minimal vertex shader : only transform the vertex coordinates
void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * vec4(attrVertex, 1.0f);
}
		)";


	// Very simple vertex shader : transform coordinates and propagate texture coordinates
	const char* const vsTexCoord =
		R"(
#version 130

in vec3 attrVertex;
in vec2 attrTexCoord;
out vec2 texCoord;

// Very simple vertex shader : transform coordinates and propagate texture coordinates
void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * vec4(attrVertex, 1.0f);
	texCoord = attrTexCoord;
}
		)";


	// For 2D post shaders, texture coordinates are calculated by transforming vertex position
	// from [-1,1] to [0,1]
	const char* const vs2D =
		R"(
#version 130

in vec2 attrVertex;
out vec2 texCoord;

// For 2D post shaders, texture coordinates are calculated by transforming vertex position
// from [-1,1] to [0,1]
void main()
{
	gl_Position = vec4(attrVertex, 0.0f, 1.0f);
	texCoord = (attrVertex + 1.0) / 2.0;
}
		)";


	// Pass the color as attribute
	const char* const vsColorAttr =
		R"(
#version 130

in vec3 attrVertex;
in vec4 attrColor;
out vec4 color;

// Pass the color as attribute
void main()
{
	color = attrColor;
	gl_Position = gl_ModelViewProjectionMatrix * vec4(attrVertex, 1.0f);
}
		)";


	// Sample a texture using a rectangle, do not resize the image, fill empty parts with a color
	const char* const vsImageRectWithGeom =
		R"(
#version 130

in vec3 attrVertex;
in vec2 attrTexCoord;
out vec2 texCoord;
out bool hasBlank; // The rect leaves some blank on the borders
uniform vec4 Bounds; // Stored as (xMin, yMin, xMax, yMax)

// Use a rectangle overlay over a texture, store which parts of the overlay are out of bounds
void main()
{
	texCoord = attrTexCoord;
	vec4 vertex = vec4(attrVertex, 1.0f);
	hasBlank = (vertex.x < Bounds.x || vertex.x > Bounds.z || vertex.y < Bounds.y || vertex.y > Bounds.w) ? 1.0f : 0.0f;
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
		)";

	const char* const vsImageRect =
		R"(
#version 130

in vec3 attrVertex;
in vec2 attrTexCoord;
out vec2 texCoord;
out float isEmpty; // Used as a boolean (0 = false, 1 = true)
uniform vec4 Bounds; // Stored as (xMin, yMin, xMax, yMax)

// Use a rectangle overlay over a texture, store which parts of the overlay are out of bounds
void main()
{
	texCoord = attrTexCoord;
	vec4 vertex = vec4(attrVertex, 1.0f);
	isEmpty = (vertex.x < Bounds.x || vertex.x > Bounds.z || vertex.y < Bounds.y || vertex.y > Bounds.w) ? 1.0f : 0.0f;
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
		)";


	// Phong shading
	const char* const vsPhong =
		R"(
#version 130

const uint MAX_LIGHTS = 4u;

in vec3 attrVertex;
in vec3 attrNormal;
in vec2 attrTexCoord;
out vec3 vertex;
out vec3 normal;
out vec2 texCoord;
uniform uint LightCount;
uniform vec4 LightPosition[MAX_LIGHTS];

const vec3 pos = vec3(-0.5, 0.5, 0.0);

// Vertex shader for Phong shading with several lights
void main(void)
{
	vertex = vec3(gl_ModelViewMatrix * vec4(attrVertex, 1));
	normal = normalize(gl_NormalMatrix * attrNormal);
	texCoord = attrTexCoord;

	// for (uint i = 0u; i < LightCount; ++i)
	// {
	// 	vec3 lightPos = vec3(gl_ModelViewMatrix * LightPosition[i]);
	// 	// Directional
	// 	lightDir[i] = normalize(-lightPos);
	// 	// Omni / Point
	// 	// lightDir[i] = normalize(lightPos - vertex);
	// }
	//lightDir0 = normalize(vec3(gl_ModelViewMatrix * vec4(pos, 0.0)));

	gl_Position = gl_ModelViewProjectionMatrix * vec4(attrVertex, 1.0f);
}
		)";


	const char* const vsCubeMap =
		R"(
#version 130

in vec3 attrVertex;
out vec3 texCoord;

// 3D texture coordinates for a cubemap are actually the vertex' position
void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * vec4(attrVertex, 1.0f);
	texCoord = normalize(attrVertex);
}
		)";







	/////////////// FRAGMENT SHADERS




	// Use a single color given as uniform
	const char* const fsColorUniform =
		R"(
#version 130

out vec4 gl_FragColor;
uniform vec4 Color;

// Use a single color given as uniform
void main()
{
	gl_FragColor = Color;
}
		)";


	// Use a single color given as attribute
	const char* const fsColorAttr =
		R"(
#version 130

in vec4 color;
out vec4 gl_FragColor;

// Use color passed as an attribute
void main()
{
	gl_FragColor = color;
}
		)";


	// Use directly the texture value, no lighting
	const char* const fsSimpleTexture =
		R"(
#version 130

in vec2 texCoord;
out vec4 gl_FragColor;
uniform sampler2D Texture0;

// Use directly the texture value, no lighting
void main()
{
	gl_FragColor = texture(Texture0, texCoord);
}
		)";


	// Sample a texture using a rectangle, do not resize the image, fill empty parts with a color
	const char* const fsImageRect =
		R"(
#version 130

in vec2 texCoord;
in float isEmpty; // Used as a boolean (0 = false, 1 = true)
out vec4 gl_FragColor;
uniform sampler2D Texture0;
uniform vec4 FillColor = vec4(0.0f, 0.0f, 0.0f, 0.0f); // Full transparent
uniform float Opacity = 1.0f; // Opaque

void main()
{
	// Sample the texture
	vec4 texColor = texture(Texture0, texCoord);
	// Multiply by the given opacity
	texColor = vec4(texColor.rgb, texColor.a * Opacity);
	// Take either the texture color or the fill color depending on if we are out of bounds
	gl_FragColor = mix(texColor, FillColor, isEmpty);
}
		)";


	// Freetype with normal render mode generates alpha-only bitmaps, stored as GL_R textures
	// This shader displays them with the proper color.
	const char* const fsText =
		R"(
#version 130

in vec2 texCoord;
out vec4 gl_FragColor;
uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform vec4 TextColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform vec4 BackColor = vec4(0.0f, 0.0f, 0.0f, 1.0f); // Black
uniform float HasBGColor = 0.0f;

// Freetype with normal render mode generates alpha-only bitmaps, stored as GL_R textures
// This shader displays them with the proper color.
void main(void)
{
	vec4 alpha = texture(Texture0, texCoord);
	//vec4 material = texture(Texture1, texCoord);

	vec4 withBGColor = mix(BackColor, TextColor, alpha.r);
	vec4 withoutBGColor = vec4(TextColor.r, TextColor.g, TextColor.b, TextColor.a * alpha.r);
	gl_FragColor = mix(withoutBGColor, withBGColor, HasBGColor);
}
		)";


	// Color picking
	const char* const fsPicking =
		R"(
#version 410

out vec4 gl_FragColor;

// uint is 32-bit
uniform uint ObjectID = 0u;

// Color picking
void main()
{
	float highest = (ObjectID >> 24u) / 255.0f;
	float high = ((ObjectID >> 16u) & 0xff) / 255.0f;
	float low = ((ObjectID >> 8u) & 0xff) / 255.0f;
	float lowest = (ObjectID & 0xff) / 255.0f;
	gl_FragColor = vec4(highest, high, low, lowest);
}
		)";


	// Skybox : cube map sampling
	const char* const fsSkybox =
		R"(
#version 130

in vec3 texCoord;
out vec4 gl_FragColor;
uniform samplerCube Texture0;

// Cube map access
void main()
{
	gl_FragColor = texture(Texture0, texCoord);
}
		)";


	// Phong shading
	const char* const fsPhong =
		R"(
#version 130

const uint MAX_LIGHTS = 4u;

in vec3 vertex;
in vec3 normal;
in vec2 texCoord;
out vec4 gl_FragColor;
uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform uint LightCount;
uniform vec4 LightPosition[MAX_LIGHTS];
uniform vec4 LightDiffuse[MAX_LIGHTS];
uniform vec4 LightAmbient[MAX_LIGHTS];
uniform vec4 LightSpecular[MAX_LIGHTS];
uniform vec4 MaterialDiffuse = vec4(1.0f, 0.0f, 0.0f, 0.0f);
uniform vec4 MaterialAmbient = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform vec4 MaterialSpecular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform vec4 MaterialEmission = vec4(0.0f, 0.0f, 0.0f, 1.0f);
uniform float MaterialShininess;


// Phong shading with several lights
void main()
{
	vec3 eye = normalize(-vertex); // we are in Eye Coordinates, so EyePos is (0,0,0)
	vec4 texColor = vec4(texture2D(Texture0, texCoord).rgb, 1.0);

	vec4 matColor = vec4(0, 0, 0, 0);
	for (uint i = 0u; i < LightCount && i < MAX_LIGHTS; ++i)
	{
		vec3 L = normalize(vec3(gl_ModelViewMatrix * vec4(LightPosition[i].xyz, 0.0f)));
		vec3 R = normalize(-reflect(L, normal));

		// calculate Ambient Term:
		matColor += LightAmbient[i] * MaterialAmbient;

		// calculate Diffuse Term:
		vec4 diff = LightDiffuse[i] * MaterialDiffuse * texColor * max(-dot(normal,L), 0.0);
		diff = clamp(diff, 0.0, 1.0);
		matColor += diff;

		// calculate Specular Term:
		//vec4 spec = LightSpecular[i] * MaterialSpecular * pow(max(dot(R, eye), 0.0), 0.3 * MaterialShininess);
		vec4 spec = LightSpecular[i] * MaterialSpecular * max(pow(-dot(R, eye), MaterialShininess), 0.0);
		spec = clamp(spec, 0.0, 1.0);
		matColor += spec;
	}
	// write Total Color:
	gl_FragColor = matColor;
}
		)";


	const char* const fsYuv2Rgb =
		R"(
#version 130

in vec2 texCoord;
out vec4 gl_FragColor;
uniform uint PaddedWidth = 1u;
uniform uint Width = 1u; // The only important thing here is the Width / PaddedWidth ratio
uniform sampler2D TextureY;
uniform sampler2D TextureU;
uniform sampler2D TextureV;

void main()
{
	// Correct horizontal texture coordinate to account for padding
	vec2 fixedCoord = vec2(texCoord.s * Width / PaddedWidth, 1.0 - texCoord.t);

	// Sample the texture
	float y = texture2D(TextureY, fixedCoord).r;
	float cb = texture2D(TextureU, fixedCoord).r;
	float cr = texture2D(TextureV, fixedCoord).r;

	y = 1.1643 * (y - 0.0625);
	cb = cb - 0.5;
	cr = cr - 0.5;

	float r = y + 1.5958 * cr;
	float g = y - 0.39173 * cb - 0.81290 * cr;
	float b = y + 2.017 * cb;

	gl_FragColor = vec4(r, g, b, 1.0);
}
		)";






	/////////////// GEOMETRY SHADERS




	// Generate empty borders for image rectangles
	const char* const gsImageRect =
		R"(
// Necessary version for the new geometry shader syntax
#version 150

layout(triangles) in;
layout(triangle_strip, max_vertices = ) out;

uniform vec4 Bounds; // Stored as (xMin, yMin, xMax, yMax)
in bool hasBlank[3]; // Which vertices leave blank on their side
in vec2 texCoord[3]; // Texture coordinates
out float isEmpty; // Used as a boolean (0 = false, 1 = true)
out vec3 texCoord[3];

void main()
{
	vec4 vertex;
	for (int i = 0; i < gl_in.length(); ++i)
	{
		gl_Position = gl_in[i].gl_Position;
		isEmpty = false;
		EmitVertex();
	}
	EndPrimitive();
	for (int i = 0; i < gl_in.length(); ++i)
		if (hasBlank[i])
		{
			
		}
}
		)";



} // namespace Gfx3D
} // namespace Yuni
